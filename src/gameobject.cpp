#include "gameobject.h"

#include "components/camera.h"

namespace engine {
    bool GameObject::has_camera() const {
        return !registry_->view<Camera>().empty();
    }

    GameObject::GameObject(entt::registry &registry)
        : registry_{&registry}
        , entity_{registry.create()} {
    }

    GameObject::GameObject(entt::registry &registry, entt::entity entity)
        : registry_{&registry}
        , entity_{entity} {
    }
}// namespace engine
