#pragma once
#include "../filesystem/ResourceManager.h"
#include <tr.h>

namespace tr {
class GraphicsHandler;
class GLSLShader;

class Simple2DRenderer {
public:
    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Render();
    void Shutdown();

private:
    ResHandle<GLSLShader> mShader     = nullptr;
    GraphicsHandler *     mGfxHandler = nullptr;
    ResourceManager *     mResManager = nullptr;
    uint                  mVao        = 0;
};
}
