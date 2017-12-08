#pragma once

#include "tr.h"

namespace tr {
class Timer {
private:
    int64 mStartPoint;

public:
    Timer();

    float GetElapsed() const;
    float Reset();
};
}
