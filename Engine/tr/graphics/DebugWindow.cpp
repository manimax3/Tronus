#include "DebugWindow.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../util/Keys.h"
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
    }
    ImGui::End();
}
