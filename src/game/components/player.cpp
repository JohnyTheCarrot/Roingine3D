#include "player.h"

#include <bgfx/bgfx.h>

#include "components/camera.h"
#include "entity.h"
#include "misc/service_locator.h"

namespace game {
    Player::Player(entt::registry &registry)
        : Component{registry}
        , entity_ptr_{&get_gameobject().get_or_add_component<Entity>()} {
        auto &input_service =
                engine::ServiceLocator<engine::KeyboardInputService>::Get();

        commands_.emplace_back(input_service.add_command(
                engine::InputKey::W, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Forward
                )
        ));
        commands_.emplace_back(input_service.add_command(
                engine::InputKey::S, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Backward
                )
        ));
        commands_.emplace_back(input_service.add_command(
                engine::InputKey::A, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Left
                )
        ));
        commands_.emplace_back(input_service.add_command(
                engine::InputKey::D, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Right
                )
        ));
        commands_.emplace_back(input_service.add_command(
                engine::InputKey::Space, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Up
                )
        ));
        commands_.emplace_back(input_service.add_command(
                engine::InputKey::LeftShift, engine::KeyEventType::Down,
                std::make_unique<MoveEntityCommand>(
                        *entity_ptr_, MovementDirection::Down
                )
        ));

        get_gameobject().add_component<engine::Camera>();
    }

    void Player::update() {
        auto const transform_ptr =
                get_gameobject().get_component<engine::Transform>();
        auto const pos = transform_ptr->get_position();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "{%f, %f, %f}", pos.x, pos.y, pos.z);
    }
}// namespace game
