#include "transform.h"

namespace engine {
    std::array<float, 4 * 4> Transform::get_transform_matrix() const {
        std::array<float, 4 * 4> matrix{};
        bx::mtxScale(matrix.data(), scale_.x, scale_.y, scale_.z);
        bx::mtxFromQuaternion(matrix.data(), rotation_, position_);
        return matrix;
    }

    void Transform::translate(bx::Vec3 const &translation) {
        position_ = add(position_, translation);
    }
}// namespace engine
