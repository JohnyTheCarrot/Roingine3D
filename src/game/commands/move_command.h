#ifndef MOVE_COMMAND_H
#define MOVE_COMMAND_H

#include "commands/command.h"
#include "game/components/entity.h"

namespace game {
    class Entity;

    class MoveEntityCommand final : public engine::Command<> {
        MovementDirection direction_;
        Entity           *entity_ptr_;

    public:
        MoveEntityCommand(Entity &entity, MovementDirection direction);

        void execute() const override;
    };
}// namespace game

#endif//MOVE_COMMAND_H
