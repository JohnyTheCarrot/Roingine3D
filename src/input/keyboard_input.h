#ifndef KEYBOARD_INPUT_H
#define KEYBOARD_INPUT_H

#include <any>
#include <memory>
#include <unordered_map>
#include <utility>

#include "commands/command.h"
#include "misc/unique_handle.h"

namespace {
    template<typename A, typename B>
    class PairHasher {
    public:
        constexpr std::size_t operator()(std::pair<A, B> const &pair) const {
            return std::hash<A>()(pair.first) ^ std::hash<B>()(pair.second);
        }
    };
}// namespace

namespace engine {
    enum class InputKey : int {
        A,
        B,
        C,
        D,
        E,
        F,
        G,
        H,
        I,
        J,
        K,
        L,
        M,
        N,
        O,
        P,
        Q,
        R,
        S,
        T,
        U,
        V,
        W,
        X,
        Y,
        Z,
        Space,
        Escape,
        LeftShift,
        LeftCtrl,
        Enter,
        F1,
        F2,
        F3,
        F4,
        F5,
        F6,
        F7,
        F8,
        F9,
        F10,
        F11,
        F12,
        KeyCount
    };

    enum class KeyEventType { Up, Down, Held };

    using CommandsKey = std::pair<InputKey, KeyEventType>;
    using CommandsMap = std::unordered_multimap<
            CommandsKey, std::unique_ptr<Command>,
            PairHasher<CommandsKey::first_type, CommandsKey::second_type>>;

    struct CommandHandle final {
        KeyboardInputService *keyboard_input_service_ptr_{nullptr};
        InputKey              key_{};
        KeyEventType          event_type_{};

        CommandHandle() = default;

        CommandHandle(
                KeyboardInputService &input_service, InputKey key,
                KeyEventType event_type
        )
            : keyboard_input_service_ptr_{&input_service}
            , key_{key}
            , event_type_{event_type} {
        }
    };

    struct KeyboardCommandHandleDestroyer final {
        void operator()(CommandHandle command) const;
    };

    using UniqueKeyboardCommandHandle =
            UniqueHandle<CommandHandle, {}, KeyboardCommandHandleDestroyer>;

    class KeyboardInputService {
        std::any data_;

    protected:
        [[nodiscard]]
        std::any const &get_data() const {
            return data_;
        }

        explicit KeyboardInputService(std::any data = std::any{})
            : data_(std::move(data)) {
        }

    public:
        virtual ~KeyboardInputService() = default;

        virtual void process_input() = 0;

        [[nodiscard]]
        virtual UniqueKeyboardCommandHandle add_command(
                InputKey key, KeyEventType event_type,
                std::unique_ptr<Command> command
        ) = 0;

        virtual void remove_command(InputKey key, KeyEventType event_type) = 0;
    };
}// namespace engine

#endif//KEYBOARD_INPUT_H
