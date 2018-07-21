#pragma once
#include <array>
#include <tr.h>

#include "../event/CommonEvents.h"

namespace tr {
class Engine;
class DebugWindow {
public:
    DebugWindow(Engine &engine);

    void OnRenderDebugEvent();
    void OnEvent(const InputEvent &e);

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
