#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

#include "components/component.h"
#include "game/commands/move_command.h"
#include "input/mouse_keyboard_input.h"

namespace game {
    class Entity;

    class Player final
        : public engine::Component<Player>
        , public engine::Updatable {
        Entity                                          *entity_ptr_;
        engine::GameObject                               camera_gameobject_;
        std::vector<engine::UniqueKeyboardCommandHandle> keyboard_commands_{};
        std::vector<engine::UniqueMouseCommandHandle>    mouse_commands_{};
        float cam_pitch_{}, cam_yaw_{-90.f};
        float rot_speed_{9.f};

        void orient();

    public:
        explicit Player(entt::registry &registry);

        // We need to explicitly remind the compiler of this because for some reason std::vector pretends to be copyable no matter if the element is. :)
        Player(Player &&)            = default;
        Player &operator=(Player &&) = default;

        void update() override;

        void rotate(float delta_x, float delta_y);
    };
}// namespace game

#endif//PLAYER_H
