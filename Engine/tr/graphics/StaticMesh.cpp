#include "StaticMesh.h"
#include "../filesystem/BinaryFileHelper.h"
#include "../profile/Profiler.h"
#include "../util/Log.h"
#include "Texture.h"

#include "nlohmann/json.hpp"

tr::ResourcePtr<>
tr::PhongMaterialLoader::LoadResource(ResourceLoadingInformation info,
                                      const ResourceType &       type,
                                      ResourceManager &          rm,
                                      ResourceLoadingContext     context)
{
    EASY_BLOCK("PhongMaterialLoader::LoadResource");
    json &handle = *info;

    try {
        const auto &diffues  = handle.at("diffuse");
        const auto &specular = handle.at("specular");

        const float shininess = handle.at("shininess");

        auto &dt = context.dependencies.at(diffues);
        auto &st = context.dependencies.at(specular);

        if (std::get<0>(dt) != ResourceLoadingContext::Loaded
            || std::get<0>(st) != ResourceLoadingContext::Loaded) {
            Log().warn("Dependencies of PhongMaterial hadnt been loaded");
            return nullptr;
        }

        return ResourcePtr<>(new PhongMaterial(

            ResCast<Texture>(std::get<1>(dt)->lock()),
            ResCast<Texture>(std::get<1>(st)->lock()), shininess));
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
