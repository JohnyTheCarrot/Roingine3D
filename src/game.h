#ifndef GAME_H
#define GAME_H

namespace engine {
    class Game {
    public:
        virtual ~Game() = default;

        virtual void setup()              = 0;
        virtual void update()             = 0;
        virtual void render() const       = 0;
        virtual void debug_render() const = 0;
    };
}// namespace engine

#endif//GAME_H
