#ifndef CAMERA_H
#define CAMERA_H

#include <bx/math.h>
#include <optional>

#include "component.h"
#include "transform.h"

namespace engine {
    class Camera final
        : public Component<Camera>
        , public Updatable {
        Transform const        *transform_ptr_;
        bx::Vec3                look_direction_{0.0f, 0.0f, -1.0f};
        std::optional<bx::Vec3> look_at_vec_override_{};

    public:
        explicit Camera(entt::registry &registry);

        void render() const;

        void update() override;

        [[nodiscard]]
        bx::Vec3 get_look_at() const;
    };
}// namespace engine

#endif//CAMERA_H
