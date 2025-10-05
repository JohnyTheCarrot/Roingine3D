#include "engine.h"

#include <bgfx/bgfx.h>
#include <functional>
#include <iostream>
#include <memory>

#include "application.h"
#include "constants.h"
#include "input/mouse_keyboard_input.h"
#include "misc/service_locator.h"
#include "presentation/game_host.h"
#include "types.h"

namespace engine {
    class Engine::Impl final {
        std::unique_ptr<Game>           game_ptr_;
        std::string                     title_;
        std::unique_ptr<core::GameHost> host_;

    public:
        explicit Impl(
                std::unique_ptr<Game> game, std::string title, int width,
                int height, std::function<void()> shut_down
        )
            : game_ptr_{std::move(game)}
            , title_{std::move(title)}
            , host_{std::make_unique<core::GameHost>(
                      core::bootstrap_info::Info{
                              core::bootstrap_info::WindowSettings{
                                      std::move(title), width, height
                              }
                      },
                      [this]() { return this->main_loop(); },
                      std::move(shut_down)
              )}

        {
            init_engine();
        }

        void init_engine() {
            bgfx::Init init{};
            host_->init_bgfx(init);

            init.resolution.width  = host_->get_render_resolution().width;
            init.resolution.height = host_->get_render_resolution().height;
            init.resolution.reset  = BGFX_RESET_VSYNC;

            if (!bgfx::init(init)) {
                throw std::runtime_error("Failed to initialize bgfx");
            }

            constexpr auto clear_color = 0x264B56FF;
            // constexpr auto clear_color = 0x000000FF;// Black
            bgfx::setViewClear(
                    0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, clear_color, 1.0f, 0
            );
            bgfx::setViewRect(
                    core::constants::clear_view, 0, 0,
                    bgfx::BackbufferRatio::Equal
            );
            Vertex::setup_layout();
            game_ptr_->setup();
        }

        void main_loop() {
            auto &app = Application::get_instance();

            app.update_size([this](auto &width, auto &height) {
                auto const res = host_->get_render_resolution();
                width          = res.width;
                height         = res.height;
            });

            // This dummy draw call is here to make sure that view 0 is cleared if no
            // other draw calls are submitted to view 0.
            bgfx::touch(core::constants::clear_view);
            bgfx::dbgTextClear();

            bgfx::setDebug(BGFX_DEBUG_TEXT);

            ServiceLocator<KeyboardMouseInputService>::Get().process_input();

            if (app.has_active_scene()) {
                game_ptr_->update();
                auto &scene = app.get_active_scene();
                scene.update();
                scene.render(*game_ptr_);
            }

            bgfx::frame();
        }

        void enter_main_loop() {
            host_->enter_main_loop();
        }
    };

    Engine::Engine(
            std::unique_ptr<Game> game, std::string title, int width, int height
    )
        : impl_ptr_{new Impl{
                  std::move(game), std::move(title), width, height,
                  [this]() { this->cleanup(); }
          }} {
    }

    Engine::~Engine() = default;

    void Engine::cleanup() {
        TextureStore::get_instance().clear();
        Application::get_instance().clear_active_scene();
        bgfx::shutdown();
        delete impl_ptr_;
    }

    void Engine::enter_main_loop() const {
        impl_ptr_->enter_main_loop();
    }
}// namespace engine
