#include "glfw_window.h"

#include <bx/platform.h>

#include "application.h"
#include "glfw_incl.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "platform_specific/host/glfw/glfw_input.h"
#include "presentation/game_host.h"
#include "presentation/window.h"

namespace engine::core {
    void GLFWwindowDestroyer::operator()(GLFWwindow *ptr) const {
        glfwDestroyWindow(ptr);
    }

    Window::Impl::Impl(bootstrap_info::WindowSettings settings)
        : title_{std::move(settings.title)}
        , width_{settings.width}
        , height_{settings.height}
        , window_ptr_{[&] {
#ifdef BUILD_FOR_X11
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#else
            glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
#endif
            if (!glfwInit()) {
                throw std::runtime_error("Error: failed initializing glfw");
            }
            glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
            glfwWindowHint(
                    GLFW_MAXIMIZED, settings.maximized ? GLFW_TRUE : GLFW_FALSE
            );

            return glfwCreateWindow(
                    width_, height_, title_.c_str(), nullptr, nullptr
            );
        }()} {
        if (!window_ptr_) {
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwSetInputMode(window_ptr_.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        ServiceLocator<KeyboardMouseInputService>::Provide(
                std::make_unique<GLFWInputService>(*window_ptr_)
        );
    }

    Window::NativeWindowHandle Window::Impl::get_native_handle() const {
#ifdef BX_PLATFORM_LINUX
#    ifdef BUILD_FOR_X11
        return reinterpret_cast<void *>(glfwGetX11Window(window_ptr));
#    else
        return glfwGetWaylandWindow(window_ptr_.get());
#    endif
#elifdef BX_PLATFORM_OSX
        return glfwGetCocoaWindow(window_ptr);
#elifdef BX_PLATFORM_WINDOWS
        return glfwGetWin32Window(window_ptr);
#endif
    }

    Window::NativeDisplayType Window::Impl::get_native_display_type() const {
#ifdef BX_PLATFORM_LINUX
#    ifndef BUILD_FOR_X11
        return glfwGetWaylandDisplay();
#    else
        return glfwGetX11Display();
#    endif
#elifdef BX_PLATFORM_OSX
        return glfwGetCocoaWindow(window_ptr);
#elifdef BX_PLATFORM_WINDOWS
        return glfwGetWin32Window(window_ptr);
#endif
    }

    [[nodiscard]]
    bool Window::Impl::closure_requested() const {
        return closure_requested_ | glfwWindowShouldClose(window_ptr_.get());
    }

    void Window::Impl::close() {
        closure_requested_ = true;
    }

    void Window::Impl::query_size() {
        auto &app = Application::get_instance();
        int   old_width{app.get_width()}, old_height{app.get_height()};
        app.update_size([this](int &w, int &h) {
            glfwGetWindowSize(window_ptr_.get(), &w, &h);
        });
    }

    void Window::Impl::update() {
        glfwPollEvents();
        query_size();
    }

    Resolution Window::Impl::get_size() const {
        return {width_, height_};
    }

    Window::Window(bootstrap_info::WindowSettings settings)
        : impl_ptr_{std::make_unique<Impl>(std::move(settings))} {
    }

    void Window::close() const {
        impl_ptr_->close();
    }

    bool Window::closure_requested() const {
        return impl_ptr_->closure_requested();
    }

    void Window::update() const {
        impl_ptr_->update();
    }

    Window::NativeWindowHandle Window::get_native_handle() const {
        return impl_ptr_->get_native_handle();
    }

    Window::NativeDisplayType Window::get_native_display_type() const {
        return impl_ptr_->get_native_display_type();
    }

    Resolution Window::get_size() const {
        return impl_ptr_->get_size();
    }
}// namespace engine::core
