#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "component.h"
#include "math/quaternion.h"
#include "math/vec.h"
#include "misc/dirty.h"

namespace engine {
    class Camera;

    class Transform final : public Component<Transform> {
        Dirty<math::Vec3>                position_{0.0f, 0.0f, 0.0f};
        Dirty<math::Vec3>                scale_{1.0f, 1.0f, 1.0f};
        Dirty<math::Quaternion>          rotation_{0.0f, 0.0f, -1.0f, 0.0f};
        mutable std::array<float, 4 * 4> trans_mat_{};

        friend class Camera;

        [[nodiscard]]
        std::array<float, 4 * 4> get_view_matrix() const;

    public:
        using Component::Component;

        template<class... Args>
            requires std::constructible_from<math::Vec3, Args...>
        void set_position(Args &&...args) {
            position_ = math::Vec3{std::forward<Args>(args)...};
        }

        template<class... Args>
            requires std::constructible_from<math::Vec3, Args...>
        void set_scale(Args &&...args) {
            scale_ = math::Vec3{std::forward<Args>(args)...};
        }

        template<class... Args>
            requires std::constructible_from<math::Quaternion, Args...>
        void set_rotation(Args &&...args) {
            rotation_ = math::Quaternion(std::forward<Args>(args)...);
        }

        [[nodiscard]]
        math::Vec3 get_position() const {
            return position_.get();
        }

        [[nodiscard]]
        math::Vec3 get_world_position() const;

        [[nodiscard]]
        math::Vec3 get_scale() const {
            return scale_.get();
        }

        [[nodiscard]]
        math::Vec3 get_world_scale() const;

        [[nodiscard]]
        math::Quaternion get_rotation() const {
            return rotation_.get();
        }

        [[nodiscard]]
        math::Quaternion get_world_rotation() const;

        [[nodiscard]]
        std::array<float, 4 * 4> get_transform_matrix() const;

        void translate(math::Vec3 const &translation);

        [[nodiscard]]
        math::Vec3 orient_vec(math::Vec3 const &vec) const;

        [[nodiscard]]
        Transform *get_parent() const;

        [[nodiscard]]
        math::Vec3 get_forward() const;

        [[nodiscard]]
        math::Vec3 get_right() const;

        [[nodiscard]]
        math::Vec3 get_up() const;
    };
}// namespace engine

#endif//TRANSFORM_H
