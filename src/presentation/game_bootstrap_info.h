#ifndef CPP_ENGINE_GAME_BOOTSTRAP_INFO_H
#define CPP_ENGINE_GAME_BOOTSTRAP_INFO_H

#include <string>

namespace engine::core {
    struct Resolution final {
        int width;
        int height;
    };

    namespace bootstrap_info {
        struct WindowSettings final {
            std::string title;
            int         width;
            int         height;
            bool        maximized;
        };

        struct Info final {
            WindowSettings window_settings = {"My Game", 800, 600};
        };
    }// namespace bootstrap_info
}// namespace engine::core

#endif//CPP_ENGINE_GAME_BOOTSTRAP_INFO_H
