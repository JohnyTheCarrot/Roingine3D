#ifndef MATRIX_H
#define MATRIX_H

#include <array>

#include "misc/spaced_span.h"
#include "quaternion.h"
#include "vec.h"

namespace engine::math {
    namespace {
        template<typename T>
        constexpr T square(T value) {
            return value * value;
        }
    }// namespace

    template<typename El, std::size_t W, std::size_t H>
    class Matrix final {
    public:
        using Data         = std::array<El, W * H>;
        using RowVec       = Vec<El, W>;
        using RowArray     = std::array<El, W>;
        using RowSpan      = std::span<El, W>;
        using ColSpan      = SpacedSpan<El, (H - 1) * sizeof(El)>;
        using ConstRowSpan = std::span<El const, W>;
        using ConstColSpan = SpacedSpan<El const, ColSpan::c_SpaceBetween>;
        using Self         = Matrix<El, W, H>;
        using Rows         = std::array<RowSpan, H>;
        using ConstRows    = std::array<ConstRowSpan, H>;
        using ConstCols    = std::array<ConstColSpan, W>;

        constexpr Matrix()
            : values_{} {
            if constexpr (W == H) {
                // Identity matrix
                for (std::size_t i = 0; i < W; ++i) {
                    values_[i * W + i] = El{1};
                }
            }
        }

        template<typename... Args>
            requires(std::convertible_to<RowVec, Args> && ...) ||
                    (std::convertible_to<RowArray, Args> && ...)
        constexpr explicit Matrix(Args &&...args) {
            auto it = values_.begin();
            for (auto const &vec :
                 std::initializer_list{std::forward<Args>(args)...}) {
                std::copy(vec.cbegin(), vec.cend(), it);
                it += W;
            }
        }

        constexpr Matrix(std::initializer_list<El> init) {
            std::copy(init.begin(), init.end(), values_.begin());
        }

        constexpr explicit Matrix(Data const &data)
            : values_{data} {
        }

        constexpr explicit Matrix(std::span<El const> data) {
            if (data.size() != W * H) {
                throw std::runtime_error{"Matrix data size mismatch"};
            }

            std::copy(data.cbegin(), data.cend(), values_.begin());
        }

        [[nodiscard]]
        constexpr ConstRows get_rows() const {
            auto const seq = std::make_index_sequence<H>();

            return [this]<std::size_t... I>(std::index_sequence<I...>) {
                return std::array<ConstRowSpan, H>{ConstRowSpan{get_row(I)}...};
            }(seq);
        }

        [[nodiscard]]
        constexpr Rows get_rows() {
            auto const seq = std::make_index_sequence<H>();

            return [this]<std::size_t... I>(std::index_sequence<I...>) {
                return std::array<RowSpan, H>{RowSpan{get_row(I)}...};
            }(seq);
        }

        [[nodiscard]]
        constexpr ConstRowSpan get_row(std::size_t row) const {
            if (row >= H) {
                throw std::out_of_range{"Row index out of range"};
            }

            return std::span<El const, W>{values_.cbegin() + row * W, W};
        }

        [[nodiscard]]
        constexpr RowSpan get_row(std::size_t row) {
            if (row >= H) {
                throw std::out_of_range{"Row index out of range"};
            }

            return std::span<El, W>{values_.begin() + row * W, W};
        }

        [[nodiscard]]
        ConstCols get_cols() const {
            std::array<ConstColSpan, W> cols{};

            for (std::size_t i = 0; i < W; ++i) {
                cols[i] = ConstColSpan{values_.data() + i, H};
            }

            return cols;
        }

        [[nodiscard]]
        std::array<ColSpan, W> get_cols() {
            std::array<ColSpan, W> cols{};

            for (std::size_t i = 0; i < W; ++i) {
                cols[i] = ColSpan{values_.data() + i, H};
            }

            return cols;
        }

        [[nodiscard]]
        ColSpan get_col(std::size_t col) {
            if (col >= W) {
                throw std::out_of_range{"Column index out of range"};
            }

            return ColSpan{values_.data() + col, H};
        }

        [[nodiscard]]
        ConstColSpan get_col(std::size_t col) const {
            if (col >= W) {
                throw std::out_of_range{"Column index out of range"};
            }

            return ConstColSpan{values_.data() + col, H};
        }

        [[nodiscard]]
        constexpr RowVec get_vec(std::size_t row) const {
            return RowVec{get_row(row)};
        }

        [[nodiscard]]
        Data const &get_data() const {
            return values_;
        }

        [[nodiscard]]
        Data &get_data() {
            return values_;
        }

        [[nodiscard]]
        El &get(std::size_t row, std::size_t col) {
            if (row >= H || col >= W) {
                throw std::out_of_range{"Index out of range"};
            }

            return values_[row * W + col];
        }

        [[nodiscard]]
        std::span<El const, W * H> get_span() const {
            return std::span<El const, W * H>{values_.data(), W * H};
        }

        [[nodiscard]]
        std::span<El, W * H> get_span() {
            return std::span<El, W * H>{values_.data(), W * H};
        }

        template<
                std::size_t OtherW, std::size_t OtherH, std::size_t OurW = W,
                std::size_t OurH = H>
            requires(OurW == OtherH && OurH == OtherW)
        [[nodiscard]]
        Matrix<El, W, OtherH>
        operator*(Matrix<El, OtherW, OtherH> const &other) const {
            Matrix<El, W, OtherH> result{};

            for (std::size_t i = 0; i < W; ++i) {
                auto row_a = get_row(i);
                for (std::size_t j = 0; j < OtherH; ++j) {
                    auto col_b = other.get_col(j);

                    result.get(i, j) = std::transform_reduce(
                            row_a.cbegin(), row_a.cend(), col_b.cbegin(), El{0},
                            std::plus<El>{}, [](El a, El b) { return a * b; }
                    );
                }
            }

            return result;
        }

        template<std::size_t VecH, std::size_t OurW = W, std::size_t OurH = H>
            requires(OurW == VecH && OurH == OurW)
        [[nodiscard]]
        Vec<El, VecH> operator*(Vec<El, VecH> const &vec) const {
            Vec<El, VecH> result{};

            for (std::size_t i = 0; i < W; ++i) {
                auto row  = get_row(i);
                result[i] = std::inner_product(
                        row.cbegin(), row.cend(), vec.cbegin(), El{0},
                        std::plus<El>{}, [](El a, El b) { return a * b; }
                );
            }

            return result;
        }

        template<
                std::size_t Width = W, std::size_t Height = H,
                std::enable_if_t<(Width == Height), bool> = true>
        Self &transpose() {
            if (W != H) {
                throw std::runtime_error{"Cannot transpose non-square matrix"};
            }

            for (std::size_t i = 0; i < W; ++i) {
                for (std::size_t j = i + 1; j < H; ++j) {
                    std::swap(get(i, j), get(j, i));
                }
            }

            return *this;
        }

        Self &operator*=(Self const &other) {
            *this = *this * other;
            return *this;
        }

        template<
                std::size_t Width = W, std::size_t Height = H,
                std::enable_if_t<(Width == Height && Width == 4), bool> = true>
        Self &scale(Vec<El, Width - 1> const &scale_by) {
            Self scale_matrix{};

            for (std::size_t i = 0; i < W - 1; ++i) {
                scale_matrix.values_[i * W + i] = scale_by[i];
            }

            *this *= scale_matrix;

            return *this;
        }

        template<
                std::size_t Width = W, std::size_t Height = H,
                std::enable_if_t<(Width == Height && Width == 4), bool> = true>
        Self &translate(Vec<El, Width - 1> const &translate_by) {
            Self translation_matrix{};

            auto last_col = translation_matrix.get_col(3).begin();
            last_col[0]   = translate_by[0];
            last_col[1]   = translate_by[1];
            last_col[2]   = translate_by[2];

            *this *= translation_matrix;

            return *this;
        }

        template<
                std::size_t Width = W, std::size_t Height = H,
                std::enable_if_t<(Width == Height && Width == 4), bool> = true>
        Self &rotate(Quaternion const &quat) {
            Self rotation_matrix{
                    // row 1
                    1.f - 2.f * square(quat.y_) - 2.f * square(quat.z_),
                    2.f * quat.x_ * quat.y_ - 2.f * quat.z_ * quat.w_,
                    2.f * quat.x_ * quat.z_ + 2.f * quat.y_ * quat.w_, 0.f,
                    // row 2
                    2.f * quat.x_ * quat.y_ + 2.f * quat.z_ * quat.w_,
                    1.f - 2.f * square(quat.x_) - 2.f * square(quat.z_),
                    2.f * quat.y_ * quat.z_ - 2.f * quat.x_ * quat.w_, 0.f,
                    // row 3
                    2.f * quat.x_ * quat.z_ - 2.f * quat.y_ * quat.w_,
                    2.f * quat.y_ * quat.z_ + 2.f * quat.x_ * quat.w_,
                    1.f - 2.f * square(quat.x_) - 2.f * square(quat.y_), 0.f,
                    // row 4
                    0.f, 0.f, 0.f, 1.f
            };

            *this *= rotation_matrix;

            return *this;
        }

    private:
        Data values_{};
    };

    template<typename El = float, std::size_t S = 4>
    using SquareMatrix = Matrix<El, S, S>;

    consteval bool matrix_get() {
        constexpr std::array      data{1.f, 2.f, 3.f};
        Matrix<float, 3, 1> const matrix{data};

        auto const res = matrix.get_row(0);
        for (std::size_t i = 0; i < 3; ++i) {
            if (res[i] != data[i]) {
                return false;
            }
        }

        return true;
    }

    static_assert(matrix_get());

    consteval bool matrix_get_vec() {
        constexpr Vec3            a{1.f, 2.f, 3.f};
        Matrix<float, 3, 1> const matrix{a};

        return matrix.get_vec(0) == a;
    }

    static_assert(matrix_get_vec());

    consteval bool matrix_from_vecs() {
        Vec3 const a{1.f, 0.f, 0.f};
        Vec3 const b{0.f, 1.f, 0.f};
        Vec3 const c{0.f, 0.f, 1.f};

        Matrix<float, 3, 3> projection_matrix{a, b, c};

        if (projection_matrix.get_vec(0) != a)
            return false;
        if (projection_matrix.get_vec(1) != b)
            return false;
        if (projection_matrix.get_vec(2) != c)
            return false;

        return true;
    }

    static_assert(matrix_from_vecs());

    consteval bool square_matrix_default_init_identity() {
        SquareMatrix<float, 3> const matrix{};

        if (matrix.get_vec(0) != Vec3{1.f, 0.f, 0.f})
            return false;
        if (matrix.get_vec(1) != Vec3{0.f, 1.f, 0.f})
            return false;
        if (matrix.get_vec(2) != Vec3{0.f, 0.f, 1.f})
            return false;

        return true;
    }

    static_assert(square_matrix_default_init_identity());
}// namespace engine::math

#endif//MATRIX_H
