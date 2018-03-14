#pragma once
#include "../core/Subsystem.h"
#include "../event/EventSystem.h"
#include "../math/Math.h"
#include "ImguiRenderer.h"
#include "Renderer2D.h"
#include "Simple2DRenderer.h"
#include <memory>
#include <queue>
#include <tr.h>

namespace tr {

class GfxCommand;
class Engine;

struct RenderContext {
    void *window = nullptr;
    bool  valid  = false;
};

class GraphicsHandler : public Subsystem<GraphicsHandler> {
public:
    bool               Initialize(Engine *e) override;
    void               PostInit() override;
    bool               Tick() override;
    bool               Shutdown() override;
    inline std::string GetName() const override { return "GraphicsHandler"; };

    bool SubmitCommand(std::unique_ptr<GfxCommand> &&command);

    inline RenderContext &Context() { return mContext; }

    bool Valid() const;

    void Render();

    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    // Input functionality
    const char *GetClipboard() const;
    void        SetClipboard(const std::string &c);

    double GetTime() const;

    Vec2 GetWindowSize() const;

    inline Renderer2D &GetRenderer2D() { return mRenderer2D; }

    friend class CreateWindowCmd;

private:
    std::queue<GfxCommand *> mGfxCommandBuffer;
    class CreateWindowCmd *  mWindowCmd = nullptr;

    RenderContext    mContext;
    Simple2DRenderer mSimpleRenderer2D;
    Renderer2D       mRenderer2D;
    ImguiRenderer    mImguiRenderer;
};

struct GfxCommand {
    explicit GfxCommand(bool delete_after_exec = true)
        : DeleteAfterExecution(delete_after_exec)
    {
    }

    virtual ~GfxCommand()          = default;
    GfxCommand(const GfxCommand &) = default;
    GfxCommand(GfxCommand &&)      = default;
    GfxCommand &operator=(const GfxCommand &) = default;
    GfxCommand &operator=(GfxCommand &&) = default;

    virtual void Execute(GraphicsHandler *handler) = 0;

    bool DeleteAfterExecution;
};

struct CreateWindowCmd : public GfxCommand {
    void Execute(GraphicsHandler *handler) override;

    Vec2        Size          = { 1280, 720 };
    std::string Name          = "Tronus Engine";
    bool        Fullscreen    = false;
    bool        Resizeable    = true;
    bool        VSync         = false;
    Vec4        ClearColor    = { 0.2f, 0.3f, 0.3f, 1.0f };
    Vec2        OpenGLVersion = { 3, 3 };
};
}
