#ifndef CPP_ENGINE_SPACEDSPAN_H
#define CPP_ENGINE_SPACEDSPAN_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <numeric>

namespace engine {
    namespace {
        template<typename T, std::size_t SpaceBetween>
        constexpr std::size_t Jump = sizeof(T) + SpaceBetween;

        template<typename T>
        using DataEl = std::conditional_t<
                std::is_const_v<T>, std::uint8_t const, std::uint8_t>;

        template<typename El, std::size_t SpaceBetween>
        DataEl<El> *index_data(DataEl<El> *base, std::size_t i) {
            return base + i * Jump<El, SpaceBetween>;
        }

        template<typename El, std::size_t SpaceBetween>
        El *index(DataEl<El> *base, std::size_t i) {
            return reinterpret_cast<El *>(
                    index_data<El, SpaceBetween>(base, i)
            );
        }
    }// namespace

    template<typename T, std::size_t SpaceBetween>
    class SpacedSpanIterator final {
        using Data = DataEl<T> *;

    public:
        using iterator_category = std::random_access_iterator_tag;
        using difference_type   = std::ptrdiff_t;
        using value_type        = T;
        using reference         = T &;

        SpacedSpanIterator() = default;

        explicit SpacedSpanIterator(T *data, std::size_t size)
            : data_{reinterpret_cast<Data>(data)}
            , end_{index_data<T, SpaceBetween>(data_, size)} {
        }

        T operator*() const {
            return *reinterpret_cast<T *>(data_);
        }

        SpacedSpanIterator &operator++() {
            *this += 1;
            return *this;
        }

        [[nodiscard]]
        SpacedSpanIterator operator++(int) {
            auto tmp = *this;
            ++*this;
            return tmp;
        }

        SpacedSpanIterator &operator--() {
            *this -= 1;
            return *this;
        }

        [[nodiscard]]
        SpacedSpanIterator operator--(int) {
            auto tmp = *this;
            --*this;
            return tmp;
        }

        SpacedSpanIterator &operator+=(difference_type n) {
            data_ = std::min(index_data<T, SpaceBetween>(data_, n), end_);

            return *this;
        }

        [[nodiscard]]
        SpacedSpanIterator operator+(difference_type n) const {
            SpacedSpanIterator tmp{*this};
            tmp += n;

            return tmp;
        }

        SpacedSpanIterator &operator-=(difference_type n) {
            data_ = index<T, SpaceBetween>(data_, -n);
            return *this;
        }

        [[nodiscard]]
        SpacedSpanIterator operator-(difference_type n) const {
            SpacedSpanIterator tmp{*this};
            tmp -= n;

            return tmp;
        }

        [[nodiscard]]
        difference_type operator-(SpacedSpanIterator other) const {
            return (data_ - other.data_) / Jump<T, SpaceBetween>;
        }

        [[nodiscard]]
        auto operator<=>(SpacedSpanIterator const &) const = default;

        [[nodiscard]]
        T operator[](std::ptrdiff_t n) const {
            return *index<T const, SpaceBetween>(data_, n);
        }

        [[nodiscard]]
        reference operator[](std::ptrdiff_t n) {
            return *index<T, SpaceBetween>(data_, n);
        }

        [[nodiscard]]
        friend SpacedSpanIterator
        operator+(difference_type n, SpacedSpanIterator const &it) {
            SpacedSpanIterator tmp{it};
            tmp += n;
            return tmp;
        }

    private:
        Data data_{nullptr};
        Data end_{nullptr};
    };

    static_assert(std::input_iterator<SpacedSpanIterator<int, 1>>);
    static_assert(std::forward_iterator<SpacedSpanIterator<int, 1>>);
    static_assert(std::bidirectional_iterator<SpacedSpanIterator<int, 1>>);
    static_assert(std::random_access_iterator<SpacedSpanIterator<int, 1>>);

    template<typename T, std::size_t SpaceBetween>
    class SpacedSpan final {
        T          *data_{nullptr};
        std::size_t size_{0};

        [[nodiscard]]
        DataEl<T> *get_bytes() {
            return reinterpret_cast<DataEl<T> *>(data_);
        }

    public:
        SpacedSpan() = default;

        SpacedSpan(T *data, std::size_t size)
            : data_{data}
            , size_{size} {
        }

        [[nodiscard]]
        SpacedSpanIterator<T const, SpaceBetween> cbegin() const {
            return SpacedSpanIterator<T const, SpaceBetween>{data_, size_};
        }

        [[nodiscard]]
        SpacedSpanIterator<T const, SpaceBetween> cend() const {
            return SpacedSpanIterator<T const, SpaceBetween>{
                    index<T, SpaceBetween>(data_, size_)
            };
        }

        [[nodiscard]]
        SpacedSpanIterator<T, SpaceBetween> begin() {
            return SpacedSpanIterator<T, SpaceBetween>{data_, size_};
        }

        [[nodiscard]]
        SpacedSpanIterator<T, SpaceBetween> end() {
            return SpacedSpanIterator<T, SpaceBetween>{
                    index<T, SpaceBetween>(get_bytes(), size_), 0
            };
        }

        [[nodiscard]]
        T *get_data() {
            return data_;
        }

        [[nodiscard]]
        T const *get_data() const {
            return data_;
        }

        [[nodiscard]]
        std::size_t get_size() const {
            return size_;
        }

        /**
         * Returns the number of bytes the span refers to.
         *
         * @note The span range assumes a trailing SpaceBetween bytes are present.
         * @return The number of bytes the span refers to
         */
        [[nodiscard]]
        std::size_t get_size_bytes() const {
            return size_ * Jump<T, SpaceBetween>;
        }

        static constexpr auto c_SpaceBetween = SpaceBetween;
    };
}// namespace engine

#endif//CPP_ENGINE_SPACEDSPAN_H
