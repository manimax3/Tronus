#pragma once
#include "../event/EventSystem.h"
#include "../math/Math.h"
#include "GLSLShader.h"
#include <stack>
#include <tr.h>

namespace tr {
class GraphicsHandler;
class ResourceManager;

class Renderer2D : public EventListener {
public:
    using Camera = Mat4;

    struct Vertex {
        Vec2 pos;
        Vec4 color;
        int tid;
        Vec2 uv;
    };

    struct Renderable {
        bool visible = true;
        bool dirty = false;
        Vec2 top_left;
        Vec2 top_right;
        Vec2 bottom_left;
        Vec2 bottom_right;
        Vec4 color;
        int tid = -1;
        Vec4 uv;

        friend class Renderer2D;

    private:
        uint64 id;
    };

    // How many rectangles get allocated when running out of vram
    static constexpr uint RENDERABLE_SIZE = 60000;
    static constexpr uint I_BUF_SIZE      = RENDERABLE_SIZE * 6;
    static constexpr uint V_BUF_SIZE = RENDERABLE_SIZE * sizeof(Vertex) * 4;
    static constexpr const char *SHADER_ID = "Renderer2D.json";

    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Tick();
    void Render();
    void Shutdown();

    Renderable *GetNewRenderable();
    void        DeleteRenderable(Renderable *r);

    void             OnEvent(const Event &e, int channel) override;
    std::vector<int> SubscripeTo() const override;

private:
    GLSLShader *            mShader      = nullptr;
    uint                    mVao         = 0;
    uint                    mVbo         = 0;
    uint                    mIbo         = 0;
    uint                    mRenderCount = 0;
    uint64                  mIdCounter   = 0;
    Camera                  mCamera      = Mat4::Identity();
    GraphicsHandler *       mGfxHandler  = nullptr;
    ResourceManager *       mResManager  = nullptr;
    Mat4                    mProjectionMatrix;
    std::vector<Renderable> mRenderables;
    std::vector<Vertex>     mUpdateBuffer;
};
}
