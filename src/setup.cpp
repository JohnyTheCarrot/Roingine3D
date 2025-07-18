#include "setup.h"

#include "components/mesh_renderer.h"
#include "scene.h"

void game::setup() {
    engine::Scene scene{};

    auto game_object = scene.create_game_object();
    game_object.add_component<engine::MeshRenderer>();
}
