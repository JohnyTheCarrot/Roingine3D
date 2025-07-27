#include "entity.h"

namespace game {
    Entity::Entity(entt::registry &registry)
        : Component{registry}
        , transform_ptr_{
                  &get_gameobject().get_or_add_component<engine::Transform>()
          } {
    }

    void Entity::move(MovementDirection dir) const {
        auto const forward = transform_ptr_->get_forward();
        auto const up      = transform_ptr_->get_up();
        auto const right   = transform_ptr_->get_right();

        switch (dir) {
            case MovementDirection::Forward:
                transform_ptr_->translate(forward * get_movement_speed());
                break;
            case MovementDirection::Backward:
                transform_ptr_->translate(forward * -get_movement_speed());
                break;
            case MovementDirection::Left:
                transform_ptr_->translate(right * get_movement_speed());
                break;
            case MovementDirection::Right:
                transform_ptr_->translate(right * -get_movement_speed());
                break;
            case MovementDirection::Up:
                transform_ptr_->translate(up * get_movement_speed());
                break;
            case MovementDirection::Down:
                transform_ptr_->translate(up * -get_movement_speed());
                break;
        }
    }
}// namespace game
