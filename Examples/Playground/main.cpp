#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/GameObject.h"
#include "tr/gameobject/SpriteComponent.h"
#include "tr/gameobject/World.h"
#include "tr/graphics/GraphicsHandler.h"
#include "tr/graphics/Texture.h"
#include "tr/util/Keys.h"
#include "tr/gameobject/Collision2DCapability.h"
#include "tr/gameobject/ColliderComponent2D.h"

class MyEntity : public tr::GameObject {
public:
    void OnWorldEnter() override
    {
        sSprite   = CreateComponent<tr::FlipbookComponent>("Test sprite");
        sCollider = CreateComponent<tr::ColliderComponent2D>("Collider");

        GetWorld().GetEngine().sResourceManager->LoadResource(
            "test_texture.json");

        sSprite->SetTexture("test_texture.json");
        sSprite->SetSize(tr::Vec2{ 5.f, 5.f });
        /* sSprite->SetUVs(tr::Vec2{ 0.f }, tr::Vec2{ 256.f, 256.f }); */
        sSprite->AddFrame(tr::Rect(0, 0, 256.f, 256.f), 500);
        sSprite->AddFrame(tr::Rect(100, 100, 256.f, 256.f), 1000);
        sSprite->AddFrame(tr::Rect(200, 200, 256.f, 256.f), 250);

        sCollider->AddShapeRect(tr::Vec4{ 0, 0, 5, 5 });
        sCollider->EnablePhysicsSimulation = true;
        sCollider->SetRelativeScale2D({ 1.f, 1.f });

        RootComponent = sCollider;
        sCollider->AttachChildComponent(sSprite);

        cInputComponent->InputRecieved.connect([&](const tr::InputEvent &e) {
            if (e.type == tr::InputEvent::Mouse) {
                tr::Vec2 newp{ e.XPos, e.YPos };
                sCollider->SetRelativePosition2D(newp);
            }
        });

       TickingGameObject = true; 
    }

private:
    tr::FlipbookComponent *    sSprite;
    tr::ColliderComponent2D *sCollider;
};

class MyGame : public tr::Game {
    void PreWorldLoad(tr::World &world) override {
        world.AttachWorldCapability<tr::Collision2DCapability>();
    }
    void PreWorldStart(tr::World &world) override { world.Spawn<MyEntity>(); }
};

int main()
{
    MyGame game;
    tr::Engine::Get().Start(&game);
    return 0;
}
