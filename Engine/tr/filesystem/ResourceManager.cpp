#include "ResourceManager.h"

#include "../core/Engine.h"
#include "../core/JobHandler.h"
#include "../gameobject/Game.h"
#include "../graphics/GLSLShader.h"
#include "../graphics/Image.h"
#include "../graphics/Texture.h"
#include "Filesystem.h"
#include "spdlog/fmt/fmt.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

tr::ResourceNotFoundError::ResourceNotFoundError(
    std::variant<ResourceName, ResourceID> search_term)
    : std::runtime_error(std::visit(
          [](auto &&t) {
              using T = std::decay_t<decltype(t)>;
              if constexpr (std::is_same_v<T, ResourceID>) {
                  return fmt::format(
                      "Couldnt find resource specified with ResourceID: {}", t);
              } else if constexpr (std::is_same_v<T, ResourceName>) {
                  return fmt::format(
                      "Couldnt find resource specified with ResourceName: {}",
                      t);
              }
          },
          search_term))
{
}

bool tr::ResourceManager::Initialize(Engine *engine)
{
    mJHandler = engine->sJobHandler;

    if (mJHandler == nullptr)
        return false;

    AddLoader<GLSLShaderLoader>();
    AddLoader<ImageLoader>();
    AddLoader<TextureLoader>();

    return Subsystem::Initialize(engine);
}

std::optional<std::string> tr::ResourceManager::InvalidInfoCheck(
    const ResourceLoadingInformation &info) const
{
    if (!info)
        return "ResourceLoadingInformation was nullptr";

    const json &res_info = *info;

    if (!res_info.is_object())
        return "The ResourceLoadingInformation isnt a json object";

    try {
        if (!res_info.at("type").is_string())
            return "type in Resource info not of type string";
    } catch (const json::out_of_range &e) {
        return "in the info is no type specified | JsonException: "s + e.what();
    }

    return {};
}

tr::ResourcePtr<>
tr::ResourceManager::LoadResource(ResourceLoadingInformation  info,
                                  std::optional<ResourceName> namehint)
{
    if (const auto &opt = InvalidInfoCheck(info); opt) {

        Log().warn("ResourceLoading aborted because of invalid Loading "
                   "information. Reason: {}",
                   opt.value());
        throw ResourceNotLoadedError("Invalid ResourceLoadingInformation: \n"s
                                     + opt.value());
    }

    json &res_info = *info;

    const auto type = res_info.at("type").get<std::string>();

    ResourceLoadingContext context;
    LoadDependecies(info, context);

    const auto loader_iter = mResourceLoaders.find(type);
    if (loader_iter == std::end(mResourceLoaders)) {
        Log().warn("No resource loader found for type: {}", type);
        throw ResourceNotLoadedError("No loader found");
    }

    auto &loader_ptr = std::get<1>(*loader_iter);
    auto  res_ptr    = loader_ptr->LoadResource(std::move(info), type, *this,
                                            std::move(context));

    if (!res_ptr) {
        Log().warn("ResourceLoadHandler returned nullptr while loadint a "
                   "resource of type: {}",
                   type);
        throw ResourceNotLoadedError("ResourceLoadHandler returned nullptr");
    }

    ResourceID   id;
    ResourceName name;
    if (namehint.has_value()) {
        const auto v = namehint.value();
        id           = GetResourceID(v);
        name         = namehint.value();
    } else {
        name = loader_ptr->ResourceName(res_ptr);
        id   = GetResourceID(name);
    }

    if (id == RESOURCE_ID_INVALID) {
        mResourceIDGenerator.CreateID(id);
    }

    mResources[id]          = res_ptr;
    mResourceIDLookup[name] = id;

    return res_ptr;
}

tr::ResourcePtr<>
tr::ResourceManager::LoadResource(std::string_view            file,
                                  std::optional<ResourceName> namehint)
{
    std::string f(file.data(), file.size());
    f = GetEngineAssetPath() + f;

    if (!fs::FileExists(f)) {
        Log().warn("Couldnt find file: {}", f);
        throw ResourceNotLoadedError("File not found: "s + f);
    }

    std::ifstream in(f);
    if (!namehint.has_value())
        namehint = std::string(file.data(), file.size());

    try {
        return LoadResource(in, std::move(namehint));
    } catch (const ResourceNotLoadedError &e) {
        Log().warn("Error Loading resource from file: {}", f);
        throw;
    }
}
tr::ResourcePtr<>
tr::ResourceManager::LoadResource(std::istream &              in,
                                  std::optional<ResourceName> namehint)
{
    auto info = std::make_shared<json>();
    try {
        in >> *info;
    } catch (const json::parse_error &e) {
        Log().warn(
            "Could not parse json object for resourceloading from stream | {}",
            e.what());
        throw ResourceNotLoadedError("Parsing from Stream");
    }

    return LoadResource(std::move(info), std::move(namehint));
}

tr::ResourcePtr<> tr::ResourceManager::GetResource(std::string_view name)
{
    const auto id = mResourceIDLookup.find(name);
    if (id == std::end(mResourceIDLookup))
        throw ResourceNotFoundError(ResourceName(fmt::format("{}", name)));
    return GetResource(std::get<1>(*id));
}

tr::ResourcePtr<> tr::ResourceManager::GetResource(ResourceID id)
{
    return mResources[id];
}

tr::ResourceWeakPtr<>
tr::ResourceManager::GetResourceWeak(std::string_view name)
{
    return GetResource(name);
}

tr::ResourceWeakPtr<> tr::ResourceManager::GetResourceWeak(ResourceID id)
{
    return GetResource(id);
}

void tr::ResourceManager::DeleteResource(std::string_view name)
{
    auto id = mResourceIDLookup.find(name);

    DeleteResource(std::get<1>(*id));

    if (id != std::end(mResourceIDLookup)) {
        mResourceIDLookup.erase(id);
        mResourceIDGenerator.DestroyID(std::get<1>(*id));
    }
}

void tr::ResourceManager::DeleteResource(ResourceID id)
{
    if (auto res = mResources.find(id); res != std::end(mResources)) {
        mResources.erase(res);
    }

    for (auto &[name, _id] : mResourceIDLookup) {
        if (_id != id)
            continue;
        else {
            mResourceIDLookup.erase(mResourceIDLookup.find(name));
            break;
        }
    }
}

tr::ResourceID tr::ResourceManager::GetResourceID(std::string_view name)
{
    for (auto &[res_name, id] : mResourceIDLookup) {
        if (res_name == name) {
            return id;
        }
    }
    return RESOURCE_ID_INVALID;
}

tr::ResourceID tr::ResourceManager::GetResourceID(const ResourcePtr<> &res)
{
    for (auto &[id, ptr] : mResources) {
        if (ptr == res)
            return id;
    }

    return RESOURCE_ID_INVALID;
}

bool tr::ResourceManager::IsResourceLoaded(std::string_view name)
{
    const auto id = mResourceIDLookup.find(name);

    if (id == std::end(mResourceIDLookup))
        return false;

    return IsResourceLoaded(std::get<1>(*id));
}

bool tr::ResourceManager::IsResourceLoaded(ResourceID id)
{
    return mResources.find(id) != std::end(mResources);
}

tr::ResourceName tr::ResourceManager::GetResourceName(ResourceID id)
{
    for (auto &[name, _id] : mResourceIDLookup) {
        if (_id == id)
            return name;
    }

    throw ResourceNotFoundError(id);
}

void tr::ResourceManager::LoadDependecies(ResourceLoadingInformation &info,
                                          ResourceLoadingContext &    context)
{
    assert(!InvalidInfoCheck(info));
    const json &res_info     = *info;
    const auto &dependencies = res_info.find("dependencies");

    if (dependencies == res_info.end())
        return; // No deps specified

    for (const auto &dep : *dependencies) {
        if (dep.is_string()) {

            const auto &name = dep.get_ref<const std::string &>();
            if (IsResourceLoaded(name)) {
                context.dependencies[name]
                    = { ResourceLoadingContext::Loaded, GetResourceWeak(name) };
            } else {
                // We need to try loading this dependency
                // assuming it is a file
                // Path resolving is done by the LoadResource function
                try {
                    auto ptr = LoadResource(name);
                    context.dependencies[name]
                        = { ResourceLoadingContext::Loaded, ptr };
                } catch (const ResourceNotLoadedError &e) {
                    Log().warn("Could not load dependency: {} | {}", name,
                               e.what());
                    context.dependencies[name]
                        = { ResourceLoadingContext::NotFound, {} };
                }
            }

        } else if (dep.is_object()) {
            // Apparently we have a "in memory" resource description so we just
            // create a ResourceLoadingInformation
            try {

                const auto                  id_iter = dep.find("id");
                std::optional<ResourceName> hint;
                if (id_iter != dep.end() && id_iter->is_string()) {
                    hint = id_iter->get<std::string>();
                }

                auto       info = std::make_shared<json>(dep);
                const auto ptr  = LoadResource(std::move(info), hint);
                const auto id   = GetResourceID(ptr);
                const auto name = GetResourceName(id);

                context.dependencies[name]
                    = { ResourceLoadingContext::Loaded, ptr };

            } catch (const ResourceNotLoadedError &e) {
                Log().warn("Couldnt load dependency: {} | {} \n There couldnt "
                           "be added a NotFound entry into the Laoding Context",
                           dep.dump(), e.what());
            }
        }
    }
}

std::string tr::ResourceManager::GetEngineAssetPath() const
{
    return fs::GetExecutablePath() + "/" + mEngine->GetGame().EngineAssetPrefix
        + "/";
}

std::string tr::ResourceManager::ResolvePath(std::string_view path) const
{

    if (auto pos = path.find("$ENGINE/"); pos != path.npos) {

        auto new_path = path.substr(pos + "$ENGINE/"s.length(), path.npos);

        return GetEngineAssetPath()
            + std::string(new_path.data(), new_path.size());
    }

    return std::string(path.data(), path.size());
}

