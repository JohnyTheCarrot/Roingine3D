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
    };
}// namespace engine::math

#endif//QUATERNION_H
