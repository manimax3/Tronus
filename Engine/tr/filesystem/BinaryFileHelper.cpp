#include "BinaryFileHelper.h"
#include "nlohmann/json.hpp"

tr::BinaryFileHelper::BinaryFileHelper(nlohmann::json &handle)
    : mSerializer(mInstream)
{
    try {
        const size_t      read = handle.at("_binary_read");
        const std::string file = handle.at("_binary_file");

        mInstream.open(file, std::ios::binary);
        mInstream.seekg(read, std::ios::beg);
    } catch (const nlohmann::json::out_of_range &e) {
        throw std::runtime_error(
            fmt::format("Missing fields in provided json handle for "
                        "opening a BinaryFileHelper | {}",
                        e.what()));
    }
}

