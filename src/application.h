#ifndef APPLICATION_R
#define APPLICATION_R

#include <optional>

#include "misc/singleton.h"
#include "scene.h"

namespace engine {
    class Application final : public Singleton<Application> {
        std::optional<Scene> scene_;
        int                  width_{};
        int                  height_{};
        bool                 running_{false};

    public:
        Scene &set_active_scene(Scene &&scene);

        void clear_active_scene();

        [[nodiscard]]
        float get_delta_time() const;

        [[nodiscard]]
        bool has_active_scene() const {
            return scene_.has_value();
        }

        [[nodiscard]]
        Scene &get_active_scene();

        [[nodiscard]]
        Scene const &get_active_scene() const;

        template<typename F>
            requires std::invocable<F, int &, int &>
        void update_size(F &&func) {
            func(width_, height_);
        }

        [[nodiscard]]
        int get_width() const {
            return width_;
        }

        [[nodiscard]]
        int get_height() const {
            return height_;
        }
    };
}// namespace engine

#endif//APPLICATION_R
