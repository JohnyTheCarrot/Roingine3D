#include "transform.h"

#include "api_internal/math/quaternion_utils.h"
#include "api_internal/math/vec_utils.h"

namespace engine {
    std::array<float, 4 * 4> Transform::get_view_matrix() const {
        std::array<float, 4 * 4> start_mat{1.f, 0.f, 0.f, 0.f,//
                                           0.f, 1.f, 0.f, 0.f,//
                                           0.f, 0.f, 1.f, 0.f,//
                                           0.f, 0.f, 0.f, 1.f};

        if (auto const *parent = get_parent()) {
            start_mat = parent->get_view_matrix();
        }

        if (position_.is_dirty() || scale_.is_dirty() || rotation_.is_dirty()) {
            position_.clear_dirty();
            scale_.clear_dirty();
            rotation_.clear_dirty();

            std::array<float, 4 * 4> local_mat{};
            bx::mtxScale(
                    local_mat.data(), scale_->get_x(), scale_->get_y(),
                    scale_->get_z()
            );
            bx::mtxFromQuaternion(
                    local_mat.data(),
                    bx::Quaternion{
                            rotation_->x_, rotation_->y_, rotation_->z_,
                            rotation_->w_
                    },
                    api_internals::to_bx_vec3(*position_)
            );
            trans_mat_ = local_mat;
        }

        std::array<float, 4 * 4> result{};
        bx::mtxMul(result.data(), start_mat.data(), trans_mat_.data());
        return result;
    }

    math::Vec3 Transform::get_world_position() const {
        math::Vec3 result{};

        auto *gameobject = this;

        while (gameobject) {
            result += gameobject->get_position();
            gameobject = get_parent();
        }

        return result;
    }

    math::Vec3 Transform::get_world_scale() const {
        math::Vec3 result{};

        auto *gameobject = this;

        while (gameobject) {
            result += gameobject->get_scale();
            gameobject = get_parent();
        }

        return result;
    }

    math::Quaternion Transform::get_world_rotation() const {
        math::Quaternion result{};

        auto *gameobject = this;

        while (gameobject) {
            result *= gameobject->get_rotation();
            gameobject = get_parent();
        }

        return result;
    }

    std::array<float, 4 * 4> Transform::get_transform_matrix() const {
        std::array<float, 4 * 4> matrix{};

        if (position_.is_dirty() || scale_.is_dirty() || rotation_.is_dirty()) {
            position_.clear_dirty();
            scale_.clear_dirty();
            rotation_.clear_dirty();

            bx::mtxScale(
                    matrix.data(), scale_->get_x(), scale_->get_y(),
                    scale_->get_z()
            );
            bx::mtxFromQuaternion(
                    matrix.data(),
                    bx::Quaternion{
                            rotation_->x_, rotation_->y_, rotation_->z_,
                            rotation_->w_
                    },
                    api_internals::to_bx_vec3(*position_)
            );
            trans_mat_ = matrix;
        } else {
            matrix = trans_mat_;
        }

        if (auto const *parent = get_parent()) {
            auto const parent_matrix = parent->get_transform_matrix();
            std::array<float, 4 * 4> result{};
            bx::mtxMul(result.data(), matrix.data(), parent_matrix.data());
            matrix = result;
        }

        return matrix;
    }

    void Transform::translate(math::Vec3 const &translation) {
        position_ += translation;
    }

    math::Vec3 Transform::orient_vec(math::Vec3 const &vec) const {
        auto const               world_rotation = get_world_rotation();
        auto const               bx_vec = api_internals::to_bx_vec3(vec);
        std::array<float, 4 * 4> mat{};
        bx::mtxFromQuaternion(
                mat.data(), ::api_internals::to_bx_quaternion(world_rotation)
        );

        math::Vec3 result{};
        result = api_internals::bx_to_vec3(bx::mul(bx_vec, mat.data()));

        return result;
    }

    Transform *Transform::get_parent() const {
        if (auto const *parent = get_gameobject().get_parent()) {
            return parent->get_optional_component<Transform>();
        }

        return nullptr;
    }

    math::Vec3 Transform::get_forward() const {
        math::Vec3 const forward{0.f, 0.f, -1.f};

        return orient_vec(forward);
    }

    math::Vec3 Transform::get_right() const {
        math::Vec3 const right{1.f, 0.f, 0.f};

        return orient_vec(right);
    }

    math::Vec3 Transform::get_up() const {
        math::Vec3 const up{0.f, 1.f, 0.f};

        return orient_vec(up);
    }
}// namespace engine
