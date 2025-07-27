#ifndef AUDIO_RT_GAME_H
#define AUDIO_RT_GAME_H

#include "game.h"

namespace game {
    class AudioRTGame final : public engine::Game {
    public:
        void setup() override;
        void update() override;
        void render() const override;
        void debug_render() const override;
    };
}// namespace game

#endif//AUDIO_RT_GAME_H
