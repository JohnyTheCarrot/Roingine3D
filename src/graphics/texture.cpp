#include "texture.h"

#include <bgfx/bgfx.h>
#include <fstream>
#include <stb_image.h>

namespace engine {
    void stbi_image_release(void *, void *user_data_ptr) {
        stbi_image_free(user_data_ptr);
    }

    Texture::Texture(std::filesystem::path const &path, TextureType type)
        : image_data_ptr_{[this, &path] {
            auto *ptr =
                    stbi_load(path.c_str(), &width_, &height_, &channels_, 0);
            if (ptr == nullptr) {
                throw std::runtime_error(
                        "Failed to load image: " + path.string() + " - " +
                        stbi_failure_reason()
                );
            }

            return ptr;
        }()}
        , type_{type}
        , texture_handle_{[this] {
            auto const *mem = bgfx::makeRef(
                    image_data_ptr_, get_byte_size(), stbi_image_release,
                    image_data_ptr_
            );

            return UTextureHandle{bgfx::createTexture2D(
                    width_, height_, false, 1, bgfx::TextureFormat::RGB8,
                    BGFX_TEXTURE_NONE, mem
            )};
        }()} {
        if (!bgfx::isValid(texture_handle_.get())) {
            throw std::runtime_error(
                    "Failed to create texture from image data: " + path.string()
            );
        }

        bgfx::setName(texture_handle_.get(), path.filename().c_str());
    }

    void Texture::submit(bgfx::UniformHandle uniform_handle, int stage) const {
        bgfx::setTexture(stage, uniform_handle, texture_handle_.get());
    }
}// namespace engine
