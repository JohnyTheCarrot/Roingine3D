#ifndef QUATERNION_H
#define QUATERNION_H

#include "math/vec.h"

namespace engine::math {
    struct Quaternion final {
        float x_{0.0f}, y_{0.0f}, z_{0.0f}, w_{1.0f};

        Quaternion() = default;

        Quaternion(float x, float y, float z, float w)
            : x_{x}
            , y_{y}
            , z_{z}
            , w_{w} {
        }

        explicit Quaternion(Vec3 const &vec);

        [[nodiscard]]
        auto operator<=>(Quaternion const &other) const = default;

        [[nodiscard]]
        Quaternion operator*(Quaternion const &other) const;

        Quaternion &operator*=(Quaternion const &other);

        [[nodiscard]]
        float norm() const {
            return std::sqrt(norm_squared());
        }

        [[nodiscard]]
        float norm_squared() const {
            return x_ * x_ + y_ * y_ + z_ * z_ + w_ * w_;
        }

        [[nodiscard]]
        Quaternion conjugate() const {
            return Quaternion{-x_, -y_, -z_, w_};
        }

        [[nodiscard]]
        Quaternion inverse() const {
            return conjugate() / norm_squared();
        }

        [[nodiscard]]
        Quaternion normalized() const {
            Quaternion clone{*this};
            clone.normalize();

            return clone;
        }

        void normalize() {
            float n = norm();
            if (n == 0.f) {
                return;
            }
            *this = *this / n;
        }

        [[nodiscard]]
        Vec3 rotate(Vec3 const &vec) const {
            Quaternion vec_quat{vec.get_x(), vec.get_y(), vec.get_z(), 0.f};

            Quaternion result = *this * vec_quat * conjugate();

            return Vec3{result.x_, result.y_, result.z_};
        }

        [[nodiscard]]
        Quaternion operator/(float scalar) const {
            return Quaternion{
                    x_ / scalar, y_ / scalar, z_ / scalar, w_ / scalar
            };
        }

        void set_xyz(Vec3 const &vec) {
            x_ = vec.get_x();
            y_ = vec.get_y();
            z_ = vec.get_z();
        }

        [[nodiscard]]
        bool is_unit() const {
            return std::abs(norm_squared() - 1.0f) < 1e-6f;
        }

        [[nodiscard]]
        static Quaternion rotate_between(Vec3 const &from, Vec3 const &to);

        [[nodiscard]]
        static Quaternion from_axis_angle(Vec3 const &axis, float angle) {
            Vec3  normalized_axis = axis.normalized();
            float half_angle      = angle * 0.5f;
            float sin_half_angle  = std::sin(half_angle);

            return Quaternion{
                    normalized_axis.get_x() * sin_half_angle,
                    normalized_axis.get_y() * sin_half_angle,
                    normalized_axis.get_z() * sin_half_angle,
                    std::cos(half_angle)
            };
        }
    };
}// namespace engine::math

#endif//QUATERNION_H
