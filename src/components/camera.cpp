#include "camera.h"

#include "api_internal/math/vec_utils.h"
#include "game.h"
#include "mesh_renderer.h"

namespace engine {
    Camera::Camera(entt::registry &registry)
        : Component{registry}
        , transform_ptr_{&get_gameobject().get_or_add_component<Transform>()} {
    }

    void Camera::render() const {
        std::array view_mat = transform_ptr_->get_view_matrix();

        auto const &game = Game::get_instance();
        float       proj[16];
        bx::mtxProj(
                proj, 60.0f,
                static_cast<float>(game.get_width()) /
                        static_cast<float>(game.get_height()),
                0.1f, 1000.0f, bgfx::getCaps()->homogeneousDepth
        );
        bgfx::setViewTransform(0, view_mat.data(), proj);

        {
            auto const mesh_renderers = get_registry().view<MeshRenderer>();

            mesh_renderers.each([](auto const &mesh_renderer) {
                mesh_renderer.render();
            });
        }
    }

    void Camera::update() {
    }
}// namespace engine
