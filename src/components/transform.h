#ifndef TRANSFORM_H
#define TRANSFORM_H

#include <bx/math.h>

#include "component.h"

namespace engine {
    class Transform final : public Component<Transform> {
        bx::Vec3       position_{0.0f, 0.0f, 0.0f};
        bx::Vec3       scale_{1.0f, 1.0f, 1.0f};
        bx::Quaternion rotation_{0.0f, 0.0f, 0.0f, 1.0f};

    public:
        using Component::Component;

        template<class... Args>
            requires std::constructible_from<bx::Vec3, Args...>
        void set_position(Args &&...args) {
            position_ = bx::Vec3(std::forward<Args>(args)...);
        }

        template<class... Args>
            requires std::constructible_from<bx::Vec3, Args...>
        void set_scale(Args &&...args) {
            scale_ = bx::Vec3(std::forward<Args>(args)...);
        }

        template<class... Args>
            requires std::constructible_from<bx::Quaternion, Args...>
        void set_rotation(Args &&...args) {
            rotation_ = bx::Quaternion(std::forward<Args>(args)...);
        }

        [[nodiscard]]
        bx::Vec3 get_position() const {
            return position_;
        }

        [[nodiscard]]
        bx::Vec3 get_scale() const {
            return scale_;
        }

        [[nodiscard]]
        bx::Quaternion get_rotation() const {
            return rotation_;
        }

        [[nodiscard]]
        std::array<float, 4 * 4> get_transform_matrix() const;

        void translate(bx::Vec3 const &translation);
    };
}// namespace engine

#endif//TRANSFORM_H
