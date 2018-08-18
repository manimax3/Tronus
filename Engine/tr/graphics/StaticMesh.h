#pragma once
#include <tr.h>

#include "../filesystem/ResourceLoader.h"
#include "../math/Math.h"
#include "BufferHelper.h"
#include "Texture.h"

namespace tr {

/**
 * Vertex for 3D model/mesh Rendering.
 * Contains a pos, normal, tangent, bitangent and uv.
 */
struct Vertex_PNTBU {
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
     * Returns a layout which can be used for Vertex buffer to hold the vertices
     * data
     */
    static detail::BufferLayout GetVertexBufferLayout()
    {
        detail::BufferLayout layout(sizeof(Vertex_PNTBU));
        layout.Add(0, 3, detail::BufferLayout::FLOAT,
                   (void *)offsetof(Vertex_PNTBU, position));

        layout.Add(1, 3, detail::BufferLayout::FLOAT,
                   (void *)offsetof(Vertex_PNTBU, normal));

        layout.Add(2, 3, detail::BufferLayout::FLOAT,
                   (void *)offsetof(Vertex_PNTBU, tangent));

        layout.Add(3, 3, detail::BufferLayout::FLOAT,
                   (void *)offsetof(Vertex_PNTBU, bitangent));

        layout.Add(4, 2, detail::BufferLayout::FLOAT,
                   (void *)offsetof(Vertex_PNTBU, uv));

        return layout;
    }

    /**
     * Constructs a mesh from given vertices and indices.
     */
    explicit StaticMesh(std::vector<Vertex_PNTBU> vertices,
                        std::vector<uint>         indices)
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
    Vertex_PNTBU *GetVertices() { return mVertices.data(); }

    /**
     * Pointer to the Index data.
     */
    uint *GetIndices() { return mIndices.data(); }

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
    std::vector<uint>         mIndices;
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
