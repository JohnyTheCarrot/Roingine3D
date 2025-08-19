#include "mesh.h"

#include "misc/utils.h"

namespace engine {
    Primitive::Primitive(
            IndexFormat format, std::span<Vertex const> vertices,
            std::span<Index const> indices
    )
        : vertex_buffer_uptr_{utils::verify_bgfx_handle(
                  bgfx::createVertexBuffer(
                          bgfx::copy(vertices.data(), vertices.size_bytes()),
                          Vertex::layout
                  ),
                  "failed to create vertex buffer"
          )}
        , index_buffer_uptr_{utils::verify_bgfx_handle(
                  bgfx::createIndexBuffer(
                          bgfx::copy(indices.data(), indices.size_bytes())
                  ),
                  "failed to create index buffer"
          )}
        , index_format_{format} {
    }
}// namespace engine
