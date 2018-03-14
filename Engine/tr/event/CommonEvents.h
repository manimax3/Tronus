#pragma once
#include "EventSystem.h"

namespace tr {
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
        Identifier = event::INPUT;
    }

    InputEvent(double x, double y)
        : XPos(x)
        , YPos(y)
        , type(Mouse)
    {
        Identifier = event::INPUT;
    }

    InputEvent(uint c)
        : type(Char)
        , codepoint(c)
    {
        Identifier = event::INPUT;
    }

    InputEvent(Type type)
        : type(type)
    {
        Identifier = event::INPUT;
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
        , Event(event::WINDOW)
    {
    }

    WindowEvent(int x, int y)
        : type(RESIZED)
        , Event(event::WINDOW)
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
    RenderDebugEvent()
        : Event(event::RENDER_DEBUG)
    {
    }
};

class Renderer2D;
struct Render2DEvent : public Event {
    Render2DEvent(Renderer2D &ren)
        : Event(event::RENDER_2D)
        , renderer(ren)
    {
    }
    Renderer2D &renderer;
};
}
