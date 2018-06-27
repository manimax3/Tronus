#pragma once
#include <tr.h>

#include "../filesystem/ResourceManager.h"
#include "GLSLShader.h"

namespace tr {
class GraphicsHandler;

class Simple2DRenderer {
public:
    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Render();
    void Shutdown();

private:
    ResourcePtr<GLSLShader> mShader     = nullptr;
    GraphicsHandler *       mGfxHandler = nullptr;
    ResourceManager *       mResManager = nullptr;
    uint                    mVao        = 0;
};
}
