#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/registry.hpp>

#include "gameobject.h"

namespace engine {
    class Game;

    class Scene final {
        std::unique_ptr<entt::registry> registry_{
                std::make_unique<entt::registry>()
        };

        friend class Engine;

        void update() const;

        void render(Game const &game) const;

    public:
        Scene();

        [[nodiscard]]
        GameObject create_game_object();

        // TODO: figure out how to hide this from the public API
        [[nodiscard]]
        entt::registry &get_registry() {
            return *registry_;
        }
    };
}// namespace engine

#endif//SCENE_H
