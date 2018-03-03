#pragma once
#include "EventSystem.h"

namespace tr {
struct InputEvent : public Event {
    enum Action { PRESS, RELEASE, REPEAT };
    enum Type { Mouse, Keyboard };

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
};

struct WindowEvent : public Event {
    enum Type { CLOSED };

    WindowEvent(Type type)
        : type(type)
        , Event(event::WINDOW)
    {
    }

    Type type;
};
}
