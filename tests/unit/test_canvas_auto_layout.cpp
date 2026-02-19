// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/AutoLayoutModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Row preset layout", "[layout][preset]")
{
    AutoLayoutModel model;
    model.set_spacing(10.0);
    model.set_objects({
        {"a", 0, 0, 50, 50, ""},
        {"b", 0, 0, 50, 50, ""},
        {"c", 0, 0, 50, 50, ""},
    });
    model.apply_preset(LayoutPresetType::kRow, 0.0, 0.0);
    REQUIRE(model.objects()[0].pos_x == 0.0);
    REQUIRE(model.objects()[1].pos_x == 60.0);
    REQUIRE(model.objects()[2].pos_x == 120.0);
}

TEST_CASE("Column preset layout", "[layout][preset]")
{
    AutoLayoutModel model;
    model.set_spacing(10.0);
    model.set_objects({{"a", 0, 0, 100, 40, ""}, {"b", 0, 0, 100, 40, ""}});
    model.apply_preset(LayoutPresetType::kColumn, 10.0, 10.0);
    REQUIRE(model.objects()[0].pos_y == 10.0);
    REQUIRE(model.objects()[1].pos_y == 60.0);
}

TEST_CASE("Normalize spacing sorts and respaces", "[layout][spacing]")
{
    AutoLayoutModel model;
    model.set_spacing(20.0);
    model.set_objects({
        {"b", 500, 0, 100, 100, ""},
        {"a", 0, 0, 100, 100, ""},
    });
    model.normalize_spacing();
    REQUIRE(model.objects()[0].object_id == "a");
    REQUIRE(model.objects()[1].pos_x == 120.0); // 0 + 100 + 20
}

TEST_CASE("Container bounds check passes", "[layout][bounds]")
{
    AutoLayoutModel model;
    model.set_container_bounds(0, 0, 300, 300);
    model.set_objects({{"a", 10, 10, 50, 50, ""}});
    REQUIRE(model.respects_bounds());
}

TEST_CASE("Container bounds check fails on overflow", "[layout][bounds]")
{
    AutoLayoutModel model;
    model.set_container_bounds(0, 0, 100, 100);
    model.set_objects({{"a", 80, 10, 50, 50, ""}});
    REQUIRE_FALSE(model.respects_bounds()); // 80+50 > 100
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
