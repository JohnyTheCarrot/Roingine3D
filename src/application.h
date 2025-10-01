#ifndef APPLICATION_R
#define APPLICATION_R

#include <optional>

#include "misc/singleton.h"
#include "scene.h"
#include "types.h"

namespace engine {
    class Application final : public Singleton<Application> {
        std::optional<Scene> scene_;
        int                  width_{};
        int                  height_{};
        DeltaTime            delta_time_{0.0f};
        bool                 running_{false};

        friend class GameHost;

    public:
        Scene &set_active_scene(Scene &&scene);

        void clear_active_scene();

        [[nodiscard]]
        float get_delta_time() const;

        void set_delta_time(DeltaTime delta_time) {
            delta_time_ = delta_time;
        }

        [[nodiscard]]
        bool has_active_scene() const {
            return scene_.has_value();
        }

        [[nodiscard]]
        bool is_shutdown_requested() const {
            return running_;
        }

        void request_shutdown() {
            running_ = true;
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
