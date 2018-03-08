#pragma once
#include <tr.h>

namespace tr {
class GraphicsHandler;
class ResourceManager;
class GLSLShader;

class Simple2DRenderer {
public:
    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Render();
    void Shutdown();

private:
    GLSLShader *     mShader     = nullptr;
    GraphicsHandler *mGfxHandler = nullptr;
    ResourceManager *mResManager = nullptr;
    uint             mVao        = 0;
};
}
