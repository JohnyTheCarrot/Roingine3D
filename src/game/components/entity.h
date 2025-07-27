#ifndef ENTITY_H
#define ENTITY_H

#include "components/component.h"
#include "components/transform.h"

namespace game {
    enum class MovementDirection { Forward, Backward, Left, Right, Up, Down };

    class Entity final : public engine::Component<Entity> {
        engine::Transform *transform_ptr_{};
        float              speed_{1.0f};

    public:
        explicit Entity(entt::registry &registry);

        void set_movement_speed(float speed) {
            speed_ = speed;
        }

        [[nodiscard]]
        float get_movement_speed() const {
            return speed_;
        }

        void move(MovementDirection dir) const;
    };
}// namespace game

#endif//ENTITY_H
