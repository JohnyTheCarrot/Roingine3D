#include "glfw_input.h"

#include <algorithm>

#include "application.h"
#include "misc/service_locator.h"

namespace engine {
    using GLFWKeyCode = int;
    std::unordered_map<GLFWKeyCode, InputKey> const c_KeyMap{
            {GLFW_KEY_A, InputKey::A},
            {GLFW_KEY_B, InputKey::B},
            {GLFW_KEY_C, InputKey::C},
            {GLFW_KEY_D, InputKey::D},
            {GLFW_KEY_E, InputKey::E},
            {GLFW_KEY_F, InputKey::F},
            {GLFW_KEY_G, InputKey::G},
            {GLFW_KEY_H, InputKey::H},
            {GLFW_KEY_I, InputKey::I},
            {GLFW_KEY_J, InputKey::J},
            {GLFW_KEY_K, InputKey::K},
            {GLFW_KEY_L, InputKey::L},
            {GLFW_KEY_M, InputKey::M},
            {GLFW_KEY_N, InputKey::N},
            {GLFW_KEY_O, InputKey::O},
            {GLFW_KEY_P, InputKey::P},
            {GLFW_KEY_Q, InputKey::Q},
            {GLFW_KEY_R, InputKey::R},
            {GLFW_KEY_S, InputKey::S},
            {GLFW_KEY_T, InputKey::T},
            {GLFW_KEY_U, InputKey::U},
            {GLFW_KEY_V, InputKey::V},
            {GLFW_KEY_W, InputKey::W},
            {GLFW_KEY_X, InputKey::X},
            {GLFW_KEY_Y, InputKey::Y},
            {GLFW_KEY_Z, InputKey::Z},
            {GLFW_KEY_SPACE, InputKey::Space},
            {GLFW_KEY_ESCAPE, InputKey::Escape},
            {GLFW_KEY_LEFT_SHIFT, InputKey::LeftShift},
            {GLFW_KEY_LEFT_CONTROL, InputKey::LeftCtrl},
            {GLFW_KEY_ENTER, InputKey::Enter},
            {GLFW_KEY_F1, InputKey::F1},
            {GLFW_KEY_F2, InputKey::F2},
            {GLFW_KEY_F3, InputKey::F3},
            {GLFW_KEY_F4, InputKey::F4},
            {GLFW_KEY_F5, InputKey::F5},
            {GLFW_KEY_F6, InputKey::F6},
            {GLFW_KEY_F7, InputKey::F7},
            {GLFW_KEY_F8, InputKey::F8},
            {GLFW_KEY_F9, InputKey::F9},
            {GLFW_KEY_F10, InputKey::F10},
            {GLFW_KEY_F11, InputKey::F11},
            {GLFW_KEY_F12, InputKey::F12},
    };

    void GLFWInputService::key_callback(
            GLFWwindow *, int key, int /*scancode*/, int action, int /*mods*/
    ) {
        auto &service = ServiceLocator<KeyboardMouseInputService>::GetSpecific<
                GLFWInputService>();

        auto const input_key_it = c_KeyMap.find(key);
        if (input_key_it == c_KeyMap.end()) {
            return;// Key not mapped
        }

        auto const input_key = input_key_it->second;
        auto const [current_state, did_already_exist] =
                service.key_states_.try_emplace(
                        input_key,
                        KeyState{
                                .previous_key_state = false,
                                .current_key_state  = false
                        }
                );

        auto &[previous_key_state, current_key_state] = current_state->second;

        previous_key_state = current_key_state;
        current_key_state  = action == GLFW_PRESS || action == GLFW_REPEAT;
    }

    void
    GLFWInputService::mouse_callback(GLFWwindow *, double xpos, double ypos) {
        auto &service = ServiceLocator<KeyboardMouseInputService>::GetSpecific<
                GLFWInputService>();

        auto const delta_x         = xpos - service.last_cursor_pos_x_;
        auto const delta_y         = ypos - service.last_cursor_pos_y_;
        service.last_cursor_pos_x_ = xpos;
        service.last_cursor_pos_y_ = ypos;

        if (delta_x != 0 || delta_y != 0) {
            auto const delta_x_int = static_cast<int>(delta_x);
            auto const delta_y_int = static_cast<int>(delta_y);

            service.execute_command(delta_x_int, delta_y_int);
        }
    }

    void GLFWInputService::execute_command(
            InputKey key, KeyEventType event_type
    ) const {
        auto const [first, last] =
                keyboard_commands_.equal_range({key, event_type});

        std::for_each(first, last, [](auto const &pair) {
            auto const &[_, command] = pair;
            command->execute();
        });
    }

    void GLFWInputService::execute_command(int delta_x, int delta_y) const {
        auto const move_mouse_command_it =
                move_mouse_commands_.find(current_id_);

        if (move_mouse_command_it == move_mouse_commands_.end()) {
            return;
        }

        auto const &move_mouse_command = move_mouse_command_it->second;

        move_mouse_command->execute(delta_x, delta_y);
    }

    GLFWInputService::GLFWInputService(GLFWwindow &window)
        : KeyboardMouseInputService(&window) {
        glfwSetKeyCallback(&window, key_callback);
        glfwSetCursorPosCallback(&window, mouse_callback);
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(&window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    GLFWInputService::~GLFWInputService() {
        auto const window_ptr = std::any_cast<GLFWwindow *>(get_data());
        glfwSetWindowUserPointer(window_ptr, nullptr);
        glfwSetKeyCallback(window_ptr, nullptr);
        glfwSetCursorPosCallback(window_ptr, nullptr);
    }

    void GLFWInputService::process_input() {
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

    UniqueKeyboardCommandHandle GLFWInputService::add_command(
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

    UniqueMouseCommandHandle
    GLFWInputService::add_command(std::unique_ptr<MouseMoveCommand> command) {
        move_mouse_commands_.emplace(++current_id_, std::move(command));

        return UniqueMouseCommandHandle{
                MouseCommandHandle{*this, current_id_, MouseAction::Move}
        };
    }

    void
    GLFWInputService::remove_command(InputKey key, KeyEventType event_type) {
        auto const range = keyboard_commands_.equal_range({key, event_type});
        if (range.first == range.second) {
            return;// No command found
        }

        keyboard_commands_.erase(range.first, range.second);
    }

    void
    GLFWInputService::remove_command(MouseCommandId id, MouseAction action) {
        if (action == MouseAction::Move) {
            move_mouse_commands_.erase(id);
            return;
        }

        mouse_commands_.erase({id, action});
    }
}// namespace engine
