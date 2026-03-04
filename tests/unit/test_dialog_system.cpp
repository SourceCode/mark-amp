/// @file test_dialog_system.cpp
/// @brief Phase 44 — Unit tests for dialog system models.

#include "ui/DialogStackManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

// ============================================================================
// DialogStackModel
// ============================================================================

TEST_CASE("DialogStackModel: push and pop", "[dialog_system]")
{
    DialogStackModel stack;
    REQUIRE(stack.is_empty());

    auto id1 = stack.push("Confirm Delete");
    REQUIRE_FALSE(stack.is_empty());
    REQUIRE(stack.depth() == 1);
    REQUIRE(stack.top_id() == id1);

    auto id2 = stack.push("Save Changes");
    REQUIRE(stack.depth() == 2);
    REQUIRE(stack.top_id() == id2);

    stack.pop();
    REQUIRE(stack.depth() == 1);
    REQUIRE(stack.top_id() == id1);
}

TEST_CASE("DialogStackModel: remove by ID", "[dialog_system]")
{
    DialogStackModel stack;
    auto id1 = stack.push("Dialog 1");
    auto id2 = stack.push("Dialog 2");
    auto id3 = stack.push("Dialog 3");

    stack.remove(id2);
    REQUIRE(stack.depth() == 2);
    REQUIRE(stack.top_id() == id3);
}

TEST_CASE("DialogStackModel: bring to top", "[dialog_system]")
{
    DialogStackModel stack;
    auto id1 = stack.push("Dialog 1");
    [[maybe_unused]] auto id2 = stack.push("Dialog 2");

    stack.bring_to_top(id1);
    REQUIRE(stack.top_id() == id1);
}

TEST_CASE("DialogStackModel: pop empty is safe", "[dialog_system]")
{
    DialogStackModel stack;
    stack.pop(); // should not crash
    REQUIRE(stack.is_empty());
}

TEST_CASE("DialogStackModel: unique IDs", "[dialog_system]")
{
    DialogStackModel stack;
    auto id1 = stack.push("A");
    auto id2 = stack.push("B");
    auto id3 = stack.push("C");
    REQUIRE(id1 != id2);
    REQUIRE(id2 != id3);
    REQUIRE(id1 != id3);
}

// ============================================================================
// WizardModel
// ============================================================================

TEST_CASE("WizardModel: step navigation", "[dialog_system]")
{
    WizardModel wizard;
    wizard.set_steps({
        {.title = "Welcome"},
        {.title = "Configuration"},
        {.title = "Finish"},
    });

    REQUIRE(wizard.current_step() == 0);
    REQUIRE(wizard.is_first_step());
    REQUIRE_FALSE(wizard.is_last_step());

    wizard.next();
    REQUIRE(wizard.current_step() == 1);

    wizard.next();
    REQUIRE(wizard.current_step() == 2);
    REQUIRE(wizard.is_last_step());

    // Cannot go past end
    REQUIRE_FALSE(wizard.next());
    REQUIRE(wizard.current_step() == 2);

    wizard.previous();
    REQUIRE(wizard.current_step() == 1);
}

TEST_CASE("WizardModel: go_to_step", "[dialog_system]")
{
    WizardModel wizard;
    wizard.set_steps({
        {.title = "A"},
        {.title = "B"},
        {.title = "C"},
    });

    wizard.go_to_step(2);
    REQUIRE(wizard.current_step() == 2);
    REQUIRE(wizard.current_step_info()->title == "C");
}

TEST_CASE("WizardModel: completion tracking", "[dialog_system]")
{
    WizardModel wizard;
    wizard.set_steps({
        {.title = "Step 1"},
        {.title = "Step 2"},
    });

    REQUIRE_FALSE(wizard.can_proceed());
    wizard.mark_step_complete(0);
    REQUIRE(wizard.can_proceed());
    REQUIRE(wizard.progress() == 0.5);

    wizard.mark_step_complete(1);
    REQUIRE(wizard.progress() == 1.0);
}

TEST_CASE("WizardModel: optional step can proceed", "[dialog_system]")
{
    WizardModel wizard;
    wizard.set_steps({
        {.title = "Optional Step", .is_optional = true},
        {.title = "Required Step"},
    });

    REQUIRE(wizard.can_proceed()); // optional = can always proceed
}

// ============================================================================
// InputDialogModel
// ============================================================================

TEST_CASE("InputDialogModel: value and validation", "[dialog_system]")
{
    InputDialogModel model;
    model.set_prompt("Enter filename:");
    model.set_validator(
        [](const std::string& val) -> std::string
        {
            if (val.empty())
                return "Filename cannot be empty";
            if (val.find('/') != std::string::npos)
                return "No slashes allowed";
            return {};
        });

    model.set_value("");
    REQUIRE_FALSE(model.is_valid());
    REQUIRE(model.validate() == "Filename cannot be empty");

    model.set_value("hello/world");
    REQUIRE_FALSE(model.is_valid());
    REQUIRE(model.validate() == "No slashes allowed");

    model.set_value("valid_name");
    REQUIRE(model.is_valid());
}

TEST_CASE("InputDialogModel: no validator is always valid", "[dialog_system]")
{
    InputDialogModel model;
    model.set_value("anything");
    REQUIRE(model.is_valid());
}
