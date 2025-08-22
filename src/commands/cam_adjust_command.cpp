#include "cam_adjust_command.h"

#include "../components/player.h"

namespace engine {
    CameraAdjustCommand::CameraAdjustCommand(Player &player_ptr)
        : player_ptr_{&player_ptr} {
    }

    void CameraAdjustCommand::execute(int delta_x, int delta_y) const {
        player_ptr_->rotate(
                static_cast<float>(delta_x), static_cast<float>(delta_y)
        );
    }
}// namespace engine
