#ifndef DIRTY_H
#define DIRTY_H

#include <algorithm>

namespace engine {
    template<typename T>
    class Dirty final {
        T            value_{};
        mutable bool dirty_{false};

    public:
        template<typename... Args>
            requires std::constructible_from<T, Args...>
        explicit Dirty(Args &&...args)
            : value_{std::forward<Args>(args)...}
            , dirty_{true} {
        }

        explicit Dirty(T value = T{})
            : value_{value}
            , dirty_{true} {
        }

        [[nodiscard]]
        T const &get() const {
            return value_;
        }

        [[nodiscard]]
        T &operator*() {
            return value_;
        }

        [[nodiscard]]
        T const &operator*() const {
            return value_;
        }

        T *operator->() {
            return &value_;
        }

        T const *operator->() const {
            return &value_;
        }

        // TODO: add concept
        T &operator+=(T const &other) {
            set(value_ + other);
            return value_;
        }

        void set(T &&new_value) {
            if (value_ != new_value) {
                value_ = std::move(new_value);
                dirty_ = true;
            }
        }

        T &operator=(T const &new_value) {
            set(new_value);
            return value_;
        }

        T &operator=(T &&new_value) {
            set(std::move(new_value));
            return value_;
        }

        void set(T const &new_value) {
            if (value_ != new_value) {
                value_ = new_value;
                dirty_ = true;
            }
        }

        [[nodiscard]]
        bool is_dirty() const {
            return dirty_;
        }

        void clear_dirty() const {
            dirty_ = false;
        }
    };
}// namespace engine

#endif//DIRTY_H
