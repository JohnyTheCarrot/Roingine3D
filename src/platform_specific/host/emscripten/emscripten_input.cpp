#include "emscripten_input.h"

#include <iostream>

#include "emscripten_constants.h"

namespace engine::core {
    constexpr bool c_event_use_capture{true};
    std::unordered_map<std::string_view, InputKey> const c_key_map{
            {"A", InputKey::A},
            {"a", InputKey::A},
            {"B", InputKey::B},
            {"b", InputKey::B},
            {"C", InputKey::C},
            {"c", InputKey::C},
            {"D", InputKey::D},
            {"d", InputKey::D},
            {"E", InputKey::E},
            {"e", InputKey::E},
            {"F", InputKey::F},
            {"f", InputKey::F},
            {"G", InputKey::G},
            {"g", InputKey::G},
            {"H", InputKey::H},
            {"h", InputKey::H},
            {"I", InputKey::I},
            {"i", InputKey::I},
            {"J", InputKey::J},
            {"j", InputKey::J},
            {"K", InputKey::K},
            {"k", InputKey::K},
            {"L", InputKey::L},
            {"l", InputKey::L},
            {"M", InputKey::M},
            {"m", InputKey::M},
            {"N", InputKey::N},
            {"n", InputKey::N},
            {"O", InputKey::O},
            {"o", InputKey::O},
            {"P", InputKey::P},
            {"p", InputKey::P},
            {"Q", InputKey::Q},
            {"q", InputKey::Q},
            {"R", InputKey::R},
            {"r", InputKey::R},
            {"S", InputKey::S},
            {"s", InputKey::S},
            {"T", InputKey::T},
            {"t", InputKey::T},
            {"U", InputKey::U},
            {"u", InputKey::U},
            {"V", InputKey::V},
            {"v", InputKey::V},
            {"W", InputKey::W},
            {"w", InputKey::W},
            {"X", InputKey::X},
            {"x", InputKey::X},
            {"Y", InputKey::Y},
            {"y", InputKey::Y},
            {"Z", InputKey::Z},
            {"z", InputKey::Z},

            {" ", InputKey::Space},
            {"Escape", InputKey::Escape},
            {"Shift", InputKey::LeftShift},
            {"Control", InputKey::LeftCtrl},
            {"Enter", InputKey::Enter},

            {"F1", InputKey::F1},
            {"F2", InputKey::F2},
            {"F3", InputKey::F3},
            {"F4", InputKey::F4},
            {"F5", InputKey::F5},
            {"F6", InputKey::F6},
            {"F7", InputKey::F7},
            {"F8", InputKey::F8},
            {"F9", InputKey::F9},
            {"F10", InputKey::F10},
            {"F11", InputKey::F11},
            {"F12", InputKey::F12},
    };

    EmscriptenInputService::WasEventConsumed
    EmscriptenInputService::mouse_move_callback(
            int, EmscriptenMouseEvent const *mouse_event, void *user_data
    ) {
        auto *const input_service =
                reinterpret_cast<EmscriptenInputService *>(user_data);
        input_service->mouse_move_handler(mouse_event);

        return event_consumed;
    }

    EmscriptenInputService::WasEventConsumed
    EmscriptenInputService::pointer_lock_change_callback(
            int /* EmMouseEventType */,
            EmscriptenPointerlockChangeEvent const *event, void *
    ) {
        std::cout << "TODO: pointer_lock_change_callback" << std::endl;

        return event_consumed;
    }

    EmscriptenInputService::WasEventConsumed
    EmscriptenInputService::key_event_callback(
            int event_type, EmscriptenKeyboardEvent const *event,
            void *user_data
    ) {
        auto *const input_service =
                reinterpret_cast<EmscriptenInputService *>(user_data);
        auto key_event_type = event_type == EMSCRIPTEN_EVENT_KEYDOWN
                                    ? KeyEventType::Down
                                    : KeyEventType::Up;

        input_service->key_event_handler(key_event_type, event);

        return event_consumed;
    }

    void EmscriptenInputService::request_pointer_lock() {
        emscripten_request_pointerlock(constants::canvas_target.data(), true);
    }

    void EmscriptenInputService::mouse_move_handler(
            EmscriptenMouseEvent const *mouse_event
    ) {
        auto const delta_x = mouse_event->movementX;
        auto const delta_y = mouse_event->movementY;

        execute_command(delta_x, delta_y);
    }

    void EmscriptenInputService::key_event_handler(
            KeyEventType event_type, EmscriptenKeyboardEvent const *event
    ) {
        auto const input_key_it = c_key_map.find(event->key);
        if (input_key_it == c_key_map.end()) {
            return;// Key not mapped
        }

        auto const input_key   = input_key_it->second;
        auto       is_key_down = event_type == KeyEventType::Down;

        change_key_state(input_key, is_key_down);
    }

    EmscriptenInputService::EmscriptenInputService()
        : KeyboardMouseInputService{} {
        request_pointer_lock();

        emscripten_set_keydown_callback(
                constants::canvas_target.data(), this, c_event_use_capture,
                &key_event_callback
        );
        emscripten_set_keyup_callback(
                constants::canvas_target.data(), this, c_event_use_capture,
                &key_event_callback
        );

        emscripten_set_mousemove_callback(
                constants::canvas_target.data(), this, c_event_use_capture,
                &EmscriptenInputService::mouse_move_callback
        );
        emscripten_set_pointerlockchange_callback(
                EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, c_event_use_capture,
                &EmscriptenInputService::pointer_lock_change_callback
        );
    }

    EmscriptenInputService::~EmscriptenInputService() {
        // unregister keyboard callbacks
        emscripten_set_keydown_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_keyup_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );

        // unregister mouse callbacks
        emscripten_set_click_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_mousedown_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_mouseup_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_dblclick_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_mousemove_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_mouseenter_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_mouseleave_callback(
                constants::canvas_target.data(), nullptr, c_event_use_capture,
                nullptr
        );
        emscripten_set_pointerlockchange_callback(
                EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, c_event_use_capture,
                nullptr
        );
    }
}// namespace engine::core
