#ifndef VEC_UTILS_H
#define VEC_UTILS_H

#include <bx/math.h>

#include "math/vec.h"

namespace engine::api_internals {
    [[nodiscard]]
    bx::Vec3 to_bx_vec3(math::Vec3 const &vec);

    [[nodiscard]]
    math::Vec3 bx_to_vec3(bx::Vec3 const &vec);
}// namespace engine::api_internals

#endif//VEC_UTILS_H
