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

    struct GenericBgfxDestroyer final {
        void operator()(auto const handle) const {
            bgfx::destroy(handle);
        }
    };

    using VertexBufferUPtr = UniqueHandle<
            bgfx::VertexBufferHandle, BGFX_INVALID_HANDLE,
            GenericBgfxDestroyer>;

    using IndexBufferUPtr = UniqueHandle<
            bgfx::IndexBufferHandle, BGFX_INVALID_HANDLE, GenericBgfxDestroyer>;

    using ShaderUPtr = UniqueHandle<
            bgfx::ShaderHandle, BGFX_INVALID_HANDLE, GenericBgfxDestroyer>;

    using ProgramUPtr = UniqueHandle<
            bgfx::ProgramHandle, BGFX_INVALID_HANDLE, GenericBgfxDestroyer>;
}// namespace engine

#endif//TYPES_H
