#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/CameraComponent.h"
#include "tr/gameobject/ColliderComponent2D.h"
#include "tr/gameobject/Collision2DCapability.h"
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
        sSprite   = CreateComponent<tr::FlipbookComponent>("Test sprite");
        sCollider = CreateComponent<tr::ColliderComponent2D>("Collider");
        sCamera   = CreateComponent<tr::CameraComponent>("Camera");

        GetWorld().GetEngine().sResourceManager->LoadResource(
            "test_texture.json");

        sSprite->SetTexture("test_texture.json");
        sSprite->SetSize(tr::Vec2{ 5.f, 5.f });
        sSprite->SetUVs(tr::Vec2{ 0.f }, tr::Vec2{ 256.f, 256.f });
        sSprite->AddFrame(tr::Rect(0, 0, 256.f, 256.f), 500);
        sSprite->AddFrame(tr::Rect(100, 100, 256.f, 256.f), 1000);
        sSprite->AddFrame(tr::Rect(200, 200, 256.f, 256.f), 250);

        sCollider->AddShapeRect(tr::Vec4{ 0, 0, 5, 5 });
        sCollider->EnablePhysicsSimulation = true;
        sCollider->SetRelativeScale2D({ 1.f, 1.f });

        RootComponent = sCollider;
        sCollider->AttachChildComponent(sSprite);
        sSprite->AttachChildComponent(sCamera);
        sCamera->RelativeComponent = false;

        cInputComponent->InputRecieved.connect([&](const tr::InputEvent &e) {
            if (e.type == tr::InputEvent::Mouse) {
                tr::Vec2 newp{ e.XPos, e.YPos };
                sCollider->SetRelativePosition2D(newp);
            }
        });

        TickingGameObject = true;
    }

private:
    tr::FlipbookComponent *  sSprite;
    tr::ColliderComponent2D *sCollider;
    tr::CameraComponent *    sCamera;
};

class MyGame : public tr::Game {
    void PreWorldLoad(tr::World &world) override
    {
        world.AttachWorldCapability<tr::Collision2DCapability>();
    }
    void PreWorldStart(tr::World &world) override
    {
        world.Spawn<MyEntity>();
        tr::ForwardRenderer::RenderInfo info;
        info.mesh = tr::ResCast<tr::StaticMesh>(
            tr::Engine::Get().sResourceManager->LoadResource("Object001.trb"));
        info.material
            = std::shared_ptr<tr::PhongMaterial>(new tr::PhongMaterial(
                tr::ResCast<tr::Texture>(
                    tr::Engine::Get().sResourceManager->GetResource(
                        "test_texture.json")),
                tr::ResCast<tr::Texture>(
                    tr::Engine::Get().sResourceManager->GetResource(
                        "test_texture.json")),
                tr::ResCast<tr::Texture>(
                    tr::Engine::Get().sResourceManager->GetResource(
                        "test_texture.json")),
                1.f));

        tr::Mat4 m(1.f);
        m          = tr::math::translate(m, tr::Vec3(0, -10, -50));
        m          = tr::math::scale(m, tr::Vec3(20.f));
        info.model = m;

        tr::Engine::Get()
            .sGraphicsHandler->GetForwardRenderer()
            .CreateRenderInfo(std::move(info));
    }
};

int main()
{
    MyGame game;
    tr::Engine::Get().Start(&game);
    return 0;
}
