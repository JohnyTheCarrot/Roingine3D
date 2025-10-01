#ifndef GLFW_INPUT_H
#define GLFW_INPUT_H

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "input/mouse_keyboard_input.h"

namespace engine {
    class GLFWInputService final : public KeyboardMouseInputService {
        struct KeyState {
            bool previous_key_state{false};
            bool current_key_state{false};
        };

        std::unordered_map<InputKey, KeyState> key_states_;
        KeyboardCommandMap                     keyboard_commands_;
        MouseCommandMap                        mouse_commands_;
        MoveMouseCommandMap                    move_mouse_commands_;
        MouseCommandId                         current_id_{};

        int last_cursor_pos_x_{0};
        int last_cursor_pos_y_{0};

        static void
        key_callback(GLFWwindow *, int key, int scancode, int action, int mods);

        static void
        mouse_callback(GLFWwindow *window, double xpos, double ypos);

        void execute_command(InputKey key, KeyEventType event_type) const;

        // mouse move
        void execute_command(int delta_x, int delta_y) const;

    public:
        explicit GLFWInputService(GLFWwindow &window);

        ~GLFWInputService() override;

        GLFWInputService(GLFWInputService const &)            = delete;
        GLFWInputService &operator=(GLFWInputService const &) = delete;
        GLFWInputService(GLFWInputService &&)                 = delete;
        GLFWInputService &operator=(GLFWInputService &&)      = delete;

        void process_input() override;

        UniqueKeyboardCommandHandle add_command(
                InputKey key, KeyEventType event_type,
                std::unique_ptr<Command<>> command
        ) override;

        UniqueMouseCommandHandle
        add_command(std::unique_ptr<MouseMoveCommand> command) override;

        void remove_command(InputKey key, KeyEventType event_type) override;

        void remove_command(MouseCommandId id, MouseAction action) override;

        [[nodiscard]]
        bool get_key_state(InputKey key) const override;
    };
}// namespace engine

#endif//GLFW_INPUT_H
