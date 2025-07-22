#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

#include "components/component.h"
#include "game/commands/move_command.h"
#include "input/keyboard_input.h"

namespace game {
    class Entity;

    class Player final
        : public engine::Component<Player>
        , public engine::Updatable {
        Entity                                          *entity_ptr_;
        std::vector<engine::UniqueKeyboardCommandHandle> commands_{};

    public:
        explicit Player(entt::registry &registry);

        // We need to explicitly remind the compiler of this because for some reason std::vector pretends to be copyable no matter if the element is. :)
        Player(Player &&)            = default;
        Player &operator=(Player &&) = default;

        void update() override;
    };
}// namespace game

#endif//PLAYER_H
