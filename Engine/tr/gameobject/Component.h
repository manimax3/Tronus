#pragma once
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

    virtual void TickComponent(){};

protected:
    virtual void OnChildAdd(SceneComponent *child){};
    virtual void OnChildRemove(SceneComponent *child){};
    virtual void OnWorldEnter(){};
    virtual void OnWorldLeave(){};

public:
    bool        mTickable = false;
    std::string mName;
    Mat4        mRelTransform = Mat4::Identity();

protected:
    GameObject *                  mOwner           = nullptr;
    SceneComponent *              mParentComponent = nullptr;
    std::vector<SceneComponent *> mChilds;
};
}
