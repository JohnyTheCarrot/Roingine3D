#include <bgfx/bgfx.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <print>

#ifdef __linux__
#define GLFW_EXPOSE_NATIVE_WAYLAND
#elif __WIN64
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

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
        std::println("Error: failed initializing glfw");
        fatal();
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    std::unique_ptr<GLFWwindow, GLFWwindowDestroyer> window{glfwCreateWindow(
            c_Width, c_Height, "Hello, bgfx!", nullptr, nullptr
    )};

    if (!window) {
        std::println("Error: failed creating window");
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
        return 1;
    }

    constexpr bgfx::ViewId clear_view = 0;
    bgfx::setViewClear(clear_view, BGFX_CLEAR_COLOR);
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

        // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
        bgfx::touch(clear_view);

        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 0, 0x0f, "le pog");

        bgfx::Stats const *stats = bgfx::getStats();
        bgfx::dbgTextPrintf(
                0, 2, 0x0f,
                "Backbuffer %dW x %dH in pixels, debug text %dW x %dH in "
                "characters.",
                stats->width, stats->height, stats->textWidth, stats->textHeight
        );

        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::frame();
    }

    bgfx::shutdown();
    glfwTerminate();

    return 0;
}
