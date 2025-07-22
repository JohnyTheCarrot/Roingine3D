#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include <span>

#include "component.h"
#include "misc/utils.h"
#include "types.h"

namespace engine {
    class Transform;

    class MeshRenderer : public Component<MeshRenderer> {
        Transform const *transform_ptr_{};
        VertexBufferUPtr vertex_buffer_uptr_{};
        IndexBufferUPtr  index_buffer_uptr_{};
        ShaderUPtr       vert_shader_uptr_{utils::load_shader("cube_vert")};
        ShaderUPtr       frag_shader_uptr_{utils::load_shader("cube_frag")};
        ProgramUPtr      program_uptr_{bgfx::createProgram(
                vert_shader_uptr_.get(), frag_shader_uptr_.get(), true
        )};

    public:
        explicit MeshRenderer(entt::registry &registry);

        void add_mesh(
                std::span<Vertex const> vertices, std::span<Index const> indices
        );

        void render() const;
    };
}// namespace engine

#endif//MESH_RENDERER_H
