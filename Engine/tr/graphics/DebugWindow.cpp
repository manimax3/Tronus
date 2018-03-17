#include "DebugWindow.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../util/Keys.h"
#include "Texture.h"
#include "imgui.h"

tr::DebugWindow::DebugWindow(Engine &engine)
    : mEngine(engine)
{
    engine.sEventSystem->AddListener(this);
}

void tr::DebugWindow::OnEvent(const Event &e, int channel)
{
    if (channel == RENDER_CHANNEL && e.Identifier == event::RENDER_DEBUG) {
        if (mShouldDraw)
            draw();
    } else if (channel == RENDER_CHANNEL && e.Identifier == event::RENDER_2D
               && render2d_test_open) {

        renderer2d_enable_test_window(
            static_cast<const Render2DEvent &>(e).renderer);

    } else if (e.Identifier == event::INPUT) {
        auto ie = static_cast<const InputEvent &>(e);
        if (ie.type == InputEvent::Keyboard && ie.Key == KEY_F3) {
            if (ie.action == InputEvent::REPEAT
                || ie.action == InputEvent::RELEASE)
                return;
            else
                mShouldDraw = !mShouldDraw;
        }
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
        ImGui::Checkbox("World Debug", &world_debug_open);
    }
    ImGui::End();

    if (world_debug_open)
        mEngine.mWorld.RenderDebug();
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
        int                    c = 0;
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
