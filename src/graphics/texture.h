#ifndef TEXTURE_H
#define TEXTURE_H

#include <bgfx/bgfx.h>
#include <filesystem>

#include "misc/unique_handle.h"
#include "types.h"

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
        TextureType    type_;
        UTextureHandle texture_handle_;

    public:
        Texture(std::filesystem::path const &path, TextureType type);

        [[nodiscard]]
        std::size_t get_byte_size() const {
            return width_ * height_ * channels_;
        }

        [[nodiscard]]
        TextureType get_type() const {
            return type_;
        }

        void submit(bgfx::UniformHandle uniform_handle, int stage) const;
    };
}// namespace engine

#endif//TEXTURE_H
