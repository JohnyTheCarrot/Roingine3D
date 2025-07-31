#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include <span>

#include "component.h"
#include "graphics/mesh.h"
#include "graphics/texture.h"
#include "misc/utils.h"
#include "types.h"

namespace engine {
    class Transform;

    class MeshRenderer : public Component<MeshRenderer> {
        Transform const      *transform_ptr_{};
        std::unique_ptr<Mesh> mesh_uptr_;
        ShaderUPtr          vert_shader_uptr_{utils::load_shader("cube_vert")};
        ShaderUPtr          frag_shader_uptr_{utils::load_shader("cube_frag")};
        ProgramUPtr         program_uptr_{bgfx::createProgram(
                vert_shader_uptr_.get(), frag_shader_uptr_.get(), true
        )};
        UniformUniqueHandle albedo_texture_uniform_{
                bgfx::createUniform("u_albedo", bgfx::UniformType::Sampler)
        };
        std::unordered_map<TextureType, Texture> textures_;

    public:
        MeshRenderer(entt::registry &registry, std::unique_ptr<Mesh> mesh);

        template<typename T>
            requires std::is_base_of_v<Texture, T>
        void add_texture(T &&texture) {
            textures_.emplace(texture.type, std::forward<T>(texture));
        }

        template<typename... Args>
            requires std::constructible_from<Texture, Args...>
        void add_texture(Args &&...args) {
            Texture texture{std::forward<Args>(args)...};
            textures_.emplace(texture.get_type(), std::move(texture));
        }

        void render() const;
    };
}// namespace engine

#endif//MESH_RENDERER_H
