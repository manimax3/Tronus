#include "Image.h"

#include "../core/Engine.h"
#include "../filesystem/Filesystem.h"
#include "nlohmann/json.hpp"
#include "stb_image.h"

using json = nlohmann::json;

tr::Image::Image(const std::string &file)
{
    int   x, y, n;
    const std::string path = fs::GetExecutablePath() + file;
    byte *data = stbi_load(path.c_str(), &x, &y, &n,
                           4); // We want always four channels

    if (data == NULL) {
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

tr::Resource *tr::Image::Loader(ResourceManager::ResHandle handle,
                                ResourceManager *          rm)
{
    json jhandle = json::parse(handle);

    if (auto it = jhandle.find("file"); it != jhandle.end()) {

        if (!it->is_string())
            return nullptr;
        return new Image(it->get<std::string>());

    } else if (auto it = jhandle.find("memory"); it != jhandle.end()) {

        if (!it->is_number_integer())
            return nullptr;

        int xSize, ySize;

        try {
            xSize = jhandle["xSize"];
            ySize = jhandle["ySize"];
        } catch (json::out_of_range e) {
            rm->GetEngine().Logger().log("Not size specified in: "s + handle,
                                         LogLevel::WARNING);
            return nullptr;
        }

        uint32 *ptr = reinterpret_cast<uint32*>(it->get<uint64>());

        return new Image(ptr, xSize, ySize);

    } else if (auto it = jhandle.find("color"); it != jhandle.end()) {

        uint32 color;
        int    xSize, ySize;

        try {
            color = jhandle["color"].get<uint32>();
            xSize = jhandle["xSize"];
            ySize = jhandle["ySize"];
        } catch (json::out_of_range e) {
            rm->GetEngine().Logger().log("Invalid handle to load from color: "s
                                             + handle,
                                         LogLevel::WARNING);
            return nullptr;
        }

        return new Image(color, xSize, ySize);
    }

    return nullptr;
}
