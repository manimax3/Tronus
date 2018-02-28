#include "GraphicsHandler.h"
#include "GLFW/glfw3.h"
#include "easy/profiler.h"

namespace tr {
struct RenderContext {
    GLFWwindow *  window;
    volatile bool valid;
};
}

bool tr::GraphicsHandler::Initialize(Engine *e)
{
    e->Logger().log("Starting GraphicsHandler...");
    this->SubmitCommand(std::unique_ptr<GfxCommand>(new CreateWindowCmd()));
    std::thread([this]() { this->render(); }).detach();
    return Subsystem::Initialize(e);
}

bool tr::GraphicsHandler::Tick()
{
    GfxCommand *cmd;
    while (mMainCommandBuffer.dequeue(cmd)) {
        cmd->Execute(this);
        delete cmd;
    }

    if (mContext && mContext->valid)
        glfwPollEvents();

    return true;
}

bool tr::GraphicsHandler::Shutdown() { return true; }

bool tr::GraphicsHandler::SubmitCommand(std::unique_ptr<GfxCommand> &&_cmd)
{
    GfxCommand *cmd = _cmd.release();

    if (!cmd)
        return false;

    if (cmd->RequiresMain())
        mMainCommandBuffer.enqueue(cmd);
    else
        mGfxCommandBuffer.enqueue(cmd);

    return true;
}

void tr::GraphicsHandler::render()
{
    EASY_THREAD_SCOPE("Render");

    while (true) {

        EASY_BLOCK("FrameCycle");

        GfxCommand *cmd;
        //while (mGfxCommandBuffer.dequeue(cmd)) {
        //    cmd->Execute(this);
        //    delete cmd;
        //}

        if (!mContext || !mContext->valid)
            continue; // We dont have a valid context

        if (glfwWindowShouldClose(mContext->window))
            break; // Close the window. Maybe send some type of Event

        // glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(mContext->window);
    }
}

tr::GfxCommand::GfxCommand(bool b)
    : mRequiresMain(b)
{
}

tr::CreateWindowCmd::CreateWindowCmd()
    : GfxCommand(true)
{
}

void tr::CreateWindowCmd::Execute(GraphicsHandler *handler)
{
    EASY_BLOCK("CreateWindowCmd");

    if (!glfwInit())
        handler->GetEngine().Logger().log(
            "Error Initializing GLFW!", LogLevel::ERROR);

    handler->mContext = new RenderContext();

    GLFWwindow *window
        = glfwCreateWindow(Size.x, Size.y, Name.c_str(), NULL, NULL);

    if (!window)
        handler->GetEngine().Logger().log(
            "Error Creating a GLFW Window", LogLevel::ERROR);

    handler->mContext->window = window;

    handler->mContext->valid = true;
}
