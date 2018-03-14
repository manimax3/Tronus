#pragma once
#include "../event/EventSystem.h"
#include <tr.h>

namespace tr {
class Engine;
class DebugWindow : public EventListener {
public:
    DebugWindow(const Engine &engine);

    std::vector<int> SubscripeTo() const override
    {
        return { ENGINE_CHANNEL, RENDER_CHANNEL };
    }
    void             OnEvent(const Event &e, int channel) override;

private:
    void draw();

    bool render2d_test_open = false;
    void renderer2d_enable_test_window(class Renderer2D &ren);

    const Engine &mEngine;
    bool          mShouldDraw = false;
};
}
