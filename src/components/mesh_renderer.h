#ifndef MESH_RENDERER_H
#define MESH_RENDERER_H

#include "component.h"
#include "graphics/mesh.h"
#include "misc/utils.h"
#include "types.h"

namespace engine {
    class Transform;

    // TODO: the mesh renderer should not own the mesh, the mesh should be a separate component
    class MeshRenderer : public Component<MeshRenderer> {
        Transform const      *transform_ptr_{};
        std::unique_ptr<Mesh> mesh_uptr_;
        ShaderUPtr  vert_shader_uptr_{utils::load_shader("cube_vert")};
        ShaderUPtr  frag_shader_uptr_{utils::load_shader("cube_frag")};
        ProgramUPtr program_uptr_{bgfx::createProgram(
                vert_shader_uptr_.get(), frag_shader_uptr_.get(), true
        )};

    public:
        MeshRenderer(entt::registry &registry, std::unique_ptr<Mesh> mesh);

        void render() const;
    };
}// namespace engine

#endif//MESH_RENDERER_H
