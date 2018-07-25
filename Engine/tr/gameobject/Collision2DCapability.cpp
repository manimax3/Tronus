#include "Collision2DCapability.h"

#include "../profile/Profiler.h"

void tr::Collision2DCapability::OnUpdate()
{
    EASY_BLOCK("Box2D Simultation Step");
    mBox2DWorld->Step(1.f / 60.f, VelocityIterations, PositionIterations);
}

void tr::Collision2DCapability::OnLoad()
{
    mBox2DWorld
        = std::make_unique<b2World>(b2Vec2{ Gravitation.x, Gravitation.y });
}
