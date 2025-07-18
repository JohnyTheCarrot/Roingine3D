#ifndef TYPES_H
#define TYPES_H

#include <bgfx/bgfx.h>

#include "misc/unique_handle.h"

namespace engine {
    struct Vertex final {
        float x_{}, y_{}, z_{};

        static bgfx::VertexLayout layout;

        static void setup_layout() {
            layout.begin()
                    .add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
                    .end();
        }
    };

    using Index = uint16_t;

    struct VertexBufferDestroyer final {
        void operator()(bgfx::VertexBufferHandle const handle) const {
            bgfx::destroy(handle);
        }
    };

    using VertexBufferUPtr = UniqueHandle<
            bgfx::VertexBufferHandle, BGFX_INVALID_HANDLE,
            VertexBufferDestroyer>;

    struct IndexBufferDestroyer final {
        void operator()(bgfx::IndexBufferHandle const handle) const {
            bgfx::destroy(handle);
        }
    };

    using IndexBufferUPtr = UniqueHandle<
            bgfx::IndexBufferHandle, BGFX_INVALID_HANDLE, IndexBufferDestroyer>;
}// namespace engine

#endif//TYPES_H
