#include "game.h"

namespace engine {
    Scene &Game::set_active_scene(Scene &&scene) {
        scene_ = std::move(scene);

        return scene_.value();
    }

    float Game::get_delta_time() const {
        return 0.0f;// Placeholder for delta time calculation
    }

    Scene &Game::get_active_scene() {
        return scene_.value();
    }

    Scene const &Game::get_active_scene() const {
        return scene_.value();
    }
}// namespace engine
