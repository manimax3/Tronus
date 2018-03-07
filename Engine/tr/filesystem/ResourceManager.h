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

    void LoadResource(const std::string &identifier);

    std::future<void> LoadResourceAsync(const std::string &identifier,
                                        Callback           cb       = nullptr,
                                        void *             userdata = nullptr);

    std::optional<std::shared_ptr<Resource>>
    GetResource(const std::string &identifier);

    void AddLoader(const ResType &type, LoaderFunc func);

private:
    std::shared_mutex                                mResLock;
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
}
