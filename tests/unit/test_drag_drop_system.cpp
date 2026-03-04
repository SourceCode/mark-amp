/// @file test_drag_drop_system.cpp
/// @brief Phase 45 — Unit tests for drag and drop system models.

#include "ui/DragController.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// DragControllerModel — Lifecycle
// ============================================================================

TEST_CASE("DragControllerModel: starts idle", "[drag_drop_system]")
{
    DragControllerModel model;
    REQUIRE(model.phase() == DragPhase::kIdle);
}

TEST_CASE("DragControllerModel: begin potential drag", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(
        100,
        200,
        DragPayloadType::kTab,
        TabDragPayload{.tab_index = 2, .source_group_id = 0, .file_path = "/test.md"});

    REQUIRE(model.phase() == DragPhase::kPending);
    REQUIRE(model.start_x() == 100);
    REQUIRE(model.start_y() == 200);
    REQUIRE(model.payload_type() == DragPayloadType::kTab);
}

TEST_CASE("DragControllerModel: threshold detection", "[drag_drop_system]")
{
    DragControllerModel model;
    model.set_distance_threshold(5);
    model.begin_potential_drag(
        100, 100, DragPayloadType::kFile, FileDragPayload{.file_path = "/test.md"});

    // Small move, below threshold
    auto exceeded = model.update_position(102, 101);
    REQUIRE_FALSE(exceeded);
    REQUIRE(model.phase() == DragPhase::kPending);

    // Large move, exceeds threshold
    exceeded = model.update_position(110, 110);
    REQUIRE(exceeded);
    REQUIRE(model.phase() == DragPhase::kDragging);
}

TEST_CASE("DragControllerModel: distance calculation", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0, 0, DragPayloadType::kTab, TabDragPayload{});
    model.update_position(3, 4);
    REQUIRE(model.drag_distance_squared() == 25); // 3² + 4² = 25
}

TEST_CASE("DragControllerModel: complete drop", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0, 0, DragPayloadType::kTab, TabDragPayload{});
    model.update_position(100, 100);
    REQUIRE(model.phase() == DragPhase::kDragging);

    model.complete_drop();
    REQUIRE(model.phase() == DragPhase::kDropping);
}

TEST_CASE("DragControllerModel: cancel drag", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0, 0, DragPayloadType::kTab, TabDragPayload{});
    model.update_position(100, 100);
    REQUIRE(model.phase() == DragPhase::kDragging);

    model.cancel();
    REQUIRE(model.phase() == DragPhase::kCancelled);
}

TEST_CASE("DragControllerModel: reset returns to idle", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0, 0, DragPayloadType::kTab, TabDragPayload{});
    model.reset();
    REQUIRE(model.phase() == DragPhase::kIdle);
}

// ============================================================================
// DragControllerModel — Drop Zones
// ============================================================================

TEST_CASE("DragControllerModel: register and query drop zones", "[drag_drop_system]")
{
    DragControllerModel model;
    model.register_drop_zone({.zone_id = "editor-left",
                              .accepted_type = DragPayloadType::kTab,
                              .position = DropPosition::kLeft});
    model.register_drop_zone({.zone_id = "editor-right",
                              .accepted_type = DragPayloadType::kTab,
                              .position = DropPosition::kRight});

    REQUIRE(model.active_drop_zone() == nullptr);

    model.set_active_zone("editor-left");
    auto* zone = model.active_drop_zone();
    REQUIRE(zone != nullptr);
    REQUIRE(zone->zone_id == "editor-left");
    REQUIRE(zone->position == DropPosition::kLeft);
}

TEST_CASE("DragControllerModel: clear drop zones", "[drag_drop_system]")
{
    DragControllerModel model;
    model.register_drop_zone({.zone_id = "test"});
    model.set_active_zone("test");
    model.clear_drop_zones();
    REQUIRE(model.active_drop_zone() == nullptr);
}

TEST_CASE("DragControllerModel: clear active zone", "[drag_drop_system]")
{
    DragControllerModel model;
    model.register_drop_zone({.zone_id = "test"});
    model.set_active_zone("test");
    model.clear_active_zone();
    REQUIRE(model.active_drop_zone() == nullptr);
}

// ============================================================================
// DragControllerModel — Payload Types
// ============================================================================

TEST_CASE("DragControllerModel: tab payload", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(
        0,
        0,
        DragPayloadType::kTab,
        TabDragPayload{.tab_index = 3, .source_group_id = 1, .file_path = "/readme.md"});

    REQUIRE(model.payload_type() == DragPayloadType::kTab);
    auto& payload = std::get<TabDragPayload>(model.payload());
    REQUIRE(payload.tab_index == 3);
    REQUIRE(payload.file_path == "/readme.md");
}

TEST_CASE("DragControllerModel: file payload", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(
        0,
        0,
        DragPayloadType::kFile,
        FileDragPayload{.file_path = "/docs/guide.md", .is_directory = false});

    auto& payload = std::get<FileDragPayload>(model.payload());
    REQUIRE(payload.file_path == "/docs/guide.md");
    REQUIRE_FALSE(payload.is_directory);
}

TEST_CASE("DragControllerModel: multi-select payload", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0,
                               0,
                               DragPayloadType::kMultiSelect,
                               MultiSelectPayload{.file_paths = {"/a.md", "/b.md", "/c.md"}});

    auto& payload = std::get<MultiSelectPayload>(model.payload());
    REQUIRE(payload.file_paths.size() == 3);
}

TEST_CASE("DragControllerModel: panel payload", "[drag_drop_system]")
{
    DragControllerModel model;
    model.begin_potential_drag(0,
                               0,
                               DragPayloadType::kPanel,
                               PanelDragPayload{.panel_id = "explorer", .source_dock_position = 0});

    auto& payload = std::get<PanelDragPayload>(model.payload());
    REQUIRE(payload.panel_id == "explorer");
}

// ============================================================================
// DragControllerModel — Configuration
// ============================================================================

TEST_CASE("DragControllerModel: custom threshold", "[drag_drop_system]")
{
    DragControllerModel model;
    model.set_distance_threshold(10);
    REQUIRE(model.distance_threshold() == 10);

    model.begin_potential_drag(0, 0, DragPayloadType::kTab, TabDragPayload{});
    model.update_position(8, 0);
    REQUIRE(model.phase() == DragPhase::kPending); // 8 < 10
    model.update_position(12, 0);
    REQUIRE(model.phase() == DragPhase::kDragging); // 12 > 10
}
