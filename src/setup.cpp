#include "setup.h"

#include "components/mesh_renderer.h"
#include "game.h"
#include "scene.h"

static std::array verts = {
        engine::Vertex{-1.0f, 1.0f, 1.0f},   engine::Vertex{1.0f, 1.0f, 1.0f},
        engine::Vertex{-1.0f, -1.0f, 1.0f},  engine::Vertex{1.0f, -1.0f, 1.0f},
        engine::Vertex{-1.0f, 1.0f, -1.0f},  engine::Vertex{1.0f, 1.0f, -1.0f},
        engine::Vertex{-1.0f, -1.0f, -1.0f}, engine::Vertex{1.0f, -1.0f, -1.0f},
};


static uint16_t const indices[] = {
        0, 1, 2, 3, 7, 1, 5, 0, 4, 2, 6, 7, 4, 5,
};

void game::setup() {
    engine::Scene scene{};

    auto  game_object   = scene.create_game_object();
    auto &mesh_renderer = game_object.add_component<engine::MeshRenderer>();
    mesh_renderer.add_mesh(verts, indices);

    auto &game = engine::Game::get_instance();
    game.set_active_scene(std::move(scene));
}
