#include "EventSystem.h"
#include "../core/Engine.h"
#include <algorithm>

bool tr::EventSystem::Initialize(Engine *e)
{
    return Subsystem<EventSystem>::Initialize(e);
}

bool tr::EventSystem::Shutdown()
{
    // TODO: Figure out why a unique lock produces a deadlock.
    // This is not 100% thread safe but at this point there shouldnt
    // be anything else running concurrently anymore
    std::shared_lock<std::shared_mutex> lck(mListenersLock);

    for (auto &channel : mListeners) {
        for (auto *listener : channel.second) {
            listener->mEventSystem = nullptr;
        }
    }

    return Subsystem::Shutdown();
}

void tr::EventSystem::AddListener(EventListener *el)
{
    std::unique_lock<std::shared_mutex> lck(mListenersLock);

    el->mEventSystem = this;

    for (auto channel : el->SubscripeTo()) {
        mListeners[channel].push_back(el);
    }
}

void tr::EventSystem::RemoveListener(EventListener *el)
{
    if (!el->mEventSystem)
        return;

    std::unique_lock<std::shared_mutex> lck(mListenersLock);

    el->mEventSystem = nullptr;

    for (auto channel : el->SubscripeTo()) {
        if (auto it = mListeners.find(channel); it != mListeners.end())
            it->second.erase(
                std::find(std::begin(it->second), std::end(it->second), el));
    }
}

void tr::EventSystem::DispatchEvent(const Event &event, int channel)
{
    if (!HasChannel(channel))
        return;

    std::shared_lock<std::shared_mutex> lck(mListenersLock);

    for (auto listener : mListeners[channel])
        listener->OnEvent(event, channel);
}

bool tr::EventSystem::HasChannel(int channel)
{
    std::shared_lock<std::shared_mutex> lck(mListenersLock);
    return mListeners.find(channel) != mListeners.end();
}

tr::EventListener::~EventListener()
{
    if (mEventSystem)
        mEventSystem->RemoveListener(this);
}
