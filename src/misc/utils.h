#ifndef UTILS_H
#define UTILS_H

#include <bgfx/bgfx.h>
#include <filesystem>
#include <functional>
#include <string_view>

#include "types.h"

namespace bx {
    struct Vec3;
}

namespace engine::utils {
    bgfx::Memory const *
    read_file_to_bgfx_memory(std::filesystem::path const &path);

    ShaderUPtr load_shader(std::string_view path);

    [[nodiscard]]
    float deg_to_rad(float degrees);

    template<typename T>
    T verify_bgfx_handle(T &&handle, std::string_view error_message) {
        if (!bgfx::isValid(handle)) {
            throw std::runtime_error(
                    "BGFX handle is invalid: " + std::string{error_message}
            );
        }

        return std::forward<T>(handle);
    }

    template<typename T>
    T verify_bgfx_handle(
            T &&handle, std::function<std::string_view()> const &error_message
    ) {
        if (!bgfx::isValid(handle)) {
            throw std::runtime_error(
                    "BGFX handle is invalid: " + std::string{error_message()}
            );
        }

        return std::forward<T>(handle);
    }
}// namespace engine::utils

#endif//UTILS_H
