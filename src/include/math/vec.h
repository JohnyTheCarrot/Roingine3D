#ifndef VEC3_H
#define VEC3_H

#include <algorithm>
#include <array>
#include <cmath>
#include <functional>
#include <iostream>
#include <numeric>
#include <ranges>
#include <type_traits>

namespace engine::math {
    template<typename T, std::size_t N>
        requires std::is_arithmetic_v<T> && (N > 1)
    class Vec final {
        using Data = std::array<T, N>;
        Data data_{};

    public:
        Vec() = default;

        template<typename... Args>
            requires std::disjunction_v<std::is_convertible<Args, T>...>
        constexpr explicit Vec(Args... args)
            : data_{args...} {
            static_assert(
                    sizeof...(args) == N,
                    "Number of arguments must match the size of the vector."
            );
        }

        template<std::size_t Count = N>
            requires(Count > 2)
        constexpr Vec(Vec<T, N - 1> const &other, T last) {
            std::copy(other.cbegin(), other.cend(), data_.begin());
            data_[N - 1] = last;
        }

        constexpr explicit Vec(std::array<T, N> &&data)
            : data_{std::move(data)} {
        }

        constexpr explicit Vec(std::span<T const, N> data) {
            std::copy(data.cbegin(), data.cend(), data_.begin());
        }

        [[nodiscard]]
        constexpr typename Data::const_iterator cbegin() const noexcept {
            return data_.cbegin();
        }

        [[nodiscard]]
        constexpr typename Data::const_iterator cend() const noexcept {
            return data_.cend();
        }

        [[nodiscard]]
        constexpr Data const &get_data() const {
            return data_;
        }

        template<std::size_t I>
            requires(I < N)
        [[nodiscard]]
        T &get() const {
            return data_[I];
        }

        [[nodiscard]]
        T &operator[](std::size_t index) {
            if (index >= N) {
                throw std::out_of_range{"Index out of range"};
            }
            return data_[index];
        }

        [[nodiscard]]
        T operator[](std::size_t index) const {
            if (index >= N) {
                throw std::out_of_range{"Index out of range"};
            }
            return data_[index];
        }

        [[nodiscard]]
        T get_x() const {
            return data_[0];
        }

        void set_x(T x) {
            data_[0] = x;
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 2), bool> = true>
        [[nodiscard]]
        T get_y() const {
            return data_[1];
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 2), bool> = true>
        void set_y(T y) {
            data_[1] = y;
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 3), bool> = true>
        [[nodiscard]]
        Vec<T, 2> get_xy() const {
            return Vec<T, 2>{data_[0], data_[1]};
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 3), bool> = true>
        [[nodiscard]]
        T get_z() const {
            return data_[2];
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 3), bool> = true>
        void set_z(T z) {
            data_[2] = z;
        }

        template<std::size_t M = N, std::enable_if_t<(M >= 4), bool> = true>
        [[nodiscard]]
        Vec<T, 3> get_xyz() const {
            return Vec<T, 3>{data_[0], data_[1], data_[2]};
        }

        template<std::size_t M = N, std::enable_if_t<(M > 4)> = true>
        [[nodiscard]]
        T get_w() const {
            return data_[3];
        }

        template<typename Op>
            requires std::invocable<Op, T const &, T const &>
        [[nodiscard]]
        Vec apply(Op const &op, Vec const &other) const {
            auto const       zip = std::views::zip(data_, other.data_);
            std::array<T, N> data_arr{};

            std::transform(
                    zip.cbegin(), zip.cend(), data_arr.begin(),
                    [&](auto zipped) {
                        return op(std::get<0>(zipped), std::get<1>(zipped));
                    }
            );

            return Vec{std::move(data_arr)};
        }

        template<typename Op>
            requires std::invocable<Op, T const &, T const &>
        [[nodiscard]]
        Vec apply(Op const &op, T const &scalar) const {
            std::array<T, N> data_arr{};

            std::transform(
                    data_.cbegin(), data_.cend(), data_arr.begin(),
                    std::bind(op, std::placeholders::_1, scalar)
            );
            return Vec{std::move(data_arr)};
        }

        [[nodiscard]]
        Vec operator+(Vec const &other) const {
            return apply(std::plus<T>{}, other);
        }

        Vec &operator+=(Vec const &other) {
            *this = *this + other;
            return *this;
        }

        [[nodiscard]]
        Vec operator-() const {
            Vec clone{*this};
            for (auto &element : clone.data_) { element = -element; }

            return clone;
        }

        [[nodiscard]]
        Vec operator-(Vec const &other) const {
            return apply(std::minus<T>{}, other);
        }

        Vec &operator-=(Vec const &other) {
            *this = *this - other;
            return *this;
        }

        [[nodiscard]]
        Vec operator*(T scalar) const {
            return apply(std::multiplies<T>{}, scalar);
        }

        Vec &operator*=(T scalar) {
            *this = *this * scalar;
            return *this;
        }

        [[nodiscard]]
        Vec operator/(T scalar) const {
            return Vec{*this} * (T{1} / scalar);
        }

        Vec &operator/=(T scalar) {
            *this = *this / scalar;
            return *this;
        }

        [[nodiscard]]
        auto operator<=>(Vec const &other) const
            requires std::three_way_comparable<T>
        {
            return std::lexicographical_compare_three_way(
                    data_.cbegin(), data_.cend(), other.data_.cbegin(),
                    other.data_.cend()
            );
        }

        [[nodiscard]]
        bool operator==(Vec const &other) const = default;

        [[nodiscard]]
        T dot(Vec const &other) const {
            return std::inner_product(
                    data_.begin(), data_.end(), other.data_.begin(), T{0},
                    std::plus<T>{}, std::multiplies<T>{}
            );
        }

        template<std::size_t M = N, std::enable_if_t<M == 2, bool> = true>
        Vec det(Vec const &other) const {
            return get_x() * other.get_y() - get_y() * other.get_x();
        }

        template<std::size_t M = N, std::enable_if_t<M == 3, bool> = true>
        [[nodiscard]]
        Vec cross(Vec const &other) const {
            return Vec{
                    get_y() * other.get_z() - get_z() * other.get_y(),
                    get_z() * other.get_x() - get_x() * other.get_z(),
                    get_x() * other.get_y() - get_y() * other.get_x()
            };
        }

        template<std::size_t M = N, std::enable_if_t<M == 3, bool> = true>
        [[nodiscard]]
        Vec mul(std::span<T const, 4 * 4> mat) {
            Vec result{};

            result.set_x(
                    get_x() * mat[0] + get_y() * mat[4] + get_z() * mat[8] +
                    mat[12]
            );
            result.set_y(
                    get_x() * mat[1] + get_y() * mat[5] + get_z() * mat[9] +
                    mat[13]
            );
            result.set_z(
                    get_x() * mat[2] + get_y() * mat[6] + get_z() * mat[10] +
                    mat[14]
            );

            return result;
        }

        template<
                typename U                                          = T,
                std::enable_if_t<std::is_floating_point_v<U>, bool> = true>
        [[nodiscard]]
        U get_magnitude_squared() const {
            auto const squared =
                    data_ | std::views::transform([](auto element) {
                        return element * element;
                    });

            return std::accumulate(
                    squared.cbegin(), squared.cend(), T{0}, std::plus<T>{}
            );
        }

        template<
                typename U                                          = T,
                std::enable_if_t<std::is_floating_point_v<U>, bool> = true>
        [[nodiscard]]
        U get_magnitude() const {
            return std::sqrt(get_magnitude_squared());
        }

        template<
                typename U                                          = T,
                std::enable_if_t<std::is_floating_point_v<U>, bool> = true>
        void normalize() {
            *this /= get_magnitude();
        }

        [[nodiscard]]
        Vec normalized() const {
            Vec clone{*this};
            clone.normalize();
            return clone;
        }

        template<std::size_t M = N, std::enable_if_t<M == 3, bool> = true>
        void rotate(Vec const &by) {
            float const sx = sin(by.get_x());
            float const cx = cos(by.get_x());
            float const sy = sin(by.get_y());
            float const cy = cos(by.get_y());
            float const sz = sin(by.get_z());
            float const cz = cos(by.get_z());

            std::array<T, 16> const rotation_matrix{
                    cy * cz,
                    -cy * sz,
                    sy,
                    T{0},
                    cz * sx * sy + cx * sz,
                    cx * cz - sx * sy * sz,
                    -cx * sx,
                    T{0},
                    -cx * cz * sy + sx * sz,
                    cz * sx + cx * sy * sz,
                    cx * cy,
                    T{1}
            };

            *this = mul(rotation_matrix);
        }

        friend std::ostream &operator<<(std::ostream &os, Vec const &vec) {
            os << "Vec{";
            for (std::size_t i = 0; i < N; ++i) {
                os << vec.data_[i];
                if (i < N - 1) {
                    os << ", ";
                }
            }
            os << "}";
            return os;
        }

        friend Vec operator/(T lhs, Vec const &rhs) {
            Vec result{rhs};
            result /= T{1} / lhs;

            return result;
        }
    };

    template<typename T>
    using Vec2T = Vec<T, 2>;

    template<typename T>
    using Vec3T = Vec<T, 3>;

    using Vec2f = Vec<float, 2>;
    using Vec3f = Vec<float, 3>;
    using Vec4f = Vec<float, 4>;

    using Vec2 = Vec2f;
    using Vec3 = Vec3f;
    using Vec4 = Vec4f;
}// namespace engine::math

#endif//VEC3_H
