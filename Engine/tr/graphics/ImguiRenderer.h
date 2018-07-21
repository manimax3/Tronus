#pragma once
#include "../event/CommonEvents.h"
#include "../event/Signal.h"
#include "../math/Math.h"
#include "GLSLShader.h"
#include <tr.h>

class ImDrawData;
namespace tr {
class GraphicsHandler;
class ResourceManager;
class ImguiRenderer {
public:
    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Shutdown();

    void Render();
    void StartDebugFrame();

    void OnEvent(const WindowEvent &e);
    void OnEvent(const InputEvent &e);

    Signal<void()> DebugFrameStarted;

private:
    static constexpr const char *SHADER_ID  = "ImguiRenderer.json";
    static constexpr const char *TEXTURE_ID = "imgui_renderer_texture";
    static constexpr const char *IMAGE_ID   = "imgui_renderer_image";

    void draw_data(ImDrawData *draw_data);

    GraphicsHandler *       mGfxHandler = nullptr;
    ResourceManager *       mResManager = nullptr;
    ResourcePtr<GLSLShader> mShader     = nullptr;
    uint                    mVao        = 0;
    uint                    mVbo        = 0;
    uint                    mIbo        = 0;
    Mat4                    mProjectionMatrix;
};
}
