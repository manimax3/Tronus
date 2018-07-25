#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/GameObject.h"
#include "tr/gameobject/SpriteComponent.h"
#include "tr/gameobject/World.h"
#include "tr/graphics/GraphicsHandler.h"
#include "tr/graphics/Texture.h"
#include "tr/util/Keys.h"

class MyEntity : public tr::GameObject {
public:
    void OnWorldEnter() override
    {
        sSprite = CreateComponent<tr::SpriteComponent>("Test sprite");

        GetWorld().GetEngine().sResourceManager->LoadResource(
            "test_texture.json");

        sSprite->SetTexture("test_texture.json");
        sSprite->SetUVs(tr::Vec2{ 0.f }, tr::Vec2{ 256.f, 256.f });
        sSprite->SetSize(tr::Vec2{ 50.f, 50.f });

        RootComponent = sSprite;

        cInputComponent->InputRecieved.connect([&](const tr::InputEvent &e) {
            if (e.type == tr::InputEvent::Mouse) {
                tr::Vec2 newp{ e.XPos, e.YPos };
                sSprite->SetRelativePostion2D(newp);
            }
        });
    }

private:
    tr::SpriteComponent *sSprite;
};

class MyGame : public tr::Game {
    void OnWorldLoad(tr::World &world) override { world.Spawn<MyEntity>(); }
};

int main()
{
    MyGame game;
    tr::Engine::Get().Start(&game);
    return 0;
}
