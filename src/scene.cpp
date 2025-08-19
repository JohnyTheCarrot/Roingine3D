#include "scene.h"

#include "components/camera.h"
#include "components/mesh_renderer.h"
#include "game/components/player.h"

namespace engine {
    Scene::Scene() {
        registry_->ctx().emplace<Scene *>(this);
    };

    Scene::Scene(Scene &&other) noexcept
        : registry_{std::move(other.registry_)}
        , texture_store_{std::move(other.texture_store_)} {
        auto *&ptr = registry_->ctx().get<Scene *>();
        ptr        = this;
    }

    Scene &Scene::operator=(Scene &&other) noexcept {
        if (this == &other)
            return *this;

        registry_      = std::move(other.registry_);
        texture_store_ = std::move(other.texture_store_);

        auto *&ptr = registry_->ctx().get<Scene *>();
        ptr        = this;

        return *this;
    }

    GameObject Scene::create_game_object() {
        return {*registry_, registry_->create()};
    }

    void Scene::update() const {
        Camera::update_of_type();
    }

    void Scene::render(Game const &game) const {
        registry_->view<Camera>().each([&](auto const &camera) {
            camera.render(game);
        });
    }
}// namespace engine
