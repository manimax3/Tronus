#include "catch.hpp"
#include "tr/filesystem/Stream.h"
#include "tr/filesystem/StreamBuffer.h"
#include <fstream>

TEST_CASE("FileSystem")
{
        tr::Outstream out;
        out.Push<tr::FileBuffer>("Hallo.WasGeht", tr::FileBuffer::WRITING);
        out.Write("Hallo Welt\n", 11);
        out << 12 << '\n';
        out << 'n' << sizeof(int) << 'n';
        out.Flush();
        tr::Instream in;
        in.Push<tr::FileBuffer>("Hallo.WasGeht", tr::FileBuffer::READING);
        char input[12];
        input[11] = 0;
        in.Read(input, 11);
        REQUIRE_FALSE(std::strcmp(input, "Hallo Welt\n"));

        int i;
        in.Read(reinterpret_cast<char *>(&i), sizeof(int));
        REQUIRE(i == 12);
}
