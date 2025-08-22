#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "components/entity.h"
#include "commands/command.h"

namespace engine {
    class Entity;

    class MoveEntityCommand final : public Command<> {
        MovementDirection direction_;
        Entity           *entity_ptr_;

    public:
        MoveEntityCommand(Entity &entity, MovementDirection direction);

        void execute() const override;
    };
}// namespace engine

#endif//MOVE_COMMAND_H
