#pragma once
#include "../event/CommonEvents.h"
#include "ForwardRenderer.h"
#include "GBuffer.h"
#include "GLSLShader.h"
#include "StaticMesh.h"

namespace tr {
class CameraComponent;

struct PointLight {
    Vec3 position;
    Vec3 color;

    float Constant;
    float Linear;
    float Quadratic;

    bool operator==(const PointLight &other)
    {
        return position == other.position && color == other.color
            && Constant == other.Constant && Linear == other.Linear
            && Quadratic == other.Quadratic;
    }
};

class DeferredRenderer {
public:
    void Init(class GraphicsHandler &gfx);
    void Render();
    void Tick(){};
    void OnEvent(const WindowEvent &e);

    void AddMesh(ResourcePtr<StaticMesh>    mesh,
                 ResourcePtr<PhongMaterial> material,
                 Mat4                       model);

    void RemoveMesh(const ResourcePtr<StaticMesh> &mesh);

    void AddPointLight(const PointLight &light);

    void RemovePointLight(const PointLight &light);

private:
    void  GeometryPass();
    void  StencilPass(const PointLight &light, const Mat4 &mvp);
    void  LightingPass(const PointLight &light, const Mat4 &mvp);
    void  FinalPass();
    float CalcPointLightScale(const PointLight &light);

    GBuffer mGBuffer;

    ResourcePtr<GLSLShader> mGeometryShader;
    ResourcePtr<GLSLShader> mLightingPoint;
    ResourcePtr<GLSLShader> mNullShader;
    ResourcePtr<GLSLShader> mFinalShader;

    ResourcePtr<StaticMesh> mLightSphere;

    CameraComponent *mCamera = nullptr;

    using Renderable
        = std::tuple<ResourcePtr<StaticMesh>, ResourcePtr<Material>, Mat4>;

    std::vector<Renderable> mRenderables;
    std::vector<PointLight> mPointLights;
};
}
