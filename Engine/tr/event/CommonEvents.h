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
        Identifier = INPUT_ID;
    }

    InputEvent(double x, double y)
        : XPos(x)
        , YPos(y)
        , type(Mouse)
    {
        Identifier = INPUT_ID;
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
}
