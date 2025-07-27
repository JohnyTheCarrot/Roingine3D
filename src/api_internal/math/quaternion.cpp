#include "math/quaternion.h"

#include "quaternion_utils.h"

namespace engine::math {
    Quaternion::Quaternion(Vec3 const &vec)
        : x_{vec.get_x()}
        , y_{vec.get_y()}
        , z_{vec.get_z()}
        , w_{0.f} {
    }

    Quaternion Quaternion::operator*(Quaternion const &other) const {
        Quaternion result;

        result.w_ =
                w_ * other.w_ - x_ * other.x_ - y_ * other.y_ - z_ * other.z_;
        result.x_ =
                w_ * other.x_ + x_ * other.w_ + y_ * other.z_ - z_ * other.y_;
        result.y_ =
                w_ * other.y_ - x_ * other.z_ + y_ * other.w_ + z_ * other.x_;
        result.z_ =
                w_ * other.z_ + x_ * other.y_ - y_ * other.x_ + z_ * other.w_;

        return result;
    }

    Quaternion &Quaternion::operator*=(Quaternion const &other) {
        *this = *this * other;
        return *this;
    }
}// namespace engine::math

namespace engine::api_internals {
    bx::Quaternion to_bx_quaternion(math::Quaternion const &q) {
        return bx::Quaternion{q.x_, q.y_, q.z_, q.w_};
    }

    math::Quaternion bx_to_quaternion(bx::Quaternion const &q) {
        return math::Quaternion{q.x, q.y, q.z, q.w};
    }
}// namespace engine::api_internals
