#ifndef CPP_ENGINE_GAME_HOST_H
#define CPP_ENGINE_GAME_HOST_H

#include <bgfx/bgfx.h>
#include <functional>
#include <memory>

#include "game_bootstrap_info.h"

namespace engine::core {
    class GameHost final {
        class Impl;
        std::unique_ptr<Impl> impl_ptr_;

    public:
        GameHost(bootstrap_info::Info info, std::function<void()> main_loop);

        ~GameHost();

        void enter_main_loop() const;

        void init_platform_data(bgfx::PlatformData &platform_data) const;

        [[nodiscard]]
        Resolution get_render_resolution() const;
    };
}// namespace engine::core

#endif//CPP_ENGINE_GAME_HOST_H
