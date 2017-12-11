#include "StreamDevice.h"
#include "Stream.h"
#include <sstream>
#include <iostream>

tr::FileDevice::FileDevice(const std::string &path, std::ios_base::openmode mode)
{
    /*std::ios_base::openmode mode = std::ios::trunc;

    if (flags & APPEND)
        mode |= std::ios::app;
    else if (flags & WRITING)
        mode |= std::ios::out;
    else if (flags & READING)
        mode |= std::ios::out;

    if (flags & SAFE)
        mode &= ~std::ios::trunc;
    if (flags & BINARY)
        mode |= std::ios::binary;
*/
    mBuffer = new std::filebuf;

    if (mBuffer->open(path, std::move(mode)))
        ;
}

std::streambuf *tr::FileDevice::Get() { return mBuffer; }
