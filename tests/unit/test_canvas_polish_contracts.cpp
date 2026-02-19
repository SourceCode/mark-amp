// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/PolishModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Microinteraction enable and disable", "[polish][micro]")
{
    PolishModel model;
    model.enable_interaction(MicroInteraction::kHover);
    model.enable_interaction(MicroInteraction::kSnap);
    REQUIRE(model.is_enabled(MicroInteraction::kHover));
    REQUIRE(model.is_enabled(MicroInteraction::kSnap));
    model.disable_interaction(MicroInteraction::kHover);
    REQUIRE_FALSE(model.is_enabled(MicroInteraction::kHover));
}

TEST_CASE("Easing preset lookup", "[polish][easing]")
{
    PolishModel model;
    model.set_easings({
        {"hover", 150.0, "ease-out"},
        {"select", 200.0, "ease-in-out"},
    });
    REQUIRE(model.easing_for("hover").duration_ms == 150.0);
    REQUIRE(model.easing_for("nonexistent").duration_ms == 0.0);
}

TEST_CASE("Empty state descriptors", "[polish][empty]")
{
    PolishModel model;
    model.set_empty_states({
        {"no-selection", "Nothing selected", "Click an object to select"},
        {"no-results", "No matches", "Try a different search term"},
    });
    REQUIRE(model.empty_states().size() == 2);
}

TEST_CASE("Label consistency tracking", "[polish][labels]")
{
    PolishModel model;
    REQUIRE(model.labels_consistent());
    model.set_label_issues(3);
    REQUIRE_FALSE(model.labels_consistent());
    REQUIRE(model.label_issues() == 3);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
