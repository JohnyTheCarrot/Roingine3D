#include "gltf_loader.h"

#include <fastgltf/tools.hpp>
#include <fastgltf/types.hpp>
#include <iostream>
#include <limits>

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
                std::vector<Vertex>      &vertices
        ) {
            vertices.resize(posAccessor.count);

            int visited = 0;
            fastgltf::iterateAccessorWithIndex<fastgltf::math::fvec3>(
                    asset, posAccessor,
                    [&](fastgltf::math::fvec3 const &pos, std::size_t idx) {
                        auto &vertex = vertices.at(idx);

                        vertex.x_ = pos.x();
                        vertex.y_ = pos.y();
                        vertex.z_ = pos.z();

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
                        vertex.u_    = static_cast<int16_t>(
                                tex_coord.x() *
                                std::numeric_limits<int16_t>::max()
                        );
                        vertex.v_ = static_cast<int16_t>(
                                tex_coord.y() *
                                std::numeric_limits<int16_t>::max()
                        );
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
                auto                result = read_accessor(
                        asset, primitive, pos_attr,
                        [&asset,
                         &vertices](fastgltf::Accessor const &pos_accessor) {
                            read_vertices(asset, pos_accessor, vertices);
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

                if (primitive.materialIndex.has_value()) {
                    auto const &mat =
                            asset.materials[primitive.materialIndex.value()];

                    auto &albedo_texture_info = mat.pbrData.baseColorTexture;
                    if (albedo_texture_info.has_value()) {
                        auto &texture = asset.textures[albedo_texture_info
                                                               ->textureIndex];
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
                            fastgltf::ComponentType::UnsignedShort)
                    throw std::runtime_error{
                            "Mesh primitive indices accessor has unsupported "
                            "component type"
                    };

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
                            throw std::runtime_error{
                                    "Unsupported primitive type: " +
                                    static_cast<int>(primitive.type)
                            };
                    }
                }();

                mesh.primitives_.emplace_back(
                        primitive_type, vertices, indices
                );
            }

            return mesh;
        }
    };// namespace gltf_mesh_loading

    constexpr fastgltf::Options gltf_options{
            fastgltf::Options::DecomposeNodeMatrices |
            fastgltf::Options::DontRequireValidAssetMember |
            fastgltf::Options::AllowDouble |
            fastgltf::Options::LoadExternalBuffers |
            fastgltf::Options::LoadExternalImages |
            fastgltf::Options::GenerateMeshIndices
    };

    void load_gltf_scene(
            Scene &scene, std::filesystem::path const &scene_file_path
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

            // auto      gltf_mat = fastgltf::getTransformMatrix(node);
            // std::span mat_span{gltf_mat.data(), gltf_mat.size_bytes()};
            // math::SquareMatrix<> mat{mat_span};
            auto const &[translation, rotation, scale] =
                    std::get<fastgltf::TRS>(node.transform);
            std::cout << "Loaded mesh: " << node.name << '\n';
            std::cout << "tX = " << translation.x()
                      << ", tY = " << translation.y()
                      << ", tZ = " << translation.z() << '\n';
            std::cout << "rX = " << rotation.x() << ", rY = " << rotation.y()
                      << ", rZ = " << rotation.z() << ", rW = " << rotation.w()
                      << '\n';
            std::cout << "sX = " << scale.x() << ", sY = " << scale.y()
                      << ", sZ = " << scale.z() << '\n';

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

            auto &mesh     = meshes.at(node.meshIndex.value());
            auto &renderer = obj.add_component<MeshRenderer>(
                    std::make_unique<Mesh>(std::move(mesh))
            );
            renderer.add_texture("assets/stare.png", TextureType::Albedo);
        };

        for (auto const scene_node : gltf_scene.nodeIndices) {
            fn(scene_node, fn);
        }
        return;
        fastgltf::iterateSceneNodes(
                asset.get(), scene_index, fastgltf::math::fmat4x4(),
                [&](fastgltf::Node &node, fastgltf::math::fmat4x4) {
                    auto obj = scene.create_game_object();

                    if (!node.meshIndex.has_value())
                        return;

                    auto &mesh     = meshes.at(node.meshIndex.value());
                    auto &renderer = obj.add_component<MeshRenderer>(
                            std::make_unique<Mesh>(std::move(mesh))
                    );
                    renderer.add_texture(
                            "assets/stare.png", TextureType::Albedo
                    );
                    auto &transform_ptr = obj.get_component<Transform>();
                    auto const &[translation, rotation, scale] =
                            std::get<fastgltf::TRS>(node.transform);
                    transform_ptr.set_position(
                            translation.x(), translation.y(), translation.z()
                    );
                    transform_ptr.set_rotation(
                            rotation.x(), rotation.y(), rotation.z(),
                            rotation.w()
                    );
                    auto constexpr temp_scale = 0.3f;
                    transform_ptr.set_scale(
                            scale.x() + temp_scale, scale.y() + temp_scale,
                            scale.z() + temp_scale
                    );
                    std::cout << "Loaded mesh: " << node.name << '\n';
                    std::cout << "tX = " << translation.x()
                              << ", tY = " << translation.y()
                              << ", tZ = " << translation.z() << '\n';
                    std::cout << "rX = " << rotation.x()
                              << ", rY = " << rotation.y()
                              << ", rZ = " << rotation.z()
                              << ", rW = " << rotation.w() << '\n';
                    std::cout << "sX = " << scale.x() << ", sY = " << scale.y()
                              << ", sZ = " << scale.z() << '\n';
                }
        );
    }
}// namespace engine
