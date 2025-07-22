#include "scene.h"

#include "components/camera.h"
#include "components/mesh_renderer.h"
#include "game/components/player.h"

namespace engine {
    Scene::Scene() = default;

    GameObject Scene::create_game_object() {
        return {*registry_, registry_->create()};
    }

    void Scene::update() const {
        Camera::update_of_type(*registry_);
        game::Player::update_of_type(*registry_);
    }

    void Scene::render() const {
        registry_->view<Camera>().each([](auto const &camera) {
            camera.render();
        });
    }
}// namespace engine
