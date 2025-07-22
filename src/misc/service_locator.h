#ifndef SERVICE_LOCATOR_H
#define SERVICE_LOCATOR_H
#include <memory>

namespace engine {
    template<typename T>
    class ServiceLocator {
        static inline std::unique_ptr<T> service_instance_{nullptr};

    public:
        static void Provide(std::unique_ptr<T> &&service) {
            service_instance_ = std::move(service);
        }

        [[nodiscard]]
        static T &Get() {
            if (!service_instance_) {
                throw std::runtime_error("Service not provided");
            }

            return *service_instance_;
        }

        /**
         * For retrieving a specific service provider that inherits from T.
         * This is relatively unsafe in that if the service is not of the requested type the program will throw an exception.
         * I.e. only use this if you are sure that the service is of the requested type.
         *
         * @tparam SpecificService A specific service type that inherits from T
         * @return
         */
        template<typename SpecificService>
        [[nodiscard]]
        static SpecificService &GetSpecific() {
            auto &service = Get();

            if (auto specific_service =
                        dynamic_cast<SpecificService *>(&service)) {
                return *specific_service;
            }

            throw std::runtime_error("Service is not of the requested type");
        }
    };
}// namespace engine

#endif//SERVICE_LOCATOR_H
