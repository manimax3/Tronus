#pragma once
#include <vector>

namespace tr {

struct Event;

class EventListener {
public:
    virtual ~EventListener(); 
    virtual void             OnEvent(const Event &e, int channel) = 0;
    virtual std::vector<int> SubscripeTo() const                  = 0;

    friend class EventSystem;
private:
    class EventSystem *mEventSystem = nullptr;
};

struct Event {
    Event(int id = -1)
        : Identifier(id)
    {
    }

    virtual ~Event() = default;

    Event(const Event &) = default;
    Event(Event &&)      = default;
    Event &operator=(const Event &) = default;
    Event &operator=(Event &&) = default;

    int Identifier = -1;
};
}
