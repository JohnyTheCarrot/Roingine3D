#ifndef UNIQUE_HANDLE_H
#define UNIQUE_HANDLE_H

#include <compare>
#include <concepts>
#include <type_traits>
#include <utility>

namespace engine {
    template<typename T, T TNull = T{}, typename TDeleter = void(T const &)>
    class UniqueHandle final {
        T    contents_{TNull};
        bool valid_{false};

        void destroy_contents() {
            if (!valid_)
                return;

            TDeleter{}(contents_);
            contents_ = TNull;
            valid_    = false;
        }

    public:
        using handle = T;

        UniqueHandle() = default;

        explicit UniqueHandle(T &&contents)
            : contents_{std::move(contents)}
            , valid_{true} {
        }

        ~UniqueHandle() {
            destroy_contents();

            // if TDeleter is a no-op (i.e. no custom destruction is required), worry not: regular destruction will occur
        }

        UniqueHandle(UniqueHandle const &) = delete;

        UniqueHandle &operator=(UniqueHandle const &) = delete;

        UniqueHandle(UniqueHandle &&other) noexcept
            : contents_{std::move(other.contents_)}
            , valid_{true} {
            other.contents_ = TNull;
            other.valid_    = false;
        }

        UniqueHandle &operator=(UniqueHandle &&other) noexcept {
            if (this == &other)
                return *this;

            contents_ = std::move(other.contents_);
            valid_    = true;

            other.contents_ = TNull;

            return *this;
        }

        template<typename TReset>
            requires std::constructible_from<T, TReset>
        void reset(TReset &&new_contents) {
            destroy_contents();
            contents_ = std::forward<TReset>(new_contents);
            valid_    = true;
        }

        void reset() {
            destroy_contents();
        }

        T &get() {
            return contents_;
        }

        T const &get() const {
            return contents_;
        }

        T *operator->() {
            return &contents_;
        }

        T const *operator->() const {
            return &contents_;
        }

        template<
                typename U                                          = T,
                std::enable_if_t<std::equality_comparable<U>, bool> = true>
        explicit operator bool() const {
            return contents_ != TNull;
        }

        template<
                typename U                                          = T,
                std::enable_if_t<std::equality_comparable<U>, bool> = true>
        bool operator!() const {
            return contents_ == TNull;
        }

        template<typename T2, T2 T2Null, typename T2Deleter>
            requires std::three_way_comparable_with<T, T2>
        [[nodiscard]]
        std::compare_three_way_result_t<T, T2>
        operator<=>(UniqueHandle<T2, T2Null, T2Deleter> const &other) const {
            return contents_ <=> other.get();
        }
    };
}// namespace engine

#endif//UNIQUE_HANDLE_H
