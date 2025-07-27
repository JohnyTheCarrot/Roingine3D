#ifndef ENGINE_H
#define ENGINE_H

#include <memory>
#include <string>

#include "game.h"

namespace engine {
    class Engine final {
        class Impl;

        std::unique_ptr<Impl> impl_ptr_;
        bool                  running_{false};
        friend class Application;

    public:
        Engine(std::unique_ptr<Game> game, std::string title, int width,
               int height);

        ~Engine();

        void main_loop() const;
    };
}// namespace engine

#endif//ENGINE_H
