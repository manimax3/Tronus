#pragma once
#include <tr.h>

#include "../event/CommonEvents.h"
#include "BufferHelper.h"
#include "StaticMesh.h"

#include <forward_list>

namespace tr {

class CameraComponent;

/**
 * Renderer used for forward rendering.
 */
class ForwardRenderer {
public:
    /**
     * Different available lighting models
     */
    enum RenderTechnique { PHONG, PHONG_BLINN };

    using Camera = Mat4;

    /**
     * Information ForwardRenderer needs to get a mesh onto the screen
     */
    struct RenderInfo {
        /**
         * The geometry data
         */
        ResourcePtr<StaticMesh> mesh;

        /**
         * The material used to render the mesh
         */
        ResourcePtr<PhongMaterial> material;

        /**
         * The model matrix in order to apply an transform to the mesh
         */
        Mat4 model;
    };

    /**
     * Internal representation of the renderable data
     */
    class RenderInfoHandle {
    public:
        friend class ForwardRenderer;

        /**
         * Construct a internal renderables based on the provided data
         */
        explicit RenderInfoHandle(RenderInfo info);

        /**
         * Disable the rendering of this Renderable
         */
        bool visible = true;

        /**
         * Enable to queue for destruction of this renderable
         */
        bool destroy = false;

    private:
        /**
         * Used to store some data.
         * May be in an invalid state (not useable for reuse)
         */
        RenderInfo mInfo;

        /**
         * The amount of indices in this mesh
         */
        size_t mIndexCount = 0;

        /**
         * The amount of vertices
         */
        size_t mVertexCount = 0;

        /**
         * The buffer representing the gpu state
         */
        detail::AttributBufferStore mBuffer;

        /**
         * Material used to render this mesh
         */
        ResourcePtr<Material> mMaterial;
    };

    /**
     * init the renderer
     */
    void Init(class GraphicsHandler &gfx);

    /**
     * process logic tasks
     */
    void Tick();

    /**
     * Renderer onto the screen
     */
    void Render();

    /**
     * cleanup
     */
    void Shutdown();

    /**
     * Tell the renderer about a new Renderable for rendering
     */
    RenderInfoHandle &CreateRenderInfo(RenderInfo info);

    /**
     * Sets the camera
     */
    void SetCamera(CameraComponent *cam) { mCamera = cam; };

private:
    /**
     * Pass to render the geometry onto the screen.
     */
    void GeometryPass();

    /**
     * Shader used for rendering with the phong model
     */
    ResourcePtr<GLSLShader> mPhongShader;

    /**
     * The currently active camera
     */
    CameraComponent *mCamera = nullptr;

    /**
     * The currently active renderables
     */
    std::list<RenderInfoHandle> mRenderables;
};
}
