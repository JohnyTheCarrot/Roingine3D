#ifndef COMPONENT_H
#define COMPONENT_H

#include <entt/entity/helper.hpp>
#include <entt/entity/registry.hpp>

#include "application.h"
#include "gameobject.h"

namespace engine {
    class Updatable {
    public:
        virtual ~Updatable()  = default;
        virtual void update() = 0;
    };

    class Renderable {
    public:
        virtual ~Renderable()       = default;
        virtual void render() const = 0;
    };

    template<class Comp>
    concept IsUpdatable = std::derived_from<Comp, Updatable>;

    class GameObject;

    template<class Derived>
    class Component {
        entt::registry *registry_;

    protected:
        [[nodiscard]]
        entt::registry const &get_registry() const {
            return *registry_;
        }

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

        template<class D = Derived>
            requires IsUpdatable<D>
        static void update_of_type() {
            auto &scene = Application::get_instance().get_active_scene();

            scene.get_registry().view<Derived>().each([](Derived &component) {
                component.update();
            });
        }
    };
}// namespace engine

#endif//COMPONENT_H
