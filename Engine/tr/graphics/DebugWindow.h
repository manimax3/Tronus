#pragma once
#include "../event/EventSystem.h"
#include <tr.h>

namespace tr {
class Engine;
class DebugWindow : public EventListener {
public:
    DebugWindow(Engine &engine);

    std::vector<int> SubscripeTo() const override
    {
        return { ENGINE_CHANNEL, RENDER_CHANNEL };
    }
    void OnEvent(const Event &e, int channel) override;

private:
    void draw();

    bool render2d_test_open = false;
    void renderer2d_enable_test_window(class Renderer2D &ren);

    bool im_demo_window   = false;
    bool world_debug_open = false;
    bool rm_debug_open    = false;
    bool rm_load_from_mem = false;
    bool rm_load_async    = false;
    void rm_debug_window();

    Engine &mEngine;
    bool    mShouldDraw = false;

    std::array<char, 512> buffer = { 0 };
};
}
