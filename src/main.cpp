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
#include <bx/math.h>
#include <bx/timer.h>
#include <GLFW/glfw3native.h>
#include <iostream>

#include "game.h"
#include "setup.h"
#include "types.h"

constexpr int c_Width{640};
constexpr int c_Height{480};

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
    std::unique_ptr<GLFWwindow, GLFWwindowDestroyer> window{
            glfwCreateWindow(c_Width, c_Height, "poggers", nullptr, nullptr)
    };

    if (!window) {
        std::cerr << "Error: failed creating window\n";
        fatal();
    }

    bgfx::Init init{};
    init_platform_data(window.get(), init.platformData);
    int width{}, height{};
    glfwGetWindowSize(window.get(), &width, &height);
    init.resolution.width  = static_cast<uint32_t>(width);
    init.resolution.height = static_cast<uint32_t>(height);
    init.resolution.reset  = BGFX_RESET_VSYNC;

    if (!bgfx::init(init)) {
        fatal();
    }

    engine::Vertex::setup_layout();
    game::setup();

    constexpr bgfx::ViewId clear_view = 0;
    bgfx::setViewClear(
            0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x264B56FF, 1.0f, 0
    );
    bgfx::setViewRect(clear_view, 0, 0, bgfx::BackbufferRatio::Equal);

    while (!glfwWindowShouldClose(window.get())) {
        glfwPollEvents();

        int old_width{width}, old_height{height};
        glfwGetWindowSize(window.get(), &width, &height);

        if (width != old_width || height != old_height) {
            bgfx::reset(
                    static_cast<uint32_t>(width), static_cast<uint32_t>(height),
                    BGFX_RESET_VSYNC
            );
            bgfx::setViewRect(clear_view, 0, 0, bgfx::BackbufferRatio::Equal);
        }

        bx::Vec3        forward = {0.0f, 0.0f, 1.0f};
        static bx::Vec3 eye     = {0.0f, 0.0f, -15.0f};
        if (glfwGetKey(window.get(), GLFW_KEY_W) == GLFW_PRESS) {
            eye.x += forward.x;
            eye.y += forward.y;
            eye.z += forward.z;
        }

        if (glfwGetKey(window.get(), GLFW_KEY_S) == GLFW_PRESS) {
            eye.x -= forward.x;
            eye.y -= forward.y;
            eye.z -= forward.z;
        }

        if (glfwGetKey(window.get(), GLFW_KEY_A) == GLFW_PRESS) {
            bx::Vec3 left = {forward.z, 0.0f, -forward.x};
            eye.x += left.x;
            eye.y += left.y;
            eye.z += left.z;
        }

        if (glfwGetKey(window.get(), GLFW_KEY_D) == GLFW_PRESS) {
            bx::Vec3 right = {-forward.z, 0.0f, forward.x};
            eye.x += right.x;
            eye.y += right.y;
            eye.z += right.z;
        }

        float          view[16];
        bx::Vec3 const at = {0.0f, 0.0f, 0.0f};
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(
                proj, 60.0f,
                static_cast<float>(width) / static_cast<float>(height), 0.1f,
                100.0f, bgfx::getCaps()->homogeneousDepth
        );
        bgfx::setViewTransform(0, view, proj);

        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
        bgfx::touch(clear_view);

        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "le pog");

        bgfx::setDebug(BGFX_DEBUG_TEXT);

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
