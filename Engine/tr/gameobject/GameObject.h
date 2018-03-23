#pragma once
#include "../math/Math.h"
#include "Component.h"
#include <memory>
#include <string>
#include <tr.h>

namespace tr {

class World;

struct GOContext {
    class ResourceManager *ResManager = nullptr;
    class EventSystem *    EvtHandler = nullptr;
    class GraphicsHandler *GfxHandler = nullptr;
    class Engine *         Eng        = nullptr;
    class World *          Wrld       = nullptr;
};

class GameObject {
public:
    GameObject(World *w, const std::string &name, Mat4 transform);
    virtual ~GameObject() = default;

    void EnterWorld();
    void LeaveWorld();

    void TickGameObject(double delta);
    void HandleEvent(const Event &e);

    SceneComponent *cRootComponent = nullptr;
    bool            mTickable      = false;

    const std::string &GetName() const { return mName; }

    GOContext Context;

    friend class World;

protected:
    virtual void OnWorldEnter(){};
    virtual void OnWorldLeave(){};
    virtual void OnTick(){};
    virtual void OnEvent(const Event &e) {}

    const std::string mName;
    double            mLastDelta = 0.0;

    template<
        typename C,
        typename = std::enable_if_t<
            (std::is_base_of_v<SceneComponent,
                               C> || std::is_same_v<C, SceneComponent>)&&std::
                is_constructible_v<C,
                                   const std::string &,
                                   GameObject *,
                                   SceneComponent *>>>
    C *CreateComponent(const std::string &name,
                       SceneComponent *   parent = nullptr)
    {
        CompUPtr c(new C(name, this, parent));
        mComponents.push_back(std::move(c));

        if (parent) {
            parent->AddChild(mComponents.back().get());
        }

        return static_cast<C *>(mComponents.back().get());
    }

public:
    using CompUPtr = std::unique_ptr<SceneComponent>;
    std::vector<CompUPtr> mComponents;
};
};
