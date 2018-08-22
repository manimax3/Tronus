#include "Image.h"

#include "../core/Engine.h"
#include "../filesystem/Filesystem.h"
#include "../filesystem/ResourceManager.h"
#include "../profile/Profiler.h"
#include "nlohmann/json.hpp"
#include "stb_image.h"

using json = nlohmann::json;

tr::Image::Image(const std::string &file)
{
    int   x, y, n;
    byte *data = stbi_load(file.c_str(), &x, &y, &n,
                           4); // We want always four channels

    if (data == nullptr) {
        throw std::runtime_error("Could not load image from: " + file
                                 + " | Reason: " + stbi_failure_reason());
    }

    mSizeX = x;
    mSizeY = y;
    mPixels.resize(x * y);

    std::memcpy(mPixels.data(), data, x * y * 4);

    stbi_image_free(data);
}

tr::Image::Image(const uint32 *pixels, int sizeX, int sizeY)
    : mSizeX(sizeX)
    , mSizeY(sizeY)
{
    mPixels.resize(sizeX * sizeY);
    std::memcpy(mPixels.data(), pixels, sizeX * sizeY * 4);
}

tr::Image::Image(uint32 color, int sizeX, int sizeY)
    : mSizeX(sizeX)
    , mSizeY(sizeY)
{
    mPixels.resize(sizeX * sizeY);
    std::fill(std::begin(mPixels), std::end(mPixels), color);
}

const tr::uint32 *tr::Image::GetPixels() const { return mPixels.data(); }

void tr::Image::FlipH()
{
    throw std::logic_error("Image::FlipH is not Implemented");
}

void tr::Image::FlipV()
{
    throw std::logic_error("Image::FlipV is not Implemented");
}

tr::uint32 tr::Image::GetPixelAt(int x, int y) const
{
    return mPixels[mSizeX * y + x];
}

tr::ResourcePtr<> tr::ImageLoader::LoadResource(ResourceLoadingInformation info,
                                                const ResourceType &       type,
                                                ResourceManager &          rm,
                                                ResourceLoadingContext context)
{
    EASY_BLOCK("ImageLoader::LoadResource");

    json &jhandle = *info;

    if (auto it = jhandle.find("file"); it != jhandle.end()) {

        if (!it->is_string())
            return nullptr;
        auto s = /*fs::GetExecutablePath() + */ it->get<std::string>();
        s      = rm.ResolvePath(s);
        return ResourcePtr<>(new Image(s));

    } else if (auto it = jhandle.find("memory"); it != jhandle.end()) {

        if (!it->is_number_integer())
            return nullptr;

        int xSize, ySize;

        try {
            xSize = jhandle["xSize"];
            ySize = jhandle["ySize"];
        } catch (json::out_of_range e) {
            Log().warn("No size specified in: {}", jhandle.dump());
            return nullptr;
        }

        auto *ptr = reinterpret_cast<uint32 *>(it->get<uint64>());

        return ResourcePtr<>(new Image(ptr, xSize, ySize));

    } else if (auto it = jhandle.find("color"); it != jhandle.end()) {

        uint32 color;
        int    xSize, ySize;

        try {
            color = jhandle["color"].get<uint32>();
            xSize = jhandle["xSize"];
            ySize = jhandle["ySize"];
        } catch (json::out_of_range e) {
            Log().warn("Invalid handle to load from color: {}", jhandle.dump());
            return nullptr;
        }

        return ResourcePtr<>(new Image(color, xSize, ySize));
    }

    return nullptr;
}
