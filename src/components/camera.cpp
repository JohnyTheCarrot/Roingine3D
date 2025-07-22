#include "camera.h"

#include "game.h"
#include "mesh_renderer.h"

namespace engine {
    Camera::Camera(entt::registry &registry)
        : Component{registry}
        , transform_ptr_{&get_gameobject().get_or_add_component<Transform>()} {
    }

    void Camera::render() const {
        auto const location = transform_ptr_->get_position();

        float view[16];
        bx::mtxLookAt(view, location, get_look_at());

        auto const &game = Game::get_instance();
        float       proj[16];
        bx::mtxProj(
                proj, 60.0f,
                static_cast<float>(game.get_width()) /
                        static_cast<float>(game.get_height()),
                0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth
        );
        bgfx::setViewTransform(0, view, proj);

        {
            auto const mesh_renderers = get_registry().view<MeshRenderer>();

            mesh_renderers.each([](auto const &mesh_renderer) {
                mesh_renderer.render();
            });
        }
    }

    void Camera::update() {
    }

    bx::Vec3 Camera::get_look_at() const {
        if (look_at_vec_override_.has_value()) {
            return look_at_vec_override_.value();
        }

        return bx::add(transform_ptr_->get_position(), look_direction_);
    }
}// namespace engine
