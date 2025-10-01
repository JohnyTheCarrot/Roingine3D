#ifndef CPP_ENGINE_OBSERVER_H
#define CPP_ENGINE_OBSERVER_H

#include <algorithm>
#include <functional>
#include <memory>
#include <ranges>
#include <unordered_map>

namespace engine {
    using ObserverHandle = std::size_t;
    constexpr ObserverHandle NullObserverHandle{0};

    template<class... CallbackArgs>
    class Observer final {
        using Fn = std::function<void(CallbackArgs...)>;
        Fn callback_;

    public:
        explicit Observer(std::function<void()> callback)
            : callback_{std::move(callback)} {
        }

        void on_notified() const {
            callback_();
        }
    };

    template<class... CallbackArgs>
    class Subject final {
        using ObserverRef = std::reference_wrapper<Observer<CallbackArgs...>>;
        std::unordered_map<ObserverHandle, ObserverRef> observers_;
        ObserverHandle                                  current_handle_{};

    public:
        void notify() const {
            for (auto const &observer : observers_ | std::views::values) {
                observer.on_notified();
            }
        }

        void remove_observer(ObserverHandle handle) {
            observers_.erase(handle);
        }

        class ObserverRemover final {
            Subject<CallbackArgs...> *parent_ptr_;

        public:
            explicit ObserverRemover(Subject<CallbackArgs...> &parent)
                : parent_ptr_{&parent} {
            }

            void operator()(ObserverHandle handle) {
                if (handle == NullObserverHandle)
                    return;

                parent_ptr_->remove_observer(handle);
            }
        };

        using UniqueObserver = std::unique_ptr<ObserverHandle, ObserverRemover>;

        [[nodiscard]]
        UniqueObserver add_observer(Observer<CallbackArgs...> observer) {
            ++current_handle_;
            observers_.emplace(current_handle_, std::move(observer));

            return std::make_unique<ObserverHandle, ObserverRemover>(
                    current_handle_, *this

            );
        }
    };
}// namespace engine

#endif//CPP_ENGINE_OBSERVER_H
