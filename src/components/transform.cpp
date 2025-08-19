#include "transform.h"

#include "api_internal/math/vec_utils.h"

namespace engine {
    math::SquareMatrix<> Transform::get_view_matrix() const {
        math::SquareMatrix<> start_mat{};

        if (auto const *parent = get_parent()) {
            start_mat = parent->get_view_matrix();
        }

        if (position_.is_dirty() || scale_.is_dirty() || rotation_.is_dirty()) {
            position_.clear_dirty();
            scale_.clear_dirty();
            rotation_.clear_dirty();

            {
                math::SquareMatrix<> local_mat{};

                transform_ = local_mat.scale(1.f / scale_.get())
                                     .rotate(rotation_.get().inverse())
                                     .translate(-position_.get());
            }
        }

        return transform_ * start_mat;
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

    math::SquareMatrix<> Transform::get_transform_matrix() const {
        if (position_.is_dirty() || scale_.is_dirty() || rotation_.is_dirty()) {
            position_.clear_dirty();
            scale_.clear_dirty();
            rotation_.clear_dirty();

            math::SquareMatrix<> result{};

            result.translate(position_.get())
                    .rotate(rotation_.get())
                    .scale(scale_.get());

            transform_ = math::SquareMatrix<>{result};
        }

        if (auto const *parent = get_parent()) {
            auto const           parent_matrix = parent->get_transform_matrix();
            math::SquareMatrix<> result{};
            bx::mtxMul(
                    result.get_span().data(), transform_.get_data().data(),
                    parent_matrix.get_data().data()
            );

            return math::SquareMatrix<>{result};
        }

        return transform_;
    }

    void Transform::translate(math::Vec3 const &translation) {
        position_ += translation;
    }

    math::Vec3 Transform::orient_vec(math::Vec3 const &vec) const {
        auto const world_rotation = get_world_rotation();

        math::SquareMatrix<> mat{};
        mat.rotate(world_rotation);
        math::Vec4 result4{mat * math::Vec4{vec, 0.f}};
        math::Vec3 result = result4.get_xyz();

        return result;
    }

    Transform *Transform::get_parent() const {
        if (auto const *parent = get_gameobject().get_parent()) {
            return parent->get_optional_component<Transform>();
        }

        return nullptr;
    }

    math::Vec3 Transform::get_forward() const {
        return orient_vec(c_DefaultForward);
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
