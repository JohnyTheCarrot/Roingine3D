#ifndef GLFW_WINDOW
#define GLFW_WINDOW

#include <GLFW/glfw3.h>

#include "presentation/window.h"

namespace engine::core {
    struct GLFWwindowDestroyer {
        void operator()(GLFWwindow *ptr) const;
    };

    class Window::Impl final {
        std::string                                      title_;
        int                                              width_;
        int                                              height_;
        std::unique_ptr<GLFWwindow, GLFWwindowDestroyer> window_ptr_;
        bool closure_requested_{false};

        void query_size();

    public:
        explicit Impl(bootstrap_info::WindowSettings settings);

        [[nodiscard]]
        bool closure_requested() const;

        void close();

        void update();

        [[nodiscard]]
        NativeWindowHandle get_native_handle() const;

        [[nodiscard]]
        NativeDisplayType get_native_display_type() const;

        [[nodiscard]]
        Resolution get_size() const;
    };
}// namespace engine::core

#endif// !GLFW_WINDOW
