#ifndef GLTF_LOADER_H
#define GLTF_LOADER_H

#include <fastgltf/core.hpp>

namespace engine {
    class GameObject;
    class Scene;

    void load_gltf_scene(
            Scene &scene, std::filesystem::path const &scene_file_path,
            GameObject *parent_ptr = nullptr
    );
}// namespace engine

#endif//GLTF_LOADER_H
