#include "Component.h"

#include <algorithm>
#include <assert.h>

tr::GameObjectComponent::GameObjectComponent() {}

tr::GameObjectComponent::~GameObjectComponent()
{
    for (auto ptr : mChildComponents) {
        if (!ptr)
            continue;

        // We need to make sure that no child component keeps refering to us
        ptr->mParentComponent = nullptr;
    }
}

void tr::GameObjectComponent::AttachChildComponent(GameObjectComponent *ptr)
{
    assert(ptr);

    // Detach a child component from his previous parent.
    // Note: We need to make sure that a destructed parent component cleans up
    // his references to his childs
    if (ptr->mParentComponent) {
        ptr->mParentComponent->DetachChildComponent(ptr);
        ptr->mParentComponent = nullptr;
    }

    // Adding the same child twice would result in a double tick
    if (!IsChild(ptr)) {
        mChildComponents.push_back(ptr);
        ptr->mParentComponent = this;
    }
}

void tr::GameObjectComponent::DetachChildComponent(GameObjectComponent *ptr)
{
    if (auto it = std::find(std::begin(mChildComponents),
                            std::end(mChildComponents), ptr);
        it != std::end(mChildComponents)) {

        (*it)->mParentComponent = nullptr;
        mChildComponents.erase(it);
    }
}

bool tr::GameObjectComponent::IsChild(GameObjectComponent *ptr) const
{
    return std::find(std::begin(mChildComponents), std::end(mChildComponents),
                     ptr)
        != std::end(mChildComponents);
}

void tr::GameObjectComponent::UpdateComponent() { OnComponentUpdate(); }

void tr::GameObjectComponent::SetOwner(GameObject &owner)
{
    if (!mOwner) {
        mOwner = &owner;
    }
}

tr::GameObject &tr::GameObjectComponent::GetOwner()
{

    if (!mOwner) {
        throw std::runtime_error(
            "Tried to get owner of component but no owner has been set.");
    }

    return *mOwner;
}

void tr::SceneComponent::SetRelativeTranslation(Vec3 translation)
{
    mTransform.translation = translation;
}

void tr::SceneComponent::SetRelativeRotation(EulerAngles rotation)
{
    mTransform.rotation = Quaternion(rotation);
}

void tr::SceneComponent::SetRelativeScale(Vec3 scale)
{
    mTransform.scale = scale;
}

void tr::SceneComponent::AddRelativeTranslation(Vec3 translation)
{
    mTransform.translation += translation;
}

void tr::SceneComponent::AddRelativeRotation(EulerAngles rotation)
{
    Quaternion rot(rotation);
    mTransform.rotation *= rot;
}

void tr::SceneComponent::AddRelativeScale(Vec3 scale)
{
    mTransform.scale *= scale;
}

tr::Vec3 tr::SceneComponent::GetRelativeTranslation() const
{
    return mTransform.translation;
}

tr::EulerAngles tr::SceneComponent::GetRelativeRotation() const
{
    return math::eulerAngles(mTransform.rotation);
}

tr::Vec3 tr::SceneComponent::GetRelativeScale() const
{
    return mTransform.scale;
}

tr::Vec3 tr::SceneComponent::GetAbsoluteTranslation() const
{
    auto *const p = GetParentAsSceneComponent();
    if (p) {
        return p->GetAbsoluteTranslation() + GetRelativeTranslation();
    } else {
        return GetRelativeTranslation();
    }
}

tr::EulerAngles tr::SceneComponent::GetAbsoluteRotation() const
{
    auto *const p = GetParentAsSceneComponent();
    if (p) {
        Quaternion p_quat(p->GetAbsoluteRotation());
        return math::eulerAngles(p_quat * mTransform.rotation);
    } else {
        return GetRelativeRotation();
    }
}

tr::Vec3 tr::SceneComponent::GetAbsoluteScale() const
{
    auto *const p = GetParentAsSceneComponent();
    if (p) {
        return p->GetAbsoluteScale() * GetRelativeScale();
    } else {
        return GetRelativeScale();
    }
}

tr::Mat4 tr::SceneComponent::GetRelativeTransform() const
{
    return static_cast<Mat4>(mTransform);
}

tr::Mat4 tr::SceneComponent::GetAbsoluteTransform() const
{
    auto *const p = GetParentAsSceneComponent();
    if (p) {
        return p->GetAbsoluteTransform() * GetRelativeTransform();
    } else {
        return p->GetRelativeTransform();
    }
}

tr::SceneComponent *tr::SceneComponent::GetParentAsSceneComponent() const
{
    if (!HasParent() || !(GetParent()->IsComponentRelative()))
        return nullptr;

    return static_cast<SceneComponent *>(GetParent());
}
