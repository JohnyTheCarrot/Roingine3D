#ifndef MESH_H
#define MESH_H

#include <optional>
#include <span>
#include <vector>

#include "math/vec.h"
#include "texture_store.h"
#include "types.h"

namespace engine {
    class Texture;

    struct TextureIndices final {
        TextureHandle albedo_{};
    };

    class Primitive final {
    public:
        enum class IndexFormat {
            TriangleList,
            TriangleStrip,
        };
        Primitive(
                IndexFormat format, std::span<Vertex const> vertices,
                std::span<Index const> indices,
                TextureIndices const  &texture_indices,
                math::Vec4 const      &base_color_factor = math::Vec4{
                        1.0f, 1.0f, 1.0f, 1.0f
                }
        );
        Primitive(Primitive const &)            = delete;
        Primitive(Primitive &&)                 = default;
        Primitive &operator=(Primitive const &) = delete;
        Primitive &operator=(Primitive &&)      = default;

        [[nodiscard]]
        IndexFormat get_format() const {
            return index_format_;
        }

        [[nodiscard]]
        VertexBufferUPtr::handle get_vertex_buffer() const {
            return vertex_buffer_uptr_.get();
        }

        [[nodiscard]]
        IndexBufferUPtr::handle get_index_buffer() const {
            return index_buffer_uptr_.get();
        }

        [[nodiscard]]
        TextureIndices const &get_texture_indices() const {
            return texture_indices_;
        }

        [[nodiscard]]
        math::Vec4 const &get_base_color_factor() const {
            return base_color_factor_;
        }

    private:
        VertexBufferUPtr vertex_buffer_uptr_{};
        IndexBufferUPtr  index_buffer_uptr_{};
        IndexFormat      index_format_;
        TextureIndices   texture_indices_;
        math::Vec4       base_color_factor_{1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct Mesh final {
        std::vector<Primitive> primitives_{};

        explicit Mesh(std::vector<Primitive> primitives)
            : primitives_{std::move(primitives)} {
        }

        Mesh(Mesh const &)                = default;
        Mesh(Mesh &&) noexcept            = default;
        Mesh &operator=(Mesh const &)     = default;
        Mesh &operator=(Mesh &&) noexcept = default;
    };
}// namespace engine

#endif//MESH_H
