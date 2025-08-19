#ifndef GAMEOBJECT_H
#define GAMEOBJECT_H

#include <entt/entity/registry.hpp>
#include <unordered_set>

namespace engine {
    class Scene;

    class Camera;

    namespace {
        struct Relationship final {
            entt::entity                     parent{entt::null};
            std::unordered_set<entt::entity> children{};
        };
    }// namespace

    class GameObject final {
        entt::registry *registry_;
        entt::entity    entity_;

        [[nodiscard]]
        bool has_camera() const;

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

            if constexpr (std::is_same_v<T, Camera>) {
                if (has_camera()) {
                    throw std::runtime_error(
                            "The engine only supports one camera at the moment."
                    );
                }
            }

            return registry_->emplace<T>(
                    entity_, *registry_, std::forward<Args>(args)...
            );
        }

        void set_parent(GameObject const &new_parent) const;

        [[nodiscard]]
        GameObject *get_parent() const;

        template<class T>
        [[nodiscard]]
        T *get_optional_component() const {
            if (!has_component<T>())
                return nullptr;

            return &registry_->get<T>(entity_);
        }

        template<class T, class... Args>
            requires std::constructible_from<T, entt::registry &, Args...>
        T &get_or_add_component(Args &&...args) {
            if (auto *comp_ptr = get_optional_component<T>())
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
        T &get_component() {
            return registry_->get<T>(entity_);
        }

        template<class T>
        [[nodiscard]]
        T const &get_component() const {
            return registry_->get<T>(entity_);
        }

        [[nodiscard]]
        GameObject add_child() const;

        [[nodiscard]]
        Scene const &get_scene() const;

        [[nodiscard]]
        Scene &get_scene();
    };
}// namespace engine

#endif//GAMEOBJECT_H
