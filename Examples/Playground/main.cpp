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
            = tr::Engine::Get().sResourceManager->LoadResource("Object001.trb");

        static std::shared_ptr<tr::PhongMaterial> material(
            new tr::PhongMaterial(
                tr::ResCast<tr::Texture>(
                    tr::Engine::Get().sResourceManager->GetResource(
                        "test_texture.json")),
                tr::ResCast<tr::Texture>(
                    tr::Engine::Get().sResourceManager->GetResource(
                        "test_texture.json")),
                1.f));

        sMesh->Material = material;

        sMesh->SetRelativeTranslation(tr::Vec3(0, -10, -50));
        sMesh->SetRelativeScale(tr::Vec3(20.f));
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
    void PreWorldStart(tr::World &world) override { world.Spawn<MyEntity>(); }
};

int main()
{
    MyGame game;
    tr::Engine::Get().Start(&game);
    return 0;
}
