#include "audio_rt_game.h"

#include <array>

#include "application.h"
#include "components/mesh_renderer.h"
#include "components/player.h"
#include "graphics/mesh.h"
#include "scene.h"
#include "scene_loaders/gltf_loader.h"
#include "types.h"

namespace game {
    static std::array verts = {
            // Front (+Z)
            engine::Vertex{-1, 1, 1, 0x0000, 0x0000},  // 0
            engine::Vertex{1, 1, 1, 0x7FFF, 0x0000},   // 1
            engine::Vertex{-1, -1, 1, 0x0000, 0x7FFF}, // 2
            engine::Vertex{1, -1, 1, 0x7FFF, 0x7FFF},  // 3
                                                       // Back (-Z)
            engine::Vertex{-1, 1, -1, 0x7FFF, 0x0000}, // 4
            engine::Vertex{1, 1, -1, 0x0000, 0x0000},  // 5
            engine::Vertex{-1, -1, -1, 0x7FFF, 0x7FFF},// 6
            engine::Vertex{1, -1, -1, 0x0000, 0x7FFF}, // 7
                                                       // Left (-X)
            engine::Vertex{-1, 1, -1, 0x0000, 0x0000}, // 8
            engine::Vertex{-1, 1, 1, 0x7FFF, 0x0000},  // 9
            engine::Vertex{-1, -1, -1, 0x0000, 0x7FFF},// 10
            engine::Vertex{-1, -1, 1, 0x7FFF, 0x7FFF}, // 11
                                                       // Right (+X)
            engine::Vertex{1, 1, 1, 0x0000, 0x0000},   // 12
            engine::Vertex{1, 1, -1, 0x7FFF, 0x0000},  // 13
            engine::Vertex{1, -1, 1, 0x0000, 0x7FFF},  // 14
            engine::Vertex{1, -1, -1, 0x7FFF, 0x7FFF}, // 15
                                                       // Top (+Y)
            engine::Vertex{-1, 1, -1, 0x0000, 0x0000}, // 16
            engine::Vertex{1, 1, -1, 0x7FFF, 0x0000},  // 17
            engine::Vertex{-1, 1, 1, 0x0000, 0x7FFF},  // 18
            engine::Vertex{1, 1, 1, 0x7FFF, 0x7FFF},   // 19
                                                       // Bottom (-Y)
            engine::Vertex{-1, -1, 1, 0x0000, 0x0000}, // 20
            engine::Vertex{1, -1, 1, 0x7FFF, 0x0000},  // 21
            engine::Vertex{-1, -1, -1, 0x0000, 0x7FFF},// 22
            engine::Vertex{1, -1, -1, 0x7FFF, 0x7FFF}  // 23
    };

    void AudioRTGame::setup() {
        using namespace engine::math;

        engine::Scene scene{};

        auto player = scene.create_game_object();
        player.add_component<engine::Player>();
        player.get_optional_component<engine::Transform>()->set_position(
                0.f, 0.f, 20.f
        );

        {
            auto const scene_path = std::getenv("SCENE");
            if (!scene_path) {
                throw std::runtime_error{"SCENE environment variable is not set"
                };
            }
            engine::load_gltf_scene(scene, scene_path);
        }

        // Vec3 spawnPos{0.f, 0.f, 0.f};
        // int  i = 0;
        // for (auto &scene_path : scenes) {
        //     engine::GameObject scene1 = scene.create_game_object();
        //     auto &transform = scene1.add_component<engine::Transform>();
        //     transform.set_position(spawnPos);
        //     engine::load_gltf_scene(scene, scene_path);
        //
        //     ++i;
        // }

        auto &game = engine::Application::get_instance();
        game.set_active_scene(std::move(scene));
    }

    void AudioRTGame::update() {
    }

    void AudioRTGame::render() const {
    }

    void AudioRTGame::debug_render() const {
    }
}// namespace game
