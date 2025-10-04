#include <bgfx/bgfx.h>
#include <chrono>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>

#include "application.h"
#include "constants.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "presentation/game_bootstrap_info.h"
#include "presentation/game_host.h"

namespace engine::core {
    void emscripten_call_loop(void *impl);

    class GameHost::Impl final {
        std::function<void()>             main_loop_{};
        std::function<void()>             shut_down_{};
        bool                              running_{true};
        static constexpr std::string_view canvas_ref = "#canvas";

    public:
        Impl(bootstrap_info::Info info, std::function<void()> main_loop,
             std::function<void()> shut_down)
            : main_loop_{std::move(main_loop)} {
            emscripten_set_canvas_element_size(
                    canvas_ref.data(), info.window_settings.width,
                    info.window_settings.height
            );
        }

        void init_bgfx(bgfx::Init &init) const {
            init.type             = bgfx::RendererType::OpenGLES;
            init.platformData.nwh = (void *) "#canvas";
        }

        void frame_requested() {
            using Clock = std::chrono::high_resolution_clock;

            auto &app = Application::get_instance();
            app.set_delta_time(0.1f);

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
            int               width, height;
            EMSCRIPTEN_RESULT result = emscripten_get_canvas_element_size(
                    canvas_ref.data(), &width, &height
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
        bool get_key_state(InputKey key) const {
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
                std::make_unique<NullInputService>()
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
