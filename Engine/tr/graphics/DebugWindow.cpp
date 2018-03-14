#include "DebugWindow.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../util/Keys.h"
#include "Texture.h"
#include "imgui.h"

tr::DebugWindow::DebugWindow(const Engine &engine)
    : mEngine(engine)
{
    engine.sEventSystem->AddListener(this);
}

void tr::DebugWindow::OnEvent(const Event &e, int channel)
{
    if (e.Identifier == event::RENDER_DEBUG) {
        if (mShouldDraw)
            draw();
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
            renderer2d_enable_test_window(!render2d_test_open);
        }
    }
    ImGui::End();
}

void tr::DebugWindow::renderer2d_enable_test_window(bool disable)
{
    auto *gfx = mEngine.sGraphicsHandler;
    auto &r2d = gfx->GetRenderer2D();

    static Renderer2D::Renderable *r = nullptr;
    if (!r && !disable) {
        r = r2d.GetNewRenderable();
        mEngine.sResourceManager->LoadResource("test_texture.json");
        auto *t
            = mEngine.sResourceManager->GetRes<Texture>("test_texture.json");

        r2d.PushTexture(t);
        r->top_left     = { 0.f, 0.f };
        r->top_right    = { gfx->GetWindowSize().x, 0.f };
        r->bottom_left  = { 0.f, gfx->GetWindowSize().y };
        r->bottom_right = { gfx->GetWindowSize().x, gfx->GetWindowSize().y };
        r->color        = Vec4(1.f, 1.f, 1.f, 1.f);
        r->uv           = Vec4(0, 0, 1, 1);
        r->visible      = true;
    } else if (r && disable) {
        r->visible = false;
    } else if (r && !disable) {
        r->visible = true;
    }
}
