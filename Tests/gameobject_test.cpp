#include "catch.hpp"

#include "tr/gameobject/System.h"

#include <iostream>

using namespace tr;

struct TestComponent {
    float x, y;
};

struct TestSystem : public System<TestComponent> {
    explicit TestSystem(World &w)
        : System<TestComponent>(w)
    {
    }

    void OnUpdate() override
    {
        for (auto &[handle, go] : mRegisteredGameObjects) {
        }
    }
};

TEST_CASE("GameObject")
{
    World w;
    w.AttachSystem<TestSystem>();
    auto g = w.CreateGameObject<TestComponent>();
    REQUIRE(std::get<0>(w.GetComponents<TestComponent>(g)));

    w.Update();

    w.DestroyGameObject(g);
}
