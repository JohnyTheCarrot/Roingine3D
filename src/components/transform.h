#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "component.h"

namespace engine {
    class Transform final : public Component<Transform> {
    public:
        using Component::Component;
    };
}// namespace engine

#endif//TRANSFORM_H
