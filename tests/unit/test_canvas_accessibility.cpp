// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/AccessibilityModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Tab-order traversal", "[accessibility][traversal]")
{
    AccessibilityModel model;
    model.set_objects({
        {"obj-c", "Circle", "shape", 3},
        {"obj-a", "Arrow", "connector", 1},
        {"obj-b", "Box", "shape", 2},
    });
    // Should be sorted by tab_order
    REQUIRE(model.focused_object().object_id == "obj-a");
    model.focus_next();
    REQUIRE(model.focused_object().object_id == "obj-b");
    model.focus_next();
    REQUIRE(model.focused_object().object_id == "obj-c");
    model.focus_next();
    REQUIRE(model.focused_object().object_id == "obj-a"); // wraps
}

TEST_CASE("Focus prev wraps backward", "[accessibility][traversal]")
{
    AccessibilityModel model;
    model.set_objects({{"a", "A", "shape", 1}, {"b", "B", "shape", 2}});
    model.focus_prev(); // wraps to last
    REQUIRE(model.focused_object().object_id == "b");
}

TEST_CASE("Focus ring visibility and contrast", "[accessibility][focus]")
{
    AccessibilityModel model;
    REQUIRE(model.focus_ring_visible());
    model.set_focus_ring_visible(false);
    REQUIRE_FALSE(model.focus_ring_visible());
    model.set_focus_ring_contrast(2.5);
    REQUIRE(model.focus_ring_contrast() == 2.5);
}

TEST_CASE("Reduced motion mode", "[accessibility][motion]")
{
    AccessibilityModel model;
    REQUIRE_FALSE(model.reduced_motion());
    model.set_reduced_motion(true);
    REQUIRE(model.reduced_motion());
}

TEST_CASE("Empty objects safe", "[accessibility][empty]")
{
    AccessibilityModel model;
    REQUIRE(model.object_count() == 0);
    REQUIRE(model.focused_object().object_id.empty());
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
