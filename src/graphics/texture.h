#ifndef TEXTURE_H
#define TEXTURE_H

#include <bgfx/bgfx.h>
#include <filesystem>
#include <span>

#include "misc/unique_handle.h"
#include "types.h"

namespace {
    typedef unsigned char stbi_uc;
}

namespace bgfx {
    struct Memory;
}

namespace engine {
    enum class TextureType {
        Albedo,
    };

    class Texture final {
        using UTextureHandle = UniqueHandle<
                bgfx::TextureHandle, BGFX_INVALID_HANDLE, GenericBgfxDestroyer>;

        int            width_{};
        int            height_{};
        int            channels_{};
        void          *image_data_ptr_;
        UTextureHandle texture_handle_;

        Texture(std::string const &name, stbi_uc *image_data_ptr, int width,
                int height, int channels);

    public:
        explicit Texture(
                std::filesystem::path const &path, std::string const &name
        );

        explicit Texture(
                std::span<stbi_uc const> image_data, std::string const &name
        );

        [[nodiscard]]
        std::size_t get_byte_size() const {
            return width_ * height_ * channels_;
        }

        void submit(TextureType type, int stage) const;
    };
}// namespace engine

#endif//TEXTURE_H
