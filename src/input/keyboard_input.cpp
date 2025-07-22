#include "keyboard_input.h"

namespace engine {
    void KeyboardCommandHandleDestroyer::operator()(CommandHandle command
    ) const {
        if (!command.keyboard_input_service_ptr_) {
            return;
        }

        auto      &service    = *command.keyboard_input_service_ptr_;
        auto const key        = command.key_;
        auto const event_type = command.event_type_;

        service.remove_command(key, event_type);
    }
}// namespace engine
