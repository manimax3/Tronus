#include "ResourceManager.h"

#include "../core/Engine.h"
#include "../profile/Profiler.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

bool tr::ResourceManager::Initialize(Engine *engine)
{
    mJHandler = engine->GetSystem<JobHandler>();
    if (mJHandler == nullptr)
        return false;

    AddLoader("SimpleString",
              [](ResHandle handle, ResourceManager *rm) -> Resource * {
                  json        jhandle = json::parse(handle);
                  std::string file    = jhandle["file"];

                  auto res = new StringResource;

                  {
                      std::ifstream     ifs(file, std::ios::in);
                      std::stringstream str;
                      str << ifs.rdbuf();
                      res->data = str.str();
                  }

                  return res;
              });

    return Subsystem::Initialize(engine);
}

void tr::ResourceManager::LoadResource(const std::string &identifier)
{
    EASY_FUNCTION();

    {
        std::shared_lock<std::shared_mutex> lck(mResLock);
        if (mResourceList.find(identifier) != mResourceList.end()) {
            mEngine->Logger().log("Tried to load resource twice: "s
                                      + identifier,
                                  LogLevel::ERROR);
            return;
        }
    }

    // Load the json Handle
    json handle;

    {
        std::ifstream ifs(identifier, std::ios::in);
        ifs >> handle;
    }

    std::vector<std::future<void>> mDepsLoaded;

    try {
        auto a = handle.at("dependencies");
        for (const auto &dep : a) {
            auto future = LoadResourceAsync(dep.get<std::string>());
            mDepsLoaded.push_back(std::move(future));
        }
    } catch (json::out_of_range e) {
        // Expected Error in case there are no dependencies
    } catch (json::type_error e) {
        mEngine->Logger().log("Error parsing json dependencies of "s
                                  + identifier,
                              LogLevel::ERROR);
    }

    if (mDepsLoaded.size() > 0)
        for (auto &fut : mDepsLoaded)
            fut.get();

    // All dependencies have been laoded
    // So we can call the Loader for this resource

    ResType type;
    try {
        type = handle["type"].get<ResType>();
    } catch (json::type_error e) {
        mEngine->Logger().log("Error determining the type of "s + identifier,
                              LogLevel::ERROR);
    }

    std::shared_ptr<Resource> res(mLoaders[type](handle.dump(), this));

    std::unique_lock<std::shared_mutex> lck(mResLock);
    mResourceList[identifier] = std::move(res);

    mEngine->Logger().log("Loaded resource: "s + identifier);
}

std::future<void>
tr::ResourceManager::LoadResourceAsync(const std::string &identifier,
                                       Callback           cb,
                                       void *             userdata)
{
    auto prom = std::make_shared<std::promise<void>>();
    auto fut  = prom->get_future();

    mJHandler->AddJob([=]() mutable {
        this->LoadResource(identifier);
        if (cb)
            cb(identifier, userdata);
        prom->set_value();
    });

    return fut;
}

std::optional<std::shared_ptr<tr::Resource>>
tr::ResourceManager::GetResource(const std::string &identifier)
{
    std::unique_lock<std::shared_mutex> lck(mResLock);

    if (auto res = mResourceList.find(identifier); res != mResourceList.end())
        return res->second;

    return {};
}

void tr::ResourceManager::AddLoader(const ResType &type, LoaderFunc func)
{
    mLoaders[type] = func;
}
