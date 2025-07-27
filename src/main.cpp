#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <memory>

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif __WIN64
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>
#include <iostream>

#include "game.h"
#include "game/setup.h"
#include "input/glfw_input.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "types.h"

constexpr int c_Width{800};
constexpr int c_Height{600};

void init_platform_data(
        GLFWwindow *window_ptr, bgfx::PlatformData &platform_data
) {
#ifdef __linux__
    platform_data.ndt  = glfwGetWaylandDisplay();
    platform_data.nwh  = glfwGetWaylandWindow(window_ptr);
    platform_data.type = bgfx::NativeWindowHandleType::Wayland;
#elifdef __APPLE__
    platform_data.nwh = glfwGetCocoaWindow(window_ptr);
#elifdef __WIN64
    platform_data.nwh = glfwGetWin32Window(window_ptr);
#endif
}

struct GLFWwindowDestroyer {
    void operator()(GLFWwindow *ptr) const {
        glfwDestroyWindow(ptr);
    }
};

[[noreturn]]
void fatal() {
    exit(EXIT_FAILURE);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Error: failed initializing glfw\n";
        fatal();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    std::unique_ptr<GLFWwindow, GLFWwindowDestroyer> window_ptr{
            glfwCreateWindow(c_Width, c_Height, "eh bobba e", nullptr, nullptr)
    };

    if (!window_ptr) {
        std::cerr << "Error: failed creating window\n";
        fatal();
    }

    bgfx::Init init{};
    init_platform_data(window_ptr.get(), init.platformData);
    int window_width{}, window_height{};
    glfwGetWindowSize(window_ptr.get(), &window_width, &window_height);
    init.resolution.width  = static_cast<uint32_t>(window_width);
    init.resolution.height = static_cast<uint32_t>(window_height);
    init.resolution.reset  = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        fatal();
    }

    engine::ServiceLocator<engine::KeyboardMouseInputService>::Provide(
            std::make_unique<engine::GLFWInputService>(*window_ptr)
    );

    engine::Vertex::setup_layout();
    game::setup();

    constexpr bgfx::ViewId clear_view = 0;
    // auto const clear_color = 0x264B56FF;
    auto const clear_color = 0x000000FF;// Black
    bgfx::setViewClear(
            0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clear_color, 1.0f, 0
    );
    bgfx::setViewRect(clear_view, 0, 0, bgfx::BackbufferRatio::Equal);
    glfwSetInputMode(window_ptr.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    while (!glfwWindowShouldClose(window_ptr.get())) {
        glfwPollEvents();

        int old_width{window_width}, old_height{window_height};
        engine::Game::get_instance().update_size([&](int &width, int &height) {
            glfwGetWindowSize(window_ptr.get(), &width, &height);
            window_width  = width;
            window_height = height;
        });

        if (window_width != old_width || window_height != old_height) {
            bgfx::reset(
                    static_cast<uint32_t>(window_width),
                    static_cast<uint32_t>(window_height), BGFX_RESET_VSYNC
            );
            bgfx::setViewRect(clear_view, 0, 0, bgfx::BackbufferRatio::Equal);
        }

        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
        bgfx::touch(clear_view);

        bgfx::dbgTextClear();

        bgfx::setDebug(BGFX_DEBUG_TEXT);

        engine::ServiceLocator<engine::KeyboardMouseInputService>::Get()
                .process_input();
        if (engine::Game &game = engine::Game::get_instance();
            game.has_active_scene()) {
            auto &scene = game.get_active_scene();
            scene.update();
            scene.render();
        }

        bgfx::frame();
    }

    engine::Game::get_instance().clear_active_scene();
    bgfx::shutdown();
    glfwTerminate();

    return 0;
}
