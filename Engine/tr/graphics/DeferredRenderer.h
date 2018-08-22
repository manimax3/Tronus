#pragma once
#include "../event/CommonEvents.h"
#include "GBuffer.h"
#include "GLSLShader.h"

namespace tr {
class DeferredRenderer {
public:
    void Init(class GraphicsHandle &gfx);
    void Render();
    void Tick();
    void OnEvent(const WindowEvent &e);

private:
    GBuffer mGBuffer;

    ResourcePtr<GLSLShader> mGeometryShader;
    ResourcePtr<GLSLShader> mLightingShader;
    ResourcePtr<GLSLShader> mFinalShader;
};
}
