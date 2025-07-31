#ifndef MESH_H
#define MESH_H

#include <span>

#include "types.h"

namespace engine {
    class Mesh final {
    public:
        enum class IndexFormat {
            TriangleList,
            TriangleStrip,
        };
        Mesh(IndexFormat format, std::span<Vertex const> vertices,
             std::span<Index const> indices);

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
}// namespace engine

#endif//MESH_H
