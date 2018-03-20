#pragma once
#include "../event/CommonEvents.h"
#include "../math/Math.h"
#include <string>
#include <tr.h>
#include <vector>

namespace tr {
class GameObject;

class SceneComponent {
public:
    SceneComponent(const std::string &name,
                   GameObject *       owner,
                   SceneComponent *   parent = nullptr);
    virtual ~SceneComponent() = default;

    Mat4               GetFinalTransform() const;
    void               AddChild(SceneComponent *child);
    void               RemoveChild(SceneComponent *child);
    void               WorldEnter();
    void               WorldLeave();
    const std::string &GetName() const;

    void HandleEvent(const Event &e);

    virtual void TickComponent(){};

protected:
    virtual void OnChildAdd(SceneComponent *child){};
    virtual void OnChildRemove(SceneComponent *child){};
    virtual void OnWorldEnter(){};
    virtual void OnWorldLeave(){};
    virtual void OnEvent(const Event &e) {}

public:
    bool        mTickable = false;
    std::string mName;
    Mat4        mRelTransform;

protected:
    GameObject *                  mOwner           = nullptr;
    SceneComponent *              mParentComponent = nullptr;
    std::vector<SceneComponent *> mChilds;
};
}
