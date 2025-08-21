#include "gltf_loader.h"

#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <iostream>
#include <magic_enum.hpp>
#include <span>

#include "components/mesh_renderer.h"
#include "components/transform.h"
#include "graphics/mesh.h"
#include "scene.h"
#include "types.h"

namespace engine {
    constexpr std::string_view pos_attr{"POSITION"};
    constexpr std::string_view texcoord_color_attr{"TEXCOORD_0"};

    template<typename F>
    bool read_accessor(
            fastgltf::Asset const &asset, fastgltf::Primitive const &primitive,
            std::string_view attr, F &&callback
    ) {
        auto const it = primitive.findAttribute(attr);
        if (it == primitive.attributes.end()) {
            throw std::runtime_error{
                    "Mesh primitive does not contain " + std::string{attr} +
                    " attribute"
            };
        }

        auto const &accessor = asset.accessors[it->accessorIndex];
        if (!accessor.bufferViewIndex.has_value()) {
            return false;
        }

        callback(accessor);

        return true;
    }

    namespace gltf_mesh_loading {
        static void read_vertices(
                fastgltf::Asset const    &asset,
                fastgltf::Accessor const &posAccessor,
                std::vector<Vertex> &vertices, math::Vec3 &min_pos,
                math::Vec3 &max_pos
        ) {
            vertices.resize(posAccessor.count);

            int visited = 0;
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset, posAccessor,
                    [&](fastgltf::math::fvec3 const &pos, std::size_t idx) {
                        auto &vertex = vertices.at(idx);

                        if (pos.x() < min_pos.get_x())
                            min_pos.set_x(pos.x());
                        if (pos.y() < min_pos.get_y())
                            min_pos.set_y(pos.y());
                        if (pos.z() < min_pos.get_z())
                            min_pos.set_z(pos.z());
                        if (pos.x() > max_pos.get_x())
                            max_pos.set_x(pos.x());
                        if (pos.y() > max_pos.get_y())
                            max_pos.set_y(pos.y());
                        if (pos.z() > max_pos.get_z())
                            max_pos.set_z(pos.z());

                        vertex.x_ = pos.x();
                        vertex.y_ = pos.y();
                        vertex.z_ = -pos.z();

                        vertex.u_ = 0;
                        vertex.v_ = 0;
                        ++visited;
                    }
            );
        }

        static void read_uvs(
                fastgltf::Asset const    &asset,
                fastgltf::Accessor const &accessor,
                std::vector<Vertex>      &vertices
        ) {
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec2>(
                    asset, accessor,
                    [&](fastgltf::math::fvec2 const &tex_coord,
                        std::size_t                  idx) {
                        auto &vertex = vertices[idx];
                        vertex.u_    = tex_coord.x();
                        vertex.v_    = tex_coord.y();
                    }
            );
        }

        [[nodiscard]]
        Mesh load_mesh(
                fastgltf::Asset const &asset, fastgltf::Mesh const &gltf_mesh
        ) {
            Mesh mesh{{}};

            for (auto const &primitive : gltf_mesh.primitives) {
                if (!primitive.indicesAccessor.has_value()) {
                    throw std::runtime_error{
                            "Mesh primitive does not contain indices"
                    };
                }

                std::vector<Vertex> vertices;
                auto const lowest_float = {std::numeric_limits<float>::lowest()
                };
                math::Vec3 min_pos, max_pos{};
                auto       result = read_accessor(
                        asset, primitive, pos_attr,
                        [&](fastgltf::Accessor const &pos_accessor) {
                            read_vertices(
                                    asset, pos_accessor, vertices, min_pos,
                                    max_pos
                            );
                        }
                );
                if (!result)
                    continue;

                result = read_accessor(
                        asset, primitive, texcoord_color_attr,
                        [&asset,
                         &vertices](fastgltf::Accessor const &tex_accessor) {
                            read_uvs(asset, tex_accessor, vertices);
                        }
                );
                if (!result)
                    continue;

                math::Vec4     base_color_factor{1.0f, 1.0f, 1.0f, 1.0f};
                TextureIndices texture_indices{};
                if (primitive.materialIndex.has_value()) {
                    auto const &mat =
                            asset.materials[primitive.materialIndex.value()];

                    auto &albedo_texture_info = mat.pbrData.baseColorTexture;
                    if (albedo_texture_info.has_value()) {
                        auto &texture = asset.textures[albedo_texture_info
                                                               ->textureIndex];
                        if (texture.imageIndex.has_value())
                            texture_indices.albedo_ =
                                    TextureHandle{texture.imageIndex.value()};
                        base_color_factor = math::Vec4{
                                mat.pbrData.baseColorFactor[0],
                                mat.pbrData.baseColorFactor[1],
                                mat.pbrData.baseColorFactor[2],
                                mat.pbrData.baseColorFactor[3]
                        };
                    }
                }

                auto const &index_accessor =
                        asset.accessors[primitive.indicesAccessor.value()];
                if (!index_accessor.bufferViewIndex.has_value())
                    throw std::runtime_error{
                            "Mesh primitive indices accessor does not "
                            "contain a buffer view"
                    };
                std::vector<Index> indices(index_accessor.count);

                if (index_accessor.componentType !=
                            fastgltf::ComponentType::UnsignedByte &&
                    index_accessor.componentType !=
                            fastgltf::ComponentType::UnsignedShort &&
                    index_accessor.componentType !=
                            fastgltf::ComponentType::UnsignedInt)
                    throw std::runtime_error{std::format(
                            "Mesh primitive indices accessor has unsupported "
                            "component type: {}",
                            magic_enum::enum_name(index_accessor.componentType)
                    )};

                fastgltf::copyFromAccessor<Index>(
                        asset, index_accessor, indices.data()
                );
                auto const primitive_type = [&primitive] {
                    switch (primitive.type) {
                        case fastgltf::PrimitiveType::Triangles:
                            return Primitive::IndexFormat::TriangleList;
                        case fastgltf::PrimitiveType::TriangleStrip:
                            return Primitive::IndexFormat::TriangleStrip;
                        default:
                            throw std::runtime_error{std::format(
                                    "Unsupported primitive type: {}",
                                    static_cast<int>(primitive.type)
                            )};
                    }
                }();

                mesh.primitives_.emplace_back(
                        primitive_type, vertices, indices, texture_indices,
                        base_color_factor
                );
            }

            return mesh;
        }
    };// namespace gltf_mesh_loading

    [[nodiscard]]
    Texture load_image_data(
            std::string const &img_name, fastgltf::Asset &asset,
            fastgltf::DataSource data, std::filesystem::path const &cwd
    ) {
        return std::visit(
                fastgltf::visitor{
                        [](auto &) -> Texture {
                            throw std::runtime_error{"Unhandled image format"};
                        },
                        [&](fastgltf::sources::URI const &file_path
                        ) -> Texture {
                            auto const path = cwd / file_path.uri.string();

                            return Texture{path, img_name};
                        },
                        [&](fastgltf::sources::Array &array) {
                            return Texture{
                                    std::span{
                                            reinterpret_cast<stbi_uc const *>(
                                                    array.bytes.data()
                                            ),
                                            array.bytes.size()
                                    },
                                    img_name
                            };
                        },
                        [&](fastgltf::sources::BufferView &view) {
                            auto &buffer_view =
                                    asset.bufferViews[view.bufferViewIndex];
                            auto const &buffer =
                                    asset.buffers[buffer_view.bufferIndex];

                            auto array{std::get<fastgltf::sources::Array>(
                                    buffer.data
                            )};

                            return Texture{
                                    std::span{
                                            reinterpret_cast<stbi_uc const *>(
                                                    array.bytes.data() +
                                                    buffer_view.byteOffset
                                            ),
                                            buffer_view.byteLength
                                    },
                                    img_name
                            };
                        }
                },
                data
        );
    }

    void load_image(
            fastgltf::Asset &asset, fastgltf::Image &image,
            std::filesystem::path const &cwd
    ) {
        std::string const img_name{image.name};

        auto texture = load_image_data(img_name, asset, image.data, cwd);

        TextureStore::get_instance().add_texture(img_name, std::move(texture));
    }

    constexpr fastgltf::Options gltf_options{
            fastgltf::Options::DecomposeNodeMatrices |
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble |
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages |
            fastgltf::Options::GenerateMeshIndices
    };

    void load_gltf_scene(
            Scene &scene, std::filesystem::path const &scene_file_path,
            GameObject *parent_ptr
    ) {
        auto buffer = fastgltf::MappedGltfFile::FromPath(scene_file_path);
        if (!buffer) {
            throw std::runtime_error{std::format(
                    "Failed to read glTF file: {} for reason: {}",
                    scene_file_path.string(),
                    fastgltf::getErrorName(buffer.error())
            )};
        }

        fastgltf::Parser parser;
        auto             asset = parser.loadGltf(
                buffer.get(), scene_file_path.parent_path(), gltf_options
        );
        if (asset.error() != fastgltf::Error::None) {
            throw std::runtime_error{
                    "Failed to load glTF file: " +
                    std::string{fastgltf::getErrorName(asset.error())}
            };
        }
        for (auto &image : asset->images) {
            load_image(asset.get(), image, scene_file_path.parent_path());
        }

        std::vector<Mesh> meshes;
        meshes.reserve(asset->meshes.size());
        for (auto const &gltf_mesh : asset->meshes) {
            meshes.emplace_back(
                    gltf_mesh_loading::load_mesh(asset.get(), gltf_mesh)
            );
        }

        auto const scene_index = asset->defaultScene.value_or(0);

        auto &gltf_scene = asset->scenes[scene_index];

        auto fn = [&](auto node_index, auto &self,
                      GameObject const *parent_ptr = nullptr) -> void {
            fastgltf::Node &node = asset->nodes[node_index];

            auto obj = scene.create_game_object();
            if (parent_ptr) {
                obj.set_parent(*parent_ptr);
            }

            auto const &[translation, rotation, scale] =
                    std::get<fastgltf::TRS>(node.transform);

            auto &transform = obj.add_component<Transform>();
            transform.set_position(
                    translation.x(), translation.y(), translation.z()
            );
            transform.set_rotation(
                    rotation.x(), rotation.y(), rotation.z(), rotation.w()
            );
            transform.set_scale(scale.x(), scale.y(), scale.z());

            for (auto const child_index : node.children) {
                self(child_index, self, &obj);
            }

            if (!node.meshIndex.has_value())
                return;

            auto &mesh = meshes.at(node.meshIndex.value());
            obj.add_component<MeshRenderer>(
                    std::make_unique<Mesh>(std::move(mesh))
            );
        };

        for (auto const scene_node : gltf_scene.nodeIndices) {
            fn(scene_node, fn, parent_ptr);
        }
    }
}// namespace engine
