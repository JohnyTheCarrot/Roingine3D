#include "gameobject.h"

namespace engine {
    GameObject::GameObject(entt::registry &registry)
        : registry_{&registry}
        , entity_{registry.create()} {
    }

    GameObject::GameObject(entt::registry &registry, entt::entity entity)
        : registry_{&registry}
        , entity_{entity} {
    }
}// namespace engine
