#ifndef UTILS_H
#define UTILS_H

#include <bgfx/bgfx.h>
#include <filesystem>
#include <string_view>

#include "types.h"

namespace bx {
    struct Vec3;
}

namespace engine::utils {
    bgfx::Memory const *
    read_file_to_bgfx_memory(std::filesystem::path const &path);

    ShaderUPtr load_shader(std::string_view path);
}// namespace engine::utils

#endif//UTILS_H
