#include <bx/platform.h>
#include <chrono>
#include <iostream>
#include <memory>

#include "application.h"
#include "bgfx/bgfx.h"
#include "constants.h"
#include "glfw_incl.h"
#include "glfw_window.h"
#include "presentation/game_host.h"
#include "types.h"

namespace engine::core {
    class GameHost::Impl final {
        std::unique_ptr<Window> window_ptr_;
        std::function<void()>   main_loop_{};

    public:
        Impl(bootstrap_info::Info info, std::function<void()> main_loop)
            : window_ptr_{std::make_unique<Window>(
                      std::move(info.window_settings)
              )}
            , main_loop_{std::move(main_loop)} {
        }

        ~Impl() {
            glfwTerminate();
        }

        void enter_main_loop() const {
            using Clock = std::chrono::high_resolution_clock;

            auto &app = Application::get_instance();

            auto last_time = Clock::now();
            while (!app.is_shutdown_requested()) {

                auto current_time = Clock::now();
                auto delta_time =
                        std::chrono::duration<float>(current_time - last_time)
                                .count();
                last_time = current_time;
                app.set_delta_time(delta_time);

                auto old_width{app.get_width()};
                auto old_height{app.get_height()};
                window_ptr_->update();
                if (window_ptr_->closure_requested()) {
                    app.request_shutdown();
                }

                if (app.get_width() != old_width ||
                    app.get_height() != old_height) {
                    bgfx::reset(
                            static_cast<uint32_t>(app.get_width()),
                            static_cast<uint32_t>(app.get_height()),
                            BGFX_RESET_VSYNC
                    );
                    bgfx::setViewRect(
                            constants::clear_view, 0, 0,
                            bgfx::BackbufferRatio::Equal
                    );
                }

                main_loop_();
            }
        }

        void init_platform_data(bgfx::PlatformData &platform_data) const {
            platform_data.ndt = window_ptr_->get_native_display_type();
            platform_data.nwh = window_ptr_->get_native_handle();
#ifdef BX_PLATFORM_LINUX
#    ifndef BUILD_FOR_X11
            platform_data.type = bgfx::NativeWindowHandleType::Wayland;
            std::cout << "Using Wayland display\n";
#    else
            platform_data.type = bgfx::NaNativeWindowHandleType::Default;
            std::cout << "Using X11 display\n";
#    endif
#endif
        }

        Resolution get_render_resolution() const {
            return window_ptr_->get_size();
        }
    };

    void GameHost::enter_main_loop() const {
        impl_ptr_->enter_main_loop();
    }

    GameHost::GameHost(
            bootstrap_info::Info info, std::function<void()> main_loop
    )
        : impl_ptr_{
                  std::make_unique<Impl>(std::move(info), std::move(main_loop))
          } {
    }

    GameHost::~GameHost() = default;

    void GameHost::init_platform_data(bgfx::PlatformData &platform_data) const {
        impl_ptr_->init_platform_data(platform_data);
    }

    Resolution GameHost::get_render_resolution() const {
        return impl_ptr_->get_render_resolution();
    }
}// namespace engine::core
