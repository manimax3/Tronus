#pragma once
#include "../core/Engine.h"
#include "../math/Math.h"
#include "../util/MPSCQueue.h"
#include <tr.h>

namespace tr {
class GfxCommand;
class RenderContext;
class GraphicsHandler : public Subsystem<GraphicsHandler> {
public:
    bool Initialize(Engine *e) override;
    bool Tick() override;
    bool Shutdown() override;

    bool SubmitCommand(std::unique_ptr<GfxCommand> &&command);

    inline RenderContext *Context() { return mContext; }

    friend class CreateWindowCmd;

private:
    MPSCQueue<GfxCommand *> mGfxCommandBuffer;
    MPSCQueue<GfxCommand *> mMainCommandBuffer;

    RenderContext *mContext = nullptr;

    void render();
};

class GfxCommand {
public:
    explicit GfxCommand(bool requires_main = false);
    virtual ~GfxCommand()          = default;
    GfxCommand(const GfxCommand &) = default;
    GfxCommand(GfxCommand &&)      = default;
    GfxCommand &operator=(const GfxCommand &) = default;
    GfxCommand &operator=(GfxCommand &&) = default;

    virtual void Execute(GraphicsHandler *handler) = 0;

    inline bool RequiresMain() const { return mRequiresMain; }

private:
    bool mRequiresMain = false;
};

struct CreateWindowCmd : public GfxCommand {
    CreateWindowCmd();
    void Execute(GraphicsHandler *handler) override;

    Vec2        Size       = { 1280, 720 };
    std::string Name       = "Tronus Engine";
    bool        Fullscreen = false;
    bool        VSync      = false;
};
}
