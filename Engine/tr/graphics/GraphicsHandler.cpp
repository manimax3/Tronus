#include "GraphicsHandler.h"
#include "GLFW/glfw3.h"
#include "easy/profiler.h"

bool tr::GraphicsHandler::Initialize(Engine *e)
{
    e->Logger().log("Starting GraphicsHandler...");
    this->SubmitCommand(std::unique_ptr<GfxCommand>(new CreateWindowCmd()));
    return Subsystem::Initialize(e);
}

bool tr::GraphicsHandler::Tick()
{
    if (mContext.valid)
        glfwPollEvents();

    return true;
}

bool tr::GraphicsHandler::Shutdown() { return true; }

bool tr::GraphicsHandler::SubmitCommand(std::unique_ptr<GfxCommand> &&_cmd)
{
    GfxCommand *cmd = _cmd.release();

    if (!cmd)
        return false;

    mGfxCommandBuffer.push(cmd);

    return true;
}

void tr::GraphicsHandler::Render()
{
    EASY_FUNCTION();

    // Execute the commands in the command buffer
    if (const auto size = mGfxCommandBuffer.size(); size > 0) {
        for (int i = 0; i < size; i++) {
            mGfxCommandBuffer.front()->Execute(this);
            delete mGfxCommandBuffer.front();
            mGfxCommandBuffer.pop();
        }
    }

    if (!mContext.valid)
        return; // We dont have a valid context

    if (glfwWindowShouldClose(static_cast<GLFWwindow *>(mContext.window))) {
        SubmitCommand(std::make_unique<CloseWindowCmd>());
        return;
    }

    // glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(static_cast<GLFWwindow *>(mContext.window));
}

void tr::CreateWindowCmd::Execute(GraphicsHandler *handler)
{
    EASY_BLOCK("CreateWindowCmd");

    if (!glfwInit())
        handler->GetEngine().Logger().log(
            "Error Initializing GLFW!", LogLevel::ERROR);

    GLFWwindow *window
        = glfwCreateWindow(Size.x, Size.y, Name.c_str(), NULL, NULL);

    if (!window)
        handler->GetEngine().Logger().log(
            "Error Creating a GLFW Window", LogLevel::ERROR);

    handler->mContext.window = static_cast<void *>(window);

    handler->mContext.valid = true;
}

void tr::CloseWindowCmd::Execute(GraphicsHandler *handler)
{
    if (!handler->Context().valid)
        return;

    handler->Context().valid = false;

    glfwDestroyWindow(static_cast<GLFWwindow *>(handler->Context().window));
    glfwTerminate();
}
