#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H

#include <string>
#include <unordered_map>

#include "graphics/texture.h"

namespace engine {
    class TextureStore final {
        std::unordered_map<std::string, Texture> store_;

    public:
        TextureStore() = default;

        [[nodiscard]]
        Texture const &
        get_texture(std::filesystem::path const &path, TextureType type);
    };
}// namespace engine

#endif//TEXTURE_STORE_H
