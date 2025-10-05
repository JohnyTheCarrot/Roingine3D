#ifndef EMSCRIPTEN_INPUT_H
#define EMSCRIPTEN_INPUT_H

#include <emscripten/html5.h>

#include "input/mouse_keyboard_input.h"

namespace engine::core {
    class EmscriptenInputService final : public KeyboardMouseInputService {
        using WasEventConsumed = bool;
        static constexpr WasEventConsumed event_not_consumed{false};
        static constexpr WasEventConsumed event_consumed{true};

        MoveMouseCommandMap move_mouse_commands_;
        MouseCommandId      current_mouse_move_id{};

        static WasEventConsumed mouse_move_callback(
                int event_type, EmscriptenMouseEvent const *mouse_event,
                void *user_data
        );

        static WasEventConsumed pointer_lock_change_callback(
                int /* EmMouseEventType */              event_type,
                EmscriptenPointerlockChangeEvent const *event, void *user_data
        );

        static WasEventConsumed key_event_callback(
                int event_type, EmscriptenKeyboardEvent const *event,
                void *user_data
        );

        static void request_pointer_lock();

        void mouse_move_handler(EmscriptenMouseEvent const *mouse_event);

        enum class KeyEventType { Down, Up };

        void key_event_handler(
                KeyEventType event_type, EmscriptenKeyboardEvent const *event
        );

        void execute_mouse_move_commands(int delta_x, int delta_y) const;

    public:
        EmscriptenInputService();

        ~EmscriptenInputService() override;
    };
}// namespace engine::core

#endif
