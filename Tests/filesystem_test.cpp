#include "catch.hpp"
#include "tr/filesystem/Stream.h"
#include "tr/filesystem/StreamDevice.h"
#include <fstream>

TEST_CASE("FileSystem")
{
    {
        tr::Outstream tro;
        tro.Push<tr::FileDevice>(
            "Hallo.super", tr::FileDevice::WRITING | tr::FileDevice::DELETE);
        tro << "Hallo";
    }
    {
        tr::Instream tri;
        tri.Push<tr::FileDevice>(
                "Hallo.super", tr::FileDevice::READING);

        char input[6];
        tri.read(input, 6);
        REQUIRE_FALSE(std::strcmp(input, "Hallo"));
    }
}
