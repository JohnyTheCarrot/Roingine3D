#ifndef GAME_H
#define GAME_H

#include <optional>

#include "misc/singleton.h"
#include "scene.h"

namespace engine {
    class Game final : public Singleton<Game> {
        std::optional<Scene> scene_;

    public:
        Scene &set_active_scene(Scene &&scene);

        void clear_active_scene();

        [[nodiscard]]
        float get_delta_time() const;

        [[nodiscard]]
        bool has_active_scene() const {
            return scene_.has_value();
        }

        [[nodiscard]]
        Scene &get_active_scene();

        [[nodiscard]]
        Scene const &get_active_scene() const;
    };
}// namespace engine

#endif//GAME_H
