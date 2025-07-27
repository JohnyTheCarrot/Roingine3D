#include "utils.h"

#include <cmath>
#include <fstream>
#include <vector>

namespace engine::utils {

    bgfx::Memory const *
    read_file_to_bgfx_memory(std::filesystem::path const &path) {
        std::ifstream file(path, std::ios::binary);

        if (!file) {
            throw std::runtime_error("Failed to open file: " + path.string());
        }

        std::vector const buffer(
                (std::istreambuf_iterator(file)),
                std::istreambuf_iterator<char>()
        );
        return bgfx::copy(buffer.data(), buffer.size());
    }

    ShaderUPtr load_shader(std::string_view file_name) {
        std::filesystem::path path = "shaders/";
        switch (bgfx::getRendererType()) {
            case bgfx::RendererType::Direct3D11:
            case bgfx::RendererType::Direct3D12:
                path /= "dx11/";
                break;
            case bgfx::RendererType::OpenGL:
                path /= "glsl/";
                break;
            case bgfx::RendererType::OpenGLES:
                path /= "essl/";
                break;
            case bgfx::RendererType::Vulkan:
                path /= "spirv/";
                break;
            case bgfx::RendererType::Metal:
                path /= "metal/";
                break;
            default:
                throw std::runtime_error(
                        "Unsupported renderer type: " +
                        std::to_string(bgfx::getRendererType())
                );
        }
        path /= std::format("{}.sc.bin", file_name);
        auto const mem = read_file_to_bgfx_memory(path);
        if (!mem) {
            throw std::runtime_error(
                    "Failed to load shader file: " + path.string()
            );
        }

        return ShaderUPtr{bgfx::createShader(mem)};
    }

    float deg_to_rad(float degrees) {
        return degrees * (M_PIf / 180.0f);
    }
}// namespace engine::utils
