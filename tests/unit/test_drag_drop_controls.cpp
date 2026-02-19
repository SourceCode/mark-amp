// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/DragDropModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Initial state is idle", "[dragdrop][state]")
{
    DragDropModel model;
    REQUIRE(model.state() == DragState::kIdle);
}

TEST_CASE("Drag lifecycle: pending → dragging → completed", "[dragdrop][state]")
{
    DragDropModel model;
    model.set_drag_threshold(5);
    model.set_drop_zones({{"zone-a", "Zone A", DropValidity::kValid}});

    model.begin_drag("tab-1");
    REQUIRE(model.state() == DragState::kPending);
    REQUIRE(model.source_id() == "tab-1");

    model.update_distance(3);
    REQUIRE(model.state() == DragState::kPending); // below threshold

    model.update_distance(3);
    REQUIRE(model.state() == DragState::kDragging); // crossed threshold

    model.complete("zone-a");
    REQUIRE(model.state() == DragState::kCompleted);
}

TEST_CASE("Cancel drag via escape", "[dragdrop][cancel]")
{
    DragDropModel model;
    model.begin_drag("item-1");
    model.update_distance(10);
    REQUIRE(model.state() == DragState::kDragging);

    model.cancel();
    REQUIRE(model.state() == DragState::kCancelled);
}

TEST_CASE("Invalid drop produces error and cancels", "[dragdrop][error]")
{
    DragDropModel model;
    model.set_drop_zones({{"zone-bad", "Bad Zone", DropValidity::kInvalid}});
    model.begin_drag("item-1");
    model.update_distance(10);
    model.complete("zone-bad");

    REQUIRE(model.state() == DragState::kCancelled);
    REQUIRE_FALSE(model.last_error().empty());
}

TEST_CASE("Reset returns to idle", "[dragdrop][reset]")
{
    DragDropModel model;
    model.begin_drag("item-1");
    model.update_distance(10);
    model.reset();
    REQUIRE(model.state() == DragState::kIdle);
    REQUIRE(model.source_id().empty());
    REQUIRE(model.last_error().empty());
}

TEST_CASE("Threshold clamped to minimum 1", "[dragdrop][threshold]")
{
    DragDropModel model;
    model.set_drag_threshold(0);
    REQUIRE(model.drag_threshold() == 1);
}

TEST_CASE("Zone validity lookup", "[dragdrop][zone]")
{
    DragDropModel model;
    model.set_drop_zones({
        {"z1", "Zone 1", DropValidity::kValid},
        {"z2", "Zone 2", DropValidity::kInvalid},
    });
    REQUIRE(model.zone_validity("z1") == DropValidity::kValid);
    REQUIRE(model.zone_validity("z2") == DropValidity::kInvalid);
    REQUIRE(model.zone_validity("unknown") == DropValidity::kUnknown);
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
