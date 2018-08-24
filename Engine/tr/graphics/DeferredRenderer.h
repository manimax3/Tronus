#pragma once
#include "../event/CommonEvents.h"
#include "ForwardRenderer.h"
#include "GBuffer.h"
#include "GLSLShader.h"
#include "StaticMesh.h"

namespace tr {
class CameraComponent;

/**
 * Represents a point light
 */
struct PointLight {
    /**
     * Lights position
     */
    Vec3 position;

    /**
     * Lights color
     */
    Vec3 color;

    /**
     * Constant attenuation part.
     */
    float Constant = 1.f;

    /**
     * Linear attenuation part.
     */
    float Linear = 0.09f;

    /**
     * Quadratic attenuation part.
     */
    float Quadratic = 0.032f;

    /**
     * Equal comparison
     */
    bool operator==(const PointLight &other)
    {
        return position == other.position && color == other.color
            && Constant == other.Constant && Linear == other.Linear
            && Quadratic == other.Quadratic;
    }
};

/**
 * 3D renderer using deferred shading.
 */
class DeferredRenderer {
public:
    /**
     * Init the renderer
     */
    void Init(class GraphicsHandler &gfx);

    /**
     * Render the scene
     */
    void Render();

    /**
     * Update
     */
    void Tick(){};

    /**
     * Handle window event.
     * Used for handling resizing
     */
    void OnEvent(const WindowEvent &e);

    /**
     * Adds a new mesh to the scene
     * @param[in] the mesh to render (Gets uploaded to the gpu)
     * @param[in] The material used to render the mesh
     * @param model Model matrix of the mesh
     */
    void AddMesh(ResourcePtr<StaticMesh>    mesh,
                 ResourcePtr<PhongMaterial> material,
                 Mat4                       model);

    /**
     * Removes a mesh
     */
    void RemoveMesh(const ResourcePtr<StaticMesh> &mesh);

    /**
     * Updates the model matrix of mesh
     */
    void UpdateModel(const ResourcePtr<StaticMesh> &mesh, Mat4 model);

    /**
     * Adds a point light to the scene
     */
    void AddPointLight(const PointLight &light);

    /**
     * Removes a point light from the scene.
     */
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

    /**
     * Used to render the point lights
     */
    ResourcePtr<StaticMesh> mLightSphere;

    CameraComponent *mCamera = nullptr;

    using Renderable
        = std::tuple<ResourcePtr<StaticMesh>, ResourcePtr<Material>, Mat4>;

    std::vector<Renderable> mRenderables;
    std::vector<PointLight> mPointLights;
};
}
