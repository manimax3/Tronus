#pragma once
#include "../core/Engine.h"
#include "../math/Math.h"
#include <queue>
#include <tr.h>

namespace tr {

class GfxCommand;

struct RenderContext {
    void *window = nullptr;
    bool  valid  = false;
};

class GraphicsHandler : public Subsystem<GraphicsHandler> {
public:
    bool Initialize(Engine *e) override;
    bool Tick() override;
    bool Shutdown() override;

    bool SubmitCommand(std::unique_ptr<GfxCommand> &&command);

    inline RenderContext &Context() { return mContext; }

    bool Valid() const;

    void Render();

    friend class CreateWindowCmd;

private:
    std::queue<GfxCommand *> mGfxCommandBuffer;

    RenderContext mContext;
};

struct GfxCommand {
    explicit GfxCommand()          = default;
    virtual ~GfxCommand()          = default;
    GfxCommand(const GfxCommand &) = default;
    GfxCommand(GfxCommand &&)      = default;
    GfxCommand &operator=(const GfxCommand &) = default;
    GfxCommand &operator=(GfxCommand &&) = default;

    virtual void Execute(GraphicsHandler *handler) = 0;
};

struct CreateWindowCmd : public GfxCommand {
    void Execute(GraphicsHandler *handler) override;

    Vec2        Size       = { 1280, 720 };
    std::string Name       = "Tronus Engine";
    bool        Fullscreen = false;
    bool        Resizeable = true;
    bool        VSync      = true;
    Vec4        ClearColor = { 0.2f, 0.3f, 0.3f, 1.0f };
};

struct CloseWindowCmd : public GfxCommand {
    void Execute(GraphicsHandler *handler) override;
};
}
