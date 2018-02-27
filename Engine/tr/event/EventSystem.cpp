#include "EventSystem.h"
#include <algorithm>

bool tr::EventSystem::Initialize(Engine *e)
{
    return Subsystem<EventSystem>::Initialize(e);
}

void tr::EventSystem::AddListener(EventListener *el)
{
    std::unique_lock<std::shared_mutex> lck(mListenersLock);

    for (auto channel : el->SubscripeTo()) {
        mListeners[channel].push_back(el);
    }
}

void tr::EventSystem::RemoveListener(EventListener *el)
{
    std::unique_lock<std::shared_mutex> lck(mListenersLock);

    for (auto channel : el->SubscripeTo()) {
        if (auto it = mListeners.find(channel); it != mListeners.end())
            it->second.erase(std::find(
                std::begin(it->second), std::end(it->second), el));
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

