#ifndef COMPONENT_H
#define COMPONENT_H

#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>

#include "gameobject.h"

namespace engine {
    class GameObject;

    template<class Derived>
    class Component {
        entt::registry *registry_;

    public:
        explicit Component(entt::registry &registry)
            : registry_(&registry) {
        }

        [[nodiscard]]
        GameObject get_gameobject() const {
            return {*registry_, entt::to_entity(
                                        registry_->storage<Derived>(),
                                        *static_cast<Derived const *>(this)
                                )};
        }
    };
}// namespace engine

#endif//COMPONENT_H
