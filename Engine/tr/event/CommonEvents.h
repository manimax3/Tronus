#pragma once
#include <tr.h>

namespace tr {

struct Event {
};

struct InputEvent : public Event {
    enum Action { PRESS, RELEASE, REPEAT };
    enum Type {
        Mouse,
        Keyboard,
        Char,
        CursorEntered,
        CursorLeave,
        MouseButton,
        Scroll
    };

    InputEvent(int key, Action action)
        : Key(key)
        , action(action)
        , type(Keyboard)
    {
    }

    InputEvent(double x, double y)
        : XPos(x)
        , YPos(y)
        , type(Mouse)
    {
    }

    InputEvent(uint c)
        : type(Char)
        , codepoint(c)
    {
    }

    InputEvent(Type type)
        : type(type)
    {
    }

    Type type;

    // Keyboard
    Action action;
    int    Key;
    bool   Shift   = false;
    bool   Control = false;
    bool   Alt     = false;
    bool   Super   = false;

    // Mouse
    double XPos;
    double YPos;

    // Char
    uint codepoint = 0;

    // Scroll
    double xoffset;
    double yoffset;
};

struct WindowEvent : public Event {
    enum Type { CLOSED, RESIZED };

    WindowEvent(Type type)
        : type(type)
    {
    }

    WindowEvent(int x, int y)
        : type(RESIZED)
        , xSize(x)
        , ySize(y)
    {
    }

    Type type;

    // Window Resized
    // Not Framebuffer not actual window size
    int xSize, ySize;
};

struct RenderDebugEvent : public Event {
    RenderDebugEvent() {}
};

class Renderer2D;
struct Render2DEvent : public Event {
    Render2DEvent(Renderer2D &ren)
        : renderer(ren)
    {
    }
    Renderer2D &renderer;
};
}
