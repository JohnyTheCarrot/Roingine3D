#include "gameobject.h"

#include "components/camera.h"
#include "scene.h"

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

    void GameObject::set_parent(GameObject const &new_parent) const {
        if (this == &new_parent) {
            throw std::runtime_error("Cannot set self as parent.");
        }

        auto &rel_parent =
                registry_->get_or_emplace<Relationship>(entity_, Relationship{})
                        .parent;
        if (rel_parent != entt::null) {
            // don't forget to remove this entity from the parent's children
            registry_->get<Relationship>(rel_parent).children.erase(entity_);
        }

        rel_parent = new_parent.entity_;

        auto &parent_children =
                registry_
                        ->get_or_emplace<Relationship>(
                                new_parent.entity_, Relationship{}
                        )
                        .children;
        parent_children.emplace(entity_);
    }

    GameObject *GameObject::get_parent() const {
        if (!registry_->storage<Relationship>().contains(entity_))
            return nullptr;

        auto const &relationship = registry_->get<Relationship>(entity_);
        if (relationship.parent == entt::null) {
            return nullptr;
        }

        return new GameObject{*registry_, relationship.parent};
    }

    GameObject GameObject::add_child() const {
        GameObject const child{*registry_, registry_->create()};

        child.set_parent(*this);

        return child;
    }

    Scene const &GameObject::get_scene() const {
        return *registry_->ctx().get<Scene *>();
    }

    Scene &GameObject::get_scene() {
        return *registry_->ctx().get<Scene *>();
    }
}// namespace engine
