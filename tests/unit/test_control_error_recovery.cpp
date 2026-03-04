/**
 * @file test_control_error_recovery.cpp
 * @brief Phase 34: Unit tests for PreflightValidator and RecoveryActionProvider.
 */

#include "ui/PreflightValidator.h"
#include "ui/RecoveryActionProvider.h"

#include <catch2/catch_test_macros.hpp>

#include <string>

using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// PreflightResult
// ═══════════════════════════════════════════════════════

TEST_CASE("PreflightResult - is blocking", "[error_recovery][preflight]")
{
    PreflightResult info;
    info.severity = PreflightSeverity::kInfo;
    CHECK_FALSE(info.is_blocking());

    PreflightResult warning;
    warning.severity = PreflightSeverity::kWarning;
    CHECK_FALSE(warning.is_blocking());

    PreflightResult error;
    error.severity = PreflightSeverity::kError;
    CHECK(error.is_blocking());

    PreflightResult blocker;
    blocker.severity = PreflightSeverity::kBlocker;
    CHECK(blocker.is_blocking());
}

TEST_CASE("PreflightResult - message format", "[error_recovery][preflight]")
{
    PreflightResult result;
    result.what_happened = "File is locked";
    result.why = "Another process has the file open";
    result.what_to_do = "Close the other process and retry";

    auto msg = result.message();
    CHECK(msg.find("File is locked") != std::string::npos);
    CHECK(msg.find("Another process") != std::string::npos);
    CHECK(msg.find("retry") != std::string::npos);
}

TEST_CASE("PreflightResult - severity name", "[error_recovery][preflight]")
{
    PreflightResult result;

    result.severity = PreflightSeverity::kInfo;
    CHECK(result.severity_name() == "info");

    result.severity = PreflightSeverity::kBlocker;
    CHECK(result.severity_name() == "blocker");
}

// ═══════════════════════════════════════════════════════
// PreflightValidator — registration
// ═══════════════════════════════════════════════════════

TEST_CASE("PreflightValidator - empty state", "[error_recovery][preflight]")
{
    PreflightValidator validator;
    CHECK(validator.check_count("delete") == 0);
    CHECK(validator.can_proceed("delete"));
}

TEST_CASE("PreflightValidator - register and validate", "[error_recovery][preflight]")
{
    PreflightValidator validator;
    validator.register_check(
        "delete",
        "has_selection",
        [](const std::string& /*action*/) -> PreflightResult {
            return {"has_selection", PreflightSeverity::kInfo, "Selection exists", "", ""};
        });

    CHECK(validator.check_count("delete") == 1);
    auto results = validator.validate("delete");
    CHECK(results.size() == 1);
    CHECK(results[0].severity == PreflightSeverity::kInfo);
}

TEST_CASE("PreflightValidator - blocker prevents proceed", "[error_recovery][preflight]")
{
    PreflightValidator validator;
    validator.register_check("delete",
                             "file_locked",
                             [](const std::string& /*action*/) -> PreflightResult
                             {
                                 return {"file_locked",
                                         PreflightSeverity::kBlocker,
                                         "File is locked",
                                         "Cannot modify",
                                         "Close other app"};
                             });

    CHECK_FALSE(validator.can_proceed("delete"));
    CHECK(validator.highest_severity("delete") == PreflightSeverity::kBlocker);
}

TEST_CASE("PreflightValidator - mixed severities", "[error_recovery][preflight]")
{
    PreflightValidator validator;
    validator.register_check(
        "save",
        "disk_space",
        [](const std::string& /*action*/) -> PreflightResult {
            return {"disk_space", PreflightSeverity::kWarning, "Low disk space", "", ""};
        });
    validator.register_check(
        "save",
        "permissions",
        [](const std::string& /*action*/) -> PreflightResult {
            return {"permissions", PreflightSeverity::kInfo, "Write access OK", "", ""};
        });

    CHECK(validator.can_proceed("save"));
    CHECK(validator.highest_severity("save") == PreflightSeverity::kWarning);
    CHECK(validator.check_count("save") == 2);
}

TEST_CASE("PreflightValidator - clear checks", "[error_recovery][preflight]")
{
    PreflightValidator validator;
    validator.register_check("delete",
                             "check1",
                             [](const std::string& /*action*/) -> PreflightResult {
                                 return {"check1", PreflightSeverity::kInfo, "", "", ""};
                             });
    CHECK(validator.check_count("delete") == 1);

    validator.clear_checks("delete");
    CHECK(validator.check_count("delete") == 0);
}

// ═══════════════════════════════════════════════════════
// RecoveryAction
// ═══════════════════════════════════════════════════════

TEST_CASE("RecoveryAction - type name", "[error_recovery][recovery]")
{
    RecoveryAction action;
    action.type = RecoveryActionType::kRetry;
    CHECK(action.type_name() == "retry");

    action.type = RecoveryActionType::kOpenLog;
    CHECK(action.type_name() == "open_log");

    action.type = RecoveryActionType::kUndo;
    CHECK(action.type_name() == "undo");
}

// ═══════════════════════════════════════════════════════
// RecoveryActionProvider — registry
// ═══════════════════════════════════════════════════════

TEST_CASE("RecoveryActionProvider - empty state", "[error_recovery][recovery]")
{
    RecoveryActionProvider provider;
    CHECK_FALSE(provider.has_recoveries("network_error"));
    CHECK(provider.recoveries_for("network_error").empty());
}

TEST_CASE("RecoveryActionProvider - register and query", "[error_recovery][recovery]")
{
    RecoveryActionProvider provider;
    provider.register_recovery("network_error",
                               {RecoveryActionType::kRetry, "Retry Connection", "refresh"});
    provider.register_recovery("network_error",
                               {RecoveryActionType::kOpenSettings, "Check Settings", "settings"});

    CHECK(provider.has_recoveries("network_error"));
    auto actions = provider.recoveries_for("network_error");
    CHECK(actions.size() == 2);
    CHECK(actions[0].type == RecoveryActionType::kRetry);
    CHECK(actions[1].type == RecoveryActionType::kOpenSettings);
}

// ═══════════════════════════════════════════════════════
// RecoveryActionProvider — undo/redo
// ═══════════════════════════════════════════════════════

TEST_CASE("RecoveryActionProvider - undo stack empty", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    CHECK_FALSE(provider.can_undo());
    CHECK_FALSE(provider.can_redo());
    CHECK(provider.undo_depth() == 0);
    CHECK_FALSE(provider.undo());
    CHECK_FALSE(provider.redo());
}

TEST_CASE("RecoveryActionProvider - push and undo", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    int value = 10;

    provider.push_undo(
        {"op1", "Set value to 10", [&value]() { value = 0; }, [&value]() { value = 10; }});

    CHECK(provider.can_undo());
    CHECK(provider.undo_description() == "Set value to 10");
    CHECK(provider.undo_depth() == 1);

    CHECK(provider.undo());
    CHECK(value == 0);
    CHECK_FALSE(provider.can_undo());
    CHECK(provider.can_redo());
}

TEST_CASE("RecoveryActionProvider - undo and redo", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    int value = 0;

    provider.push_undo(
        {"op1", "Increment value", [&value]() { value--; }, [&value]() { value++; }});

    provider.undo();
    CHECK(value == -1);

    provider.redo();
    CHECK(value == 0);
    CHECK(provider.can_undo());
    CHECK_FALSE(provider.can_redo());
}

TEST_CASE("RecoveryActionProvider - new action clears redo", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    int value = 0;

    provider.push_undo({"op1", "Op 1", [&value]() { value = 0; }, [&value]() { value = 1; }});
    provider.undo();
    CHECK(provider.can_redo());

    // Push new action clears redo
    provider.push_undo({"op2", "Op 2", [&value]() { value = 0; }, [&value]() { value = 2; }});
    CHECK_FALSE(provider.can_redo());
    CHECK(provider.undo_depth() == 1);
}

TEST_CASE("RecoveryActionProvider - clear undo", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    int value = 0;

    provider.push_undo({"op1", "Op 1", [&value]() { value = 0; }, nullptr});
    provider.push_undo({"op2", "Op 2", [&value]() { value = 0; }, nullptr});
    CHECK(provider.undo_depth() == 2);

    provider.clear_undo();
    CHECK(provider.undo_depth() == 0);
    CHECK_FALSE(provider.can_undo());
    CHECK_FALSE(provider.can_redo());
}

TEST_CASE("RecoveryActionProvider - multi-level undo", "[error_recovery][undo]")
{
    RecoveryActionProvider provider;
    int value = 0;

    provider.push_undo({"op1", "Set to 1", [&value]() { value = 0; }, [&value]() { value = 1; }});
    value = 1;

    provider.push_undo({"op2", "Set to 2", [&value]() { value = 1; }, [&value]() { value = 2; }});
    value = 2;

    CHECK(provider.undo_depth() == 2);
    CHECK(provider.undo_description() == "Set to 2");

    provider.undo();
    CHECK(value == 1);

    provider.undo();
    CHECK(value == 0);

    CHECK(provider.undo_depth() == 0);
    CHECK(provider.redo_description() == "Set to 1");
}
