#pragma once
#include <tr.h>

#include "GraphicsHandler.h"
#include "StaticMesh.h"
#include "BufferHelper.h"

namespace tr {
/**
 * Renderer used for forward rendering.
 */
class ForwardRenderer {
public:
    enum RenderTechnique { PHONG, PHONG_BLINN };
    using Camera = Mat4;

    void Init(GraphicsHandler *gfx, ResourceManager *rm);
    void Tick();
    void Render();
    void Shutdown();
    void OnEvent(const WindowEvent &e);

    int AddPhongMesh(const StaticMesh &   mesh,
                     const PhongMaterial &material,
                     Mat4                 model_matrix = Mat4(),
                     bool                 enable       = true,
                     bool                 use_blinn    = false);

    void RemoveMesh(int id);

    void ModifyRenderable(int id, Mat4 model_matrix);
    void ModifyRenderable(int id, bool enabled);

private:
    ResourcePtr<GLSLShader> mShader;
    Camera                  mCamera;
};
}
