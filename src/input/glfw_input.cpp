#include "glfw_input.h"

#include <algorithm>

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
        auto &service = ServiceLocator<KeyboardInputService>::GetSpecific<
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

    void GLFWInputService::execute_command(
            InputKey key, KeyEventType event_type
    ) const {
        auto const [first, last] = commands_.equal_range({key, event_type});

        std::for_each(first, last, [](auto const &pair) {
            auto const &[_, command] = pair;
            command->execute();
        });
    }

    GLFWInputService::GLFWInputService(GLFWwindow &window)
        : KeyboardInputService(&window) {
        glfwSetKeyCallback(&window, key_callback);
    }

    GLFWInputService::~GLFWInputService() {
        auto const window_ptr = std::any_cast<GLFWwindow *>(get_data());
        glfwSetKeyCallback(window_ptr, nullptr);
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
            std::unique_ptr<Command> command
    ) {
        commands_.emplace(std::make_pair(key, event_type), std::move(command));

        return UniqueKeyboardCommandHandle(
                CommandHandle{*this, key, event_type}
        );
    }

    void
    GLFWInputService::remove_command(InputKey key, KeyEventType event_type) {
        auto const range = commands_.equal_range({key, event_type});
        if (range.first == range.second) {
            return;// No command found
        }

        commands_.erase(range.first, range.second);
    }
}// namespace engine
