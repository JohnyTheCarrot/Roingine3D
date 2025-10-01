#include "audio_rt_game.h"
#include "engine.h"

int main() {
    std::unique_ptr<game::AudioRTGame> game =
            std::make_unique<game::AudioRTGame>();
    engine::Engine const engine{std::move(game), "Audio RT", 1280, 720};

    engine.enter_main_loop();
}
