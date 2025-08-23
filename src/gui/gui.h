#ifndef CPP_ENGINE_GUI_H
#define CPP_ENGINE_GUI_H

#include <memory>

namespace engine::ui {
    class GUI final {
        class Impl;
        std::unique_ptr<Impl> impl_;

    public:
        GUI();
    };
}// namespace engine::ui

#endif//CPP_ENGINE_GUI_H
