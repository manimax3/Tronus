#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"
#include "Texture.h"

namespace tr {

/**
 * Vertex for 3D model/mesh Rendering.
 * Contains a pos, normal, tangent, bitangent and uv.
 */
class Vertex_PNTBU {
    Vec3 position;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
    Vec2 uv;
};

/**
 * Representing the triangles of a Model.
 */
class StaticMesh : public Resource {
public:
    /**
     * Constructs a mesh from given vertices and indices.
     */
    explicit StaticMesh(std::vector<Vertex_PNTBU> vertices,
                        std::vector<int>          indices)
        : mVertices(std::move(vertices))
        , mIndices(std::move(indices))
    {
    }

    /**
     * Amount of vertices.
     */
    int GetVertexCount() const { return mVertices.size(); }

    /**
     * Amount of indices.
     */
    int GetIndexCount() const { return mIndices.size(); }

    /**
     * Amount of Triangles.
     */
    int GetTriangleCount() const { return GetIndexCount() / 3; }

    /**
     * Pointer to the Vertex data.
     */
    const Vertex_PNTBU *GetVertices() const { return mVertices.data(); }

    /**
     * Pointer to the Index data.
     */
    const int *GetIndices() const { return mIndices.data(); }

    /**
     * Frees up the memory space.
     */
    void FreeData()
    {
        mVertices.clear();
        mIndices.clear();
    }

private:
    std::vector<Vertex_PNTBU> mVertices;
    std::vector<int>          mIndices;
};

/**
 * Base class for different types of material.
 */
class Material : public Resource {
};

/**
 * Material used for rendering with Phong(Blinn) Shading.
 */
class PhongMaterial : public Material {
public:
    /**
     * Constructs the material given the required textures and shininess.
     */
    explicit PhongMaterial(const ResourcePtr<Texture> &ambient,
                           const ResourcePtr<Texture> &diffuse,
                           const ResourcePtr<Texture> &specular,
                           float                       shininess)
        : mAmbient(ambient)
        , mDiffues(diffuse)
        , mSpecular(specular)
        , mShininess(shininess)
    {
        assert(ambient);
        assert(diffuse);
        assert(specular);
    }

private:
    ResourcePtr<Texture> mAmbient;
    ResourcePtr<Texture> mDiffues;
    ResourcePtr<Texture> mSpecular;
    float                mShininess;
};
}
