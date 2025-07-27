#include "application.h"

#include <iostream>

#include "misc/service_locator.h"

namespace engine {
    class KeyboardMouseInputService;

    Scene &Application::set_active_scene(Scene &&scene) {
        scene_ = std::move(scene);

        return scene_.value();
    }

    void Application::clear_active_scene() {
        scene_.reset();
    }

    float Application::get_delta_time() const {
        return 0.0f;// Placeholder for delta time calculation
    }

    Scene &Application::get_active_scene() {
        return scene_.value();
    }

    Scene const &Application::get_active_scene() const {
        return scene_.value();
    }
}// namespace engine
