#pragma once
#include <tr.h>

#include "../util/Serializer.h"
#include "nlohmann/json_fwd.hpp"

#include <fstream>

namespace tr {
/**
 * Helper class for Loading Tronus Binary Files.
 * Files extensions is .trb
 *
 * The loading of the files Header Information is already done by the
 * ResourceManager it self. After that header part (which consists of version
 * data and the json handle) comes pure binary information. This class can be
 * used to deserialize it. Serialization is currently not suppored as it would
 * require each resource to implement a conversion to json which currently no
 * one is doing.
 */
class BinaryFileHelper {
public:
    /**
     * Constuctor.
     * Requrires the json handle (*ResourceLoadingInformation) because the
     * ResourceManager stores the required informations in threre.
     */
    explicit BinaryFileHelper(nlohmann::json &handle);

    /**
     * Deserialize the data into t
     */
    template<typename T>
    void Deserialize(T &t)
    {
        mSerializer >> t;
    }

    Serializer<std::ifstream> &GetSerializer() { return mSerializer; }

private:
    Serializer<std::ifstream> mSerializer;
    std::ifstream             mInstream;
};
}
