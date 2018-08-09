#pragma once
#include <tr.h>

#include "../core/JobHandler.h"
#include "nlohmann/json_fwd.hpp"

#include <functional>
#include <list>
#include <string>
#include <variant>

namespace tr {

class ResourceManager;

/**
 * Base class for resources
 */
struct Resource {
    virtual ~Resource() = default;
};

template<typename Res>
inline constexpr bool is_resource_v
    = std::is_base_of_v<Resource, Res> || std::is_same_v<Res, Resource>;

template<typename Res = Resource,
         typename     = std::enable_if_t<is_resource_v<Res>>>
using ResourcePtr = std::shared_ptr<Res>;

template<typename Res = Resource,
         typename     = std::enable_if_t<is_resource_v<Res>>>
using ResourceWeakPtr = std::weak_ptr<Res>;

// shared ptr to the forward decalred (!) json object
// ResouceHandlers can keep this for whatever reason if needed
// or if the resource itself requires it
using ResourceLoadingInformation = std::shared_ptr<nlohmann::json>;

// Can be returned from the LoadResource member function in order to delay the
// execution of function laoding into an syncronized envireoment
using ResourceLoadingTask = std::function<ResourcePtr<>(ResourceManager &)>;

using ResourceID   = uint;
using ResourceName = std::string;
using ResourceType = std::string;

constexpr ResourceID RESOURCE_ID_INVALID = 0;

/**
 * Context where you can look up inforamtion about the current resource loading
 * process.
 */
struct ResourceLoadingContext {
    enum State { Loaded, NotFound, OptionallyLoadable };

    bool loading_async = false;
    std::map<ResourceName,
             std::tuple<State, std::optional<ResourceWeakPtr<>>>,
             std::less<>>
        dependencies;
};

/**
 * Base class representing a resource loadger for a specified type
 */
class ResourceLoadHandler {
public:
    explicit ResourceLoadHandler() = default;
    virtual ~ResourceLoadHandler() = default;

    virtual ResourcePtr<> LoadResource(ResourceLoadingInformation info,
                                       const ResourceType &       type,
                                       ResourceManager &          rm,
                                       ResourceLoadingContext     context)
        = 0;

    // If the ResourceManager can not determine a suitable it asks the
    // LoadHandler for a suitable name based on the ResourceLoadingInformation
    virtual std::string ResourceName(ResourceWeakPtr<> res) const
    {
        throw std::logic_error(
            std::string("ResourceName(...) not implemented by ")
            + typeid(this).name());
    }

    virtual std::list<ResourceType> GetSupportedTypes() const = 0;
};

template<typename T>
ResourcePtr<T> ResCast(const ResourcePtr<> &r)
{
    static_assert(is_resource_v<T>);
    return std::dynamic_pointer_cast<T>(r);
}

template<typename T>
ResourceWeakPtr<T> ResCast(const ResourceWeakPtr<> &r)
{
    static_assert(is_resource_v<T>);
    return ResCast<T>(r.lock());
}
}
