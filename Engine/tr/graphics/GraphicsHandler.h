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

class Engine;

struct CreateWindowInfo {
    Vec2        Size          = { 1280, 720 };
    std::string Name          = "Tronus Engine";
    bool        Fullscreen    = false;
    bool        Resizeable    = true;
    bool        VSync         = false;
    Vec4        ClearColor    = { 0.2f, 0.3f, 0.3f, 1.0f };
    Vec2        OpenGLVersion = { 3, 3 };
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

    std::vector<int> SubscripeTo() const override;
    void             OnEvent(const Event &e, int channel) override;

    // Input functionality
    const char *GetClipboard() const;
    void        SetClipboard(const std::string &c);

    double GetTime() const;

    Vec2 GetWindowSize() const;
    Vec2 GetMousePos() const;

    inline Renderer2D &GetRenderer2D() { return mRenderer2D; }

private:
    RenderContext    mContext;
    Simple2DRenderer mSimpleRenderer2D;
    Renderer2D       mRenderer2D;
    ImguiRenderer    mImguiRenderer;
};
}
