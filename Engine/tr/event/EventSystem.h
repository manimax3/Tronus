#pragma once
#include "../core/Subsystem.h"
#include <map>
#include <shared_mutex>
#include <tr.h>
#include <vector>

namespace tr {

// Channels used by the Engine (0-100 reserved)
constexpr int ENGINE_CHANNEL = 0;

// Keys to determine type of Events (0-1000 reserved)
namespace event {
    constexpr int INPUT        = 1;
    constexpr int WINDOW       = 2;
    constexpr int RENDER_DEBUG = 3;
}

class EventListener;
struct Event;

class EventSystem : public Subsystem<EventSystem> {
public:
    bool Initialize(Engine *e) override;
    inline std::string GetName() const override { return "EventSystem"; }

    void AddListener(EventListener *el);
    void RemoveListener(EventListener *el);
    void DispatchEvent(const Event &event, int channel = ENGINE_CHANNEL);
    bool HasChannel(int channel);

private:
    std::shared_mutex                           mListenersLock;
    std::map<int, std::vector<EventListener *>> mListeners;
};

class EventListener {
public:
    virtual void             OnEvent(const Event &e, int channel) = 0;
    virtual std::vector<int> SubscripeTo() const                  = 0;
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
