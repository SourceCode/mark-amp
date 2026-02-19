// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "canvas/SelectionTransformModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;

TEST_CASE("Align left", "[selection][align]")
{
    SelectionTransformModel model;
    model.set_selection({
        {"a", 10.0, 0.0, 50.0, 50.0},
        {"b", 100.0, 0.0, 50.0, 50.0},
        {"c", 200.0, 0.0, 50.0, 50.0},
    });
    const auto result = model.aligned(AlignDirection::kLeft);
    REQUIRE(result[0].x == 10.0);
    REQUIRE(result[1].x == 10.0);
    REQUIRE(result[2].x == 10.0);
}

TEST_CASE("Align right", "[selection][align]")
{
    SelectionTransformModel model;
    model.set_selection({
        {"a", 10.0, 0.0, 50.0, 50.0},
        {"b", 100.0, 0.0, 80.0, 50.0},
    });
    const auto result = model.aligned(AlignDirection::kRight);
    // Right edge = max(60, 180) = 180
    REQUIRE(result[0].x == 130.0); // 180 - 50
    REQUIRE(result[1].x == 100.0); // 180 - 80
}

TEST_CASE("Distribute horizontal", "[selection][distribute]")
{
    SelectionTransformModel model;
    model.set_selection({
        {"a", 0.0, 0.0, 20.0, 20.0},
        {"b", 100.0, 0.0, 20.0, 20.0},
        {"c", 50.0, 0.0, 20.0, 20.0},
    });
    const auto result = model.distributed(DistributeDirection::kHorizontal);
    // Sorted by x: a(0), c(50), b(100); total width = 60, range = 120, gap = 20
    REQUIRE(result[0].x == 0.0);
    REQUIRE(result[2].x == 100.0);
    // Middle should be equally spaced
    REQUIRE(result[1].x > 0.0);
    REQUIRE(result[1].x < 100.0);
}

TEST_CASE("Pivot clamped 0-1", "[selection][pivot]")
{
    SelectionTransformModel model;
    model.set_pivot(-0.5, 1.5);
    REQUIRE(model.pivot_x() == 0.0);
    REQUIRE(model.pivot_y() == 1.0);
}

TEST_CASE("Active handle type", "[selection][handle]")
{
    SelectionTransformModel model;
    REQUIRE(model.active_handle() == HandleType::kMove);
    model.set_active_handle(HandleType::kRotate);
    REQUIRE(model.active_handle() == HandleType::kRotate);
}

TEST_CASE("Selection count", "[selection][basic]")
{
    SelectionTransformModel model;
    model.set_selection({
        {"a", 0, 0, 10, 10},
        {"b", 0, 0, 10, 10},
    });
    REQUIRE(model.selection_count() == 2);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
