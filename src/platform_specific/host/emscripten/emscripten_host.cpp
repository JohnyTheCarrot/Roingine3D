#include <bgfx/bgfx.h>
#include <chrono>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <functional>
#include <memory>

#include "application.h"
#include "constants.h"
#include "emscripten_constants.h"
#include "emscripten_input.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "presentation/game_bootstrap_info.h"
#include "presentation/game_host.h"

namespace engine::core {
    void emscripten_call_loop(void *impl);

    class GameHost::Impl final {
        std::function<void()> main_loop_{};
        std::function<void()> shut_down_{};
        float                 last_time{};
        Resolution            full_screen_size_{};
        bool                  running_{true};
        bool                  is_fullscreen_{false};

        static bool on_fullscreen_change(
                int, EmscriptenFullscreenChangeEvent const *event,
                void *impl_void_ptr
        ) {
            auto *impl_ptr           = reinterpret_cast<Impl *>(impl_void_ptr);
            impl_ptr->is_fullscreen_ = event->isFullscreen;
            impl_ptr->full_screen_size_.width  = event->screenWidth;
            impl_ptr->full_screen_size_.height = event->screenHeight;

            return true;
        }


    public:
        Impl(bootstrap_info::Info info, std::function<void()> main_loop,
             std::function<void()> shut_down)
            : main_loop_{std::move(main_loop)} {
            emscripten_set_canvas_element_size(
                    constants::canvas_target.data(), info.window_settings.width,
                    info.window_settings.height
            );
            emscripten_set_fullscreenchange_callback(
                    constants::canvas_target.data(), this, false,
                    &on_fullscreen_change
            );
        }

        ~Impl() {
            emscripten_set_fullscreenchange_callback(
                    constants::canvas_target.data(), nullptr, false, nullptr
            );
        }

        void init_bgfx(bgfx::Init &init) const {
            init.type             = bgfx::RendererType::OpenGLES;
            init.platformData.nwh = (void *) constants::canvas_target.data();
        }

        void frame_requested() {
            auto now   = emscripten_get_now() / 1000.;
            auto delta = now - last_time;
            last_time  = now;

            auto &app = Application::get_instance();
            app.set_delta_time(delta);

            auto old_width{app.get_width()};
            auto old_height{app.get_height()};

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
            if (!app.is_shutdown_requested())
                return;

            shut_down_();
        }

        void enter_main_loop() const {
            emscripten_set_main_loop_arg(
                    &emscripten_call_loop, (void *) this, 0, false
            );
        }

        [[nodiscard]]
        Resolution get_render_resolution() const {
            if (is_fullscreen_) {
                return full_screen_size_;
            }

            int width, height;
            emscripten_get_canvas_element_size(
                    constants::canvas_target.data(), &width, &height
            );

            return {width, height};
        }
    };

    void emscripten_call_loop(void *impl_ptr) {
        auto impl = (GameHost::Impl *) (impl_ptr);
        impl->frame_requested();
    }

    class NullInputService final : public KeyboardMouseInputService {
    public:
        void process_input() {
        }

        [[nodiscard]]
        UniqueKeyboardCommandHandle add_command(
                InputKey key, KeyEventType event_type,
                std::unique_ptr<Command<>> command
        ) {
            return UniqueKeyboardCommandHandle{KeyboardMouseCommandHandle{}};
        }

        [[nodiscard]]
        UniqueMouseCommandHandle
        add_command(std::unique_ptr<MouseMoveCommand> command) {
            return UniqueMouseCommandHandle{MouseCommandHandle{}};
        }

        void remove_command(InputKey key, KeyEventType event_type) {
        }

        void remove_command(MouseCommandId id, MouseAction action) {
        }

        [[nodiscard]]
        KeyboardMouseInputService::IsKeyDown get_key_state(InputKey key) const {
            return false;
        }
    };

    GameHost::GameHost(
            bootstrap_info::Info info, std::function<void()> main_loop,
            std::function<void()> shut_down
    )
        : impl_ptr_{std::make_unique<GameHost::Impl>(
                  std::move(info), std::move(main_loop), std::move(shut_down)
          )} {
        ServiceLocator<KeyboardMouseInputService>::Provide(
                std::make_unique<EmscriptenInputService>()
        );
    }

    GameHost::~GameHost() = default;

    void GameHost::enter_main_loop() const {
        impl_ptr_->enter_main_loop();
    }

    void GameHost::init_bgfx(bgfx::Init &init) const {
        impl_ptr_->init_bgfx(init);
    }

    Resolution GameHost::get_render_resolution() const {
        return impl_ptr_->get_render_resolution();
    }
}// namespace engine::core
