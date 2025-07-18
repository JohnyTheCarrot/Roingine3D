#ifndef SCENE_H
#define SCENE_H

#include <entt/entity/registry.hpp>

#include "gameobject.h"

namespace engine {
    class Scene final {
        entt::registry registry_{};

    public:
        Scene();

        [[nodiscard]]
        GameObject create_game_object();

        void update() const;

        void render() const;
    };
}// namespace engine

#endif//SCENE_H
