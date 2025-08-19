#include "player.h"

#include <bgfx/bgfx.h>

#include "components/camera.h"
#include "entity.h"
#include "game/commands/cam_adjust_command.h"
#include "math/quaternion.h"
#include "misc/service_locator.h"
#include "misc/utils.h"

namespace game {
    void Player::orient() {
        namespace math = engine::math;

        {
            auto const pitch_adjusted = cam_pitch_ - 90.f;
            math::Vec3 look_direction{};
            look_direction.set_y(sin(engine::utils::deg_to_rad(pitch_adjusted))
            );
            look_direction.set_z(cos(engine::utils::deg_to_rad(pitch_adjusted))
            );
            look_direction.normalize();

            engine::math::Quaternion rotation{
                    look_direction.get_x(), look_direction.get_y(),
                    look_direction.get_z(), 0.f
            };

            auto &cam_transform =
                    camera_gameobject_.get_component<engine::Transform>();

            cam_transform.set_rotation(rotation);
        }

        auto orientation_quat{math::Quaternion::from_axis_angle(
                math::Vec3{0.f, 1.f, 0.f}, engine::utils::deg_to_rad(cam_yaw_)
        )};

        auto &transform = get_gameobject().get_component<engine::Transform>();
        transform.set_rotation(orientation_quat);
    }

    Player::Player(entt::registry &registry)
        : Component{registry}
        , entity_ptr_{&get_gameobject().get_or_add_component<Entity>()}
        , camera_gameobject_{get_gameobject().add_child()} {
        auto &input_service =
                engine::ServiceLocator<engine::KeyboardMouseInputService>::Get(
                );

        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::W, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Forward
                )
        ));
        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::S, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Backward
                )
        ));
        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::A, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Left
                )
        ));
        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::D, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Right
                )
        ));
        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::Space, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Up
                )
        ));
        keyboard_commands_.emplace_back(input_service.add_command(
                engine::InputKey::LeftShift, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Down
                )
        ));

        camera_gameobject_.add_component<engine::Camera>();

        mouse_commands_.emplace_back(input_service.add_command(
                std::make_unique<CameraAdjustCommand>(*this)
        ));
        orient();
    }

    void Player::update() {
        auto const transform_ptr =
                get_gameobject().get_optional_component<engine::Transform>();
        auto const pos = transform_ptr->get_position();
        bgfx::dbgTextPrintf(
                0, 0, 0x0f, "{%f, %f, %f}", pos.get_x(), pos.get_y(),
                pos.get_z()
        );
        bgfx::dbgTextPrintf(
                0, 1, 0x0f, "Cam pitch: %f, yaw: %f", cam_pitch_, cam_yaw_
        );
        bgfx::dbgTextPrintf(
                0, 2, 0x0f, "rotQ: %f + %fi + %fj + %fk",
                transform_ptr->get_rotation().x_,
                transform_ptr->get_rotation().y_,
                transform_ptr->get_rotation().z_,
                transform_ptr->get_rotation().w_
        );
        bgfx::dbgTextPrintf(
                0, 3, 0x0f, "fwd: {%f, %f, %f}",
                transform_ptr->get_forward().get_x(),
                transform_ptr->get_forward().get_y(),
                transform_ptr->get_forward().get_z()
        );
    }

    void Player::rotate(float delta_x, float delta_y) {
        auto const factor =
                engine::Application::get_instance().get_delta_time() *
                rot_speed_;

        cam_pitch_ += delta_y * factor;
        cam_yaw_ += delta_x * factor;
        cam_pitch_ = std::clamp(cam_pitch_, -45.0f, 45.0f);
        cam_yaw_   = std::fmod(cam_yaw_, 360.0f);

        orient();
    }
}// namespace game
