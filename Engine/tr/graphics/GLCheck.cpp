#include "GLCheck.h"
#include "../util/Log.h"
#include "glad/glad.h"
#include <iostream>
#include <sstream>

void GLCheck(const char *file, int line, const char *functions)
{
    GLenum error = glGetError();
    if (error == GL_NO_ERROR)
        return;
    while (error != GL_NO_ERROR) {

        std::stringstream ss;
        ss << "OpenGL Code: " << error << " Function: " << functions
           << " File: " << file << " Line: " << line;

        if (tr::Log::STATIC_LOGGER)
            tr::Log::STATIC_LOGGER->log(ss.str(), tr::LogLevel::ERROR);
        else
            std::cout << ss.str() << '\n';

        error = glGetError();
    }
}
