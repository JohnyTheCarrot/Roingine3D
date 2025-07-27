#include "vec_utils.h"

#include "math/vec.h"

namespace engine::api_internals {
    [[nodiscard]]
    bx::Vec3 to_bx_vec3(math::Vec3 const &vec) {
        return bx::Vec3{vec.get_x(), vec.get_y(), vec.get_z()};
    }

    math::Vec3 bx_to_vec3(bx::Vec3 const &vec) {
        return math::Vec3{vec.x, vec.y, vec.z};
    }
}// namespace engine::api_internals
