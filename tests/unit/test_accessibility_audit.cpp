// test_accessibility_audit.cpp — 10 tests for AccessibilityAudit
#include "core/AccessibilityAudit.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("AccessibilityAudit starts empty and passed", "[a11y]")
{
    AccessibilityAudit audit;
    CHECK(audit.issue_count() == 0);
    CHECK(audit.passed());
}

TEST_CASE("AccessibilityAudit check_contrast flags low ratio", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_contrast("btn1", "Submit Button", 2.0, 4.5);
    CHECK(audit.issue_count() == 1);
    auto issues = audit.issues_by_category(AuditCategory::kContrast);
    CHECK(issues.size() == 1);
}

TEST_CASE("AccessibilityAudit check_contrast passes good ratio", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_contrast("btn1", "Submit Button", 5.0, 4.5);
    CHECK(audit.issue_count() == 0);
}

TEST_CASE("AccessibilityAudit check_missing_label flags missing", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_missing_label("input1", "Text Input", false);
    CHECK(audit.issue_count() == 1);
}

TEST_CASE("AccessibilityAudit check_missing_alt_text flags missing", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_missing_alt_text("img1", "Logo", false);
    CHECK(audit.issue_count() == 1);
}

TEST_CASE("AccessibilityAudit check_keyboard_access flags inaccessible", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_keyboard_access("custom1", "Custom Widget", false);
    CHECK(audit.issue_count() == 1);
}

TEST_CASE("AccessibilityAudit issues_by_severity filters", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_contrast("c1", "Low", 1.5, 4.5);
    audit.check_missing_label("l1", "Missing", false);
    auto critical = audit.issues_by_severity(AuditSeverity::kCritical);
    CHECK_FALSE(critical.empty());
}

TEST_CASE("AccessibilityAudit reset clears everything", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_missing_label("x", "X", false);
    CHECK(audit.issue_count() == 1);
    audit.reset();
    CHECK(audit.issue_count() == 0);
    CHECK(audit.passed());
}

TEST_CASE("AccessibilityAudit severity_name and category_name", "[a11y]")
{
    CHECK_FALSE(AccessibilityAudit::severity_name(AuditSeverity::kCritical).empty());
    CHECK_FALSE(AccessibilityAudit::severity_name(AuditSeverity::kWarning).empty());
    CHECK_FALSE(AccessibilityAudit::category_name(AuditCategory::kContrast).empty());
    CHECK_FALSE(AccessibilityAudit::category_name(AuditCategory::kLabel).empty());
}

TEST_CASE("AccessibilityAudit to_json returns valid JSON", "[a11y]")
{
    AccessibilityAudit audit;
    audit.check_missing_label("el1", "Element", false);
    auto json = audit.to_json();
    CHECK_FALSE(json.empty());
}
