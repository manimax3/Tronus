#include "Timer.h"
#include <chrono>

using namespace tr;

using HighResClock = std::chrono::high_resolution_clock;

Timer::Timer()
{
    mStartPoint = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now().time_since_epoch()).count();
}

float Timer::GetElapsed() const
{
    const auto now = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now().time_since_epoch()).count();
    return static_cast<float>(now - mStartPoint);
}

float Timer::Reset()
{
    const auto elapsed = GetElapsed();
    mStartPoint        = std::chrono::duration_cast<std::chrono::milliseconds>(HighResClock::now().time_since_epoch()).count();
    return elapsed;
}
