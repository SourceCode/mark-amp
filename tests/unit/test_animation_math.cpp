#include "ui/animation/EasingFunctions.h"
#include "ui/animation/SpringPhysics.h"
#include "ui/animation/ValueInterpolator.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>

using namespace markamp::ui::animation;

TEST_CASE("Easing functions output correct bounds", "[animation][easing]")
{
    SECTION("Linear")
    {
        REQUIRE(Easing::calculate(0.0f, EasingType::Linear) == 0.0f);
        REQUIRE(Easing::calculate(0.5f, EasingType::Linear) == 0.5f);
        REQUIRE(Easing::calculate(1.0f, EasingType::Linear) == 1.0f);
    }

    SECTION("EaseOutQuad")
    {
        REQUIRE(Easing::calculate(0.0f, EasingType::EaseOutQuad) == 0.0f);
        REQUIRE(Easing::calculate(1.0f, EasingType::EaseOutQuad) == 1.0f);
        REQUIRE(Easing::calculate(0.5f, EasingType::EaseOutQuad) == 0.75f);
    }
}

TEST_CASE("ValueInterpolator computes correct midpoints", "[animation][interpolator]")
{
    SECTION("Float ranges")
    {
        REQUIRE(ValueInterpolator::interpolate(0.0f, 100.0f, 0.5f) == 50.0f);
        REQUIRE(ValueInterpolator::interpolate(10.0f, 20.0f, 1.0f) == 20.0f);
    }

    SECTION("wxColour ranges")
    {
        wxColour start(0, 0, 0, 0);
        wxColour end(255, 255, 255, 255);
        wxColour mid = ValueInterpolator::interpolate(start, end, 0.5f);
        REQUIRE(mid.Red() == 128);
        REQUIRE(mid.Green() == 128);
        REQUIRE(mid.Blue() == 128);
        REQUIRE(mid.Alpha() == 128);
    }

    SECTION("wxRect interpolation")
    {
        wxRect r1(0, 0, 100, 100);
        wxRect r2(100, 100, 200, 200);
        wxRect r_mid = ValueInterpolator::interpolate(r1, r2, 0.5f);
        REQUIRE(r_mid.GetX() == 50);
        REQUIRE(r_mid.GetY() == 50);
        REQUIRE(r_mid.GetWidth() == 150);
        REQUIRE(r_mid.GetHeight() == 150);
    }
}

TEST_CASE("SpringPhysics yields reasonable displacements", "[animation][spring]")
{
    SpringConfig config;
    config.stiffness = 100.0f;
    config.damping = 20.0f; // critically damped for mass=1.0
    config.mass = 1.0f;

    SECTION("Critical damping starts at 0 and goes towards 1")
    {
        REQUIRE(SpringPhysics::evaluate(0.0f, config) == 0.0f);

        float t01 = SpringPhysics::evaluate(0.1f, config);
        float t05 = SpringPhysics::evaluate(0.5f, config);
        float t20 = SpringPhysics::evaluate(2.0f, config);

        REQUIRE(t01 > 0.0f);
        REQUIRE(t05 > t01);

        // At 2 seconds, it should be very close to 1.0
        REQUIRE_THAT(t20, Catch::Matchers::WithinAbs(1.0f, 0.01f));
    }
}
