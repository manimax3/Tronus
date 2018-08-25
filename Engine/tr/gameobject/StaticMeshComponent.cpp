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

    tr::Engine::Get().sGraphicsHandler->GetDeferredRenderer().AddMesh(
        std::move(mesh), std::move(mat), GetAbsoluteTransform());
}

void tr::StaticMeshComponent::OnRelativeChange()
{
    auto mesh = ResCast<StaticMesh>(Mesh.lock());
    tr::Engine::Get().sGraphicsHandler->GetDeferredRenderer().UpdateModel(
        mesh, GetAbsoluteTransform());
}
