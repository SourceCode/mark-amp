// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/SnappingModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Grid snap to nearest", "[snap][grid]")
{
    SnappingModel model;
    model.set_grid_spacing(20.0);
    model.set_snap_tolerance(8.0);

    const auto result = model.snap_to_grid(42.0, 57.0);
    REQUIRE(result.did_snap_x);
    REQUIRE(result.snapped_x == 40.0);
    REQUIRE(result.did_snap_y);
    REQUIRE(result.snapped_y == 60.0);
}

TEST_CASE("No snap outside tolerance", "[snap][grid]")
{
    SnappingModel model;
    model.set_grid_spacing(20.0);
    model.set_snap_tolerance(5.0);

    const auto result = model.snap_to_grid(33.0, 33.0);
    REQUIRE_FALSE(result.did_snap_x);
    REQUIRE_FALSE(result.did_snap_y);
}

TEST_CASE("Snap disabled returns original", "[snap][config]")
{
    SnappingModel model;
    model.set_snap_enabled(false);
    const auto result = model.snap_to_grid(42.0, 57.0);
    REQUIRE(result.snapped_x == 42.0);
    REQUIRE(result.snapped_y == 57.0);
}

TEST_CASE("Grid spacing clamped to min 1", "[snap][grid]")
{
    SnappingModel model;
    model.set_grid_spacing(0.0);
    REQUIRE(model.grid_spacing() == 1.0);
}

TEST_CASE("Snap target enable/disable", "[snap][config]")
{
    SnappingModel model;
    REQUIRE(model.is_target_enabled(SnapTarget::kGrid));
    model.disable_target(SnapTarget::kGrid);
    REQUIRE_FALSE(model.is_target_enabled(SnapTarget::kGrid));
    model.enable_target(SnapTarget::kGuide);
    REQUIRE(model.is_target_enabled(SnapTarget::kGuide));
}

TEST_CASE("Grid style and visibility", "[snap][grid]")
{
    SnappingModel model;
    model.set_grid_style(GridStyle::kLine);
    model.set_grid_visible(false);
    REQUIRE(model.grid_style() == GridStyle::kLine);
    REQUIRE_FALSE(model.grid_visible());
}

TEST_CASE("Major interval clamped to min 1", "[snap][grid]")
{
    SnappingModel model;
    model.set_major_interval(0);
    REQUIRE(model.major_interval() == 1);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
