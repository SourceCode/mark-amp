/// @file test_v20_highlight_degradation.cpp
/// @brief V20 Phase 07 – HighlightDegradation unit tests.

#include "core/HighlightDegradation.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("HighlightDegradation: construction", "[v20][highlight]")
{
    HighlightDegradationService service;
    REQUIRE(service.pass_count() == 0);
}

TEST_CASE("HighlightDegradation: determine_mode", "[v20][highlight]")
{
    HighlightDegradationService service;

    REQUIRE(service.determine_mode(500) == DegradationMode::kFull);
    REQUIRE(service.determine_mode(10000) == DegradationMode::kFull);
    REQUIRE(service.determine_mode(15000) == DegradationMode::kViewport);
    REQUIRE(service.determine_mode(30000) == DegradationMode::kBudgeted);
    REQUIRE(service.determine_mode(80000) == DegradationMode::kMinimal);
    REQUIRE(service.determine_mode(150000) == DegradationMode::kNone);
}

TEST_CASE("HighlightDegradation: set_budget", "[v20][highlight]")
{
    HighlightDegradationService service;

    HighlightBudget budget;
    budget.max_lines_per_pass = 1000;
    budget.total_line_threshold = 5000;
    budget.max_time_ms = 8.0;
    service.set_budget(budget);

    REQUIRE(service.budget().max_lines_per_pass == 1000);
    REQUIRE(service.budget().total_line_threshold == 5000);

    // With custom threshold
    REQUIRE(service.determine_mode(4000) == DegradationMode::kFull);
    REQUIRE(service.determine_mode(6000) == DegradationMode::kViewport);
}

TEST_CASE("HighlightDegradation: record_pass", "[v20][highlight]")
{
    HighlightDegradationService service;

    service.record_pass("doc-1", 500, 12.5);
    REQUIRE(service.pass_count() == 1);

    auto st = service.state("doc-1");
    REQUIRE(st.highlighted_lines == 500);
    REQUIRE(st.last_pass_ms == 12.5);
}

TEST_CASE("HighlightDegradation: multiple passes accumulate", "[v20][highlight]")
{
    HighlightDegradationService service;

    service.record_pass("doc-1", 500, 10.0);
    service.record_pass("doc-1", 300, 8.0);

    auto st = service.state("doc-1");
    REQUIRE(st.highlighted_lines == 800);
    REQUIRE(st.last_pass_ms == 8.0);
}

TEST_CASE("HighlightDegradation: unknown document state", "[v20][highlight]")
{
    HighlightDegradationService service;
    auto st = service.state("nonexistent");
    REQUIRE(st.mode == DegradationMode::kFull);
    REQUIRE(st.highlighted_lines == 0);
}

TEST_CASE("HighlightDegradation: should_share_tokens", "[v20][highlight]")
{
    HighlightDegradationService service;

    REQUIRE(service.should_share_tokens(HighlightSurface::kEditor));
    REQUIRE(service.should_share_tokens(HighlightSurface::kNotebookCell));
    REQUIRE(service.should_share_tokens(HighlightSurface::kMarkdownFence));
    REQUIRE(service.should_share_tokens(HighlightSurface::kDiffPanel));
    REQUIRE(service.should_share_tokens(HighlightSurface::kCanvasEmbed));
}

TEST_CASE("HighlightDegradation: shared_surfaces", "[v20][highlight]")
{
    HighlightDegradationService service;
    auto surfaces = service.shared_surfaces();
    REQUIRE(surfaces.size() == 5);
}

TEST_CASE("HighlightState: coverage_percent", "[v20][highlight]")
{
    HighlightState st;
    st.total_lines = 1000;
    st.highlighted_lines = 750;
    REQUIRE(st.coverage_percent() == 75.0);
    REQUIRE_FALSE(st.is_degraded());

    st.mode = DegradationMode::kViewport;
    REQUIRE(st.is_degraded());
}

TEST_CASE("HighlightBudget: defaults", "[v20][highlight]")
{
    HighlightBudget budget;
    REQUIRE(budget.preserve_keywords);
    REQUIRE(budget.preserve_comments);
    REQUIRE(budget.preserve_strings);
}

TEST_CASE("Phase 07 events: HighlightDegradationChangedEvent", "[v20][highlight-events]")
{
    events::HighlightDegradationChangedEvent evt;
    evt.document_id = "doc-1";
    evt.mode = 2;
    evt.total_lines = 50000;
    REQUIRE(evt.total_lines == 50000);
}

TEST_CASE("Phase 07 events: CrossSurfaceHighlightEvent", "[v20][highlight-events]")
{
    events::CrossSurfaceHighlightEvent evt;
    evt.language_id = "python";
    evt.surface = "notebook";
    evt.shared_tokens = true;
    REQUIRE(evt.shared_tokens);
}
