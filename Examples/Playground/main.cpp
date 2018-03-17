#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/Sprite2DComponent.h"
#include "tr/gameobject/World.h"

class MyEntity : public tr::GameObject {
public:
    MyEntity(tr::World *w, const std::string &name, tr::Mat4 transform)
        : tr::GameObject(w, name, transform)
    {
        mSpriteComponent = CreateComponent<tr::Sprite2DComponent>("Sprite", cRootComponent);
        mSpriteComponent->mColor = tr::Vec4(1.f, 0.f, 0.f, 1.f);
        mSpriteComponent->mDrawBounds
            = tr::Rect(tr::Vec2(10.f, 10.f), tr::Vec2(30.f));

        mTickable = true;
    }

    void OnTick() override {
        mSpriteComponent->mDrawBounds.pos += tr::Vec2(0.1f, 0.1f);
    }

    tr::Sprite2DComponent *mSpriteComponent = nullptr;
};

class MyGame : public tr::Game {
    void OnWorldLoad(tr::World &world) override
    {
        auto go = world.SpawnGameObject<MyEntity>(std::string("Little Red Box"),
                                                  tr::Mat4::Identity());
    }
};

int main()
{
    MyGame game;
    tr::Engine engine(&game);
    engine.Start();
    return 0;
}
