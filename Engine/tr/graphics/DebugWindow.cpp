#include "DebugWindow.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "../gameobject/World.h"
#include "../util/Keys.h"
#include "GraphicsHandler.h"
#include "Renderer2D.h"
#include "Texture.h"
#include "imgui.h"

#include <sstream>

tr::DebugWindow::DebugWindow(Engine &engine)
    : mEngine(engine)
{
    auto *gfx = engine.sGraphicsHandler;
    gfx->InputRecieved.connect(InputLayer::DEBUG,
                               [&](const InputEvent &e) { this->OnEvent(e); });

    gfx->GetImguiRenderer().DebugFrameStarted.connect(
        [&]() { this->OnRenderDebugEvent(); });
}

void tr::DebugWindow::OnRenderDebugEvent()
{
    /* if (channel == RENDER_CHANNEL && e.Identifier == event::RENDER_DEBUG) {
     */
    if (mShouldDraw)
        draw();
    /* } else if (channel == RENDER_CHANNEL && e.Identifier == event::RENDER_2D
     */
    /*            && render2d_test_open) { */

    /*     renderer2d_enable_test_window( */
    /*         static_cast<const Render2DEvent &>(e).renderer); */

    /* } */
}

void tr::DebugWindow::OnEvent(const InputEvent &e)
{
    if (e.type == InputEvent::Keyboard && e.Key == KEY_F3) {
        if (e.action == InputEvent::REPEAT || e.action == InputEvent::RELEASE)
            return;
        else
            mShouldDraw = !mShouldDraw;
    }
}

void tr::DebugWindow::draw()
{

    if (ImGui::Begin("Tronus Debug", &mShouldDraw)) {
        ImGui::Text("UPS: %i FPS: %i", mEngine.mLastUps, mEngine.mLastFps);
        ImGui::Text("msPF: %f", 1000.f / mEngine.mLastFps);
        if (ImGui::Button("Renderer2D Test")) {
            render2d_test_open = !render2d_test_open;
        }
        ImGui::Checkbox("ImGui Demo", &im_demo_window);
        ImGui::Checkbox("World Debug", &world_debug_open);
        ImGui::Checkbox("ResManager Debug", &rm_debug_open);
    }
    ImGui::End();

    /* if (world_debug_open) */
    /*     mEngine.mWorld->RenderDebug(); */

    if (rm_debug_open)
        rm_debug_window();

    if (im_demo_window)
        ImGui::ShowDemoWindow();
}

tr::Vec4 rand_color()
{
    float r = (rand() % 255) / 255.f;
    float g = (rand() % 255) / 255.f;
    float b = (rand() % 255) / 255.f;
    return { r, g, b, 1.f };
}

void tr::DebugWindow::renderer2d_enable_test_window(Renderer2D &renderer)
{

    static std::vector<Renderer2D::Renderable> r_list;
    static bool                                init = false;

    if (!init) {
        Renderer2D::Renderable r;
        /* int                    c = 0; */
        for (float y = 0; y < mEngine.sGraphicsHandler->GetWindowSize().y;
             y += 10)
            for (float x = 0; x < mEngine.sGraphicsHandler->GetWindowSize().x;
                 x += 10) {
                r.top_left     = { x, y };
                r.top_right    = { x + 10, y };
                r.bottom_left  = { x, y + 10 };
                r.bottom_right = { x + 10, y + 10 };
                r.visible      = true;
                r.color        = rand_color();

                r_list.push_back(r);
            }
        init = true;
    } else {
        for (auto &r : r_list)
            renderer.Submit(r);
    }
}

void tr::DebugWindow::rm_debug_window()
{
    if (ImGui::Begin("Resource Manager")) {

        if (ImGui::InputText("Load", &buffer[0], buffer.size(),
                             ImGuiInputTextFlags_EnterReturnsTrue)) {

            std::stringstream stream;
            stream.write(buffer.data(), strlen(buffer.data()));
            try {
                if (rm_load_from_mem)
                    mEngine.sResourceManager->LoadResource(stream);
                else
                    mEngine.sResourceManager->LoadResource(stream.str());
            } catch (const ResourceNotLoadedError &e) {
            }
            std::memset(buffer.data(), 0, buffer.size());
        }

        ImGui::Checkbox("Mem", &rm_load_from_mem);

        for (const auto &[name, id] :
             mEngine.sResourceManager->mResourceIDLookup) {
            ImGui::Text("%s", name.c_str());
        }
    }
    ImGui::End();
}
