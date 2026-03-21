/// @file test_v20_rendering_integration.cpp
/// @brief V20 Phase 08 – Rendering integration tests.

#include "core/ContentStyleSystem.h"
#include "core/NotebookCellChrome.h"
#include "core/CanvasContentPresenter.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("RenderInteg: content style drives consistent feedback", "[v20][render-integ]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    // All feedback styles should be defined
    auto error = css.feedback_style("feedback.error");
    auto warning = css.feedback_style("feedback.warning");
    auto search = css.feedback_style("feedback.searchHit");
    REQUIRE(error != nullptr);
    REQUIRE(warning != nullptr);
    REQUIRE(search != nullptr);

    // Styles are distinct
    REQUIRE(error->foreground != warning->foreground);
    REQUIRE(search->is_highlight);
    REQUIRE(error->is_underline);
}

TEST_CASE("RenderInteg: notebook cells use shared spacing", "[v20][render-integ]")
{
    EventBus bus;
    ContentStyleSystem css(bus);
    NotebookCellChrome chrome(bus);

    // Spacing tokens exist for cell layout
    REQUIRE(css.spacing("spacing.sm") != nullptr);
    REQUIRE(css.spacing("spacing.md") != nullptr);

    // Cell chrome tracks state correctly
    CellChromeState state;
    state.cell_id = "cell-1";
    state.cell_type = CellChromeType::kCode;
    chrome.set_cell_state("cell-1", state);
    REQUIRE(chrome.cell_state("cell-1")->needs_prompt());
}

TEST_CASE("RenderInteg: canvas uses shared typography", "[v20][render-integ]")
{
    EventBus bus;
    ContentStyleSystem css(bus);
    CanvasContentPresenter presenter(bus);

    // Typography tokens exist for canvas text rendering
    REQUIRE(css.typography("type.body") != nullptr);
    REQUIRE(css.typography("type.code") != nullptr);

    // Canvas code-block style exists
    auto* code_style = presenter.style("code-block");
    REQUIRE(code_style != nullptr);
    REQUIRE(code_style->has_fill());
}

TEST_CASE("RenderInteg: multi-surface event flow", "[v20][render-integ]")
{
    EventBus bus;
    int feedback_events = 0;
    int chrome_events = 0;
    int visual_events = 0;

    auto sub1 = bus.subscribe<events::FeedbackStyleRegisteredEvent>(
        [&](const events::FeedbackStyleRegisteredEvent&) { ++feedback_events; });
    auto sub2 = bus.subscribe<events::CellChromeChangedEvent>(
        [&](const events::CellChromeChangedEvent&) { ++chrome_events; });
    auto sub3 = bus.subscribe<events::CanvasObjectVisualStateChangedEvent>(
        [&](const events::CanvasObjectVisualStateChangedEvent&) { ++visual_events; });

    ContentStyleSystem css(bus);
    NotebookCellChrome chrome(bus);
    CanvasContentPresenter presenter(bus);

    // Feedback events from CSS defaults
    REQUIRE(feedback_events >= 6);

    // Cell chrome event
    CellChromeState state;
    state.cell_id = "cell-1";
    chrome.set_cell_state("cell-1", state);
    chrome.set_active_cell("cell-1");
    REQUIRE(chrome_events == 1);

    // Canvas visual state event
    presenter.set_object_visual_state("obj-1", CanvasObjectVisualState::kSelected);
    REQUIRE(visual_events == 1);
}
