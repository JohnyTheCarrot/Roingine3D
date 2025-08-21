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
        auto const &texture_store     = TextureStore::get_instance();
        auto const &base_color_factor = texture_store.get_base_color_factor();
        for (auto const &primitive : mesh_uptr_->primitives_) {
            uint64_t state = BGFX_STATE_DEFAULT | BGFX_STATE_WRITE_RGB |
                             BGFX_STATE_WRITE_A | BGFX_STATE_WRITE_Z |
                             BGFX_STATE_DEPTH_TEST_LESS | BGFX_STATE_MSAA;

            state |= primitive.get_format() ==
                                     Primitive::IndexFormat::TriangleStrip
                           ? BGFX_STATE_PT_TRISTRIP
                           : 0;

            bgfx::setVertexBuffer(0, primitive.get_vertex_buffer());
            bgfx::setIndexBuffer(primitive.get_index_buffer());
            bgfx::setUniform(
                    base_color_factor,
                    primitive.get_base_color_factor().get_data().data()
            );

            auto const &texture_indices = primitive.get_texture_indices();
            if (texture_indices.albedo_) {
                texture_indices.albedo_->submit(TextureType::Albedo, 0);
            }

            auto const trans_mat =
                    transform_ptr_->get_transform_matrix().transpose();

            assert(trans_mat.get_span().size() == 4 * 4);
            bgfx::setTransform(trans_mat.get_span().data());
            bgfx::setState(state);
            bgfx::submit(0, program_uptr_.get());
        }
    }
}// namespace engine
