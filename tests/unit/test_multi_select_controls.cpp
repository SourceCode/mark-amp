/**
 * @file test_multi_select_controls.cpp
 * @brief Phase 33: Unit tests for MultiSelectModel and BulkActionController.
 */

#include "ui/BulkActionController.h"
#include "ui/MultiSelectModel.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// SelectionModifiers
// ═══════════════════════════════════════════════════════

TEST_CASE("SelectionModifiers - mode detection", "[multi_select][modifiers]")
{
    SelectionModifiers none;
    CHECK(none.mode() == SelectionMode::kSingle);

    SelectionModifiers ctrl;
    ctrl.ctrl_or_cmd = true;
    CHECK(ctrl.mode() == SelectionMode::kToggle);

    SelectionModifiers shift;
    shift.shift = true;
    CHECK(shift.mode() == SelectionMode::kRange);

    // Shift takes priority over Ctrl
    SelectionModifiers both;
    both.ctrl_or_cmd = true;
    both.shift = true;
    CHECK(both.mode() == SelectionMode::kRange);
}

// ═══════════════════════════════════════════════════════
// MultiSelectModel — basics
// ═══════════════════════════════════════════════════════

TEST_CASE("MultiSelectModel - empty state", "[multi_select][model]")
{
    MultiSelectModel model;
    CHECK(model.item_count() == 0);
    CHECK(model.selection_count() == 0);
    CHECK(model.selected_ids().empty());
}

TEST_CASE("MultiSelectModel - set items", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c", "d", "e"});
    CHECK(model.item_count() == 5);
    CHECK(model.selection_count() == 0);
}

TEST_CASE("MultiSelectModel - single select", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});

    model.select("b");
    CHECK(model.is_selected("b"));
    CHECK_FALSE(model.is_selected("a"));
    CHECK(model.selection_count() == 1);
    CHECK(model.is_focused("b"));

    // Selecting another replaces
    model.select("c");
    CHECK_FALSE(model.is_selected("b"));
    CHECK(model.is_selected("c"));
    CHECK(model.selection_count() == 1);
}

TEST_CASE("MultiSelectModel - toggle select", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});

    model.toggle("a");
    CHECK(model.is_selected("a"));

    model.toggle("b");
    CHECK(model.is_selected("a"));
    CHECK(model.is_selected("b"));
    CHECK(model.selection_count() == 2);

    model.toggle("a");
    CHECK_FALSE(model.is_selected("a"));
    CHECK(model.selection_count() == 1);
}

TEST_CASE("MultiSelectModel - range select", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c", "d", "e"});

    model.select("b");       // Sets anchor at b (index 1)
    model.select_range("d"); // Range b-d

    CHECK_FALSE(model.is_selected("a"));
    CHECK(model.is_selected("b"));
    CHECK(model.is_selected("c"));
    CHECK(model.is_selected("d"));
    CHECK_FALSE(model.is_selected("e"));
    CHECK(model.selection_count() == 3);
}

TEST_CASE("MultiSelectModel - click with modifiers", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});

    // Plain click
    model.click("a");
    CHECK(model.selection_count() == 1);

    // Ctrl+Click toggles
    SelectionModifiers ctrl;
    ctrl.ctrl_or_cmd = true;
    model.click("b", ctrl);
    CHECK(model.selection_count() == 2);

    // Shift+Click range from b
    SelectionModifiers shift;
    shift.shift = true;
    model.click("c", shift);
    // Range from b (anchor) to c
    CHECK(model.selection_count() >= 2);
}

TEST_CASE("MultiSelectModel - select all", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});
    model.select_all();
    CHECK(model.selection_count() == 3);
}

TEST_CASE("MultiSelectModel - clear", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});
    model.select_all();
    model.clear_selection();
    CHECK(model.selection_count() == 0);
    CHECK(model.focused_id().empty());
}

TEST_CASE("MultiSelectModel - summary", "[multi_select][model]")
{
    MultiSelectModel model;
    model.set_items({"a", "b", "c"});
    model.select("b");

    auto sum = model.summary();
    CHECK(sum.total_count == 1);
    CHECK(sum.focused_id == "b");
    CHECK(sum.focused_index == 1);
}

// ═══════════════════════════════════════════════════════
// BulkActionController — registration
// ═══════════════════════════════════════════════════════

TEST_CASE("BulkActionController - empty state", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    CHECK(ctrl.action_count() == 0);
}

TEST_CASE("BulkActionController - register standard", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();
    CHECK(ctrl.action_count() == 5);

    auto del = ctrl.find_action("bulk.delete");
    REQUIRE(del != nullptr);
    CHECK(del->severity == ActionSeverity::kDestructive);
    CHECK(del->requires_confirmation());
}

TEST_CASE("BulkActionController - enabled actions", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    auto enabled_1 = ctrl.enabled_actions(1);
    CHECK(enabled_1.size() == 4); // delete, move, copy, rename (export needs 2)

    auto enabled_2 = ctrl.enabled_actions(2);
    CHECK(enabled_2.size() == 5); // All 5
}

TEST_CASE("BulkActionController - execute normal action", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    auto result = ctrl.execute("bulk.copy", 3);
    CHECK(result.is_success());
    CHECK(result.affected_count == 3);
}

TEST_CASE("BulkActionController - execute destructive needs confirm", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    CHECK(ctrl.needs_confirmation("bulk.delete"));
    auto result = ctrl.execute("bulk.delete", 3);
    CHECK_FALSE(result.is_success()); // Needs confirmation first
}

TEST_CASE("BulkActionController - confirm and execute", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    auto result = ctrl.confirm_and_execute("bulk.delete", 3);
    CHECK(result.is_success());
    CHECK(result.affected_count == 3);
}

TEST_CASE("BulkActionController - confirmation message", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    auto msg = ctrl.confirmation_message("bulk.delete", 5);
    CHECK(msg.find("5 items") != std::string::npos);
    CHECK(msg.find("Delete") != std::string::npos);
}

TEST_CASE("BulkActionController - confirmation message singular", "[multi_select][bulk]")
{
    BulkActionController ctrl;
    ctrl.register_standard_actions();

    auto msg = ctrl.confirmation_message("bulk.delete", 1);
    CHECK(msg.find("1 item?") != std::string::npos);
}

TEST_CASE("BulkAction - requires confirmation", "[multi_select][bulk]")
{
    BulkAction normal;
    normal.severity = ActionSeverity::kNormal;
    CHECK_FALSE(normal.requires_confirmation());

    BulkAction destructive;
    destructive.severity = ActionSeverity::kDestructive;
    CHECK(destructive.requires_confirmation());

    BulkAction irreversible;
    irreversible.severity = ActionSeverity::kIrreversible;
    CHECK(irreversible.requires_confirmation());
}
