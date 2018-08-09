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

        tr::Log().error(ss.str());

        error = glGetError();
    }
}
