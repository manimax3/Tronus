#include "ResourceManager.h"

#include "../core/Engine.h"
#include "../graphics/GLSLShader.h"
#include "../profile/Profiler.h"
#include "Filesystem.h"

#include "nlohmann/json.hpp"
#include <fstream>
#include <sstream>

using json = nlohmann::json;

bool tr::ResourceManager::Initialize(Engine *engine)
{
    mJHandler = engine->GetSystem<JobHandler>();
    if (mJHandler == nullptr)
        return false;

    AddLoader(
        "SimpleString",
        [](ResHandle handle, ResourceManager *rm) -> Resource * {
            json jhandle   = json::parse(handle);
            bool load_file = true;

            std::string file;

            if (auto loc = jhandle.find("file"); loc != jhandle.end()) {
                // There is a file specified to load the string from
                file = *loc;
            } else if (auto loc = jhandle.find("string");
                       loc != jhandle.end()) {
                load_file = false;
                file      = *loc; // We store the string data in the file string
            } else if (Log::STATIC_LOGGER)
                Log::STATIC_LOGGER->log("Tried to load resource with "
                                        "no file or string specified",
                                        LogLevel::WARNING);

            if (load_file)
                file = fs::GetExecutablePath() + file;

            if (load_file && !fs::FileExists(file))
                if (Log::STATIC_LOGGER)
                    Log::STATIC_LOGGER->log("Could not find file: "s + file);

            StringResource *res = new StringResource;

            if (load_file) {
                std::ifstream     ifs(file, std::ios::in);
                std::stringstream str;
                str << ifs.rdbuf();
                res->data = str.str();
            } else {
                res->data = file;
            }

            return res;
        });

    AddLoader(GLSLShader::GetType(), GLSLShader::Loader);

    return Subsystem::Initialize(engine);
}

bool tr::ResourceManager::CheckIfLoaded(const std::string &identifier) const
{
    std::shared_lock<std::shared_mutex> lck(mResLock);
    return mResourceList.find(identifier) != mResourceList.end();
}

void tr::ResourceManager::LoadResource(const std::string &identifier,
                                       bool               from_memory)
{
    EASY_FUNCTION();

    std::string id     = identifier;
    std::string handle = from_memory ? id : fs::GetExecutablePath() + id;

    if (!from_memory && !fs::FileExists(handle)) {
        mEngine->Logger().log("Couldnt resolve resource identifier: "s + id,
                              LogLevel::WARNING);
        return;
    }

    // Cant check for from_memory handles yet because we dont know the actual id
    // yet
    if (!from_memory && CheckIfLoaded(id)) {
        mEngine->Logger().log("Tried to load resource twice: "s + id,
                              LogLevel::WARNING);
        return;
    }

    // Load the json Handle
    json jhandle;

    if (from_memory) {
        try {
            jhandle = json::parse(handle);
            id      = jhandle.at("id").get<std::string>();
        } catch (json::parse_error e) {
            mEngine->Logger().log(
                "Error parsing in memory resource identifier: "s + id + " | "
                    + e.what(),
                LogLevel::WARNING);

            return;
        } catch (json::out_of_range e) {
            mEngine->Logger().log(
                "In Memory Resource Identifier doesnt have an id specified: "s
                    + handle + " | " + e.what(),
                LogLevel::WARNING);

            return;
        }

        // Now we can do the check for in memory handles
        if (CheckIfLoaded(id)) {
            mEngine->Logger().log("Tried to load resource twice: "s + id,
                                  LogLevel::WARNING);
            return;
        }

    } else {
        std::ifstream ifs(handle, std::ios::in);
        try {
            ifs >> jhandle;
        } catch (json::parse_error e) {
            mEngine->Logger().log("Error parsing resource identifier: "s + id
                                      + " | " + e.what(),
                                  LogLevel::WARNING);
            return;
        }
    }

    // Those futures are used to block this thread until all dependencies are
    // fully loaded
    std::vector<std::future<void>> mDepsLoaded;

    try {
        auto a = jhandle.at("dependencies");
        for (const auto &dep : a) {
            const bool is_in_mem = dep.is_object();
            const auto dep_id = is_in_mem ? dep.dump() : dep.get<std::string>();
            auto       future = LoadResourceAsync(dep_id, is_in_mem);
            mDepsLoaded.push_back(std::move(future));
        }
    } catch (json::out_of_range e) {
        // Expected Error in case there are no dependencies
    } catch (json::type_error e) {
        mEngine->Logger().log("Error parsing json dependencies of "s + id
                                  + " | " + e.what(),
                              LogLevel::ERROR);
    }

    if (mDepsLoaded.size() > 0)
        for (auto &fut : mDepsLoaded)
            fut.get();

    // All dependencies have been laoded
    // So we can call the Loader for this resource

    ResType type;
    try {
        type = jhandle["type"].get<ResType>();
    } catch (json::type_error e) {
        mEngine->Logger().log("Error determining the type of "s + id,
                              LogLevel::ERROR);
    }

    std::unique_ptr<Resource> res(mLoaders[type](jhandle.dump(), this));

    std::unique_lock<std::shared_mutex> lck(mResLock);
    mResourceList[id] = std::move(res);

    mEngine->Logger().log("Loaded resource: "s + id);
}

std::future<void>
tr::ResourceManager::LoadResourceAsync(const std::string &identifier,
                                       bool               from_memory,
                                       Callback           cb,
                                       void *             userdata)
{
    auto prom = std::make_shared<std::promise<void>>();
    auto fut  = prom->get_future();

    mJHandler->AddJob([=]() mutable {
        this->LoadResource(identifier, from_memory);
        if (cb)
            cb(identifier, userdata);
        prom->set_value();
    });

    return fut;
}

tr::Resource *tr::ResourceManager::GetResource(const std::string &identifier)
{
    std::unique_lock<std::shared_mutex> lck(mResLock);

    if (auto res = mResourceList.find(identifier); res != mResourceList.end())
        return res->second.get();

    return nullptr;
}

void tr::ResourceManager::AddLoader(const ResType &type, LoaderFunc func)
{
    mLoaders[type] = func;
}
