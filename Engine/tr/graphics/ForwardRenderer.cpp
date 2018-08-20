#include "ForwardRenderer.h"
#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "../util/Log.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"

#include "glad/glad.h"
#include "glm/gtc/matrix_transform.hpp"

tr::ForwardRenderer::RenderInfoHandle::RenderInfoHandle(
    ForwardRenderer::RenderInfo info)
    : mInfo(std::move(info))
{
    if (!info.mesh || !info.material) {
        throw std::runtime_error("Couldnt create RenderInfoHandle because "
                                 "invalid RenderInfo provided");
    }

    mBuffer.Create();
    mBuffer.Bind();

    detail::Buffer vertex(BufferType::Vertex, BufferLocality::Static);
    detail::Buffer index(BufferType::Index, BufferLocality::Static);

    vertex.Create(sizeof(Vertex_PNTBU), mInfo.mesh->GetVertexCount(),
                  static_cast<void *>(mInfo.mesh->GetVertices()));

    index.Create(sizeof(uint), mInfo.mesh->GetIndexCount(),
                 static_cast<void *>(mInfo.mesh->GetIndices()));

    auto layout = StaticMesh::GetVertexBufferLayout();
    layout.ApplyTo(vertex);

    mBuffer.AddBuffer(vertex);
    mBuffer.AddBuffer(index);

    mBuffer.Unbind();

    mIndexCount  = mInfo.mesh->GetIndexCount();
    mVertexCount = mInfo.mesh->GetVertexCount();

    mInfo.mesh.reset();
    mMaterial = std::move(info.material);
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

    mCamera = Mat4();

    gfx.WindowChanged.connect([&](const WindowEvent &e) { this->OnEvent(e); });

    // TODO: Seems obvious doesnt it?
    mProjection = math::perspective(
        math::radians(45.f), gfx.GetWindowSize().x / gfx.GetWindowSize().y,
        0.1f, 100.f);
}

void tr::ForwardRenderer::OnEvent(const WindowEvent &e)
{
    if (e.type != WindowEvent::RESIZED)
        return;

    // TODO: See @ForwardRenderer.cpp:61
    mProjection = math::perspective(
        math::radians(45.f),
        static_cast<float>(e.xSize) / static_cast<float>(e.ySize), 0.1f, 100.f);
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
    for (auto &info : mRenderables) {
        if (!info.visible)
            continue;
        const auto mvp = mProjection * mCamera * info.mInfo.model;
        mPhongShader->Bind();
        mPhongShader->Set("mvp", mvp);
        info.mMaterial->Bind(*mPhongShader);
        info.mBuffer.Bind();
        Call(glDrawElements(GL_TRIANGLES, info.mVertexCount, GL_UNSIGNED_INT,
                            (void *)nullptr));
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
        if (!(mPhongShader->GetInterface()->IsCompatibleWith(interface))) {
            throw std::runtime_error("Incompatible shaders");
        }
    }

    mRenderables.emplace_back(std::move(info));
    return mRenderables.back();
}

