#include "texture_store.h"

namespace engine {
    TextureHandle
    TextureStore::add_texture(std::string const &name, Texture &&texture) {
        textures_.emplace_back(std::move(texture));

        return TextureHandle{textures_.size() - 1};
    }

    TextureHandle TextureStore::add_texture(
            std::span<stbi_uc const> image_data, std::string const &name
    ) {
        Texture texture{image_data, name};

        return add_texture(name, std::move(texture));
    }

    Texture const &TextureHandle::operator*() const {
        return TextureStore::get_instance().textures_[*index_];
    }

    Texture const *TextureHandle::operator->() const {
        return &TextureStore::get_instance().textures_[*index_];
    }

    TextureHandle::operator bool() const {
        return index_.has_value();
    }
}// namespace engine
