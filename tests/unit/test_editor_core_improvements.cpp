// Phase 7: Editor Core Improvements — Comprehensive Tests
// Tests EditorPerformanceBudget, EditorStateSerializer, and Phase 7 events.
// GUI-dependent features (EditorPanel Scintilla wiring) are tested via live integration.

#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/EditorPerformanceBudget.h"
#include "ui/EditorStateSerializer.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::ui;

// ═══════════════════════════════════════════════════════
// EditorPerformanceBudget Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("EditorPerformanceBudget default thresholds", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    const auto& thresholds = budget.thresholds();
    REQUIRE(thresholds.keystroke_latency_ms == 16.0);
    REQUIRE(thresholds.syntax_highlight_ms == 50.0);
    REQUIRE(thresholds.content_change_debounce_ms == 50.0);
    REQUIRE(thresholds.fold_scan_ms == 100.0);
    REQUIRE(thresholds.theme_apply_ms == 30.0);
}

TEST_CASE("EditorPerformanceBudget custom thresholds", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    EditorBudgetThresholds custom;
    custom.keystroke_latency_ms = 8.0;
    custom.syntax_highlight_ms = 25.0;
    budget.set_thresholds(custom);

    REQUIRE(budget.thresholds().keystroke_latency_ms == 8.0);
    REQUIRE(budget.thresholds().syntax_highlight_ms == 25.0);
}

TEST_CASE("EditorPerformanceBudget recording and queries", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    REQUIRE(budget.last_keystroke_latency() == 0.0);
    REQUIRE(budget.sample_count() == 0);

    budget.record_keystroke_latency(10.0);
    REQUIRE(budget.last_keystroke_latency() == 10.0);
    REQUIRE(budget.sample_count() == 1);

    budget.record_keystroke_latency(20.0);
    REQUIRE(budget.last_keystroke_latency() == 20.0);
    REQUIRE(budget.sample_count() == 2);

    REQUIRE(budget.average_keystroke_latency() == 15.0);
}

TEST_CASE("EditorPerformanceBudget syntax highlight recording", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    budget.record_syntax_highlight_duration(45.0);
    REQUIRE(budget.last_syntax_highlight_duration() == 45.0);
}

TEST_CASE("EditorPerformanceBudget no violations when under budget", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    budget.record_keystroke_latency(10.0);
    budget.record_syntax_highlight_duration(30.0);
    budget.record_content_change_debounce(40.0);
    budget.record_fold_scan_duration(50.0);
    budget.record_theme_apply_duration(20.0);

    auto violations = budget.check();
    REQUIRE(violations.empty());
}

TEST_CASE("EditorPerformanceBudget detects violations", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    // Exceed keystroke latency budget (16ms default)
    budget.record_keystroke_latency(25.0);

    auto violations = budget.check();
    REQUIRE(violations.size() == 1);
    REQUIRE(violations[0].metric_name == "keystroke_latency_ms");
    REQUIRE(violations[0].measured_value == 25.0);
    REQUIRE(violations[0].budget_value == 16.0);
    REQUIRE(violations[0].severity == BudgetSeverity::kWarning);
}

TEST_CASE("EditorPerformanceBudget critical severity", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    // More than 2x threshold → critical
    budget.record_keystroke_latency(50.0); // 50 > 16*2=32

    auto violations = budget.check();
    REQUIRE(violations.size() == 1);
    REQUIRE(violations[0].severity == BudgetSeverity::kCritical);
}

TEST_CASE("EditorPerformanceBudget multiple violations", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    budget.record_keystroke_latency(25.0);         // Over 16ms
    budget.record_syntax_highlight_duration(80.0); // Over 50ms
    budget.record_fold_scan_duration(150.0);       // Over 100ms

    auto violations = budget.check();
    REQUIRE(violations.size() == 3);
}

TEST_CASE("EditorPerformanceBudget publish events", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    bool event_received = false;
    std::string received_metric;
    auto sub = bus.subscribe<EditorBudgetViolationEvent>(
        [&](const EditorBudgetViolationEvent& evt)
        {
            event_received = true;
            received_metric = evt.metric_name;
        });

    budget.record_keystroke_latency(25.0);
    budget.check_and_publish();

    REQUIRE(event_received);
    REQUIRE(received_metric == "keystroke_latency_ms");
}

TEST_CASE("EditorPerformanceBudget reset", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    budget.record_keystroke_latency(25.0);
    budget.record_syntax_highlight_duration(80.0);
    REQUIRE(budget.sample_count() == 1);

    budget.reset();
    REQUIRE(budget.sample_count() == 0);
    REQUIRE(budget.last_keystroke_latency() == 0.0);
    REQUIRE(budget.last_syntax_highlight_duration() == 0.0);

    auto violations = budget.check();
    REQUIRE(violations.empty());
}

TEST_CASE("EditorPerformanceBudget sample cap", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    // Record 150 samples, should only keep last 100
    for (int idx = 0; idx < 150; ++idx)
    {
        budget.record_keystroke_latency(static_cast<double>(idx));
    }

    REQUIRE(budget.sample_count() == 100);
    // Average should be of samples 50-149 = (50+149)/2 = 99.5
    REQUIRE(budget.average_keystroke_latency() == 99.5);
}

// ═══════════════════════════════════════════════════════
// EditorStateSerializer Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("EditorStateSerializer default round-trip", "[editor_state_serializer]")
{
    EditorSessionSnapshot snap;
    auto json = EditorStateSerializer::serialize(snap);
    REQUIRE_FALSE(json.empty());

    auto restored = EditorStateSerializer::deserialize(json);
    REQUIRE(restored.cursor_position == 0);
    REQUIRE(restored.first_visible_line == 0);
    REQUIRE(restored.wrap_mode == 1);
    REQUIRE(restored.show_line_numbers == true);
    REQUIRE(restored.bracket_matching == true);
    REQUIRE(restored.auto_indent == true);
    REQUIRE(restored.code_folding == true);
    REQUIRE(restored.indentation_guides == true);
    REQUIRE(restored.show_whitespace == false);
    REQUIRE(restored.relative_line_numbers == false);
    REQUIRE(restored.bracket_pair_colorization == false);
    REQUIRE(restored.inline_diagnostics == true);
    REQUIRE(restored.minimap_visible == false);
    REQUIRE(restored.sticky_scroll == false);
    REQUIRE(restored.font_size == 13);
    REQUIRE(restored.tab_size == 4);
    REQUIRE(restored.edge_column == 80);
    REQUIRE(restored.zoom_level == 0);
    REQUIRE(restored.productivity_mode == 0);
    REQUIRE(restored.font_family == "Menlo");
}

TEST_CASE("EditorStateSerializer custom values round-trip", "[editor_state_serializer]")
{
    EditorSessionSnapshot snap;
    snap.cursor_position = 1234;
    snap.first_visible_line = 56;
    snap.wrap_mode = 2;
    snap.show_line_numbers = false;
    snap.bracket_matching = false;
    snap.relative_line_numbers = true;
    snap.bracket_pair_colorization = true;
    snap.inline_diagnostics = false;
    snap.minimap_visible = true;
    snap.sticky_scroll = true;
    snap.font_size = 16;
    snap.tab_size = 2;
    snap.edge_column = 120;
    snap.zoom_level = 3;
    snap.productivity_mode = 2;
    snap.font_family = "JetBrains Mono";

    auto json = EditorStateSerializer::serialize(snap);
    auto restored = EditorStateSerializer::deserialize(json);

    REQUIRE(restored.cursor_position == 1234);
    REQUIRE(restored.first_visible_line == 56);
    REQUIRE(restored.wrap_mode == 2);
    REQUIRE(restored.show_line_numbers == false);
    REQUIRE(restored.bracket_matching == false);
    REQUIRE(restored.relative_line_numbers == true);
    REQUIRE(restored.bracket_pair_colorization == true);
    REQUIRE(restored.inline_diagnostics == false);
    REQUIRE(restored.minimap_visible == true);
    REQUIRE(restored.sticky_scroll == true);
    REQUIRE(restored.font_size == 16);
    REQUIRE(restored.tab_size == 2);
    REQUIRE(restored.edge_column == 120);
    REQUIRE(restored.zoom_level == 3);
    REQUIRE(restored.productivity_mode == 2);
    REQUIRE(restored.font_family == "JetBrains Mono");
}

TEST_CASE("EditorStateSerializer empty JSON defaults", "[editor_state_serializer]")
{
    auto restored = EditorStateSerializer::deserialize("{}");
    REQUIRE(restored.font_size == 13);
    REQUIRE(restored.tab_size == 4);
    REQUIRE(restored.show_line_numbers == true);
    REQUIRE(restored.font_family == "Menlo");
}

TEST_CASE("EditorStateSerializer partial JSON", "[editor_state_serializer]")
{
    auto restored = EditorStateSerializer::deserialize(R"({"font_size": 18, "tab_size": 8})");
    REQUIRE(restored.font_size == 18);
    REQUIRE(restored.tab_size == 8);
    // Everything else should be defaults
    REQUIRE(restored.cursor_position == 0);
    REQUIRE(restored.show_line_numbers == true);
}

TEST_CASE("EditorStateSerializer negative values", "[editor_state_serializer]")
{
    EditorSessionSnapshot snap;
    snap.zoom_level = -2;
    snap.cursor_position = -1;

    auto json = EditorStateSerializer::serialize(snap);
    auto restored = EditorStateSerializer::deserialize(json);

    REQUIRE(restored.zoom_level == -2);
    REQUIRE(restored.cursor_position == -1);
}

TEST_CASE("EditorStateSerializer JSON format", "[editor_state_serializer]")
{
    EditorSessionSnapshot snap;
    auto json = EditorStateSerializer::serialize(snap);

    // Should be valid JSON-like format
    REQUIRE(json.front() == '{');
    REQUIRE(json.back() == '}');
    REQUIRE(json.find("\"cursor_position\"") != std::string::npos);
    REQUIRE(json.find("\"font_family\"") != std::string::npos);
}

// ═══════════════════════════════════════════════════════
// Phase 7 Event Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("EditorBudgetViolationEvent fields", "[editor_events]")
{
    EventBus bus;

    bool received = false;
    std::string metric;
    double measured = 0.0;
    double budget_val = 0.0;
    bool critical = false;

    auto sub = bus.subscribe<EditorBudgetViolationEvent>(
        [&](const EditorBudgetViolationEvent& evt)
        {
            received = true;
            metric = evt.metric_name;
            measured = evt.measured_value;
            budget_val = evt.budget_value;
            critical = evt.is_critical;
        });

    EditorBudgetViolationEvent evt;
    evt.metric_name = "syntax_highlight_ms";
    evt.measured_value = 75.0;
    evt.budget_value = 50.0;
    evt.is_critical = true;
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(metric == "syntax_highlight_ms");
    REQUIRE(measured == 75.0);
    REQUIRE(budget_val == 50.0);
    REQUIRE(critical);
}

TEST_CASE("EditorDiagnosticPeekEvent fields", "[editor_events]")
{
    EventBus bus;

    bool received = false;
    int line = -1;
    std::string message;
    std::string severity;

    auto sub = bus.subscribe<EditorDiagnosticPeekEvent>(
        [&](const EditorDiagnosticPeekEvent& evt)
        {
            received = true;
            line = evt.line;
            message = evt.message;
            severity = evt.severity;
        });

    EditorDiagnosticPeekEvent evt;
    evt.panel_id = "editor1";
    evt.line = 42;
    evt.message = "unused variable 'x'";
    evt.severity = "warning";
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(line == 42);
    REQUIRE(message == "unused variable 'x'");
    REQUIRE(severity == "warning");
}

TEST_CASE("EditorQuickFixRequestEvent fields", "[editor_events]")
{
    EventBus bus;

    bool received = false;
    int line = -1;
    std::string diag_id;

    auto sub = bus.subscribe<EditorQuickFixRequestEvent>(
        [&](const EditorQuickFixRequestEvent& evt)
        {
            received = true;
            line = evt.line;
            diag_id = evt.diagnostic_id;
        });

    EditorQuickFixRequestEvent evt;
    evt.line = 10;
    evt.diagnostic_id = "clang-tidy-W001";
    bus.publish(evt);

    REQUIRE(received);
    REQUIRE(line == 10);
    REQUIRE(diag_id == "clang-tidy-W001");
}

// ═══════════════════════════════════════════════════════
// Edge Case Tests
// ═══════════════════════════════════════════════════════

TEST_CASE("EditorPerformanceBudget average with zero samples", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);
    REQUIRE(budget.average_keystroke_latency() == 0.0);
}

TEST_CASE("EditorStateSerializer malformed JSON", "[editor_state_serializer]")
{
    // Should not crash, return defaults
    auto restored = EditorStateSerializer::deserialize("not json at all");
    REQUIRE(restored.font_size == 13);
    REQUIRE(restored.tab_size == 4);
}

TEST_CASE("EditorStateSerializer empty string", "[editor_state_serializer]")
{
    auto restored = EditorStateSerializer::deserialize("");
    REQUIRE(restored.font_size == 13);
    REQUIRE(restored.font_family == "Menlo");
}

TEST_CASE("EditorPerformanceBudget no events if no violations", "[editor_perf_budget]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    bool event_received = false;
    auto sub = bus.subscribe<EditorBudgetViolationEvent>([&](const EditorBudgetViolationEvent&)
                                                         { event_received = true; });

    budget.record_keystroke_latency(5.0); // Well under 16ms
    budget.check_and_publish();

    REQUIRE_FALSE(event_received);
}

// ═══════════════════════════════════════════════════════
// Benchmarks
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: Performance budget check", "[editor_perf_budget][!benchmark]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);
    budget.record_keystroke_latency(10.0);
    budget.record_syntax_highlight_duration(30.0);
    budget.record_fold_scan_duration(50.0);

    BENCHMARK("budget.check()")
    {
        return budget.check();
    };
}

TEST_CASE("Benchmark: State serialization", "[editor_state_serializer][!benchmark]")
{
    EditorSessionSnapshot snap;
    snap.cursor_position = 5000;
    snap.font_size = 14;
    snap.font_family = "Fira Code";

    BENCHMARK("serialize")
    {
        return EditorStateSerializer::serialize(snap);
    };

    auto json = EditorStateSerializer::serialize(snap);
    BENCHMARK("deserialize")
    {
        return EditorStateSerializer::deserialize(json);
    };
}

TEST_CASE("Benchmark: Performance budget recording", "[editor_perf_budget][!benchmark]")
{
    EventBus bus;
    EditorPerformanceBudget budget(bus);

    BENCHMARK("record 100 samples")
    {
        for (int idx = 0; idx < 100; ++idx)
        {
            budget.record_keystroke_latency(static_cast<double>(idx));
        }
        return budget.average_keystroke_latency();
    };
}

TEST_CASE("Benchmark: State serialization round-trip", "[editor_state_serializer][!benchmark]")
{
    EditorSessionSnapshot snap;
    snap.cursor_position = 12345;
    snap.font_size = 16;
    snap.tab_size = 2;
    snap.relative_line_numbers = true;
    snap.bracket_pair_colorization = true;
    snap.font_family = "JetBrains Mono";

    BENCHMARK("serialize + deserialize")
    {
        auto json = EditorStateSerializer::serialize(snap);
        return EditorStateSerializer::deserialize(json);
    };
}
