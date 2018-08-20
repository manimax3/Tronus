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

    template<typename A>
    void Serialize(A &a)
    {
        a &position.x &position.y &position.z &normal.x &normal.y &normal
            .z &tangent.x &tangent.y &tangent.z &bitangent.x &bitangent
            .y &bitangent.z &uv.x &uv.y;
    }
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
        layout.Add(0, ShaderElementType::Vec3,
                   (void *)offsetof(Vertex_PNTBU, position));

        layout.Add(1, ShaderElementType::Vec3,
                   (void *)offsetof(Vertex_PNTBU, normal));

        layout.Add(2, ShaderElementType::Vec3,
                   (void *)offsetof(Vertex_PNTBU, tangent));

        layout.Add(3, ShaderElementType::Vec3,
                   (void *)offsetof(Vertex_PNTBU, bitangent));

        layout.Add(4, ShaderElementType::Vec2,
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
public:
    /**
     * Adds uniforms to the interface
     */
    void AddUniformsToInterface(ShaderInterface &interface) const
    {
        for (const auto &u : mUniforms) {
            interface.AddUniform(u.type, u.name);
        }
    }

    /**
     * Add a new uniform element.
     */
    void AddUniformElement(ShaderElementType type, std::string name)
    {
        mUniforms.push_back(ShaderInterface::Uniform{ std::move(name), type });
    }

    /**
     * Bind your data to the provided shader.
     * Which is guranteed to support your specified interface.
     */
    virtual void Bind(GLSLShader &shader) = 0;

private:
    std::list<ShaderInterface::Uniform> mUniforms;
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

        AddUniformElement(ShaderElementType::Sampler2D_Other, "ambient");
        AddUniformElement(ShaderElementType::Sampler2D_Diffuse, "diffuse");
        AddUniformElement(ShaderElementType::Sampler2D_Specular, "specular");
        AddUniformElement(ShaderElementType::Float, "shininess");
    }

    void Bind(GLSLShader &shader) override
    {
        shader.Set("ambient", 0);
        shader.Set("diffuse", 1);
        shader.Set("specular", 2);
        shader.Set("shininess", mShininess);

        mAmbient->Bind();
        mDiffues->Bind(1);
        mSpecular->Bind(2);
    }

private:
    ResourcePtr<Texture> mAmbient;
    ResourcePtr<Texture> mDiffues;
    ResourcePtr<Texture> mSpecular;
    float                mShininess;
};

class PhongMaterialLoader : public ResourceLoadHandler {
public:
    ResourcePtr<> LoadResource(ResourceLoadingInformation info,
                               const ResourceType &       type,
                               ResourceManager &          rm,
                               ResourceLoadingContext     context) override;

    inline std::list<ResourceType> GetSupportedTypes() const override
    {
        return { "PhongMaterial" };
    }
};

class StaticMeshLoader : public ResourceLoadHandler {
public:
    ResourcePtr<> LoadResource(ResourceLoadingInformation info,
                               const ResourceType &       type,
                               ResourceManager &          rm,
                               ResourceLoadingContext     context) override;

    inline std::list<ResourceType> GetSupportedTypes() const override
    {
        return { "StaticMesh" };
    }
};
}
