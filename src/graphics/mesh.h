#ifndef MESH_H
#define MESH_H

#include <span>
#include <vector>

#include "types.h"

namespace engine {
    class Primitive final {
    public:
        enum class IndexFormat {
            TriangleList,
            TriangleStrip,
        };
        Primitive(
                IndexFormat format, std::span<Vertex const> vertices,
                std::span<Index const> indices
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

    private:
        VertexBufferUPtr vertex_buffer_uptr_{};
        IndexBufferUPtr  index_buffer_uptr_{};
        IndexFormat      index_format_;
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
