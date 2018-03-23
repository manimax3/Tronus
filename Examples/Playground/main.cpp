#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/Sprite2DComponent.h"
#include "tr/graphics/GraphicsHandler.h"
#include "tr/gameobject/World.h"
#include "tr/gameobject/GameObject.h"
#include "tr/util/Keys.h"
#include "glm/gtc/matrix_transform.hpp"

class MyEntity : public tr::GameObject {
public:
    MyEntity(tr::World *w, const std::string &name, tr::Mat4 transform)
        : tr::GameObject(w, name, transform)
    {
        mSpriteComponent
            = CreateComponent<tr::Sprite2DComponent>("Sprite", cRootComponent);
        mSpriteComponent->mColor = tr::Vec4(1.f, 0.f, 0.f, 1.f);
        mSpriteComponent->mDrawBounds
            = tr::Rect(tr::Vec2(0.f, 0.f), tr::Vec2(30.f, 30.f));
        mSpriteComponent->mOrigin = tr::Vec2(15.f, 15.f);

        mTickable = true;
    }

    void OnEvent(const tr::Event &e) override
    {
        if (e.Identifier != tr::event::INPUT)
            return;

        const tr::InputEvent &ie = static_cast<const tr::InputEvent &>(e);

        if (ie.type == tr::InputEvent::MouseButton
            && ie.action == tr::InputEvent::PRESS) {
            cRootComponent->mRelTransform
                = glm::translate(tr::Mat4(1.f), tr::Vec3(Context.GfxHandler->GetMousePos(), 0.f));
            mSpriteComponent->mDirty = true;
        } else if (ie.type == tr::InputEvent::Keyboard && ie.Key == tr::KEY_B
                   && ie.action == tr::InputEvent::PRESS) {
            mSpriteComponent->mRelTransform = glm::rotate(
                mSpriteComponent->mRelTransform, glm::radians(45.f), tr::Vec3(0.f, 0.f, 1.f));
            mSpriteComponent->mDirty = true;
        }
    }

    tr::Sprite2DComponent *mSpriteComponent = nullptr;
};

class MyGame : public tr::Game {
    void OnWorldLoad(tr::World &world) override
    {
        world.SpawnGameObject<MyEntity>(std::string("Little Red Box"));
    }
};

int main()
{
    MyGame     game;
    tr::Engine engine(game);
    engine.Start();
    return 0;
}
