/// @file test_v20_accessibility_audit.cpp
/// @brief V20 Phase 09 – AccessibilityAuditGate unit tests.

#include "core/AccessibilityAuditGate.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("A11yAudit: construction", "[v20][a11y-audit]")
{
    EventBus bus;
    AccessibilityAuditGate gate(bus);
    REQUIRE(gate.finding_count() == 0);
    REQUIRE(gate.gates_pass());
}

TEST_CASE("A11yAudit: record non-blocking finding", "[v20][a11y-audit]")
{
    EventBus bus;
    AccessibilityAuditGate gate(bus);

    A11yFinding finding;
    finding.id = "f-1";
    finding.surface = "toolbar";
    finding.description = "Button missing tooltip";
    finding.severity = A11ySeverity::kWarning;
    finding.category = A11yCategory::kTooltip;
    gate.record_finding(finding);

    REQUIRE(gate.finding_count() == 1);
    REQUIRE(gate.blocking_count() == 0);
    REQUIRE(gate.gates_pass());
}

TEST_CASE("A11yAudit: blocking finding fails gates", "[v20][a11y-audit]")
{
    EventBus bus;
    AccessibilityAuditGate gate(bus);

    A11yFinding finding;
    finding.id = "f-crit";
    finding.surface = "explorer";
    finding.description = "Focus not reachable via keyboard";
    finding.severity = A11ySeverity::kCritical;
    finding.category = A11yCategory::kKeyboard;
    gate.record_finding(finding);

    REQUIRE(gate.blocking_count() == 1);
    REQUIRE_FALSE(gate.gates_pass());
}

TEST_CASE("A11yAudit: visual debt tracking", "[v20][a11y-audit]")
{
    EventBus bus;
    AccessibilityAuditGate gate(bus);

    VisualDebtEntry debt;
    debt.surface = "tab-bar";
    debt.description = "Inconsistent close button spacing";
    debt.category = "spacing";
    gate.record_visual_debt(debt);

    REQUIRE(gate.unresolved_debt_count() == 1);

    gate.resolve_visual_debt("tab-bar", "spacing");
    REQUIRE(gate.unresolved_debt_count() == 0);
}

TEST_CASE("A11yAudit: findings by severity", "[v20][a11y-audit]")
{
    EventBus bus;
    AccessibilityAuditGate gate(bus);

    A11yFinding warn, err;
    warn.id = "w1"; warn.severity = A11ySeverity::kWarning;
    err.id = "e1"; err.severity = A11ySeverity::kError;
    gate.record_finding(warn);
    gate.record_finding(err);

    REQUIRE(gate.findings_by_severity(A11ySeverity::kWarning).size() == 1);
    REQUIRE(gate.findings_by_severity(A11ySeverity::kError).size() == 1);
    REQUIRE(gate.findings_by_severity(A11ySeverity::kInfo).empty());
}
