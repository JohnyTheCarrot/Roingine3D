#include "mesh_renderer.h"

#include <bx/math.h>

#include "transform.h"

namespace engine {
    MeshRenderer::MeshRenderer(entt::registry &registry)
        : Component{registry}
        , transform_ptr_{&get_gameobject().get_or_add_component<Transform>()} {
    }

    void MeshRenderer::add_mesh(
            std::span<Vertex const> const vertices,
            std::span<Index const> const  indices
    ) {
        if (auto vertex_buffer_handle_raw = bgfx::createVertexBuffer(
                    bgfx::makeRef(vertices.data(), vertices.size_bytes()),
                    Vertex::layout
            );
            bgfx::isValid(vertex_buffer_handle_raw)) {
            vertex_buffer_uptr_.reset(vertex_buffer_handle_raw);
        } else {
            throw std::runtime_error("Failed to create vertex buffer");
        }

        if (auto const index_buffer_handle_raw = bgfx::createIndexBuffer(
                    bgfx::makeRef(indices.data(), indices.size_bytes())
            );
            bgfx::isValid(index_buffer_handle_raw)) {
            index_buffer_uptr_.reset(index_buffer_handle_raw);
        } else {
            throw std::runtime_error("Failed to create index buffer");
        }
    }

    void MeshRenderer::render() const {
        if (!bgfx::isValid(vertex_buffer_uptr_.get()) ||
            !bgfx::isValid(index_buffer_uptr_.get())) {
            return;
        }

        constexpr uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_PT_TRISTRIP;

        bgfx::setVertexBuffer(0, vertex_buffer_uptr_.get());
        bgfx::setIndexBuffer(index_buffer_uptr_.get());

        std::array<float, 16> trans_mat{};
        bx::mtxTranslate(trans_mat.data(), 0, 0, 0);

        bgfx::setTransform(trans_mat.data());
        bgfx::setState(state);
        bgfx::submit(0, program_uptr_.get());
    }
}// namespace engine
