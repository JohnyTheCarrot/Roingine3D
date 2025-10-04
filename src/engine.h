#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <string>

#include "game.h"

namespace engine {
    class Engine final {
        class Impl;

        // Owning pointer. See cleanup comment.
        Impl *impl_ptr_;
        bool  running_{false};
        friend class Application;

        void main_loop();

        // To my knowledge, we can't use RAII because Emscripten's main loop works by registering a main loop function
        // after which it returns and calls this loop function at given rate.
        // This means that the enter_main_loop function will return before the game has even begun, let alone before it's
        // ready to shut down.
        void cleanup();

    public:
        Engine(std::unique_ptr<Game> game, std::string title, int width,
               int height);

        ~Engine();

        void enter_main_loop() const;
    };
}// namespace engine

#endif//ENGINE_H
