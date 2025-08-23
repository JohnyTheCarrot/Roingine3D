#include "engine.h"

#ifdef __linux__
#ifdef BUILD_FOR_X11
#define GLFW_EXPOSE_NATIVE_X11
#else
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif
#elif __WIN64
#define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <bgfx/bgfx.h>
#include <chrono>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <iostream>

#include "application.h"
#include "bx/timer.h"
#include "input/glfw_input.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "types.h"

namespace engine {
    constexpr bgfx::ViewId clear_view = 0;

    void init_platform_data(
            GLFWwindow *window_ptr, bgfx::PlatformData &platform_data
    ) {
#ifdef __linux__
#ifndef BUILD_FOR_X11
        platform_data.ndt  = glfwGetWaylandDisplay();
        platform_data.nwh  = glfwGetWaylandWindow(window_ptr);
        platform_data.type = bgfx::NativeWindowHandleType::Wayland;
        std::cout << "Using Wayland display\n";
#else
        platform_data.ndt = glfwGetX11Display();
        platform_data.nwh =
                reinterpret_cast<void *>(glfwGetX11Window(window_ptr));
        platform_data.type = bgfx::NativeWindowHandleType::Default;
        std::cout << "Using X11 display\n";
#endif
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

    class Engine::Impl final {
        std::unique_ptr<Game>                            game_ptr_;
        std::string                                      title_;
        std::unique_ptr<GLFWwindow, GLFWwindowDestroyer> window_ptr_;
        bool                                             running_{true};

    public:
        explicit Impl(
                std::unique_ptr<Game> game, std::string title, int width,
                int height
        )
            : game_ptr_{std::move(game)}
            , title_{std::move(title)}
            , window_ptr_{[&] {
#ifdef BUILD_FOR_X11
                glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_X11);
#else
                glfwInitHint(GLFW_PLATFORM, GLFW_PLATFORM_WAYLAND);
#endif
                if (!glfwInit()) {
                    std::cerr << "Error: failed initializing glfw\n";
                    exit(EXIT_FAILURE);
                }
                glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
                glfwWindowHint(GLFW_MAXIMIZED, GLFW_TRUE);

                return glfwCreateWindow(
                        width, height, title_.c_str(), nullptr, nullptr
                );
            }()} {
            if (!window_ptr_) {
                throw std::runtime_error("Failed to create GLFW window");
            }
            glfwSetInputMode(
                    window_ptr_.get(), GLFW_CURSOR, GLFW_CURSOR_DISABLED
            );
            ServiceLocator<KeyboardMouseInputService>::Provide(
                    std::make_unique<GLFWInputService>(*window_ptr_)
            );

            init_engine();
        }

        void init_engine() {
            bgfx::Init init{};
            init_platform_data(window_ptr_.get(), init.platformData);
            int width{}, height{};
            glfwGetWindowSize(window_ptr_.get(), &width, &height);
            Application::get_instance().update_size([=](int &w, int &h) {
                w = width;
                h = height;
            });

            init.resolution.width  = static_cast<uint32_t>(width);
            init.resolution.height = static_cast<uint32_t>(height);
            init.resolution.reset  = BGFX_RESET_VSYNC;

            if (!bgfx::init(init)) {
                throw std::runtime_error("Failed to initialize bgfx");
            }

            constexpr auto clear_color = 0x264B56FF;
            // constexpr auto clear_color = 0x000000FF;// Black
            bgfx::setViewClear(
                    0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clear_color, 1.0f, 0
            );
            bgfx::setViewRect(clear_view, 0, 0, bgfx::BackbufferRatio::Equal);
            Vertex::setup_layout();
            game_ptr_->setup();
        }

        void main_loop() {
            auto &app = Application::get_instance();

            using Clock = std::chrono::high_resolution_clock;

            auto last_time = Clock::now();

            while (running_) {
                auto current_time = Clock::now();
                auto delta_time =
                        std::chrono::duration<float>(current_time - last_time)
                                .count();
                app.delta_time_ = delta_time;
                last_time       = current_time;

                glfwPollEvents();
                if (glfwWindowShouldClose(window_ptr_.get())) {
                    running_ = false;
                }

                int old_width{app.get_width()}, old_height{app.get_height()};
                Application::get_instance().update_size([this](int &w, int &h) {
                    glfwGetWindowSize(window_ptr_.get(), &w, &h);
                });
                if (app.get_width() != old_width ||
                    app.get_height() != old_height) {
                    bgfx::reset(
                            static_cast<uint32_t>(app.get_width()),
                            static_cast<uint32_t>(app.get_height()),
                            BGFX_RESET_VSYNC
                    );
                    bgfx::setViewRect(
                            clear_view, 0, 0, bgfx::BackbufferRatio::Equal
                    );
                }

                // This dummy draw call is here to make sure that view 0 is cleared if no other draw calls are submitted to view 0.
                bgfx::touch(clear_view);
                bgfx::dbgTextClear();

                bgfx::setDebug(BGFX_DEBUG_TEXT);

                ServiceLocator<KeyboardMouseInputService>::Get()
                        .process_input();

                if (app.has_active_scene()) {
                    game_ptr_->update();
                    auto &scene = app.get_active_scene();
                    scene.update();
                    scene.render(*game_ptr_);
                }

                bgfx::frame();
            }
        }
    };

    Engine::Engine(
            std::unique_ptr<Game> game, std::string title, int width, int height
    )
        : impl_ptr_{std::make_unique<Impl>(
                  std::move(game), std::move(title), width, height
          )} {
    }

    Engine::~Engine() {
        TextureStore::get_instance().clear();
        Application::get_instance().clear_active_scene();
        bgfx::shutdown();
        glfwTerminate();
    }

    void Engine::main_loop() const {
        impl_ptr_->main_loop();
    }
}// namespace engine
