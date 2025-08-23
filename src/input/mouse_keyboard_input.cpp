#include "mouse_keyboard_input.h"

namespace engine {
    void KeyboardCommandHandleDestroyer::operator()(
            KeyboardMouseCommandHandle command
    ) const {
        if (!command.keyboard_mouse_input_service_ptr_) {
            return;
        }

        auto      &service    = *command.keyboard_mouse_input_service_ptr_;
        auto const key        = command.key_;
        auto const event_type = command.event_type_;

        service.remove_command(key, event_type);
    }

    void
    MouseCommandHandleDestroyer::operator()(MouseCommandHandle command) const {
        if (!command.keyboard_mouse_input_service_ptr_) {
            return;
        }

        auto &service = *command.keyboard_mouse_input_service_ptr_;

        if (command.action_ == MouseAction::Move) {}
        service.remove_command(command.id_, command.action_);
    }
}// namespace engine
