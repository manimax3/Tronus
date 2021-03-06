#include "GameObject.h"

#include "World.h"

#include <assert.h>

tr::GameObject::~GameObject()
{
    // We need to break up the hierachy
    for (auto &[n, comp] : mComponents) {
        comp->mParentComponent = nullptr;
    }
}

tr::GameObjectComponent &tr::GameObject::GetRootComponent()
{
    static SceneComponent s;
    return HasRootComponent() ? *RootComponent : s;
}

void tr::GameObject::Update()
{
    if (!PreventComponentTicking) {
        for (auto &[k, v] : mComponents) {
            if (v->IsComponentTicking())
                v->UpdateComponent();
        }
    }

    if (IsGameObjectTicking())
        OnUpdate();
}

void tr::GameObject::EnterWorld(World &world)
{
    assert(!mWorld);
    mWorld = &world;

    // Setup the input component
    if (!DisableInputComponentConstruction) {
        cInputComponent = std::make_shared<InputComponent>();
        cInputComponent->SetOwner(*this);
        cInputComponent->OnWorldEnter(world);
    }

    OnWorldEnter();

    if (!DisableInputComponent && !DisableInputComponentConstruction) {
        // Apparently no one wants to prevent us from setting our listenting up
        // Those fools ...
        cInputComponent->SetupInputListening();
        cInputComponent->PostWorldEnter();
    }

    for (const auto &[k, comp] : mComponents) {
        comp->PostWorldEnter();
    }
}

tr::World &tr::GameObject::GetWorld()
{
    if (!mWorld) {
        throw std::runtime_error("GameObject doesnt belong to a world.");
    }

    return *mWorld;
}
