#include "gui.h"

#include <bx/allocator.h>
#include <bx/math.h>
#include <cstddef>
#include <imgui/imgui_internal.h>
#include <misc/utils.h>

#include "types.h"

// Made with help from https://github.com/bkaradzic/bgfx/blob/master/examples/common/imgui/imgui.cpp

namespace engine::ui {
    static void *alloc(std::size_t size, void *) {
        return bx::alloc(nullptr, size);
    }

    static void dealloc(void *ptr, void *) {
        bx::free(nullptr, ptr);
    }

    struct ImGuiCleanup final {
        void operator()(ImGuiContext *ctx) const {
            ImGui::DestroyContext(ctx);
        }
    };

    using UniqueImGuiContextPtr = std::unique_ptr<ImGuiContext, ImGuiCleanup>;

    class GUI::Impl final {
        UniqueImGuiContextPtr ctx_;
        ImGuiIO              *io_{[] {
            auto *const io  = &ImGui::GetIO();
            io->DisplaySize = ImVec2{1280.f, 720.f};
            io->DeltaTime   = 1.f / 60.f;
            io->IniFilename = nullptr;
            io->BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;

            return io;
        }()};
        ShaderUPtr            frag_shader_{utils::load_shader("imgui_frag")};
        ShaderUPtr            vert_shader_{utils::load_shader("imgui_vert")};
        ProgramUPtr           program_uptr_{bgfx::createProgram(
                vert_shader_.get(), frag_shader_.get(), true
        )};
        UniformUniqueHandle   u_texture_{
                bgfx::createUniform("s_tex", bgfx::UniformType::Sampler)
        };
        bgfx::VertexLayout layout_;
        std::size_t        view_id_{0};

        void render(ImDrawData *draw_data) {
            int32_t disp_width = static_cast<int32_t>(
                    draw_data->DisplaySize.x * draw_data->FramebufferScale.x
            );
            int32_t disp_height = static_cast<int32_t>(
                    draw_data->DisplaySize.y * draw_data->FramebufferScale.y
            );
            if (disp_width <= 0 || disp_height <= 0) {
                return;
            }

            bgfx::setViewName(view_id_, "ImGui");
            bgfx::setViewMode(view_id_, bgfx::ViewMode::Sequential);

            auto const caps_ptr = bgfx::getCaps();
            {
                float       ortho[16];
                float const x      = draw_data->DisplayPos.x;
                float const y      = draw_data->DisplayPos.y;
                float const width  = draw_data->DisplaySize.x;
                float const height = draw_data->DisplaySize.y;

                bx::mtxOrtho(
                        ortho, x, x + width, y + height, y, 0.0f, 1000.0f, 0.0f,
                        caps_ptr->homogeneousDepth
                );
                bgfx::setViewTransform(view_id_, NULL, ortho);
                bgfx::setViewRect(
                        view_id_, 0, 0, static_cast<uint16_t>(width),
                        static_cast<uint16_t>(height)
                );
            }
        }

    public:
        Impl()
            : ctx_{[] {
                IMGUI_CHECKVERSION();

                ImGui::SetAllocatorFunctions(alloc, dealloc, nullptr);
                return ImGui::CreateContext();
            }()} {
            layout_.begin()
                    .add(bgfx::Attrib::Position, 2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
                    .add(bgfx::Attrib::Color0, 4, bgfx::AttribType::Uint8, true)
                    .end();
            ImGui::StyleColorsDark();
        }

        void begin_frame(
                int32_t mouse_x, int32_t mouse_y, int width, int height,
                float deltaTime
        ) {
            auto &io         = ImGui::GetIO();
            io.DisplaySize.x = static_cast<float>(width);
            io.DisplaySize.y = static_cast<float>(height);
            io.DeltaTime     = deltaTime;

            io.AddMousePosEvent(
                    static_cast<float>(mouse_x), static_cast<float>(mouse_y)
            );

            ImGui::NewFrame();
        }

        void end_frame() {
            ImGui::Render();
            render(ImGui::GetDrawData());
        }

        render(ImGui::GetDrawData());
    };

    GUI::GUI()
        : impl_{std::make_unique<Impl>()} {
    }
}// namespace engine::ui
