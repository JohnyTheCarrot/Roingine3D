#ifndef CAM_ADJUST_COMMAND_H
#define CAM_ADJUST_COMMAND_H

#include "input/mouse_keyboard_input.h"

namespace game {
    class Player;

    class CameraAdjustCommand final : public engine::MouseMoveCommand {
        Player *player_ptr_;

    public:
        explicit CameraAdjustCommand(Player &player_ptr);

        void execute(int delta_x, int delta_y) const override;
    };
}// namespace game

#endif//CAM_ADJUST_COMMAND_H
