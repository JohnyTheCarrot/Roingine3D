#ifndef COMMAND_H
#define COMMAND_H

namespace engine {
    class KeyboardInputService;

    class Command {
    public:
        virtual ~Command() = default;
        virtual void execute() const {};
    };
}// namespace engine

#endif//COMMAND_H
