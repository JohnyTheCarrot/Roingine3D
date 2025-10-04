#ifndef CPP_ENGINE_GAME_HOST_H
#define CPP_ENGINE_GAME_HOST_H

#include <bgfx/bgfx.h>
#include <functional>
#include <memory>

#include "game_bootstrap_info.h"

namespace engine::core {
    class GameHost final {
    public:
        class Impl;

        GameHost(
                bootstrap_info::Info info, std::function<void()> main_loop,
                std::function<void()> shut_down
        );

        ~GameHost();

        void enter_main_loop() const;

        void init_bgfx(bgfx::Init &init) const;

        [[nodiscard]]
        Resolution get_render_resolution() const;

    private:
        std::unique_ptr<Impl> impl_ptr_;
    };
}// namespace engine::core

#endif//CPP_ENGINE_GAME_HOST_H
