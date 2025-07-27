#ifndef CAMERA_H
#define CAMERA_H

#include "component.h"
#include "transform.h"

namespace engine {
    class Game;

    class Camera final
        : public Component<Camera>
        , public Updatable {
        Transform const                 *transform_ptr_;
        mutable std::array<float, 4 * 4> view_mat_{};
        float                            pitch_{}, yaw_{-90.f};

    public:
        explicit Camera(entt::registry &registry);

        void render(Game const &game) const;

        void update() override;
    };
}// namespace engine

#endif//CAMERA_H
