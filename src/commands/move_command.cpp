#include "move_command.h"

namespace engine {
    MoveEntityCommand::MoveEntityCommand(
            Entity &entity, MovementDirection direction
    )
        : direction_(direction)
        , entity_ptr_{&entity} {
    }

    void MoveEntityCommand::execute() const {
        entity_ptr_->move(direction_);
    }
}// namespace engine
