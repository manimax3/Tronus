#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/Sprite2DComponent.h"
#include "tr/gameobject/World.h"

class MyEntity : public tr::GameObject {
public:
    MyEntity(tr::World *w, const std::string &name, tr::Mat4 transform)
        : tr::GameObject(w, name, transform)
    {
        mSpriteComponent
            = CreateComponent<tr::Sprite2DComponent>("Sprite", cRootComponent);
        mSpriteComponent->mColor = tr::Vec4(1.f, 0.f, 0.f, 1.f);
        mSpriteComponent->mDrawBounds
            = tr::Rect(tr::Vec2(10.f, 10.f), tr::Vec2(30.f));

        mTickable = true;
    }

    void OnEvent(const tr::Event &e) override
    {
        if (e.Identifier != tr::event::INPUT)
            return;

        const tr::InputEvent &ie = static_cast<const tr::InputEvent &>(e);

        if (ie.type == tr::InputEvent::MouseButton
            && ie.action == tr::InputEvent::PRESS) {
            mSpriteComponent->mDrawBounds.pos
                = LastMousePos - (mSpriteComponent->mDrawBounds.size * 0.5f);
        } else if (ie.type == tr::InputEvent::Mouse) {
            LastMousePos = tr::Vec2(ie.XPos, ie.YPos);
        }
    }

    tr::Sprite2DComponent *mSpriteComponent = nullptr;
    tr::Vec2               LastMousePos     = tr::Vec2(0.f);
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
    tr::Engine engine(&game);
    engine.Start();
    return 0;
}
