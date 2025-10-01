#ifndef GLFW_INCL
#define GLFW_INCL

#ifdef __linux__
#    ifdef BUILD_FOR_X11
#        define GLFW_EXPOSE_NATIVE_X11
#    else
#        define GLFW_EXPOSE_NATIVE_WAYLAND
#    endif
#elif _WIN64
#    define NOMINMAX
#    define GLFW_EXPOSE_NATIVE_WIN32
#elif __APPLE__
#    define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

#endif
