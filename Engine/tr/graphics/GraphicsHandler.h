#pragma once
#include "../core/Subsystem.h"
#include "../event/CommonEvents.h"
#include "../event/Signal.h"
#include "../math/Math.h"
#include "DeferredRenderer.h"
#include "ForwardRenderer.h"
#include "ImguiRenderer.h"
#include "Renderer2D.h"
#include "Simple2DRenderer.h"
#include <memory>
#include <queue>
#include <tr.h>

namespace tr {

class Engine;
class CameraComponent;

struct CreateWindowInfo {
    Vec2        Size          = { 1280, 720 };
    std::string Name          = "Tronus Engine";
    bool        Fullscreen    = false;
    bool        Resizeable    = true;
    bool        VSync         = false;
    Vec4        ClearColor    = { 0.f, 0.f, 0.f, 1.f};
    Vec2        OpenGLVersion = { 4, 3 };
};

struct RenderContext {
    void *           window = nullptr;
    bool             valid  = false;
    CreateWindowInfo windowInfo;
    Vec2             lastMousePos;
};

class GraphicsHandler : public Subsystem<GraphicsHandler> {
public:
    bool               Initialize(Engine *e) override;
    void               PostInit() override;
    bool               Tick() override;
    bool               Shutdown() override;
    inline std::string GetName() const override { return "GraphicsHandler"; };

    void CreateWindow(const CreateWindowInfo &info);

    inline RenderContext &Context() { return mContext; }
    inline bool           Valid() const { return mContext.valid; }

    void Render();

    // Input functionality
    const char *GetClipboard() const;
    void        SetClipboard(const std::string &c);

    double GetTime() const;

    Vec2 GetWindowSize() const;
    Vec2 GetMousePos() const;

    inline Renderer2D &      GetRenderer2D() { return mRenderer2D; }
    inline ImguiRenderer &   GetImguiRenderer() { return mImguiRenderer; }
    inline ForwardRenderer & GetForwardRenderer() { return mForwardRenderer; }
    inline DeferredRenderer &GetDeferredRenderer() { return mDefferedRenderer; }

    Signal<void(const InputEvent &)>      InputRecieved;
    Signal<void(const WindowEvent &)>     WindowChanged;
    Signal<void(CameraComponent *, bool)> CameraUpdate;

private:
    void HandleInputEvent(const InputEvent &e);
    void HandleWindowEvent(const WindowEvent &e);

private:
    RenderContext    mContext;
    Simple2DRenderer mSimpleRenderer2D;
    Renderer2D       mRenderer2D;
    ImguiRenderer    mImguiRenderer;
    ForwardRenderer  mForwardRenderer;
    DeferredRenderer mDefferedRenderer;
};
}
