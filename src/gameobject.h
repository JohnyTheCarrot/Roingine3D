#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <entt/entity/registry.hpp>

namespace engine {
    class GameObject final {
        entt::registry *registry_;
        entt::entity    entity_;

    public:
        explicit GameObject(entt::registry &registry);

        GameObject(entt::registry &registry, entt::entity entity);

        template<class T, class... Args>
            requires std::constructible_from<T, entt::registry &, Args...>
        T &add_component(Args &&...args) {
            if (has_component<T>())
                throw std::runtime_error(
                        "GameObject already has component of type " +
                        std::string{typeid(T).name()}
                );

            return registry_->emplace<T>(
                    entity_, *registry_, std::forward<Args>(args)...
            );
        }

        template<class T>
        [[nodiscard]]
        T *get_component() {
            if (!has_component<T>())
                return nullptr;

            return &registry_->get<T>(entity_);
        }

        template<class T, class... Args>
            requires std::constructible_from<T, entt::registry &, Args...>
        T &get_or_add_component(Args &&...args) {
            if (auto *comp_ptr = get_component<T>())
                return *comp_ptr;

            return add_component<T>(std::forward<Args>(args)...);
        }

        template<class T, class... Args>
            requires std::constructible_from<T, entt::registry &, Args...>
        T const &get_or_add_component(Args &&...args) const {
            return get_or_add_component<T>(std::forward<Args>(args)...);
        }

        template<class T>
        [[nodiscard]]
        bool has_component() const {
            return registry_->storage<T>().contains(entity_);
        }

        template<class T>
        [[nodiscard]]
        T const &get_component() const {
            return registry_->get<T>(entity_);
        }
    };
}// namespace engine

#endif//GAMEOBJECT_H
