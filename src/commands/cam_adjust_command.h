#ifndef CAM_ADJUST_COMMAND_H
#define CAM_ADJUST_COMMAND_H

#include "input/mouse_keyboard_input.h"

namespace engine {
    class Player;

    class CameraAdjustCommand final : public MouseMoveCommand {
        Player *player_ptr_;

    public:
        explicit CameraAdjustCommand(Player &player_ptr);

        void execute(int delta_x, int delta_y) const override;
    };
}// namespace engine

#endif//CAM_ADJUST_COMMAND_H
