#ifndef QUATERNION_UTILS_H
#define QUATERNION_UTILS_H

#include "bx/math.h"

namespace engine::math {
    struct Quaternion;
}

namespace engine::api_internals {
    [[nodiscard]]
    bx::Quaternion to_bx_quaternion(engine::math::Quaternion const &q);

    [[nodiscard]]
    math::Quaternion bx_to_quaternion(bx::Quaternion const &q);
}// namespace engine::api_internals

#endif//QUATERNION_UTILS_H
