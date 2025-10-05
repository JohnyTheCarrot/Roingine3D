#include "mouse_keyboard_input.h"

#include <algorithm>

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

    void KeyboardMouseInputService::process_input() {
        std::ranges::for_each(key_states_, [this](auto const &pair) {
            auto const &[key, state]                            = pair;
            auto const &[previous_key_state, current_key_state] = state;

            if (previous_key_state && !current_key_state) {
                execute_command(key, KeyEventType::Up);
                return;
            }

            if (current_key_state) {
                execute_command(key, KeyEventType::Down);
            }
        });
    }

    UniqueKeyboardCommandHandle KeyboardMouseInputService::add_command(
            InputKey key, KeyEventType event_type,
            std::unique_ptr<Command<>> command
    ) {
        keyboard_commands_.emplace(
                std::make_pair(key, event_type), std::move(command)
        );

        return UniqueKeyboardCommandHandle{
                KeyboardMouseCommandHandle{*this, key, event_type}
        };
    }

    UniqueMouseCommandHandle KeyboardMouseInputService::add_command(
            std::unique_ptr<MouseMoveCommand> command
    ) {
        move_mouse_commands_.emplace(
                ++move_mouse_current_id_, std::move(command)
        );

        return UniqueMouseCommandHandle{MouseCommandHandle{
                *this, move_mouse_current_id_, MouseAction::Move
        }};
    }

    void KeyboardMouseInputService::remove_command(
            InputKey key, KeyEventType event_type
    ) {
        auto const range = keyboard_commands_.equal_range({key, event_type});
        if (range.first == range.second) {
            return;// No command found
        }

        keyboard_commands_.erase(range.first, range.second);
    }

    void KeyboardMouseInputService::remove_command(
            MouseCommandId id, MouseAction action
    ) {
        if (action == MouseAction::Move) {
            move_mouse_commands_.erase(id);
            return;
        }

        mouse_commands_.erase({id, action});
    }

    void KeyboardMouseInputService::execute_command(
            InputKey key, KeyEventType event_type
    ) const {
        auto const [first, last] =
                keyboard_commands_.equal_range({key, event_type});

        std::for_each(first, last, [](auto const &pair) {
            auto const &[_, command] = pair;
            command->execute();
        });
    }

    void
    KeyboardMouseInputService::execute_command(int delta_x, int delta_y) const {
        auto const move_mouse_command_it =
                move_mouse_commands_.find(move_mouse_current_id_);

        if (move_mouse_command_it == move_mouse_commands_.end()) {
            return;
        }

        auto const &move_mouse_command = move_mouse_command_it->second;

        move_mouse_command->execute(delta_x, delta_y);
    }

    void KeyboardMouseInputService::change_key_state(
            InputKey key, bool is_key_down
    ) {
        auto const [current_state, did_already_exist] = key_states_.try_emplace(
                key,
                KeyState{
                        .previous_key_state = false, .current_key_state = false
                }
        );

        auto &[previous_key_state, current_key_state] = current_state->second;

        previous_key_state = current_key_state;
        current_key_state  = is_key_down;
    }

    KeyboardMouseInputService::IsKeyDown
    KeyboardMouseInputService::get_key_state(InputKey key) const {
        auto const it = key_states_.find(key);
        if (it == key_states_.end()) {
            return false;// Key not found
        }

        return it->second.current_key_state;
    }
}// namespace engine
