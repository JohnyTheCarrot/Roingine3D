#include "scene.h"

#include "components/mesh_renderer.h"

namespace engine {
    Scene::Scene() = default;

    GameObject Scene::create_game_object() {
        return {registry_, registry_.create()};
    }

    void Scene::update() const {
    }

    void Scene::render() const {
        {
            auto const mesh_renderers = registry_.view<MeshRenderer>();

            mesh_renderers.each([](auto const &mesh_renderer) {
                mesh_renderer.render();
            });
        }
    }
}// namespace engine
