#pragma once
#include "../event/CommonEvents.h"
#include "../math/Math.h"
#include "../util/MakeID.h"
#include "GLSLShader.h"
#include <stack>
#include <tr.h>
#include <variant>

namespace tr {
class GraphicsHandler;
class ResourceManager;
class Texture;

class Renderer2D {
public:
    using Camera = Mat4;

    struct Vertex {
        Vec2 pos;
        Vec4 color;
        Vec2 uv;
    };

    struct Renderable {
        bool visible = true;

        Vec2 top_left;
        Vec2 top_right;
        Vec2 bottom_left;
        Vec2 bottom_right;

        Vec4     color   = Vec4(1.f);
        Vec4     uv      = { 0.f, 0.f, 1.f, 1.f };
        Texture *texture = nullptr;

        int layer = 0;

        friend class Renderer2D;

    private:
        uint id;
    };

    Renderer2D();

    // How many rectangles get allocated when running out of vram
    static constexpr uint RENDERABLE_SIZE = 60000;
    static constexpr uint I_BUF_SIZE      = RENDERABLE_SIZE * 6;
    static constexpr uint V_BUF_SIZE = RENDERABLE_SIZE * sizeof(Vertex) * 4;
    static constexpr const char *SHADER_ID = "Renderer2D.json";

    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Tick();
    void Render();
    void Shutdown();

    uint        SubmitRenderable(const Renderable &r);
    void        DeleteRenderable(uint renderable);
    Renderable *ModifyRenderable(uint r);

    void StartFrame();
    void Submit(const Renderable &r);
    void EndFrame();

    void PushTexture(Texture *const tex);

    void RenderRenderables();

    void OnEvent(const WindowEvent &e);

private:
    ResourcePtr<GLSLShader> mShader      = nullptr;
    uint                    mVao         = 0;
    uint                    mVbo         = 0;
    uint                    mIbo         = 0;
    uint                    mRenderCount = 0;
    uint64                  mIdCounter   = 0;
    Camera                  mCamera;
    GraphicsHandler *       mGfxHandler    = nullptr;
    ResourceManager *       mResManager    = nullptr;
    Vertex *                mBufferAccess  = nullptr;
    Texture *               mCurrenTexture = nullptr;
    Mat4                    mProjectionMatrix;
    std::vector<Renderable> mRenderables;
    MakeID                  mIDGenerator;
};
}
