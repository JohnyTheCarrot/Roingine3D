#include "texture.h"

#include <bgfx/bgfx.h>
#include <cassert>
#include <format>
#include <fstream>
#include <stb_image.h>

#include "misc/utils.h"
#include "texture_store.h"

namespace engine {
    void stbi_image_release(void *, void *user_data_ptr) {
        stbi_image_free(user_data_ptr);
    }

    [[nodiscard]]
    bgfx::TextureFormat::Enum get_format_from_channels(int channels) {
        switch (channels) {
            case 1:
                return bgfx::TextureFormat::R8;
            case 2:
                return bgfx::TextureFormat::RG8;
            case 3:
                return bgfx::TextureFormat::RGB8;
            case 4:
                return bgfx::TextureFormat::RGBA8;
            default:
                throw std::runtime_error{std::format(
                        "Unsupported number of channels: {}", channels
                )};
        }
    }

    Texture::Texture(
            std::string const &name, stbi_uc *image_data_ptr, int width,
            int height, int channels
    )
        : width_{width}
        , height_{height}
        , channels_{channels}
        , image_data_ptr_{[image_data_ptr] {
            if (image_data_ptr == nullptr) {
                throw std::runtime_error(
                        std::format(
                                "Failed to load image: {}",
                                stbi_failure_reason()
                        )
                );
            }

            return image_data_ptr;
        }()}

        , texture_handle_{[this] {
            auto const *mem = bgfx::makeRef(
                    image_data_ptr_, get_byte_size(), stbi_image_release,
                    image_data_ptr_
            );

            return UTextureHandle{bgfx::createTexture2D(
                    width_, height_, false, 1,
                    get_format_from_channels(channels_), BGFX_TEXTURE_NONE, mem
            )};
        }()} {
        utils::verify_bgfx_handle(texture_handle_.get(), stbi_failure_reason);

        bgfx::setName(texture_handle_.get(), name.data());
    }

    Texture::Texture(std::filesystem::path const &path, std::string const &name)
        : Texture{[&] {
            int   width;
            int   height;
            int   channels;
            auto *image_data_ptr = stbi_load(
                    path.string().c_str(), &width, &height, &channels, 0
            );

            return Texture{name, image_data_ptr, width, height, channels};
        }()} {
    }

    Texture::Texture(
            std::span<stbi_uc const> image_data, std::string const &name
    )
        : Texture{[&] {
            int   width;
            int   height;
            int   channels;
            auto *image_data_ptr = stbi_load_from_memory(
                    image_data.data(),
                    static_cast<int>(image_data.size_bytes()), &width, &height,
                    &channels, 0
            );

            return Texture{name, image_data_ptr, width, height, channels};
        }()} {
    }

    void Texture::submit(TextureType type, int stage) const {
        auto const uniform_handle = [type] -> UniformUniqueHandle::handle {
            auto const &texture_store = TextureStore::get_instance();

            switch (type) {
                case TextureType::Albedo:
                    return texture_store.get_albedo_texture_uniform();
                default:
                    assert(false);
                    return BGFX_INVALID_HANDLE;
            }
        }();

        bgfx::setTexture(stage, uniform_handle, texture_handle_.get());
    }
}// namespace engine
