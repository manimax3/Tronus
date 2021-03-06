#include "StaticMesh.h"
#include "../filesystem/BinaryFileHelper.h"
#include "../profile/Profiler.h"
#include "../util/Log.h"
#include "Texture.h"

#include "nlohmann/json.hpp"

void tr::StaticMesh::UploadToGpu(bool free_mem)
{
    mBufferStore.Create();
    mBufferStore.Bind();

    mVertexBuffer.Create(sizeof(Vertex_PNTBU), GetVertexCount(),
                         static_cast<void *>(GetVertices()));
    mIndexBuffer.Create(sizeof(uint), GetIndexCount(),
                        static_cast<void *>(GetIndices()));

    auto layout = GetVertexBufferLayout();
    layout.ApplyTo(mVertexBuffer);
    mBufferStore.AddBuffer(mVertexBuffer);
    mBufferStore.AddBuffer(mIndexBuffer);

    mVertexBuffer.Bind();
    mIndexBuffer.Bind();

    if (free_mem) {
        FreeData();
    }

    mBufferStore.Unbind();
    mVertexBuffer.Unbind();
    mIndexBuffer.Unbind();
}

tr::ResourcePtr<>
tr::PhongMaterialLoader::LoadResource(ResourceLoadingInformation info,
                                      const ResourceType &       type,
                                      ResourceManager &          rm,
                                      ResourceLoadingContext     context)
{
    EASY_BLOCK("PhongMaterialLoader::LoadResource");
    json &handle = *info;

    auto normal_it = handle.find("normal");
    bool has_normal = normal_it != handle.end();

    try {
        const auto &diffues   = handle.at("diffuse");
        const auto &specular  = handle.at("specular");
        std::string normal    = has_normal ? *normal_it : "";
        const float shininess = handle.at("shininess");

        auto &dt = context.dependencies.at(diffues);
        auto &st = context.dependencies.at(specular);

        if (std::get<0>(dt) != ResourceLoadingContext::Loaded
            || std::get<0>(st) != ResourceLoadingContext::Loaded) {
            Log().warn("Dependencies of PhongMaterial hadnt been loaded");
            return nullptr;
        }

        ResourcePtr<Texture> normalres = nullptr;
        if (has_normal) {
            const auto &nt = context.dependencies.at(normal);
            if (std::get<0>(nt) == ResourceLoadingContext::Loaded) {
                normalres = ResCast<Texture>(std::get<1>(nt).value().lock());
            }
        }

        return ResourcePtr<>(new PhongMaterial(

            ResCast<Texture>(std::get<1>(dt)->lock()),
            ResCast<Texture>(std::get<1>(st)->lock()), shininess,
            std::move(normalres)));

    } catch (const json::out_of_range &e) {
        Log().warn("Missing fields in handle for PhongMaterial loading | {}",
                   e.what());
        return nullptr;
    } catch (const std::out_of_range &e) {
        Log().warn("Dependencies of PhongMaterial were apparently not listed "
                   "in the dependencies field | {}",
                   e.what());
        return nullptr;
    }
}

tr::ResourcePtr<>
tr::StaticMeshLoader::LoadResource(ResourceLoadingInformation info,
                                   const ResourceType &       type,
                                   ResourceManager &          rm,
                                   ResourceLoadingContext     context)
{
    EASY_BLOCK("StaticMeshLoader::LoadResource");
    json &handle = *info;

    try {
        BinaryFileHelper helper(handle);

        std::vector<Vertex_PNTBU> vertices;
        std::vector<uint>         indices;

        helper.Deserialize(vertices);
        helper.Deserialize(indices);

        return ResourcePtr<>(
            new StaticMesh(std::move(vertices), std::move(indices)));
    } catch (const std::runtime_error &e) {
        Log().warn("Error deserializing binary data for staticmesh "
                   "resource loading | {}",
                   e.what());
        return nullptr;
    }

    return nullptr;
}
