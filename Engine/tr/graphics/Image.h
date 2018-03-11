#pragma once
#include <tr.h>

#include "../filesystem/ResourceManager.h"
#include "../math/Math.h"
#include <vector>

namespace tr {

/*
Handle Example:
{
"type" : "Image",

"file" : "path to file",
        or
"memory" : pointer,
        or
"color" : "FFAAFFFF",

"xSize" : 12, // Only if not from file
"ySize" : 13, // see above

"FlipH" : true,
"FlipV" : false
}
*/

class Image : public Resource {
public:
    static constexpr const char *GetType() { return "Image"; }
    static Resource *            Loader(ResourceManager::ResHandle handle,
                                        ResourceManager *          rm);

    Image(const std::string &file);
    Image(const uint32 *pixels, int sizeX, int sizeY);
    Image(uint32 color, int sizeX, int sizeY);

    const uint32 *GetPixels() const;

    void FlipH();
    void FlipV();

    uint32 GetPixelAt(int x, int y) const;
    inline int GetSizeX() const { return mSizeX; }
    inline int GetSizeY() const { return mSizeY; }

private:
    std::vector<uint32> mPixels;
    int                 mSizeX;
    int                 mSizeY;
};
}
