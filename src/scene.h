#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/registry.hpp>

#include "gameobject.h"

namespace engine {
    class Scene final {
        std::unique_ptr<entt::registry> registry_{
                std::make_unique<entt::registry>()
        };

    public:
        Scene();

        [[nodiscard]]
        GameObject create_game_object();

        void update() const;

        void render() const;
    };
}// namespace engine

#endif//SCENE_H
