#pragma once
#include "../core/Subsystem.h"
#include <future>
#include <map>
#include <memory>
#include <optional>
#include <shared_mutex>
#include <tr.h>

namespace tr {

class Resource;

class ResourceManager : public Subsystem<ResourceManager> {
public:
    bool               Initialize(Engine *engine) override;
    inline std::string GetName() const override { return "ResourceManager"; }

    using ResHandle = const std::string &;
    using ResType   = std::string;

    using LoaderFunc = Resource *(*)(ResHandle handle, ResourceManager *);
    using Callback   = void (*)(const std::string &, void *);

    void LoadResource(const std::string &identifier, bool from_memory = false);

    std::future<void> LoadResourceAsync(const std::string &identifier,
                                        bool               from_memory = false,
                                        Callback           cb       = nullptr,
                                        void *             userdata = nullptr);

    std::weak_ptr<Resource> GetResourceWeak(const std::string &identifier);
    std::shared_ptr<Resource> GetResource(const std::string &identifier);

    template<typename T,
             typename = std::enable_if_t<std::is_base_of_v<Resource, T>>>
    std::shared_ptr<T> GetRes(const std::string &identifier)
    {
        return std::static_pointer_cast<T>(GetResource(identifier));
    }

    template<typename T,
             typename = std::enable_if_t<std::is_base_of_v<Resource, T>>>
    std::weak_ptr<T> GetResWeak(const std::string &identifier)
    {
        return std::static_pointer_cast<T>(GetResourceWeak(identifier).lock());
    }

    bool DeleteResource(const std::string &identifier);

    void        AddLoader(const ResType &type, LoaderFunc func);
    std::string GetEngineAssetPath() const;
    std::string ResolvePath(
        const std::string &path) const; // Replaces $ENGINE with the asset path

    friend class DebugWindow;
private:
    bool CheckIfLoaded(const std::string &identifier) const;

    mutable std::shared_mutex                        mResLock;
    std::map<std::string, std::shared_ptr<Resource>> mResourceList;

    std::map<ResType, LoaderFunc> mLoaders;

    class JobHandler *mJHandler = nullptr;
};

struct Resource {
    Resource()          = default;
    virtual ~Resource() = default;
};

struct StringResource : public Resource {
    std::string data;
};

template<typename R>
using ResHandle = std::shared_ptr<R>;

template<typename R>
using ResHandleWeak = std::weak_ptr<R>;

}
