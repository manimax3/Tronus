#include "ForwardRenderer.h"
#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "../util/Log.h"

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

    detail::Buffer vertex(detail::BufferType::VERTEX, detail::Buffer::STATIC);
    detail::Buffer index(detail::BufferType::INDEX, detail::Buffer::STATIC);

    vertex.Create(sizeof(Vertex_PNTBU), info.mesh->GetVertexCount(),
                  static_cast<void *>(info.mesh->GetVertices()));

    index.Create(sizeof(uint), info.mesh->GetIndexCount(),
                 static_cast<void *>(info.mesh->GetIndices()));

    auto layout = StaticMesh::GetVertexBufferLayout();
    layout.ApplyTo(vertex);

    mBuffer.AddBuffer(vertex);
    mBuffer.AddBuffer(index);

    mBuffer.Unbind();

    info.mesh.reset();
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

    // TODO: Seems obvious doesnt it?
    mProjection = math::perspective(
        math::radians(45.f), gfx.GetWindowSize().x / gfx.GetWindowSize().y,
        0.1f, 100.f);
}

void tr::ForwardRenderer::OnEvent(const WindowEvent &e) {
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
}


