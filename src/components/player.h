#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

#include "commands/move_command.h"
#include "components/component.h"
#include "input/mouse_keyboard_input.h"

namespace engine {
    class Entity;

    class Player final
        : public Component<Player>
        , public Updatable {
        Entity                                  *entity_ptr_;
        GameObject                               camera_gameobject_;
        std::vector<UniqueKeyboardCommandHandle> keyboard_commands_{};
        std::vector<UniqueMouseCommandHandle>    mouse_commands_{};
        float                                    cam_pitch_{}, cam_yaw_{};
        float                                    rot_speed_{9.f};

        void orient();

    public:
        explicit Player(entt::registry &registry);

        // We need to explicitly remind the compiler of this because for some reason std::vector pretends to be copyable no matter if the element is. :)
        Player(Player &&)            = default;
        Player &operator=(Player &&) = default;

        void update() override;

        void rotate(float delta_x, float delta_y);
    };
}// namespace engine

#endif//PLAYER_H
