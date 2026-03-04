/// @file test_ui_harness.cpp
/// @brief Phase 50 — Unit tests for UI Integration Testing Harness.

#include "testing/AccessibilityAuditor.h"
#include "testing/MemoryLeakDetector.h"
#include "testing/ThemeCoverageChecker.h"
#include "testing/UIAutomation.h"
#include "testing/VisualRegressionRunner.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::testing;

// ============================================================================
// VisualRegressionRunner
// ============================================================================

TEST_CASE("VisualRegressionRunner: identical buffers match", "[ui_harness]")
{
    VisualRegressionRunner runner;
    std::vector<uint8_t> buffer(4 * 100, 128); // 10x10 solid gray

    double diff = runner.compare_buffers(buffer, buffer, 10, 10);
    REQUIRE(diff == Catch::Approx(0.0));
}

TEST_CASE("VisualRegressionRunner: different buffers report diff", "[ui_harness]")
{
    VisualRegressionRunner runner;
    std::vector<uint8_t> baseline(4 * 100, 0);
    std::vector<uint8_t> actual(4 * 100, 255);

    double diff = runner.compare_buffers(baseline, actual, 10, 10);
    REQUIRE(diff == Catch::Approx(100.0));
}

// ============================================================================
// UIAutomation
// ============================================================================

TEST_CASE("UIAutomation: click generates event", "[ui_harness]")
{
    UIAutomation::clear_events();
    UIAutomation::queue_event(UIAutomation::make_click(100, 200));

    REQUIRE(UIAutomation::event_count() == 1);
    auto& events = UIAutomation::queued_events();
    REQUIRE(events[0].type == AutomationEventType::Click);
    REQUIRE(events[0].x == 100);
    REQUIRE(events[0].y == 200);

    UIAutomation::clear_events();
}

// ============================================================================
// AccessibilityAuditor
// ============================================================================

TEST_CASE("AccessibilityAuditor: detects missing names", "[ui_harness]")
{
    AccessibilityAuditor auditor;
    auditor.add_element("btn1", "Save", "button", true);
    auditor.add_element("btn2", "", "button", true); // Missing name

    auto result = auditor.audit();
    REQUIRE(result.elements_scanned == 2);
    REQUIRE(result.error_count() == 1);
    REQUIRE(result.accessible_names_found == 1);
}

TEST_CASE("AccessibilityAuditor: compliance rate calculation", "[ui_harness]")
{
    AccessibilityAuditor auditor;
    auditor.add_element("e1", "Label", "button", true);
    auditor.add_element("e2", "Label2", "button", true);
    auditor.add_element("e3", "", "", false); // Missing name

    auto result = auditor.audit();
    REQUIRE(result.compliance_rate() < 100.0);
    REQUIRE(result.compliance_rate() > 50.0);
}

// ============================================================================
// ThemeCoverageChecker
// ============================================================================

TEST_CASE("ThemeCoverageChecker: finds hardcoded colors", "[ui_harness]")
{
    ThemeCoverageChecker checker;
    std::string code = "auto color = #FF0000;\n"
                       "auto bg = rgb(255, 0, 0);\n"
                       "auto themed = theme.foreground();";

    auto findings = checker.scan_for_hardcoded_colors(code, "test.cpp");
    REQUIRE(findings.size() == 2); // #FF0000 and rgb(255,0,0)
}

// ============================================================================
// MemoryLeakDetector
// ============================================================================

TEST_CASE("MemoryLeakDetector: detects intentional leak", "[ui_harness]")
{
    MemoryLeakDetector detector;
    detector.record_before("TestWidget", 100 * 1024 * 1024);     // 100 MB
    detector.record_after("TestWidget", 102 * 1024 * 1024, 100); // 102 MB (2MB leak)

    REQUIRE(detector.has_leaks());
    REQUIRE(detector.leak_count() == 1);
    REQUIRE(detector.results()[0].delta_mb == Catch::Approx(2.0));
}

TEST_CASE("MemoryLeakDetector: no leak under threshold", "[ui_harness]")
{
    MemoryLeakDetector detector;
    detector.record_before("SmallWidget", 100 * 1024 * 1024);
    detector.record_after("SmallWidget", 100 * 1024 * 1024 + 512 * 1024, 100); // 0.5 MB

    REQUIRE_FALSE(detector.has_leaks());
}

// ============================================================================
// VisualRegressionRunner — Pass Rate
// ============================================================================

TEST_CASE("VisualRegressionRunner: pass rate calculation", "[ui_harness]")
{
    VisualRegressionRunner runner;
    runner.record_result({.test_name = "t1", .result = ComparisonResult::Match});
    runner.record_result({.test_name = "t2", .result = ComparisonResult::Match});
    runner.record_result({.test_name = "t3", .result = ComparisonResult::Mismatch});

    REQUIRE(runner.pass_rate() == Catch::Approx(66.666).margin(0.1));
    REQUIRE(runner.pass_count() == 2);
    REQUIRE(runner.fail_count() == 1);
}

// ============================================================================
// UIAutomation — Drag Event
// ============================================================================

TEST_CASE("UIAutomation: drag event creation", "[ui_harness]")
{
    auto drag = UIAutomation::make_drag(10, 20, 100, 200);
    REQUIRE(drag.type == AutomationEventType::Drag);
    REQUIRE(drag.x == 10);
    REQUIRE(drag.end_x == 100);
}
