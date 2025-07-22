#ifndef ENTITY_H
#define ENTITY_H

#include "components/component.h"
#include "components/transform.h"

namespace bx {
    struct Vec3;
}

namespace game {
    enum class MovementDirection { Forward, Backward, Left, Right, Up, Down };

    class Entity final : public engine::Component<Entity> {
        engine::Transform *transform_ptr_{};
        bx::Vec3           forward_{0, 0, -1};
        bx::Vec3           up_{0, 1, 0};
        float              speed_{1.0f};

    public:
        explicit Entity(entt::registry &registry);

        template<class... Args>
            requires std::constructible_from<bx::Vec3, Args...>
        void set_forward(Args &&...args) {
            forward_ = bx::Vec3(std::forward<Args>(args)...);
        }

        template<class... Args>
            requires std::constructible_from<bx::Vec3, Args...>
        void set_up(Args &&...args) {
            up_ = bx::Vec3(std::forward<Args>(args)...);
        }

        [[nodiscard]]
        bx::Vec3 const &get_forward() const {
            return forward_;
        }

        [[nodiscard]]
        bx::Vec3 const &get_up() const {
            return up_;
        }

        [[nodiscard]]
        bx::Vec3 get_right() const {
            return bx::normalize(bx::cross(forward_, up_));
        }

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
