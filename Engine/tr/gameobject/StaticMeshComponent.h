#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../graphics/RenderDefinitions.h"
#include "Component.h"

namespace tr {
/**
 * Class to get a Mesh onto the screen.
 */
class StaticMeshComponent : public SceneComponent {
public:
    /**
     * Weak Resoruce Pointer to the mesh.
     * Weak because we dont want to own it.
     */
    ResourceWeakPtr<> Mesh;

    /**
     * Pointer to the used material
     */
    ResourceWeakPtr<> Material;

    /**
     * Changes the visiblity of the mesh
     */
    void SetVisibility(bool visible = true);

    /**
     * Setup the internal state
     */
    void PostWorldEnter() override;

    /**
     * Get the current visiblity
     */
    bool GetVisibility() const { return Visibility; }

    /**
     * The strategy used to render this mesh.
     * Staying with the default is recommended.
     */
    RenderStrategy Strategy = RenderStrategy::ForwardRendering;

    /**
     * Whether to cast shadows.
     */
    bool CastShadows = true;

protected:
    bool Visibility = true;

    /**
     * We need to update the model matrix.
     */
    void OnRelativeChange() override;

private:
    void *mRendererHandle = nullptr;
};
}
