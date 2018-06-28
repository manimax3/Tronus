#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/GameObject.h"
#include "tr/gameobject/Sprite2DComponent.h"
#include "tr/gameobject/World.h"
#include "tr/graphics/GraphicsHandler.h"
#include "tr/graphics/Texture.h"
#include "tr/util/Keys.h"

class MyGame : public tr::Game {
    void OnWorldLoad(tr::World &world) override
    {
        auto handle = world.CreateGameObject<tr::Sprite2DComponent,
                                             tr::SceneComponent>();
        auto sprite = world.GetComponent<tr::Sprite2DComponent>(handle);
        auto res    = world.mEngine->sResourceManager->LoadResource(
            "test_texture.json");
        sprite->texture = tr::ResCast<tr::Texture>(res).get();
        sprite->color   = tr::Vec4(1.f);
        sprite->dirty   = true;
    }
};

int main()
{
    MyGame     game;
    tr::Engine engine(game);
    engine.Start();
    return 0;
}
