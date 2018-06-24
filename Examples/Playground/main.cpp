#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/GameObject.h"
#include "tr/gameobject/Sprite2DComponent.h"
#include "tr/gameobject/World.h"
#include "tr/graphics/GraphicsHandler.h"
#include "tr/util/Keys.h"

class MyGame : public tr::Game {
    void OnWorldLoad(tr::World &world) override
    {
        world.CreateGameObject<tr::Sprite2DComponent, tr::SceneComponent>();
    }
};

int main()
{
    MyGame     game;
    tr::Engine engine(game);
    engine.Start();
    return 0;
}
