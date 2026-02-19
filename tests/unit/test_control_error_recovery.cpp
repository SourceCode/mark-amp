// NOLINTBEGIN(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
#include "ui/ErrorRecoveryModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("Preflight blocks risky action", "[error][preflight]")
{
    ErrorRecoveryModel model;
    model.set_preflights({
        {"perm", "No write permission", PreflightSeverity::kBlock},
        {"space", "Low disk space", PreflightSeverity::kWarning},
        {"ok", "Files valid", PreflightSeverity::kPass},
    });

    REQUIRE(model.is_blocked());
    REQUIRE(model.blockers().size() == 1);
    REQUIRE(model.warnings().size() == 1);
}

TEST_CASE("No blockers allows action", "[error][preflight]")
{
    ErrorRecoveryModel model;
    model.set_preflights({
        {"ok", "All checks pass", PreflightSeverity::kPass},
    });
    REQUIRE_FALSE(model.is_blocked());
    REQUIRE(model.blockers().empty());
}

TEST_CASE("Rich error context", "[error][context]")
{
    ErrorRecoveryModel model;
    model.push_error({
        "save-fail",
        "File could not be saved",
        "Disk is full",
        "Free up disk space or save to another location",
        {{"retry", "Retry", true}, {"settings", "Open Settings", false}},
    });

    REQUIRE(model.errors().size() == 1);
    const auto* latest = model.latest_error();
    REQUIRE(latest != nullptr);
    REQUIRE(latest->what_happened == "File could not be saved");
    REQUIRE(latest->actions.size() == 2);
    REQUIRE(latest->actions[0].is_primary);
}

TEST_CASE("Clear errors", "[error][context]")
{
    ErrorRecoveryModel model;
    model.push_error({"e1", "Error 1", "", "", {}});
    model.push_error({"e2", "Error 2", "", "", {}});
    REQUIRE(model.errors().size() == 2);
    model.clear_errors();
    REQUIRE(model.errors().empty());
    REQUIRE(model.latest_error() == nullptr);
}

TEST_CASE("Undo stack management", "[error][undo]")
{
    ErrorRecoveryModel model;
    REQUIRE(model.undo_stack_size() == 0);
    REQUIRE(model.last_undoable() == nullptr);

    model.push_undoable("Delete file");
    model.push_undoable("Rename file");
    REQUIRE(model.undo_stack_size() == 2);
    REQUIRE(*model.last_undoable() == "Rename file");

    model.pop_undoable();
    REQUIRE(model.undo_stack_size() == 1);
    REQUIRE(*model.last_undoable() == "Delete file");
}
// NOLINTEND(cppcoreguidelines-avoid-do-while,cert-err58-cpp,cppcoreguidelines-avoid-non-const-global-variables,misc-use-anonymous-namespace,readability-function-cognitive-complexity)
