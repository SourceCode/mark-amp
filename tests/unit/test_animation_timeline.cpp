#include "ui/animation/AnimationTimeline.h"
#include "ui/animation/TransitionManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui::animation;

TEST_CASE("AnimationTimeline respects reduced motion", "[animation][timeline]")
{
    AnimationTimeline::set_reduced_motion(true);
    REQUIRE(AnimationTimeline::is_reduced_motion() == true);

    AnimationTimeline timeline;
    float current_val = 0.0f;
    bool finished = false;

    AnimationConfig config;
    config.duration = std::chrono::milliseconds(300);

    timeline.animate<float>(
        0.0f,
        100.0f,
        config,
        [&](const float& val) { current_val = val; },
        [&]() { finished = true; });

    // Because reduced_motion is true, it should have instantly resolved
    REQUIRE(current_val == 100.0f);
    REQUIRE(finished == true);

    AnimationTimeline::set_reduced_motion(false);
}
