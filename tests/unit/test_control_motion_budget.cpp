// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/MotionModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

static auto make_test_motion() -> MotionModel
{
    MotionModel model;
    model.set_tokens({
        {"hover", MotionContext::kHover, 150, EasingCurve::kEaseOut},
        {"press", MotionContext::kPress, 80, EasingCurve::kEaseIn},
        {"reveal", MotionContext::kReveal, 250, EasingCurve::kEaseInOut},
        {"dismiss", MotionContext::kDismiss, 200, EasingCurve::kEaseOut},
    });
    return model;
}

TEST_CASE("Token context lookup", "[motion][token]")
{
    auto model = make_test_motion();
    REQUIRE(model.token_for(MotionContext::kHover).duration_ms == 150);
    REQUIRE(model.token_for(MotionContext::kPress).duration_ms == 80);
}

TEST_CASE("Fallback for missing context", "[motion][token]")
{
    auto model = make_test_motion();
    const auto token = model.token_for(MotionContext::kExpand);
    REQUIRE(token.duration_ms == 200); // default fallback
}

TEST_CASE("Reduced motion disables non-essential", "[motion][reduced]")
{
    auto model = make_test_motion();
    model.set_reduced_motion(true);
    REQUIRE(model.effective_duration(MotionContext::kHover) == 0);
    REQUIRE(model.effective_duration(MotionContext::kPress) == 0);
}

TEST_CASE("Essential contexts preserved in reduced motion", "[motion][reduced]")
{
    auto model = make_test_motion();
    model.set_reduced_motion(true);
    REQUIRE(model.effective_duration(MotionContext::kReveal) == 250);
    REQUIRE(model.effective_duration(MotionContext::kDismiss) == 200);
}

TEST_CASE("Is essential classification", "[motion][reduced]")
{
    REQUIRE(MotionModel::is_essential(MotionContext::kReveal));
    REQUIRE(MotionModel::is_essential(MotionContext::kDismiss));
    REQUIRE_FALSE(MotionModel::is_essential(MotionContext::kHover));
    REQUIRE_FALSE(MotionModel::is_essential(MotionContext::kPress));
}

TEST_CASE("Latency budget enforcement", "[motion][budget]")
{
    auto model = make_test_motion();
    model.set_latency_budget_ms(100);
    REQUIRE_FALSE(model.exceeds_budget(MotionContext::kPress)); // 80 < 100
    REQUIRE(model.exceeds_budget(MotionContext::kHover));       // 150 > 100
    REQUIRE(model.exceeds_budget(MotionContext::kReveal));      // 250 > 100
}

TEST_CASE("Normal mode returns full duration", "[motion][duration]")
{
    auto model = make_test_motion();
    REQUIRE(model.effective_duration(MotionContext::kHover) == 150);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
