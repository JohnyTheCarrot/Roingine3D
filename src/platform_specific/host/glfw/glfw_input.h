#ifndef GLFW_INPUT_H
#define GLFW_INPUT_H

#include <GLFW/glfw3.h>
#include <unordered_map>

#include "input/mouse_keyboard_input.h"

namespace engine {
    class GLFWInputService final : public KeyboardMouseInputService {
        int last_cursor_pos_x_{0};
        int last_cursor_pos_y_{0};

        static void
        key_callback(GLFWwindow *, int key, int scancode, int action, int mods);

        static void
        mouse_callback(GLFWwindow *window, double xpos, double ypos);

    public:
        explicit GLFWInputService(GLFWwindow &window);

        ~GLFWInputService() override;

        GLFWInputService(GLFWInputService const &)            = delete;
        GLFWInputService &operator=(GLFWInputService const &) = delete;
        GLFWInputService(GLFWInputService &&)                 = delete;
        GLFWInputService &operator=(GLFWInputService &&)      = delete;
    };
}// namespace engine

#endif//GLFW_INPUT_H
