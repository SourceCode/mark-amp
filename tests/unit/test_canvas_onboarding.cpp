// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/OnboardingModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Walkthrough step completion and progress", "[onboarding][walkthrough]")
{
    OnboardingModel model;
    model.set_steps({
        {"s1", "Draw", "Draw a shape", false},
        {"s2", "Style", "Style an object", false},
        {"s3", "Export", "Export the board", false},
    });
    REQUIRE(model.progress_percent() == 0);
    model.complete_step("s1");
    REQUIRE(model.progress_percent() == 33);
    model.complete_step("s2");
    model.complete_step("s3");
    REQUIRE(model.is_complete());
}

TEST_CASE("Hint dismissal is idempotent", "[onboarding][hints]")
{
    OnboardingModel model;
    REQUIRE_FALSE(model.is_hint_dismissed("tool-tip"));
    model.dismiss_hint("tool-tip");
    REQUIRE(model.is_hint_dismissed("tool-tip"));
    model.dismiss_hint("tool-tip"); // idempotent
    REQUIRE(model.is_hint_dismissed("tool-tip"));
}

TEST_CASE("Starter template catalog", "[onboarding][templates]")
{
    OnboardingModel model;
    model.set_starter_templates({
        {"t1", "Flowchart", "flowchart"},
        {"t2", "Brainstorm", "brainstorm"},
    });
    REQUIRE(model.starter_templates().size() == 2);
}

TEST_CASE("Quick reference visibility", "[onboarding][reference]")
{
    OnboardingModel model;
    REQUIRE_FALSE(model.reference_visible());
    model.set_reference_visible(true);
    REQUIRE(model.reference_visible());
}

TEST_CASE("Empty walkthrough reports complete", "[onboarding][edge]")
{
    OnboardingModel model;
    REQUIRE(model.is_complete()); // no steps = complete
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
