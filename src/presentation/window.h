#ifndef CPP_ENGINE_WINDOW_H
#define CPP_ENGINE_WINDOW_H

#include <memory>

#include "game_bootstrap_info.h"

namespace engine::core {
    class Window final {
        class Impl;
        std::unique_ptr<Impl> impl_ptr_;

    public:
        using NativeWindowHandle = void *;
        using NativeDisplayType  = void *;

        explicit Window(bootstrap_info::WindowSettings settings);

        [[nodiscard]]
        bool closure_requested() const;

        void close() const;

        void update() const;

        [[nodiscard]]
        NativeWindowHandle get_native_handle() const;

        [[nodiscard]]
        NativeDisplayType get_native_display_type() const;

        [[nodiscard]]
        Resolution get_size() const;
    };
}// namespace engine::core

#endif//CPP_ENGINE_WINDOW_H
