#include "GraphicsHandler.h"
#include "../core/Engine.h"
#include "../event/CommonEvents.h"
#include "../filesystem/ResourceManager.h"
#include "easy/profiler.h"

#include "glad/glad.h"

#include "GLCheck.h"
#include "GLFW/glfw3.h"

#include "imgui.h"

#include <functional>

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
    using namespace std::placeholders;
    InputRecieved.connect(
        [&](const InputEvent &e) { this->HandleInputEvent(e); });
    WindowChanged.connect(
        [&](const WindowEvent &e) { this->HandleWindowEvent(e); });
    return Subsystem::Initialize(e);
}

tr::Vec2 tr::GraphicsHandler::GetWindowSize() const
{
    return Valid() ? mContext.windowInfo.Size : Vec2{ 0.f, 0.f };
}

tr::Vec2 tr::GraphicsHandler::GetMousePos() const
{
    return Valid() ? mContext.lastMousePos : Vec2(0.f);
}

double tr::GraphicsHandler::GetTime() const { return glfwGetTime(); }

void tr::GraphicsHandler::PostInit() {}

bool tr::GraphicsHandler::Tick()
{
    if (!mContext.valid)
        return false;

    glfwPollEvents();
    mRenderer2D.Tick();
    mForwardRenderer.Tick();
    mDefferedRenderer.Tick();

    return true;
}

bool tr::GraphicsHandler::Shutdown()
{
    if (Context().valid)
        return true;

    mSimpleRenderer2D.Shutdown();
    mForwardRenderer.Shutdown();
    /* mDefferedRenderer.Shutdown(); */
    mRenderer2D.Shutdown();
    mImguiRenderer.Shutdown();

    Context().valid = false;

    glfwDestroyWindow(static_cast<GLFWwindow *>(Context().window));
    glfwTerminate();
    return true;
}

void tr::GraphicsHandler::Render()
{
    EASY_FUNCTION();

    if (!mContext.valid)
        return; // We dont have a valid context

    glClear(GL_COLOR_BUFFER_BIT);
    mImguiRenderer.StartDebugFrame();
    /* mForwardRenderer.Render(); */
    mDefferedRenderer.Render();
    /* mRenderer2D.Render(); */
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

void tr::GraphicsHandler::HandleWindowEvent(const WindowEvent &e)
{
    if (!mContext.valid)
        return;

    if (e.type != WindowEvent::RESIZED)
        return;

    Call(glViewport(0, 0, e.xSize, e.ySize));

    if (Valid()) {
        mContext.windowInfo.Size.x = e.xSize;
        mContext.windowInfo.Size.y = e.ySize;
    }
}

void tr::GraphicsHandler::HandleInputEvent(const InputEvent &e)
{
    if (!mContext.valid)
        return;

    mContext.lastMousePos = Vec2(e.XPos, e.YPos);
}

void tr::GraphicsHandler::CreateWindow(const CreateWindowInfo &info)
{
    EASY_BLOCK("CreateWindowCmd");

    if (!glfwInit())
        Log().error("Error Initializing GLFW!");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, info.OpenGLVersion.x);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, info.OpenGLVersion.y);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, info.Resizeable ? GL_TRUE : GL_FALSE);

#ifdef TR_DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#else
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_FALSE);
#endif

    GLFWwindow *window = glfwCreateWindow(
        info.Size.x, info.Size.y, info.Name.c_str(),
        info.Fullscreen ? glfwGetPrimaryMonitor() : nullptr, nullptr);

    if (!window) {
        Log().error("Error Creating a GLFW Window");
        return;
    }

    // Set the current GraphicsHandler as the user pointer
    glfwSetWindowUserPointer(window, static_cast<void *>(this));

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
        Log().error("Failed to initialize OpenGL context");

#ifdef TR_DEBUG
    // Enable the Debug Message Callback if available (>=GL4.3)
    if (glfwExtensionSupported("GL_ARB_debug_output") && GLVersion.major >= 4
        && GLVersion.minor >= 3)
        glDebugMessageCallback((GLDEBUGPROC)gl_debug_callback, this);
#endif

    // Log the OpenGl version
    std::string version_info("OpenGL: ");
    version_info += reinterpret_cast<char *>(
        const_cast<byte *>(glGetString(GL_VERSION)));
    version_info += " GLSL: ";
    version_info += reinterpret_cast<char *>(
        const_cast<byte *>(glGetString(GL_SHADING_LANGUAGE_VERSION)));
    Log().info(version_info);

    glfwSwapInterval(info.VSync ? 1 : 0);

    Call(glViewport(0, 0, info.Size.x, info.Size.y));
    Call(glClearColor(info.ClearColor.r, info.ClearColor.g, info.ClearColor.b,
                      info.ClearColor.z));

    mContext.window     = static_cast<void *>(window);
    mContext.windowInfo = info;
    mContext.valid      = true;

    mSimpleRenderer2D.Init(this, GetEngine().sResourceManager);
    mRenderer2D.Init(this, GetEngine().sResourceManager);
    mImguiRenderer.Init(this, GetEngine().sResourceManager);
    mForwardRenderer.Init(*this);
    mDefferedRenderer.Init(*this);
}

void tr::key_callback(GLFWwindow *window,
                      int         key,
                      int         scancode,
                      int         action,
                      int         mods)
{
    auto *handler
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

    /* handler->GetEngine().sEventSystem->DispatchEvent(event); */
    handler->InputRecieved(event);
}

void tr::cursor_position_callback(GLFWwindow *window, double xpos, double ypos)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent event(xpos, ypos);
    /* handler->GetEngine().sEventSystem->DispatchEvent(event); */
    handler->InputRecieved(event);
}

void tr::close_callback(GLFWwindow *window)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    /* handler->GetEngine().sEventSystem->DispatchEvent( */
    /*     WindowEvent(WindowEvent::CLOSED)); */
    handler->WindowChanged(WindowEvent(WindowEvent::CLOSED));
}

void tr::error_callback(int error, const char *description)
{
    Log().error("GLFW - {} Error Code: {}", description, error);
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

    auto *handler = static_cast<GraphicsHandler *>(userParam);

    std::string m("OpenGL: ");
    if (length == -1)
        m += message;
    else
        m += std::string(message, length);

    if (severity == GL_DEBUG_TYPE_ERROR)
        Log().error(m);
    else
        Log().warn(m);
}

void tr::character_callback(GLFWwindow *window, uint codepoint)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    /* handler->GetEngine().sEventSystem->DispatchEvent(InputEvent(codepoint));
     */
    handler->InputRecieved(InputEvent(codepoint));
}

void tr::cursor_enter_callback(GLFWwindow *window, int entered)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    /* handler->GetEngine().sEventSystem->DispatchEvent(InputEvent( */
    /*     entered ? InputEvent::CursorEntered : InputEvent::CursorLeave)); */
    handler->InputRecieved(InputEvent(entered ? InputEvent::CursorEntered
                                              : InputEvent::CursorLeave));
}

void tr::mouse_button_callback(GLFWwindow *window,
                               int         button,
                               int         action,
                               int         mods)
{
    auto *handler
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

    /* handler->GetEngine().sEventSystem->DispatchEvent(event); */
    handler->InputRecieved(event);
}

void tr::scroll_callback(GLFWwindow *window, double xoffset, double yoffset)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    InputEvent event(InputEvent::Scroll);
    event.xoffset = xoffset;
    event.yoffset = yoffset;

    /* handler->GetEngine().sEventSystem->DispatchEvent(event); */
    handler->InputRecieved(event);
}

void tr::framebuffer_resized_callback(GLFWwindow *window, int width, int height)
{
    auto *handler
        = static_cast<GraphicsHandler *>(glfwGetWindowUserPointer(window));

    if (width == 0 || height == 0)
        return;

    /* handler->GetEngine().sEventSystem->DispatchEvent( */
    /*     WindowEvent(width, height)); */
    handler->WindowChanged(WindowEvent(width, height));
}
