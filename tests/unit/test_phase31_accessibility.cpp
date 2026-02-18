/// @file test_phase31_accessibility.cpp
/// @brief Phase 31: Accessibility System — Comprehensive test suite
///
/// Tests: FocusRingStyler, ColorBlindnessSupport, AccessibilityAudit,
///        AccessibilityCommandProvider, 5 new events, infrastructure integration.

#include "core/AccessibilityAudit.h"
#include "core/AccessibilityCommandProvider.h"
#include "core/ColorBlindnessSupport.h"
#include "core/Events.h"
#include "core/FocusRingStyler.h"

// Existing accessibility infrastructure

#include "core/GraphAccessibility.h"
#include "core/NavigationAccessibility.h"
#include "core/SearchAccessibility.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <map> // Required by GraphAnalytics.h (transitive dependency)

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════════════════
// FocusRingStyler Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("FocusRingStyler: construction defaults", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    auto style = styler.default_style();

    CHECK(style.width_px == FocusRingStyler::kDefaultWidth);
    CHECK(style.offset_px == FocusRingStyler::kDefaultOffset);
    CHECK(style.color == FocusRingStyler::kDefaultAccentColor);
    CHECK(style.corner_radius_px == FocusRingStyler::kDefaultCornerRadius);
    CHECK_THAT(style.opacity, Catch::Matchers::WithinAbs(1.0, 0.01));
}

TEST_CASE("FocusRingStyler: keyboard mode shows ring", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    auto style = styler.compute_ring_style(false, true);

    CHECK(style.width_px == FocusRingStyler::kDefaultWidth);
    CHECK(style.color == FocusRingStyler::kDefaultAccentColor);
    CHECK_THAT(style.opacity, Catch::Matchers::WithinAbs(1.0, 0.01));
}

TEST_CASE("FocusRingStyler: mouse mode hides ring", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    auto style = styler.compute_ring_style(false, false);

    CHECK(style.width_px == 0);
    CHECK_THAT(style.opacity, Catch::Matchers::WithinAbs(0.0, 0.01));
}

TEST_CASE("FocusRingStyler: high contrast mode", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    auto style = styler.compute_ring_style(true, true);

    CHECK(style.width_px == FocusRingStyler::kHighContrastWidth);
    CHECK(style.color == FocusRingStyler::kHighContrastDefaultColor);
    CHECK_THAT(style.opacity, Catch::Matchers::WithinAbs(1.0, 0.01));
}

TEST_CASE("FocusRingStyler: high contrast without keyboard still hidden", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    auto style = styler.compute_ring_style(true, false);

    CHECK(style.width_px == 0);
}

TEST_CASE("FocusRingStyler: custom accent color", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    styler.set_accent_color(0xFF5500);
    auto style = styler.default_style();

    CHECK(style.color == 0xFF5500);
}

TEST_CASE("FocusRingStyler: custom ring width", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    styler.set_ring_width(4);
    auto style = styler.default_style();

    CHECK(style.width_px == 4);
}

TEST_CASE("FocusRingStyler: ring width clamped to minimum 1", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    styler.set_ring_width(0);
    auto style = styler.default_style();

    CHECK(style.width_px == 1);
}

TEST_CASE("FocusRingStyler: touch target — meets minimum", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    CHECK(styler.meets_touch_target(44, 44));
    CHECK(styler.meets_touch_target(100, 100));
    CHECK_FALSE(styler.meets_touch_target(43, 44));
    CHECK_FALSE(styler.meets_touch_target(44, 43));
    CHECK_FALSE(styler.meets_touch_target(10, 10));
}

TEST_CASE("FocusRingStyler: custom touch target", "[phase31][focus_ring]")
{
    FocusRingStyler styler;
    styler.set_min_touch_target(32, 32);
    CHECK(styler.meets_touch_target(32, 32));
    CHECK_FALSE(styler.meets_touch_target(31, 32));
}

TEST_CASE("FocusRingStyler: WCAG contrast ratio black vs white", "[phase31][focus_ring]")
{
    double ratio = FocusRingStyler::contrast_ratio(0x000000, 0xFFFFFF);
    CHECK_THAT(ratio, Catch::Matchers::WithinAbs(21.0, 0.1));
}

TEST_CASE("FocusRingStyler: WCAG contrast ratio same color = 1:1", "[phase31][focus_ring]")
{
    double ratio = FocusRingStyler::contrast_ratio(0x808080, 0x808080);
    CHECK_THAT(ratio, Catch::Matchers::WithinAbs(1.0, 0.01));
}

TEST_CASE("FocusRingStyler: sufficient contrast check", "[phase31][focus_ring]")
{
    CHECK(FocusRingStyler::has_sufficient_contrast(0xFFFFFF, 0x000000));
    CHECK_FALSE(FocusRingStyler::has_sufficient_contrast(0x808080, 0x909090));
}

TEST_CASE("FocusRingStyler: invisible style is fully transparent", "[phase31][focus_ring]")
{
    auto style = FocusRingStyler::invisible_style();
    CHECK(style.width_px == 0);
    CHECK(style.offset_px == 0);
    CHECK(style.corner_radius_px == 0);
    CHECK_THAT(style.opacity, Catch::Matchers::WithinAbs(0.0, 0.01));
}

// ═══════════════════════════════════════════════════════════════════
// ColorBlindnessSupport Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("ColorBlindnessSupport: git status indicators have letters", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;

    auto modified = support.indicator_for_git_status(GitFileStatus::kModified);
    CHECK(modified.letter == "M");
    CHECK(modified.label == "Modified");
    CHECK(modified.kind == IndicatorKind::kLetter);

    auto added = support.indicator_for_git_status(GitFileStatus::kAdded);
    CHECK(added.letter == "A");

    auto deleted = support.indicator_for_git_status(GitFileStatus::kDeleted);
    CHECK(deleted.letter == "D");

    auto untracked = support.indicator_for_git_status(GitFileStatus::kUntracked);
    CHECK(untracked.letter == "U");
}

TEST_CASE("ColorBlindnessSupport: all git indicators unique letters", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;
    auto indicators = support.all_git_indicators();

    CHECK(indicators.size() == 7);

    // All letters should be unique
    std::vector<std::string> letters;
    for (const auto& ind : indicators)
    {
        CHECK_FALSE(ind.letter.empty());
        letters.push_back(ind.letter);
    }
    std::sort(letters.begin(), letters.end());
    auto last = std::unique(letters.begin(), letters.end());
    CHECK(last == letters.end());
}

TEST_CASE("ColorBlindnessSupport: diagnostic indicators have icons", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;

    auto error_ind = support.indicator_for_diagnostic(DiagnosticSeverity::kError);
    CHECK(error_ind.icon_name == "error_circle");
    CHECK(error_ind.label == "Error");
    CHECK(error_ind.kind == IndicatorKind::kIcon);

    auto warning_ind = support.indicator_for_diagnostic(DiagnosticSeverity::kWarning);
    CHECK(warning_ind.icon_name == "warning_triangle");

    auto info_ind = support.indicator_for_diagnostic(DiagnosticSeverity::kInfo);
    CHECK(info_ind.icon_name == "info_circle");

    auto hint_ind = support.indicator_for_diagnostic(DiagnosticSeverity::kHint);
    CHECK(hint_ind.icon_name == "lightbulb");
}

TEST_CASE("ColorBlindnessSupport: all diagnostic indicators", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;
    auto indicators = support.all_diagnostic_indicators();
    CHECK(indicators.size() == 4);
}

TEST_CASE("ColorBlindnessSupport: search match indicators use underline", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;

    auto match = support.indicator_for_search_match();
    CHECK(match.kind == IndicatorKind::kUnderline);
    CHECK(match.label == "Search match");

    auto current = support.indicator_for_current_match();
    CHECK(current.kind == IndicatorKind::kUnderline);
    CHECK(current.label == "Current match");
    CHECK(current.color != match.color); // Different highlight colors
}

TEST_CASE("ColorBlindnessSupport: priority indicators use labels", "[phase31][color_blind]")
{
    ColorBlindnessSupport support;

    auto critical = support.indicator_for_priority(1);
    CHECK(critical.label == "Critical");
    CHECK(critical.kind == IndicatorKind::kLabel);

    auto high = support.indicator_for_priority(2);
    CHECK(high.label == "High");

    auto medium = support.indicator_for_priority(3);
    CHECK(medium.label == "Medium");

    auto low = support.indicator_for_priority(4);
    CHECK(low.label == "Low");

    auto none = support.indicator_for_priority(5);
    CHECK(none.label == "None");
}

// ═══════════════════════════════════════════════════════════════════
// AccessibilityAudit Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("AccessibilityAudit: construction starts empty", "[phase31][audit]")
{
    AccessibilityAudit audit;
    CHECK(audit.issue_count() == 0);
    CHECK(audit.passed());
}

TEST_CASE("AccessibilityAudit: contrast check adds critical issue", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_contrast("text_main", "Main Text", 2.5, 4.5);

    CHECK(audit.issue_count() == 1);
    CHECK_FALSE(audit.passed());

    auto res = audit.result();
    CHECK(res.summary.critical_count == 1);
    CHECK_FALSE(res.passed);
    CHECK(res.issues[0].category == AuditCategory::kContrast);
    CHECK(res.issues[0].severity == AuditSeverity::kCritical);
}

TEST_CASE("AccessibilityAudit: passing contrast check adds no issue", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_contrast("text_main", "Main Text", 7.0, 4.5);

    CHECK(audit.issue_count() == 0);
    CHECK(audit.passed());
}

TEST_CASE("AccessibilityAudit: missing label is critical", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_missing_label("btn_save", "Save Button", false);

    CHECK(audit.issue_count() == 1);
    auto issues = audit.issues_by_category(AuditCategory::kLabel);
    CHECK(issues.size() == 1);
    CHECK(issues[0].severity == AuditSeverity::kCritical);
}

TEST_CASE("AccessibilityAudit: missing alt text is warning", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_missing_alt_text("img_logo", "Logo Image", false);

    CHECK(audit.issue_count() == 1);
    auto issues = audit.issues_by_severity(AuditSeverity::kWarning);
    CHECK(issues.size() == 1);
    CHECK(issues[0].category == AuditCategory::kAltText);
}

TEST_CASE("AccessibilityAudit: keyboard access check", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_keyboard_access("custom_control", "Custom Control", false);

    auto issues = audit.issues_by_category(AuditCategory::kKeyboard);
    CHECK(issues.size() == 1);
    CHECK(issues[0].severity == AuditSeverity::kCritical);
}

TEST_CASE("AccessibilityAudit: focus ring check is warning", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_focus_ring("input_field", "Input Field", false);

    auto issues = audit.issues_by_category(AuditCategory::kFocusRing);
    CHECK(issues.size() == 1);
    CHECK(issues[0].severity == AuditSeverity::kWarning);
}

TEST_CASE("AccessibilityAudit: role check is info", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_role("tab_bar", "Tab Bar", false);

    auto issues = audit.issues_by_category(AuditCategory::kRole);
    CHECK(issues.size() == 1);
    CHECK(issues[0].severity == AuditSeverity::kInfo);
}

TEST_CASE("AccessibilityAudit: mixed issues pass only if no critical", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_missing_alt_text("img1", "Image 1", false); // warning
    audit.check_role("tab1", "Tab 1", false);               // info

    CHECK(audit.issue_count() == 2);
    CHECK(audit.passed()); // No critical issues

    audit.check_missing_label("btn1", "Button 1", false); // critical
    CHECK_FALSE(audit.passed());
}

TEST_CASE("AccessibilityAudit: reset clears all issues", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_missing_label("btn1", "Button 1", false);
    CHECK(audit.issue_count() == 1);

    audit.reset();
    CHECK(audit.issue_count() == 0);
    CHECK(audit.passed());
}

TEST_CASE("AccessibilityAudit: JSON export structure", "[phase31][audit]")
{
    AccessibilityAudit audit;
    audit.check_contrast("text_main", "Main Text", 2.5, 4.5);

    auto json = audit.to_json();
    CHECK_THAT(json, Catch::Matchers::ContainsSubstring("\"passed\": false"));
    CHECK_THAT(json, Catch::Matchers::ContainsSubstring("\"critical\": 1"));
    CHECK_THAT(json, Catch::Matchers::ContainsSubstring("\"element_id\": \"text_main\""));
    CHECK_THAT(json, Catch::Matchers::ContainsSubstring("\"category\": \"contrast\""));
}

TEST_CASE("AccessibilityAudit: severity and category names", "[phase31][audit]")
{
    CHECK(AccessibilityAudit::severity_name(AuditSeverity::kCritical) == "critical");
    CHECK(AccessibilityAudit::severity_name(AuditSeverity::kWarning) == "warning");
    CHECK(AccessibilityAudit::severity_name(AuditSeverity::kInfo) == "info");

    CHECK(AccessibilityAudit::category_name(AuditCategory::kContrast) == "contrast");
    CHECK(AccessibilityAudit::category_name(AuditCategory::kLabel) == "label");
    CHECK(AccessibilityAudit::category_name(AuditCategory::kKeyboard) == "keyboard");
    CHECK(AccessibilityAudit::category_name(AuditCategory::kAltText) == "alt_text");
    CHECK(AccessibilityAudit::category_name(AuditCategory::kFocusRing) == "focus_ring");
    CHECK(AccessibilityAudit::category_name(AuditCategory::kRole) == "role");
}

// ═══════════════════════════════════════════════════════════════════
// AccessibilityCommandProvider Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("AccessibilityCommandProvider: construction registers 8 commands", "[phase31][commands]")
{
    AccessibilityCommandProvider provider;
    CHECK(provider.command_count() == 8);
}

TEST_CASE("AccessibilityCommandProvider: find by ID", "[phase31][commands]")
{
    AccessibilityCommandProvider provider;

    auto cmd = provider.find_command("a11y.toggleHighContrast");
    REQUIRE(cmd.has_value());
    CHECK(cmd->title == "Accessibility: Toggle High Contrast");
    CHECK(cmd->category == "Accessibility");

    auto missing = provider.find_command("a11y.nonexistent");
    CHECK_FALSE(missing.has_value());
}

TEST_CASE("AccessibilityCommandProvider: commands by category", "[phase31][commands]")
{
    AccessibilityCommandProvider provider;

    auto a11y_cmds = provider.commands_in_category("Accessibility");
    CHECK(a11y_cmds.size() == 3); // toggleHighContrast, toggleReducedMotion, runAudit

    auto zoom_cmds = provider.commands_in_category("Zoom");
    CHECK(zoom_cmds.size() == 3); // zoomIn, zoomOut, zoomReset

    auto sr_cmds = provider.commands_in_category("Screen Reader");
    CHECK(sr_cmds.size() == 2); // readLine, navigateHeading
}

TEST_CASE("AccessibilityCommandProvider: categories list", "[phase31][commands]")
{
    AccessibilityCommandProvider provider;
    auto cats = provider.categories();
    CHECK(cats.size() == 3);
}

TEST_CASE("AccessibilityCommandProvider: all command IDs present", "[phase31][commands]")
{
    AccessibilityCommandProvider provider;

    const std::vector<std::string> expected_ids = {
        "a11y.toggleHighContrast",
        "a11y.toggleReducedMotion",
        "a11y.runAudit",
        "a11y.zoomIn",
        "a11y.zoomOut",
        "a11y.zoomReset",
        "a11y.readLine",
        "a11y.navigateHeading",
    };

    for (const auto& cmd_id : expected_ids)
    {
        auto cmd = provider.find_command(cmd_id);
        REQUIRE(cmd.has_value());
        CHECK_FALSE(cmd->title.empty());
        CHECK_FALSE(cmd->description.empty());
    }
}

// ═══════════════════════════════════════════════════════════════════
// Phase 31 Events Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Event: A11yHighContrastToggledEvent", "[phase31][events]")
{
    A11yHighContrastToggledEvent event;
    event.enabled = true;
    CHECK(event.enabled);
}

TEST_CASE("Event: A11yReducedMotionToggledEvent", "[phase31][events]")
{
    A11yReducedMotionToggledEvent event;
    event.enabled = true;
    CHECK(event.enabled);
}

TEST_CASE("Event: A11yAuditCompletedEvent", "[phase31][events]")
{
    A11yAuditCompletedEvent event;
    event.critical_count = 2;
    event.warning_count = 5;
    event.info_count = 3;
    event.passed = false;

    CHECK(event.critical_count == 2);
    CHECK(event.warning_count == 5);
    CHECK(event.info_count == 3);
    CHECK_FALSE(event.passed);
}

TEST_CASE("Event: A11yFocusChangedEvent", "[phase31][events]")
{
    A11yFocusChangedEvent event;
    event.element_id = "btn_save";
    event.element_label = "Save Button";

    CHECK(event.element_id == "btn_save");
    CHECK(event.element_label == "Save Button");
}

TEST_CASE("Event: A11yAnnouncementEvent", "[phase31][events]")
{
    A11yAnnouncementEvent event;
    event.message = "Document saved";
    event.priority = "polite";

    CHECK(event.message == "Document saved");
    CHECK(event.priority == "polite");
}

// ═══════════════════════════════════════════════════════════════════
// Infrastructure Integration Tests
// ═══════════════════════════════════════════════════════════════════

TEST_CASE("Integration: SearchAccessibility announcements", "[phase31][integration]")
{
    auto result_text = SearchAccessibility::announce_result_count(42);
    CHECK_THAT(result_text, Catch::Matchers::ContainsSubstring("42 results found"));

    auto nav_text = SearchAccessibility::announce_navigation(2, 42);
    CHECK_THAT(nav_text, Catch::Matchers::ContainsSubstring("Result 3 of 42"));

    auto zero_text = SearchAccessibility::announce_result_count(0);
    CHECK(zero_text == "No results found");
}

TEST_CASE("Integration: GraphAccessibility node info", "[phase31][integration]")
{
    GraphNode node;
    node.label = "README.md";
    node.type = GraphNodeType::Document;
    node.ref_count = 5;

    auto info = GraphAccessibility::announce_node_info(node);
    CHECK_THAT(info, Catch::Matchers::ContainsSubstring("Document"));
    CHECK_THAT(info, Catch::Matchers::ContainsSubstring("README.md"));
    CHECK_THAT(info, Catch::Matchers::ContainsSubstring("5 reference"));
}

TEST_CASE("Integration: NavigationAccessibility heading jump", "[phase31][integration]")
{
    auto text = NavigationAccessibility::announce_heading_jump("Introduction", 2);
    CHECK_THAT(text, Catch::Matchers::ContainsSubstring("heading level 2"));
    CHECK_THAT(text, Catch::Matchers::ContainsSubstring("Introduction"));
}
