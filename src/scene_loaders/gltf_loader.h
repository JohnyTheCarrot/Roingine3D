#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <fastgltf/core.hpp>

#include "graphics/mesh.h"

namespace engine {
    class Scene;

    void
    load_gltf_scene(Scene &scene, std::filesystem::path const &scene_file_path);
}// namespace engine

#endif//GLTF_LOADER_H
