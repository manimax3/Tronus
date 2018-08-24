#include "glm/gtc/matrix_transform.hpp"
#include "tr/core/Engine.h"
#include "tr/gameobject/CameraComponent.h"
#include "tr/gameobject/ColliderComponent2D.h"
#include "tr/gameobject/Collision2DCapability.h"
#include "tr/gameobject/Game.h"
#include "tr/gameobject/GameObject.h"
#include "tr/gameobject/MovementComponent.h"
#include "tr/gameobject/SpriteComponent.h"
#include "tr/gameobject/StaticMeshComponent.h"
#include "tr/gameobject/World.h"
#include "tr/graphics/DeferredRenderer.h"
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
        sMovement = CreateComponent<tr::MovementComponent>("Move");
        sMesh     = CreateComponent<tr::StaticMeshComponent>("pyramid");

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
        /* sSprite->AttachChildComponent(sMovement); */
        sMovement->AttachChildComponent(sCamera);
        sMovement->RelativeComponent = true;
        sCamera->RelativeComponent   = true;
        sCamera->SetRelativeTranslation(tr::Vec3(0, 0.f, 2.f));

        cInputComponent->InputRecieved.connect([&](const tr::InputEvent &e) {
            if (e.type == tr::InputEvent::Mouse) {
                tr::Vec2 newp{ e.XPos, e.YPos };
                sCollider->SetRelativePosition2D(newp);
            }

            if (e.type == tr::InputEvent::Keyboard) {
                if (e.action == tr::InputEvent::PRESS) {

                    if (e.Key == tr::KEY_D) {
                        sMovement->AddVelocity({ 10.f, 0, 0 });
                    } else if (e.Key == tr::KEY_A) {
                        sMovement->AddVelocity({ -10.f, 0, 0 });
                    } else if (e.Key == tr::KEY_W) {
                        sMovement->AddVelocity({ 0.f, 10.f, 0 });
                    } else if (e.Key == tr::KEY_S) {
                        sMovement->AddVelocity({ 0.f, -10.f, 0 });
                    }

                } else if (e.action == tr::InputEvent::RELEASE) {
                    if (e.Key == tr::KEY_A) {
                        sMovement->AddVelocity({ 10.f, 0, 0 });
                    } else if (e.Key == tr::KEY_D) {
                        sMovement->AddVelocity({ -10.f, 0, 0 });
                    } else if (e.Key == tr::KEY_S) {
                        sMovement->AddVelocity({ 0.f, 10.f, 0 });
                    } else if (e.Key == tr::KEY_W) {
                        sMovement->AddVelocity({ 0.f, -10.f, 0 });
                    }
                }
            }
        });

        TickingGameObject = true;

        sMesh->Mesh
            = tr::Engine::Get().sResourceManager->LoadResource("Arms.trb");

        sMesh->Material = tr::Engine::Get().sResourceManager->LoadResource(
            "material_Arm.json");

        sMesh->SetRelativeTranslation(tr::Vec3(0, -0.5, 0));
        sMesh->SetRelativeScale(tr::Vec3(1.f / 9.f));
        /* sMesh->SetRelativeScale(tr::Vec3(5.f)); */
    }

private:
    tr::FlipbookComponent *  sSprite;
    tr::ColliderComponent2D *sCollider;
    tr::CameraComponent *    sCamera;
    tr::MovementComponent *  sMovement;
    tr::StaticMeshComponent *sMesh;
};

class MyGame : public tr::Game {
    void PreWorldLoad(tr::World &world) override
    {
        world.AttachWorldCapability<tr::Collision2DCapability>();
    }
    void PreWorldStart(tr::World &world) override
    {
        world.Spawn<MyEntity>();
        auto &df = tr::Engine::Get().sGraphicsHandler->GetDeferredRenderer();
        /* tr::PointLight l; */
        /* l.position  = tr::Vec3(0.f, 0.f, 0.f); */
        /* l.color     = tr::Vec3(1.f); */
        /* l.Constant  = 1.f; */
        /* l.Linear    = 0.09f; */
        /* l.Quadratic = 0.032f; */
        /* df.AddPointLight(l); */
        tr::DirectionalLight l;
        l.color     = tr::Vec3(1.f, 0.f, 0.f);
        l.direction = tr::Vec3(0.5f, -1.f, 0.f);
        df.AddDirectionalLight(l);
    }
};

int main()
{
    MyGame game;
    tr::Engine::Get().Start(&game);
    return 0;
}
