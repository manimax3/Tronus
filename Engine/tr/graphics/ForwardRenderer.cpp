#include "ForwardRenderer.h"
#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "../gameobject/CameraComponent.h"
#include "../util/Log.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

tr::ForwardRenderer::RenderInfoHandle::RenderInfoHandle(
    ForwardRenderer::RenderInfo info)
{
    if (!info.mesh || !info.material) {
        throw std::runtime_error("Couldnt create RenderInfoHandle because "
                                 "invalid RenderInfo provided");
    }

    mBuffer.Create();
    mBuffer.Bind();

    detail::Buffer vertex(BufferType::Vertex, BufferLocality::Static);
    detail::Buffer index(BufferType::Index, BufferLocality::Static);

    vertex.Create(sizeof(Vertex_PNTBU), info.mesh->GetVertexCount(),
                  static_cast<void *>(info.mesh->GetVertices()));

    index.Create(sizeof(uint), info.mesh->GetIndexCount(),
                 static_cast<void *>(info.mesh->GetIndices()));

    auto layout = StaticMesh::GetVertexBufferLayout();
    layout.ApplyTo(vertex);

    mBuffer.AddBuffer(vertex);
    mBuffer.AddBuffer(index);
    vertex.Bind();
    index.Bind();

    mBuffer.Unbind();

    mIndexCount  = info.mesh->GetIndexCount();
    mVertexCount = info.mesh->GetVertexCount();

    info.mesh.reset();
    mMaterial = std::move(info.material);
    model     = info.model;
}

void tr::ForwardRenderer::Init(GraphicsHandler &gfx)
{
    if (!gfx.Context().valid) {
        Log().warn("Couldnt init the ForwardRenderer because of a missing "
                   "rendering context");
        return;
    }

    try {
        mPhongShader = ResCast<GLSLShader>(
            tr::Engine::Get().sResourceManager->LoadResource(
                "ForwardRenderer.json"));
    } catch (const ResourceNotLoadedError &e) {
        Log().error(
            "Couldnt get the Shader Resource for the ForwardRenderer | {}",
            e.what());
        return;
    }

    gfx.CameraUpdate.connect([&](CameraComponent *cam, bool reset) {
        if (mCamera == cam && reset) {
            mCamera = nullptr;
        }
        mCamera = cam;
    });
}

void tr::ForwardRenderer::Tick()
{
    for (auto b = std::begin(mRenderables); b != std::end(mRenderables); b++) {
        if (b->destroy) {
            b->mBuffer.Destroy();
            mRenderables.erase(b);
            continue;
        }
    }
}

void tr::ForwardRenderer::Shutdown()
{
    mRenderables.clear();
    // TODO: Delete the shader maybe?
    mPhongShader.reset();
}

void tr::ForwardRenderer::Render() { GeometryPass(); }

void tr::ForwardRenderer::GeometryPass()
{

    if (!mCamera)
        return;

    for (auto &info : mRenderables) {
        if (!info.visible)
            continue;
        const auto mvp
            = mCamera->GetProjection() * mCamera->GetView() * info.model;
        mPhongShader->Bind();
        mPhongShader->Set("mvp", mvp);
        auto &shader = *mPhongShader;
        info.mMaterial->Bind(shader);
        info.mBuffer.Bind();
        Call(glDrawElements(GL_TRIANGLES, info.mIndexCount, GL_UNSIGNED_INT,
                            (void *)0));
    }
}

tr::ForwardRenderer::RenderInfoHandle &
tr::ForwardRenderer::CreateRenderInfo(RenderInfo info)
{
    ShaderInterface interface;
    StaticMesh::GetVertexBufferLayout().AddAttributesToShaderInterface(
        interface);
    info.material->AddUniformsToInterface(interface);
    interface.AddUniform(ShaderElementType::Mat4, "mvp");

    if (mPhongShader->GetInterface().has_value()) {
        if (!(interface.IsCompatibleWith(*mPhongShader->GetInterface()))) {
            Log().warn(
                "Couldnt add RenderInfo to the ForwardRenderer because the "
                "shader is incompatible with the required interface.");
            throw ShaderInterfaceException(
                "Provided RenderInfo not compatible with PhongShader");
        }
    }

    mRenderables.emplace_back(std::move(info));
    return mRenderables.back();
}

