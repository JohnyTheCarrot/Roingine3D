#include "audio_rt_game.h"

#include <array>

#include "application.h"
#include "components/mesh_renderer.h"
#include "components/player.h"
#include "scene.h"
#include "types.h"

namespace game {
    static std::array verts = {
            engine::Vertex{-1.0f, 1.0f, 1.0f},
            engine::Vertex{1.0f, 1.0f, 1.0f},
            engine::Vertex{-1.0f, -1.0f, 1.0f},
            engine::Vertex{1.0f, -1.0f, 1.0f},
            engine::Vertex{-1.0f, 1.0f, -1.0f},
            engine::Vertex{1.0f, 1.0f, -1.0f},
            engine::Vertex{-1.0f, -1.0f, -1.0f},
            engine::Vertex{1.0f, -1.0f, -1.0f},
    };

    static uint16_t const indices[] = {
            0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5,
    };

    void AudioRTGame::setup() {
        engine::Scene scene{};

        auto player = scene.create_game_object();
        player.add_component<Player>();
        player.get_optional_component<engine::Transform>()->set_position(
                0.f, 0.f, 20.f
        );

        auto  cube          = scene.create_game_object();
        auto &mesh_renderer = cube.add_component<engine::MeshRenderer>();
        mesh_renderer.add_mesh(verts, indices);

        auto &game = engine::Application::get_instance();
        game.set_active_scene(std::move(scene));
    }

    void AudioRTGame::update() {
        Player::update_of_type();
    }

    void AudioRTGame::render() const {
    }

    void AudioRTGame::debug_render() const {
    }
}// namespace game
