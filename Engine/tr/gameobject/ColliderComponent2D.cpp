#include "ColliderComponent2D.h"
#include "Box2D/Box2D.h"
#include "Collision2DCapability.h"
#include "World.h"

void tr::ColliderComponent2D::OnWorldEnter(World &world)
{
    Collision2DCapability *cap = nullptr;

    if (!(cap = world.GetWorldCapability<Collision2DCapability>()))
        return; // World doesnt have the collison capability

    const auto pos = GetAbsolutePosition2D();

    b2BodyDef def;
    def.position.Set(pos.x / 10.f, pos.y / 10.f);
    def.active = EnablePhysicsSimulation;
    def.type   = b2_dynamicBody;

    mBody = cap->GetWorldInternal().CreateBody(&def);

    TickingComponent = true;
}

void tr::ColliderComponent2D::AddShapeRect(Rect  rect,
                                           float friction,
                                           float density,
                                           float restitution)
{
    b2PolygonShape shape;
    shape.SetAsBox(rect.size.x / 20, rect.size.y / 20,
                   b2Vec2{ (rect.pos.x + (rect.size.x)) / 10,
                           (rect.pos.y + (rect.size.y / 2)) / 10 },
                   GetAbsoluteRotation2D().get());

    b2FixtureDef def;
    def.density     = density;
    def.friction    = friction;
    def.restitution = restitution;
    def.shape       = &shape;

    (static_cast<b2Body *>(mBody))->CreateFixture(&def);
}

void tr::ColliderComponent2D::AddShapeCircle(float radius,
                                             Vec2  position,
                                             float friction,
                                             float density,
                                             float restitution)
{
    b2CircleShape shape;
    shape.m_p.Set(position.x / 10.f, position.y / 10.f);
    shape.m_radius = radius / 10.f;

    b2FixtureDef def;
    def.density     = density;
    def.friction    = friction;
    def.restitution = restitution;
    def.shape       = &shape;

    (static_cast<b2Body *>(mBody))->CreateFixture(&def);
}

void tr::ColliderComponent2D::PostWorldEnter()
{
    if (mBody)
        (static_cast<b2Body *>(mBody))->SetActive(EnablePhysicsSimulation);
}

void tr::ColliderComponent2D::OnRelativeChange()
{
    if (DisableCopyAgainSemantic)
        return;

    if (!mBody)
        return; // Not that much we can do

    b2Body &body = *static_cast<b2Body *>(mBody);
    const auto pos = GetRelativePosition2D();

    body.SetTransform({ pos.x, pos.y }, GetRelativeRotation2D().get());
}

void tr::ColliderComponent2D::OnComponentUpdate()
{
    if (!mBody)
        return; // Not that much we can do

    b2Body &body = *static_cast<b2Body *>(mBody);

    if (!body.IsActive())
        return;

    const auto pos = body.GetPosition();

    DisableCopyAgainSemantic = true;
    
    SetRelativeRotation2D(Radians(body.GetAngle()));
    SetRelativePosition2D({ pos.x, pos.y });

    DisableCopyAgainSemantic = false;
}

