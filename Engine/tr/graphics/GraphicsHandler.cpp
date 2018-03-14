#include "GraphicsHandler.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "easy/profiler.h"

#include "glad/glad.h"

#include "GLCheck.h"
#include "GLFW/glfw3.h"

#include "imgui.h"

namespace tr {
void key_callback(GLFWwindow *window,
                  int         key,
                  int         scancode,
                  int         action,
                  int         mods);
void cursor_position_callback(GLFWwindow *window, double xpos, double ypos);
void close_callback(GLFWwindow *window);
void error_callback(int error, const char *description);
void gl_debug_callback(GLenum        source,
                       GLenum        type,
                       GLuint        id,
                       GLenum        severity,
                       GLsizei       length,
                       const GLchar *message,
                       void *        userParam);
void character_callback(GLFWwindow *window, uint codepoint);
void cursor_enter_callback(GLFWwindow *window, int entered);
void mouse_button_callback(GLFWwindow *window,
                           int         button,
                           int         action,
                           int         mods);
void scroll_callback(GLFWwindow *window, double xoffset, double yoffset);
void framebuffer_resized_callback(GLFWwindow *window, int width, int height);
}

bool tr::GraphicsHandler::Initialize(Engine *e)
{
    this->mWindowCmd                       = new CreateWindowCmd;
    this->mWindowCmd->DeleteAfterExecution = false;
    this->SubmitCommand(std::unique_ptr<GfxCommand>(mWindowCmd));
    return Subsystem::Initialize(e);
}

tr::Vec2 tr::GraphicsHandler::GetWindowSize() const
{
    return mWindowCmd ? mWindowCmd->Size : Vec2{ 0.f, 0.f };
}

double tr::GraphicsHandler::GetTime() const { return glfwGetTime(); }

void tr::GraphicsHandler::PostInit() {}

bool tr::GraphicsHandler::Tick()
{
    if (!mContext.valid)
        return false;

    glfwPollEvents();
    mRenderer2D.Tick();

    return true;
}

bool tr::GraphicsHandler::Shutdown()
{
    if (Context().valid)
        return true;

    mSimpleRenderer2D.Shutdown();
    mRenderer2D.Shutdown();
    mImguiRenderer.Shutdown();

    Context().valid = false;

    glfwDestroyWindow(static_cast<GLFWwindow *>(Context().window));
    glfwTerminate();
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
            if (mGfxCommandBuffer.front()->DeleteAfterExecution)
                delete mGfxCommandBuffer.front();
            mGfxCommandBuffer.pop();
        }
    }

    if (!mContext.valid)
        return; // We dont have a valid context

    glClear(GL_COLOR_BUFFER_BIT);
    mImguiRenderer.StartDebugFrame();
    mSimpleRenderer2D.Render();
    mRenderer2D.Render();
    mImguiRenderer.Render();
    glfwSwapBuffers(static_cast<GLFWwindow *>(mContext.window));
}

const char *tr::GraphicsHandler::GetClipboard() const
{
    return glfwGetClipboardString(static_cast<GLFWwindow *>(mContext.window));
}

void tr::GraphicsHandler::SetClipboard(const std::string &c)
{
    glfwSetClipboardString(static_cast<GLFWwindow *>(mContext.window),
                           c.c_str());
}

std::vector<int> tr::GraphicsHandler::SubscripeTo() const
{
    return { ENGINE_CHANNEL };
}

void tr::GraphicsHandler::OnEvent(const Event &e, int channel)
{
    if (e.Identifier != event::WINDOW)
        return;

    if (!mContext.valid)
        return;

    const WindowEvent &we = static_cast<const WindowEvent &>(e);

    if (we.type != WindowEvent::RESIZED)
        return;

    Call(glViewport(0, 0, we.xSize, we.ySize));

    if (mWindowCmd) {
        mWindowCmd->Size.x = we.xSize;
        mWindowCmd->Size.y = we.ySize;
    }
}

void tr::CreateWindowCmd::Execute(GraphicsHandler *handler)
{
    EASY_BLOCK("CreateWindowCmd");

    auto &Logger = handler->GetEngine().Logger();

    if (!glfwInit())
        Logger.log("Error Initializing GLFW!", LogLevel::ERROR);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, OpenGLVersion.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, OpenGLVersion.y);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, Resizeable ? GL_TRUE : GL_FALSE);

#ifdef TR_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif

    GLFWwindow *window
        = glfwCreateWindow(Size.x, Size.y, Name.c_str(),
                           Fullscreen ? glfwGetPrimaryMonitor() : NULL, NULL);

    if (!window) {
        Logger.log("Error Creating a GLFW Window", LogLevel::ERROR);
        return;
    }

    // Set the current GraphicsHandler as the user pointer
    glfwSetWindowUserPointer(window, static_cast<void *>(handler));

    // Set the window callbacks
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetWindowCloseCallback(window, close_callback);
    glfwSetCharCallback(window, character_callback);
    glfwSetCursorEnterCallback(window, cursor_enter_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_resized_callback);

    glfwMakeContextCurrent(window);

    // Load the OpenGL extensions
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        Logger.log("Failed to initialize OpenGL context", LogLevel::ERROR);

#ifdef TR_DEBUG
    // Enable the Debug Message Callback if available (>=GL4.3)
    if (glfwExtensionSupported("GL_ARB_debug_output") && GLVersion.major >= 4
        && GLVersion.minor >= 3)
        glDebugMessageCallback((GLDEBUGPROC)gl_debug_callback, handler);
#endif

    // Log the OpenGl version
    std::string version_info("OpenGL: ");
    version_info += reinterpret_cast<char *>(
        const_cast<byte *>(glGetString(GL_VERSION)));
    version_info += " GLSL: ";
    version_info += reinterpret_cast<char *>(
        const_cast<byte *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    Logger.log(version_info);

    glfwSwapInterval(VSync ? 1 : 0);

    Call(glViewport(0, 0, Size.x, Size.y));
    Call(glClearColor(ClearColor.x, ClearColor.y, ClearColor.z, ClearColor.w));

    handler->mContext.window = static_cast<void *>(window);

    handler->mSimpleRenderer2D.Init(handler,
                                    handler->GetEngine().sResourceManager);
    handler->mRenderer2D.Init(handler, handler->GetEngine().sResourceManager);
    handler->mImguiRenderer.Init(handler,
                                 handler->GetEngine().sResourceManager);

    handler->mContext.valid = true;
}

void tr::key_callback(GLFWwindow *window,
                      int         key,
                      int         scancode,
                      int         action,
                      int         mods)
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

    handler->GetEngine().sEventSystem->DispatchEvent(event);
}

void tr::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent event(xpos, ypos);

    handler->GetEngine().sEventSystem->DispatchEvent(event);
}

void tr::close_callback(GLFWwindow *window)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    handler->GetEngine().sEventSystem->DispatchEvent(
        WindowEvent(WindowEvent::CLOSED));
}

void tr::error_callback(int error, const char *description)
{
    if (Log::STATIC_LOGGER)
        Log::STATIC_LOGGER->log("GLFW - "s + description
                                    + " Error Code: " + std::to_string(error),
                                LogLevel::ERROR);
}

void tr::gl_debug_callback(GLenum        source,
                           GLenum        type,
                           GLuint        id,
                           GLenum        severity,
                           GLsizei       length,
                           const GLchar *message,
                           void *        userParam)
{
    if (!(severity == GL_DEBUG_SEVERITY_HIGH
          || severity == GL_DEBUG_SEVERITY_MEDIUM))
        return;

    GraphicsHandler *handler = static_cast<GraphicsHandler *>(userParam);

    std::string m("OpenGL: ");
    if (length == -1)
        m += message;
    else
        m += std::string(message, length);

    handler->GetEngine().Logger().log(
        m,
        severity == GL_DEBUG_TYPE_ERROR ? LogLevel::ERROR : LogLevel::WARNING);
}

void tr::character_callback(GLFWwindow *window, uint codepoint)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    handler->GetEngine().sEventSystem->DispatchEvent(InputEvent(codepoint));
}

void tr::cursor_enter_callback(GLFWwindow *window, int entered)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    handler->GetEngine().sEventSystem->DispatchEvent(InputEvent(
        entered ? InputEvent::CursorEntered : InputEvent::CursorLeave));
}

void tr::mouse_button_callback(GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mods)
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

    InputEvent event(button, a);
    event.type = InputEvent::MouseButton;

    if (mods & GLFW_MOD_ALT)
        event.Alt = true;
    if (mods & GLFW_MOD_SHIFT)
        event.Shift = true;
    if (mods & GLFW_MOD_SUPER)
        event.Super = true;
    if (mods & GLFW_MOD_CONTROL)
        event.Control = true;

    handler->GetEngine().sEventSystem->DispatchEvent(event);
}

void tr::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent event(InputEvent::Scroll);
    event.xoffset = xoffset;
    event.yoffset = yoffset;

    handler->GetEngine().sEventSystem->DispatchEvent(event);
}

void tr::framebuffer_resized_callback(GLFWwindow *window, int width, int height)
{
    GraphicsHandler *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    if (width == 0 || height == 0)
        return;

    handler->GetEngine().sEventSystem->DispatchEvent(
        WindowEvent(width, height));
}
