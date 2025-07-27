#ifndef MOUSE_KEYBOARD_INPUT_H
#define MOUSE_KEYBOARD_INPUT_H

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

    using MouseCommandId = std::size_t;

    enum class MouseAction { Move, LeftClick, RightClick };

    using MouseMoveCommand = Command<int, int>;

    using CommandsKey        = std::pair<InputKey, KeyEventType>;
    using KeyboardCommandMap = std::unordered_multimap<
            CommandsKey, std::unique_ptr<Command<>>,
            PairHasher<CommandsKey::first_type, CommandsKey::second_type>>;
    using MouseCommandKey = std::pair<MouseCommandId, MouseAction>;
    using MouseCommandMap = std::unordered_map<
            MouseCommandKey, std::unique_ptr<Command<>>,
            PairHasher<
                    MouseCommandKey::first_type, MouseCommandKey::second_type>>;
    using MoveMouseCommandMap = std::unordered_map<
            MouseCommandId, std::unique_ptr<MouseMoveCommand>>;

    class KeyboardMouseInputService;

    struct KeyboardMouseCommandHandle final {
        KeyboardMouseInputService *keyboard_mouse_input_service_ptr_{nullptr};
        InputKey                   key_{};
        KeyEventType               event_type_{};

        KeyboardMouseCommandHandle() = default;

        KeyboardMouseCommandHandle(
                KeyboardMouseInputService &input_service, InputKey key,
                KeyEventType event_type
        )
            : keyboard_mouse_input_service_ptr_{&input_service}
            , key_{key}
            , event_type_{event_type} {
        }
    };

    struct MouseCommandHandle final {
        KeyboardMouseInputService *keyboard_mouse_input_service_ptr_{nullptr};
        MouseCommandId             id_{};
        MouseAction                action_{};

        MouseCommandHandle() = default;

        MouseCommandHandle(
                KeyboardMouseInputService &input_service, MouseCommandId id,
                MouseAction action
        )
            : keyboard_mouse_input_service_ptr_{&input_service}
            , id_{id}
            , action_{action} {
        }
    };

    struct KeyboardCommandHandleDestroyer final {
        void operator()(KeyboardMouseCommandHandle command) const;
    };

    using UniqueKeyboardCommandHandle = UniqueHandle<
            KeyboardMouseCommandHandle, {}, KeyboardCommandHandleDestroyer>;

    struct MouseCommandHandleDestroyer final {
        void operator()(MouseCommandHandle command) const;
    };

    using UniqueMouseCommandHandle =
            UniqueHandle<MouseCommandHandle, {}, MouseCommandHandleDestroyer>;

    class KeyboardMouseInputService {
        std::any data_;

    protected:
        [[nodiscard]]
        std::any const &get_data() const {
            return data_;
        }

        explicit KeyboardMouseInputService(std::any data = std::any{})
            : data_(std::move(data)) {
        }

    public:
        virtual ~KeyboardMouseInputService() = default;

        virtual void process_input() = 0;

        [[nodiscard]]
        virtual UniqueKeyboardCommandHandle add_command(
                InputKey key, KeyEventType event_type,
                std::unique_ptr<Command<>> command
        ) = 0;

        [[nodiscard]]
        virtual UniqueMouseCommandHandle
        add_command(std::unique_ptr<MouseMoveCommand> command) = 0;

        virtual void remove_command(InputKey key, KeyEventType event_type) = 0;

        virtual void remove_command(MouseCommandId id, MouseAction action) = 0;
    };
}// namespace engine

#endif//MOUSE_KEYBOARD_INPUT_H
