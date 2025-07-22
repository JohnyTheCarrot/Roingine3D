#ifndef GLFW_INPUT_H
#define GLFW_INPUT_H

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "keyboard_input.h"

namespace engine {
    class GLFWInputService final : public KeyboardInputService {
        struct KeyState {
            bool previous_key_state{false};
            bool current_key_state{false};
        };

        std::unordered_map<InputKey, KeyState> key_states_;
        CommandsMap                            commands_;

        static void
        key_callback(GLFWwindow *, int key, int scancode, int action, int mods);

        void execute_command(InputKey key, KeyEventType event_type) const;

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
                std::unique_ptr<Command> command
        ) override;

        void remove_command(InputKey key, KeyEventType event_type) override;
    };
}// namespace engine

#endif//GLFW_INPUT_H
