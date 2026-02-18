// ============================================================================
// File: tests/unit/test_phase30_performance.cpp
// Phase 30: Performance Optimization — Comprehensive test suite
// ============================================================================

#include "core/AdaptiveThrottle.h"
#include "core/Events.h"
#include "core/IncrementalParsePolicy.h"
#include "core/LazyService.h"
#include "core/MemoryBudgetEnforcer.h"
#include "core/PerformanceCommandProvider.h"
#include "core/PerformanceProfiler.h"
#include "core/SPSCQueue.h"
#include "core/StartupTimer.h"
#include "rendering/DirtyRegion.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <chrono>
#include <string>
#include <thread>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;
using namespace markamp::rendering;

// ============================================================================
// PerformanceProfiler Tests
// ============================================================================

TEST_CASE("PerformanceProfiler construction", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    SECTION("initial state is clean")
    {
        REQUIRE(profiler.checkpoint_count() == 0);
        REQUIRE(profiler.stall_count() == 0);
        REQUIRE(profiler.is_profiling() == false);
        REQUIRE(profiler.profile_capture_complete() == false);
    }

    SECTION("frame budget defaults to ~60fps")
    {
        REQUIRE(profiler.frame_budget_us() > 16000.0);
        REQUIRE(profiler.frame_budget_us() < 17000.0);
    }

    SECTION("initial frame stats are zero")
    {
        auto stats = profiler.frame_stats();
        REQUIRE(stats.frame_count == 0);
        REQUIRE(stats.avg_frame_us == 0.0);
        REQUIRE(stats.overbudget_count == 0);
    }
}

TEST_CASE("PerformanceProfiler startup checkpoints", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    SECTION("named checkpoints are recorded")
    {
        profiler.checkpoint("config_loaded");
        REQUIRE(profiler.checkpoint_count() == 1);

        profiler.checkpoint("theme_init");
        REQUIRE(profiler.checkpoint_count() == 2);
    }

    SECTION("phase enum checkpoints are recorded")
    {
        profiler.checkpoint(StartupPhase::LoggerInit);
        profiler.checkpoint(StartupPhase::ConfigLoad);
        REQUIRE(profiler.checkpoint_count() == 2);
    }

    SECTION("startup elapsed increases over time")
    {
        auto t1 = profiler.startup_elapsed_ms();
        std::this_thread::sleep_for(std::chrono::milliseconds(5));
        auto t2 = profiler.startup_elapsed_ms();
        REQUIRE(t2 > t1);
    }
}

TEST_CASE("PerformanceProfiler frame timing", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    SECTION("single frame records timing")
    {
        profiler.begin_frame();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        profiler.end_frame();

        auto stats = profiler.frame_stats();
        REQUIRE(stats.frame_count == 1);
        REQUIRE(stats.avg_frame_us > 0.0);
    }

    SECTION("multiple frames compute rolling average")
    {
        for (int i = 0; i < 10; ++i)
        {
            profiler.begin_frame();
            profiler.end_frame();
        }

        auto stats = profiler.frame_stats();
        REQUIRE(stats.frame_count == 10);
        REQUIRE(stats.min_frame_us <= stats.avg_frame_us);
        REQUIRE(stats.avg_frame_us <= stats.max_frame_us);
        REQUIRE(stats.p95_frame_us >= stats.avg_frame_us);
    }

    SECTION("overbudget frames are counted")
    {
        profiler.set_frame_budget(0.001); // impossibly small budget
        profiler.begin_frame();
        std::this_thread::sleep_for(std::chrono::microseconds(100));
        profiler.end_frame();

        auto stats = profiler.frame_stats();
        REQUIRE(stats.overbudget_count >= 1);
    }

    SECTION("frame budget can be changed")
    {
        profiler.set_frame_budget(8333.0);
        REQUIRE(profiler.frame_budget_us() == 8333.0);
    }
}

TEST_CASE("PerformanceProfiler profile capture", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    SECTION("capture begins and ends")
    {
        profiler.begin_profile_capture(5);
        REQUIRE(profiler.is_profiling() == true);
        REQUIRE(profiler.profile_capture_complete() == false);

        for (int i = 0; i < 5; ++i)
        {
            profiler.begin_frame();
            profiler.end_frame();
        }

        REQUIRE(profiler.is_profiling() == false);
        REQUIRE(profiler.profile_capture_complete() == true);
    }
}

TEST_CASE("PerformanceProfiler adaptive throttle", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    SECTION("default mode is idle")
    {
        REQUIRE(profiler.activity_mode() == ActivityMode::Idle);
    }

    SECTION("activity updates switch mode")
    {
        profiler.update_activity();
        REQUIRE(profiler.activity_mode() == ActivityMode::Typing);
    }

    SECTION("throttle budget is nonzero")
    {
        REQUIRE(profiler.throttle_budget().count() > 0);
    }
}

TEST_CASE("PerformanceProfiler snapshot", "[phase30][profiler]")
{
    PerformanceProfiler profiler;

    profiler.checkpoint("test");
    profiler.begin_frame();
    profiler.end_frame();

    auto snap = profiler.take_snapshot();
    REQUIRE(snap.frame_stats.frame_count == 1);
    REQUIRE(snap.timestamp_ms > 0);
    REQUIRE(snap.watchdog_stall_count == 0);
}

TEST_CASE("PerformanceProfiler JSON export", "[phase30][profiler]")
{
    PerformanceProfiler profiler;
    profiler.begin_frame();
    profiler.end_frame();

    auto json = profiler.to_json();
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("timestamp_ms"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("frame_stats"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("activity_mode"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("frame_count"));
}

// ============================================================================
// MemoryBudgetEnforcer Tests
// ============================================================================

TEST_CASE("MemoryBudgetEnforcer construction", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;

    SECTION("default budgets match PRD")
    {
        REQUIRE(enforcer.budget(MemorySubsystem::kEditor) == 50ULL * 1024 * 1024);
        REQUIRE(enforcer.budget(MemorySubsystem::kCanvas) == 200ULL * 1024 * 1024);
        REQUIRE(enforcer.budget(MemorySubsystem::kSearchIndex) == 100ULL * 1024 * 1024);
        REQUIRE(enforcer.budget(MemorySubsystem::kExtensions) == 50ULL * 1024 * 1024);
        REQUIRE(enforcer.budget(MemorySubsystem::kGeneral) == 100ULL * 1024 * 1024);
    }

    SECTION("total budget is 500MB")
    {
        REQUIRE(enforcer.total_budget() == 500ULL * 1024 * 1024);
    }

    SECTION("initial usage is zero")
    {
        REQUIRE(enforcer.total_usage() == 0);
        REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 0);
        REQUIRE(enforcer.usage_percent(MemorySubsystem::kEditor) == 0.0);
    }
}

TEST_CASE("MemoryBudgetEnforcer allocation tracking", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;

    SECTION("allocate increases usage")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 1024 * 1024); // 1MB
        REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 1024 * 1024);
        REQUIRE(enforcer.total_usage() == 1024 * 1024);
    }

    SECTION("release decreases usage")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 2048);
        enforcer.release(MemorySubsystem::kEditor, 1024);
        REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 1024);
    }

    SECTION("release does not underflow")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 100);
        enforcer.release(MemorySubsystem::kEditor, 200);
        REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 0);
    }

    SECTION("subsystems are independent")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 1000);
        enforcer.allocate(MemorySubsystem::kCanvas, 2000);
        REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 1000);
        REQUIRE(enforcer.usage(MemorySubsystem::kCanvas) == 2000);
        REQUIRE(enforcer.total_usage() == 3000);
    }
}

TEST_CASE("MemoryBudgetEnforcer severity thresholds", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;
    enforcer.set_budget(MemorySubsystem::kEditor, 1000);

    SECTION("normal at < 80%")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 500); // 50%
        REQUIRE(enforcer.severity(MemorySubsystem::kEditor) == BudgetSeverity::kNormal);
    }

    SECTION("warning at >= 80%")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 850); // 85%
        REQUIRE(enforcer.severity(MemorySubsystem::kEditor) == BudgetSeverity::kWarning);
    }

    SECTION("critical at >= 95%")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 960); // 96%
        REQUIRE(enforcer.severity(MemorySubsystem::kEditor) == BudgetSeverity::kCritical);
    }

    SECTION("usage percent is correct")
    {
        enforcer.allocate(MemorySubsystem::kEditor, 500);
        REQUIRE(enforcer.usage_percent(MemorySubsystem::kEditor) == 0.5);
    }
}

TEST_CASE("MemoryBudgetEnforcer custom budgets", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;

    enforcer.set_budget(MemorySubsystem::kGeneral, 256);
    REQUIRE(enforcer.budget(MemorySubsystem::kGeneral) == 256);
}

TEST_CASE("MemoryBudgetEnforcer snapshot", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;
    enforcer.allocate(MemorySubsystem::kEditor, 1024);
    enforcer.allocate(MemorySubsystem::kCanvas, 2048);

    auto snap = enforcer.snapshot();
    REQUIRE(snap.total_usage == 3072);
    REQUIRE(snap.timestamp_ms > 0);
    REQUIRE(snap.subsystems.at(0).subsystem == MemorySubsystem::kEditor);
    REQUIRE(snap.subsystems.at(0).usage_bytes == 1024);
}

TEST_CASE("MemoryBudgetEnforcer JSON export", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;
    enforcer.allocate(MemorySubsystem::kEditor, 1024);

    auto json = enforcer.to_json();
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("timestamp_ms"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("subsystems"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("Editor"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("usage_bytes"));
}

TEST_CASE("MemoryBudgetEnforcer reset", "[phase30][memory]")
{
    MemoryBudgetEnforcer enforcer;
    enforcer.allocate(MemorySubsystem::kEditor, 1024);
    enforcer.allocate(MemorySubsystem::kCanvas, 2048);

    enforcer.reset();
    REQUIRE(enforcer.total_usage() == 0);
    REQUIRE(enforcer.usage(MemorySubsystem::kEditor) == 0);
    REQUIRE(enforcer.usage(MemorySubsystem::kCanvas) == 0);
}

TEST_CASE("MemoryBudgetEnforcer subsystem names", "[phase30][memory]")
{
    REQUIRE(subsystem_name(MemorySubsystem::kEditor) == "Editor");
    REQUIRE(subsystem_name(MemorySubsystem::kCanvas) == "Canvas");
    REQUIRE(subsystem_name(MemorySubsystem::kSearchIndex) == "SearchIndex");
    REQUIRE(subsystem_name(MemorySubsystem::kExtensions) == "Extensions");
    REQUIRE(subsystem_name(MemorySubsystem::kGeneral) == "General");
}

// ============================================================================
// IncrementalParsePolicy Tests
// ============================================================================

TEST_CASE("IncrementalParsePolicy block boundary detection", "[phase30][parse]")
{
    SECTION("code fence detection")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("```") == BlockBoundaryKind::kCodeFence);
        REQUIRE(IncrementalParsePolicy::detect_boundary("```cpp") == BlockBoundaryKind::kCodeFence);
        REQUIRE(IncrementalParsePolicy::detect_boundary("~~~") == BlockBoundaryKind::kCodeFence);
        REQUIRE(IncrementalParsePolicy::detect_boundary("   ```") == BlockBoundaryKind::kCodeFence);
    }

    SECTION("front matter / thematic break")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("---") == BlockBoundaryKind::kFrontMatter);
        REQUIRE(IncrementalParsePolicy::detect_boundary("***") ==
                BlockBoundaryKind::kThematicBreak);
        REQUIRE(IncrementalParsePolicy::detect_boundary("___") ==
                BlockBoundaryKind::kThematicBreak);
    }

    SECTION("math block")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("$$") == BlockBoundaryKind::kMathBlock);
        REQUIRE(IncrementalParsePolicy::detect_boundary("$$\\sum_{i=0}") ==
                BlockBoundaryKind::kMathBlock);
    }

    SECTION("HTML block")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("<div>") == BlockBoundaryKind::kHTMLBlock);
        REQUIRE(IncrementalParsePolicy::detect_boundary("<table>") ==
                BlockBoundaryKind::kHTMLBlock);
        REQUIRE(IncrementalParsePolicy::detect_boundary("</div>") == BlockBoundaryKind::kHTMLBlock);
    }

    SECTION("block quote")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("> quote") ==
                BlockBoundaryKind::kBlockQuote);
    }

    SECTION("list items")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("- item") == BlockBoundaryKind::kListItem);
        REQUIRE(IncrementalParsePolicy::detect_boundary("* item") == BlockBoundaryKind::kListItem);
        REQUIRE(IncrementalParsePolicy::detect_boundary("1. item") == BlockBoundaryKind::kListItem);
    }

    SECTION("regular text is none")
    {
        REQUIRE(IncrementalParsePolicy::detect_boundary("Hello world") == BlockBoundaryKind::kNone);
        REQUIRE(IncrementalParsePolicy::detect_boundary("") == BlockBoundaryKind::kNone);
    }
}

TEST_CASE("IncrementalParsePolicy single-line edit", "[phase30][parse]")
{
    IncrementalParsePolicy policy;
    std::vector<std::string> lines = {
        "# Heading",
        "Normal text",
        "More text",
        "Final line",
    };

    EditRegion edit{1, 1, 0, 0}; // Edit line 1
    auto range = policy.compute_reparse_range(edit, 4, lines);

    REQUIRE(range.is_full_reparse == false);
    REQUIRE(range.line_count() <= 5); // ±2 context = at most 5 lines
    REQUIRE(range.start_line >= 0);
    REQUIRE(range.end_line <= 3);
}

TEST_CASE("IncrementalParsePolicy multi-line edit", "[phase30][parse]")
{
    IncrementalParsePolicy policy;
    std::vector<std::string> lines = {
        "Line 0",
        "Line 1",
        "Line 2",
        "Line 3",
        "Line 4",
        "Line 5",
        "Line 6",
        "Line 7",
    };

    EditRegion edit{2, 4, 0, 0}; // Edit lines 2-4
    auto range = policy.compute_reparse_range(edit, 8, lines);

    REQUIRE(range.is_full_reparse == false);
    REQUIRE(range.start_line <= 2);
    REQUIRE(range.end_line >= 4);
}

TEST_CASE("IncrementalParsePolicy code fence triggers full reparse", "[phase30][parse]")
{
    IncrementalParsePolicy policy;
    std::vector<std::string> lines = {
        "# Heading",
        "```", // opening fence (unclosed in edit region)
        "some code",
        "more code",
    };

    EditRegion edit{1, 1, 1, 0}; // Inserted the code fence line
    REQUIRE(policy.needs_full_reparse(edit, lines) == true);
}

TEST_CASE("IncrementalParsePolicy context lines", "[phase30][parse]")
{
    IncrementalParsePolicy policy;

    SECTION("default context is 2")
    {
        REQUIRE(policy.context_lines() == 2);
    }

    SECTION("context can be changed")
    {
        policy.set_context_lines(5);
        REQUIRE(policy.context_lines() == 5);
    }

    SECTION("context is clamped to max")
    {
        policy.set_context_lines(100);
        REQUIRE(policy.context_lines() == 10);
    }

    SECTION("context is clamped to zero")
    {
        policy.set_context_lines(-5);
        REQUIRE(policy.context_lines() == 0);
    }
}

TEST_CASE("IncrementalParsePolicy large deletion triggers full reparse", "[phase30][parse]")
{
    IncrementalParsePolicy policy;
    std::vector<std::string> lines = {"remaining line"};

    EditRegion edit{0, 0, 0, 100}; // Deleted 100 lines
    REQUIRE(policy.needs_full_reparse(edit, lines) == true);
}

TEST_CASE("IncrementalParsePolicy empty document", "[phase30][parse]")
{
    IncrementalParsePolicy policy;
    std::vector<std::string> lines;

    EditRegion edit{0, 0, 0, 0};
    auto range = policy.compute_reparse_range(edit, 0, lines);
    REQUIRE(range.is_full_reparse == true);
}

// ============================================================================
// PerformanceCommandProvider Tests
// ============================================================================

TEST_CASE("PerformanceCommandProvider construction", "[phase30][commands]")
{
    PerformanceCommandProvider provider;

    SECTION("has 7 commands")
    {
        REQUIRE(provider.command_count() == 7);
    }

    SECTION("all commands have IDs")
    {
        for (const auto& cmd : provider.commands())
        {
            REQUIRE_FALSE(cmd.id.empty());
            REQUIRE_FALSE(cmd.title.empty());
            REQUIRE_FALSE(cmd.category.empty());
        }
    }
}

TEST_CASE("PerformanceCommandProvider find command", "[phase30][commands]")
{
    PerformanceCommandProvider provider;

    SECTION("find existing command")
    {
        auto cmd = provider.find_command("perf.showMetrics");
        REQUIRE(cmd.has_value());
        REQUIRE(cmd->title == "Performance: Show Metrics");
    }

    SECTION("find nonexistent command returns nullopt")
    {
        auto cmd = provider.find_command("perf.nonexistent");
        REQUIRE_FALSE(cmd.has_value());
    }
}

TEST_CASE("PerformanceCommandProvider categories", "[phase30][commands]")
{
    PerformanceCommandProvider provider;

    auto cats = provider.categories();
    REQUIRE(cats.size() == 2);

    auto perf_cmds = provider.commands_in_category("Performance");
    REQUIRE(perf_cmds.size() == 5);

    auto diag_cmds = provider.commands_in_category("Performance Diagnostics");
    REQUIRE(diag_cmds.size() == 2);
}

TEST_CASE("PerformanceCommandProvider command IDs", "[phase30][commands]")
{
    PerformanceCommandProvider provider;

    // Verify all expected command IDs exist
    REQUIRE(provider.find_command("perf.showMetrics").has_value());
    REQUIRE(provider.find_command("perf.toggleOverlay").has_value());
    REQUIRE(provider.find_command("perf.reportStartup").has_value());
    REQUIRE(provider.find_command("perf.profileFrames").has_value());
    REQUIRE(provider.find_command("perf.memoryReport").has_value());
    REQUIRE(provider.find_command("perf.resetCounters").has_value());
    REQUIRE(provider.find_command("perf.toggleThrottle").has_value());
}

TEST_CASE("PerformanceCommandProvider available_commands", "[phase30][commands]")
{
    PerformanceCommandProvider provider;
    auto all = provider.available_commands();
    REQUIRE(all.size() == 7);
}

// ============================================================================
// Phase 30 Events Tests
// ============================================================================

TEST_CASE("PerfStartupCompletedEvent construction", "[phase30][events]")
{
    PerfStartupCompletedEvent event;
    event.startup_ms = 250.0;
    event.phase_count = 39;
    event.slowest_phase = "PluginDiscovery";

    REQUIRE(event.startup_ms == 250.0);
    REQUIRE(event.phase_count == 39);
    REQUIRE(event.slowest_phase == "PluginDiscovery");
}

TEST_CASE("PerfFrameOverbudgetEvent construction", "[phase30][events]")
{
    PerfFrameOverbudgetEvent event;
    event.frame_time_us = 20000.0;
    event.budget_us = 16667.0;
    event.frame_number = 42;

    REQUIRE(event.frame_time_us == 20000.0);
    REQUIRE(event.budget_us == 16667.0);
    REQUIRE(event.frame_number == 42);
}

TEST_CASE("PerfMemoryBudgetExceededEvent construction", "[phase30][events]")
{
    PerfMemoryBudgetExceededEvent event;
    event.subsystem = "Canvas";
    event.usage_bytes = 190 * 1024 * 1024;
    event.budget_bytes = 200 * 1024 * 1024;
    event.usage_percent = 0.95;

    REQUIRE(event.subsystem == "Canvas");
    REQUIRE(event.usage_percent == 0.95);
}

TEST_CASE("PerfProfileCapturedEvent construction", "[phase30][events]")
{
    PerfProfileCapturedEvent event;
    event.frame_count = 60;
    event.avg_frame_us = 14500.0;
    event.p95_frame_us = 16200.0;
    event.min_frame_us = 12000.0;
    event.max_frame_us = 18500.0;

    REQUIRE(event.frame_count == 60);
    REQUIRE(event.avg_frame_us == 14500.0);
    REQUIRE(event.p95_frame_us == 16200.0);
}

TEST_CASE("PerfShowMetricsEvent construction", "[phase30][events]")
{
    PerfShowMetricsEvent event;
    // A no-field event — just verify it can be constructed
    REQUIRE(sizeof(event) > 0);
}

// ============================================================================
// Existing Infrastructure Integration Tests
// ============================================================================

TEST_CASE("LazyService basic behavior", "[phase30][infra]")
{
    LazyService<std::string> service([]() { return std::make_unique<std::string>("initialized"); });

    REQUIRE(service.is_created() == false);
    auto* val = service.get_or_create();
    REQUIRE(val != nullptr);
    REQUIRE(*val == "initialized");
    REQUIRE(service.is_created() == true);
}

TEST_CASE("AdaptiveThrottle mode switching", "[phase30][infra]")
{
    AdaptiveThrottle throttle;

    REQUIRE(throttle.current_mode() == ActivityMode::Idle);
    throttle.update_activity();
    REQUIRE(throttle.current_mode() == ActivityMode::Typing);
}

TEST_CASE("StartupTimer checkpoints", "[phase30][infra]")
{
    StartupTimer timer;
    timer.checkpoint("a");
    timer.checkpoint("b");

    REQUIRE(timer.checkpoint_count() == 2);
    REQUIRE(timer.elapsed_ms() >= 0.0);

    auto json = timer.to_json();
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("a"));
    REQUIRE_THAT(json, Catch::Matchers::ContainsSubstring("b"));
}

TEST_CASE("SPSCQueue basic operations", "[phase30][infra]")
{
    SPSCQueue<int, 16> queue;
    int val = 0;

    REQUIRE(queue.try_pop(val) == false);
    REQUIRE(queue.try_push(42) == true);
    REQUIRE(queue.try_pop(val) == true);
    REQUIRE(val == 42);
}

TEST_CASE("DirtyRegionAccumulator coalescing", "[phase30][infra]")
{
    DirtyRegionAccumulator acc;

    REQUIRE(acc.has_dirty() == false);
    acc.invalidate(Rect{0, 0, 100, 100});
    REQUIRE(acc.has_dirty() == true);

    auto rects = acc.consume();
    REQUIRE(rects.size() >= 1);
    REQUIRE(acc.has_dirty() == false);
}
