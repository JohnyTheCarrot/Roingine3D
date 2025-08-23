#ifndef TEXTURE_STORE_H
#define TEXTURE_STORE_H

#include <optional>
#include <string>
#include <vector>

#include "graphics/texture.h"
#include "misc/singleton.h"

namespace engine {
    class TextureHandle;

    class TextureStore final : public Singleton<TextureStore> {
        std::vector<Texture> textures_;
        UniformUniqueHandle  albedo_texture_uniform_{
                bgfx::createUniform("u_albedo", bgfx::UniformType::Sampler)
        };
        UniformUniqueHandle base_color_factor_{bgfx::createUniform(
                "u_baseColorFactor", bgfx::UniformType::Vec4
        )};

        friend class TextureHandle;

    public:
        TextureStore() = default;

        TextureHandle add_texture(std::string const &name, Texture &&texture);

        TextureHandle add_texture(
                std::span<stbi_uc const> image_data, std::string const &name
        );

        void clear() {
            albedo_texture_uniform_.reset();
            base_color_factor_.reset();
            textures_.clear();
        }

        [[nodiscard]]
        UniformUniqueHandle::handle get_albedo_texture_uniform() const {
            return albedo_texture_uniform_.get();
        }

        [[nodiscard]]
        UniformUniqueHandle::handle get_base_color_factor() const {
            return base_color_factor_.get();
        }
    };

    class TextureHandle final {
        std::optional<std::size_t> index_{};

        friend class TextureStore;

    public:
        TextureHandle() = default;

        explicit TextureHandle(std::size_t index)
            : index_{index} {
        }

        [[nodiscard]]
        Texture const &operator*() const;

        Texture const *operator->() const;

        [[nodiscard]] operator bool() const;
    };
}// namespace engine

#endif//TEXTURE_STORE_H
