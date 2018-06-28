#pragma once
#include <tr.h>

#include "../core/Subsystem.h"
#include "../util/MakeID.h"
#include "ResourceLoader.h"

#include <future>
#include <map>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <string_view>

namespace tr {

struct ResourceNotLoadedError : public std::runtime_error {
    explicit ResourceNotLoadedError(const std::string &w
                                    = "Couldnt load resource")
        : std::runtime_error(w)
    {
    }
};

class ResourceManager : public Subsystem<ResourceManager> {
public:
    explicit ResourceManager() noexcept
        : mResourceIDGenerator(0xFFFFFFFF)
    {
    }

    bool               Initialize(Engine *engine) override;
    inline std::string GetName() const override { return "ResourceManager"; }

    ResourcePtr<> LoadResource(std::string_view            file,
                               std::optional<ResourceName> namehint = {});

    ResourcePtr<> LoadResource(ResourceLoadingInformation  info,
                               std::optional<ResourceName> namehint = {});

    ResourcePtr<> LoadResource(std::istream &              in,
                               std::optional<ResourceName> namehint = {});

    std::future<ResourceID> LoadResourceAsync(const std::string &file);
    std::future<ResourceID> LoadResourceAsync(ResourceLoadingInformation info);

    ResourcePtr<> GetResource(std::string_view name);
    ResourcePtr<> GetResource(ResourceID id);

    ResourceWeakPtr<> GetResourceWeak(std::string_view name);
    ResourceWeakPtr<> GetResourceWeak(ResourceID id);

    void DeleteResource(std::string_view name);
    void DeleteResource(ResourceID id);

    ResourceID GetResourceID(std::string_view name);
    ResourceID GetResourceID(const ResourcePtr<> &res);

    ResourceName GetResourceName(ResourceID id);

    bool IsResourceLoaded(std::string_view name);
    bool IsResourceLoaded(ResourceID id);

    template<typename T, typename... Args>
    void AddLoader(Args &&... args)
    {
        static_assert(std::is_base_of_v<ResourceLoadHandler, T>);
        auto handler = std::make_shared<T>(std::forward<Args>(args)...);
        for (const auto &type : handler->GetSupportedTypes()) {
            mResourceLoaders[type] = handler;
        }
    }

    std::string GetEngineAssetPath() const;
    std::string ResolvePath(
        std::string_view path) const; // Replaces $ENGINE with the asset path

    friend class DebugWindow;

private:
    std::optional<std::string>
    InvalidInfoCheck(const ResourceLoadingInformation &info) const;

    void LoadDependecies(ResourceLoadingInformation &info,
                         ResourceLoadingContext &    context);

    std::unordered_map<ResourceID, ResourcePtr<>>   mResources;
    std::map<ResourceName, ResourceID, std::less<>> mResourceIDLookup;

    MakeID mResourceIDGenerator;

    std::map<ResourceType, std::shared_ptr<ResourceLoadHandler>>
        mResourceLoaders;

    class JobHandler *mJHandler = nullptr;
};
}
