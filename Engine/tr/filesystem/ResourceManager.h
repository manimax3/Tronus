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

/**
 * Exception representing an error if something failed during resource loading
 */
struct ResourceNotLoadedError : public std::runtime_error {
    explicit ResourceNotLoadedError(const std::string &w
                                    = "Couldnt load resource")
        : std::runtime_error(w)
    {
    }
};

struct ResourceNotFoundError : public std::runtime_error {
    explicit ResourceNotFoundError(
        std::variant<ResourceName, ResourceID> search_term);
};

/**
 * Manages the loading and unloading of resources
 */
class ResourceManager : public Subsystem<ResourceManager> {
public:
    explicit ResourceManager() noexcept
        : mResourceIDGenerator(0xFFFFFFFF) // TODO: Why not numeric_limits
    {
    }

    bool               Initialize(Engine *engine) override;
    inline std::string GetName() const override { return "ResourceManager"; }

    /**
     * Loads a resource from a given file.
     * If no namhint is provided the file name will be used instead.
     */
    ResourcePtr<> LoadResource(std::string_view            file,
                               std::optional<ResourceName> namehint = {});

    /**
     * Loads a resource with a given ResourceLoadingInformation.
     * if no namehint is provied it looks for a name in the
     * ResourceLoadingInformation
     */
    ResourcePtr<> LoadResource(ResourceLoadingInformation  info,
                               std::optional<ResourceName> namehint = {});

    /**
     * Loads the resource from a stream.
     * The stream should provide the json information.
     */
    ResourcePtr<> LoadResource(std::istream &              in,
                               std::optional<ResourceName> namehint = {});

    std::future<ResourceID> LoadResourceAsync(const std::string &file);
    std::future<ResourceID> LoadResourceAsync(ResourceLoadingInformation info);

    /**
     * Get the resource by its name.
     */
    ResourcePtr<> GetResource(std::string_view name);

    /**
     * Get the resource by its id.
     */
    ResourcePtr<> GetResource(ResourceID id);

    /**
     * Get the resource by weak_ptr.
     * Usefule if you dont want to keep the resource alive.
     */
    ResourceWeakPtr<> GetResourceWeak(std::string_view name);

    /**
     * Get the resource by weak_ptr.
     * Usefule if you dont want to keep the resource alive.
     */
    ResourceWeakPtr<> GetResourceWeak(ResourceID id);

    /**
     * Deletes a resource.
     * Note that if someone still has an shared_ptr to this resource it will be
     * kept alive.
     */
    void DeleteResource(std::string_view name);

    /**
     * Deletes a resource.
     * Note that if someone still has an shared_ptr to this resource it will be
     * kept alive.
     */
    void DeleteResource(ResourceID id);

    /**
     * Get the resources ID.
     * Useful for faster resource lookup.
     */
    ResourceID GetResourceID(std::string_view name);

    /**
     * Get the resources ID.
     * Useful for faster resource lookup.
     */
    ResourceID GetResourceID(const ResourcePtr<> &res);

    /**
     * Get the name of an resource given its id.
     */
    ResourceName GetResourceName(ResourceID id);

    /**
     * Check if a resource is loaded.
     */
    bool IsResourceLoaded(std::string_view name);

    /**
     * Check if a resource is loaded.
     */
    bool IsResourceLoaded(ResourceID id);

    /**
     * Add a new loader of a custom resource type.
     */
    template<typename T, typename... Args>
    void AddLoader(Args &&... args)
    {
        static_assert(std::is_base_of_v<ResourceLoadHandler, T>);
        auto handler = std::make_shared<T>(std::forward<Args>(args)...);
        for (const auto &type : handler->GetSupportedTypes()) {
            mResourceLoaders[type] = handler;
        }
    }

    /**
     * Returns the by the game specified asset path in which the engine looks
     * for resources.
     */
    std::string GetEngineAssetPath() const;

    /**
     * Replaces $ENGINE with the asset path
     */
    std::string ResolvePath(std::string_view path) const;

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
