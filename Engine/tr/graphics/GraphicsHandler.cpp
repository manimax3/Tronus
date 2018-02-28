#include "GraphicsHandler.h"
#include "../event/CommonEvents.h"
#include "GLFW/glfw3.h"
#include "easy/profiler.h"

namespace tr {
void key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void close_callback(GLFWwindow *window);
void error_callback(int error, const char *description);
}

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

bool tr::GraphicsHandler::Shutdown()
{
    SubmitCommand(std::make_unique<CloseWindowCmd>());
    return true;
}

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
        EASY_BLOCK("Executing GfxCommands");

        for (int i = 0; i < size; i++) {
            mGfxCommandBuffer.front()->Execute(this);
            delete mGfxCommandBuffer.front();
            mGfxCommandBuffer.pop();
        }
    }

    if (!mContext.valid)
        return; // We dont have a valid context

    // glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(static_cast<GLFWwindow *>(mContext.window));
}

void tr::CreateWindowCmd::Execute(GraphicsHandler *handler)
{
    EASY_BLOCK("CreateWindowCmd");

    if (!glfwInit())
        handler->GetEngine().Logger().log(
            "Error Initializing GLFW!", LogLevel::ERROR);

    GLFWwindow *window = glfwCreateWindow(Size.x, Size.y, Name.c_str(),
        Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (!window)
        handler->GetEngine().Logger().log(
            "Error Creating a GLFW Window", LogLevel::ERROR);

    // Set the current GraphicsHandler as the user pointer
    glfwSetWindowUserPointer(window, static_cast<void *>(handler));
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowCloseCallback(window, close_callback);

    glfwMakeContextCurrent(window);

    glfwSwapInterval(VSync ? 1 : 0);

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

void tr::key_callback(
    GLFWwindow *window, int key, int scancode, int action, int mods)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent::Action a;

    if (action == GLFW_PRESS)
        a = InputEvent::PRESS;
    else if (action == GLFW_RELEASE)
        a = InputEvent::RELEASE;
    else
        a = InputEvent::REPEAT;

    InputEvent event(key, a);

    if (mods & GLFW_MOD_ALT)
        event.Alt = true;
    if (mods & GLFW_MOD_SHIFT)
        event.Shift = true;
    if (mods & GLFW_MOD_SUPER)
        event.Super = true;
    if (mods & GLFW_MOD_CONTROL)
        event.Control = true;

    handler->GetEngine().GetSystem<EventSystem>()->DispatchEvent(event);
}

void tr::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent event(xpos, ypos);

    handler->GetEngine().GetSystem<EventSystem>()->DispatchEvent(event);
}

void tr::close_callback(GLFWwindow *window)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    handler->SubmitCommand(std::make_unique<CloseWindowCmd>());
}

void tr::error_callback(int error, const char *description)
{
    if (Log::STATIC_LOGGER)
        Log::STATIC_LOGGER->log(std::string("GLFW - ") + description
                + " Error Code: " + std::to_string(error),
            LogLevel::ERROR);
}
