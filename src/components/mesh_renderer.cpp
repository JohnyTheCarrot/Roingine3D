#include "mesh_renderer.h"

#include <bx/math.h>

#include "transform.h"

namespace engine {
    MeshRenderer::MeshRenderer(
            entt::registry &registry, std::unique_ptr<Mesh> mesh_uptr
    )
        : Component{registry}
        , transform_ptr_{&get_gameobject().get_or_add_component<Transform>()}
        , mesh_uptr_{std::move(mesh_uptr)} {
    }

    void MeshRenderer::render() const {
        uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_RGB |
                         BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                         BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA;
        state |= mesh_uptr_->get_format() == Mesh::IndexFormat::TriangleStrip
                       ? BGFX_STATE_PT_TRISTRIP
                       : 0;

        bgfx::setVertexBuffer(0, mesh_uptr_->get_vertex_buffer());
        bgfx::setIndexBuffer(mesh_uptr_->get_index_buffer());

        if (auto const albedo_texture_it = textures_.find(TextureType::Albedo);
            albedo_texture_it != textures_.end()) {
            albedo_texture_it->second.submit(albedo_texture_uniform_.get(), 0);
        }

        auto const trans_mat = transform_ptr_->get_transform_matrix();

        bgfx::setTransform(trans_mat.data());
        bgfx::setState(state);
        bgfx::submit(0, program_uptr_.get());
    }
}// namespace engine
