#include "Component.h"

tr::SceneComponent::SceneComponent(const std::string &name,
                                   GameObject *       owner,
                                   SceneComponent *   parent)
    : mName(name)
    , mOwner(owner)
    , mParentComponent(parent)
{
}

tr::Mat4 tr::SceneComponent::GetFinalTransform() const
{
    if (mParentComponent) {
        return mParentComponent->GetFinalTransform() * mRelTransform;
    }

    return mRelTransform;
}

const std::string &tr::SceneComponent::GetName() const { return mName; }

void tr::SceneComponent::AddChild(SceneComponent *child)
{
    if (!child)
        return;

    child->mParentComponent = this;
    mChilds.push_back(child);
    OnChildAdd(this);
}

void tr::SceneComponent::RemoveChild(SceneComponent *child)
{
    if (!child)
        return;

    OnChildRemove(child);

    for (auto it = std::begin(mChilds); it != std::end(mChilds); it++) {
        if (*it == child) {
            mChilds.erase(it);
            break;
        }
    }
}

void tr::SceneComponent::WorldLeave()
{
    OnWorldLeave();

    for (auto *child : mChilds) {
        child->WorldLeave();
    }

    while (mChilds.size() > 0) {
        RemoveChild(mChilds.back());
    }
}

void tr::SceneComponent::WorldEnter()
{
    for (auto *child : mChilds) {
        child->WorldEnter();
    }

    OnWorldEnter();
}
