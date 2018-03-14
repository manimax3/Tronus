#pragma once
#include "../event/EventSystem.h"
#include <tr.h>

namespace tr {
class Engine;
class DebugWindow : public EventListener {
public:
    DebugWindow(const Engine &engine);

    std::vector<int> SubscripeTo() const override { return { ENGINE_CHANNEL }; }
    void             OnEvent(const Event &e, int channel) override;

private:
    void draw();

    const Engine &mEngine;
    bool          mShouldDraw = false;
};
}
