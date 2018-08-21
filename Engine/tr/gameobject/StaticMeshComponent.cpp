#include "StaticMeshComponent.h"
#include "../core/Engine.h"
#include "../graphics/ForwardRenderer.h"
#include "../graphics/GraphicsHandler.h"
#include "../util/Log.h"

void tr::StaticMeshComponent::SetVisibility(bool visible)
{
    if (mRendererHandle && Visibility != visible) {
        ForwardRenderer::RenderInfoHandle &h
            = *static_cast<ForwardRenderer::RenderInfoHandle *>(
                mRendererHandle);
        h.visible = visible;
    }
    Visibility = visible;
}

void tr::StaticMeshComponent::PostWorldEnter()
{

    auto mesh = ResCast<StaticMesh>(Mesh.lock());
    auto mat  = ResCast<PhongMaterial>(Material.lock());

    if (!mesh || !mat) {
        Log().warn("Couldnt add StaticMeshComponent to the renderer because "
                   "mesh or material were not provided.");
        return;
    }

    ForwardRenderer::RenderInfo info;
    info.mesh     = std::move(mesh);
    info.material = std::move(mat);
    info.model    = GetAbsoluteTransform();

    try {
        ForwardRenderer::RenderInfoHandle &handle
            = tr::Engine::Get()
                  .sGraphicsHandler->GetForwardRenderer()
                  .CreateRenderInfo(std::move(info));

        mRendererHandle = static_cast<void *>(&handle);
    } catch (const std::runtime_error &e) {
        Log().warn("Couldnt add StaticMeshComponent to ForwardRenderer | ",
                   e.what());
        return;
    }
}

void tr::StaticMeshComponent::OnRelativeChange()
{
    if (mRendererHandle) {
        ForwardRenderer::RenderInfoHandle &h
            = *static_cast<ForwardRenderer::RenderInfoHandle *>(
                mRendererHandle);
        h.model = GetAbsoluteTransform();
    }
}
