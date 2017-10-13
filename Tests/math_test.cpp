#include "catch.hpp"
#include "tr/math/Math.h"

TEST_CASE("Vector3", "[math]")
{
    // Creating two Vectors
    tr::Vec3 a;
    tr::Vec3 b;

    SECTION("Checking for Equality")
    {
        REQUIRE(a == b);
        a.x = 10;
        REQUIRE(a != b);
    }

    SECTION("Operators")
    {
        tr::Vec3 c(0);
        a = {25, 33, 12};

        b += a;
        REQUIRE(b != c);

        b -= a;
        b *= 10;
        REQUIRE(b == c);
    }
}

TEST_CASE("Quaternion", "[math]")
{
    SECTION("Rotating a vector")
    {
        tr::Quaternion q = tr::Quaternion::Rotation(3.14f / 2.f, {0,0,1});

        tr::Vec3 vec{1.f, 0, 0};

        auto r = q * vec;

        REQUIRE(r.x == Approx(0.f).margin(0.01));
        REQUIRE(r.y == Approx(1.f).margin(0.01));
    }
}