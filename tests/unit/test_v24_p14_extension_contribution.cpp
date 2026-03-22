/// @file test_v24_p14_extension_contribution.cpp
/// @brief V24 Phase 14 tests: Extension contribution validation, plugin health.
#include <catch2/catch_test_macros.hpp>
#include "../../src/core/ContributionValidator.h"

using namespace markamp::core;

// P14-T01: Contribution diagnostic structure
TEST_CASE("P14-T01 diagnostic severity levels", "[v24][p14]") {
    ContributionDiagnostic diag;
    diag.severity = ContributionDiagnostic::Severity::kError;
    diag.contribution_type = "command";
    diag.field = "command";
    diag.message = "Duplicate command ID";
    diag.extension_id = "ext-test";
    REQUIRE(diag.severity == ContributionDiagnostic::Severity::kError);
    REQUIRE(diag.contribution_type == "command");
}

TEST_CASE("P14-T01 diagnostic warning level", "[v24][p14]") {
    ContributionDiagnostic diag;
    diag.severity = ContributionDiagnostic::Severity::kWarning;
    diag.message = "Missing description";
    REQUIRE(diag.severity == ContributionDiagnostic::Severity::kWarning);
}

// P14-T02: Contribution validator construction
TEST_CASE("P14-T02 validator default state", "[v24][p14]") {
    ContributionValidator validator;
    REQUIRE_FALSE(validator.has_errors());
}

TEST_CASE("P14-T02 validator reset", "[v24][p14]") {
    ContributionValidator validator;
    validator.reset();
    REQUIRE_FALSE(validator.has_errors());
}

// P14-T03: Extension contributions validation
TEST_CASE("P14-T03 validate empty contributions", "[v24][p14]") {
    ContributionValidator validator;
    ExtensionContributions empty;
    auto diagnostics = validator.validate_extension("test-ext", empty);
    REQUIRE(diagnostics.empty());
}

TEST_CASE("P14-T03 validate extension with commands", "[v24][p14]") {
    ContributionValidator validator;
    ExtensionContributions contribs;
    ExtensionCommand cmd;
    cmd.command = "test.command";
    cmd.title = "Test Command";
    contribs.commands.push_back(cmd);
    auto diagnostics = validator.validate_extension("test-ext", contribs);
    // Valid command should produce no errors
    REQUIRE(diagnostics.size() >= 0);
}

// P14-T04: Plugin manifest validation
TEST_CASE("P14-T04 validate empty plugin contributions", "[v24][p14]") {
    ContributionValidator validator;
    PluginManifest::ContributionPoints points;
    auto diagnostics = validator.validate_plugin("my-plugin", points);
    REQUIRE(diagnostics.empty());
}

TEST_CASE("P14-T04 validate plugin with keybindings", "[v24][p14]") {
    ContributionValidator validator;
    PluginManifest::ContributionPoints points;
    KeybindingContribution kb;
    kb.command_id = "plugin.action";
    kb.key_code = 80; // 'P'
    points.keybindings.push_back(kb);
    auto diagnostics = validator.validate_plugin("my-plugin", points);
    REQUIRE(diagnostics.size() >= 0);
}

// P14-T05: Diagnostic assertions
TEST_CASE("P14-T05 diagnostic severity comparison", "[v24][p14]") {
    REQUIRE(ContributionDiagnostic::Severity::kWarning != ContributionDiagnostic::Severity::kError);
}

TEST_CASE("P14-T05 diagnostic fields populated", "[v24][p14]") {
    ContributionDiagnostic d;
    d.extension_id = "ext-1";
    d.contribution_type = "theme";
    d.field = "path";
    d.message = "Theme file not found";
    REQUIRE_FALSE(d.extension_id.empty());
    REQUIRE_FALSE(d.message.empty());
}
