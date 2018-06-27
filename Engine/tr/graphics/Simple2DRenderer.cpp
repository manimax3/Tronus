#include "Simple2DRenderer.h"
#include "../core/Engine.h"
#include "../filesystem/ResourceManager.h"
#include "GLCheck.h"
#include "GraphicsHandler.h"

#include "glad/glad.h"

void tr::Simple2DRenderer::Init(GraphicsHandler *gfx, ResourceManager *rm)
{
    mGfxHandler = gfx;
    mResManager = rm;

    const std::string shader_id = "SimpleRenderer2D.json";

    const GLfloat vertices[]
        = { // Position
            -0.5f, -0.5f, 0.0f, 0.5f, -0.5f, 0.0f, 0.0f, 0.5f, 0.0f
          };

    mShader = ResCast<GLSLShader>(
        mResManager->LoadResource(std::string_view(shader_id)));

    GLuint vbo;
    Call(glGenBuffers(1, &vbo));
    Call(glBindBuffer(GL_ARRAY_BUFFER, vbo));

    Call(glGenVertexArrays(1, &mVao));
    Call(glBindVertexArray(mVao));

    Call(glBindBuffer(GL_ARRAY_BUFFER, vbo));
    Call(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices,
                      GL_STATIC_DRAW));

    Call(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat),
                               (GLvoid *)0));

    Call(glEnableVertexAttribArray(0));
    Call(glBindVertexArray(0));
}

void tr::Simple2DRenderer::Render()
{
    mShader->Bind();
    Call(glBindVertexArray(mVao));
    Call(glDrawArrays(GL_TRIANGLES, 0, 3));
}

void tr::Simple2DRenderer::Shutdown()
{
    if (mShader)
        mResManager->DeleteResource("SimpleRenderer2D.json");
}
