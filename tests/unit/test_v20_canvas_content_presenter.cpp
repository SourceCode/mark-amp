/// @file test_v20_canvas_content_presenter.cpp
/// @brief V20 Phase 08 – CanvasContentPresenter unit tests.

#include "core/CanvasContentPresenter.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("CanvasPresenter: default styles registered", "[v20][canvas-presenter]")
{
    EventBus bus;
    CanvasContentPresenter presenter(bus);

    REQUIRE(presenter.style_count() >= 7);
    REQUIRE(presenter.style("sticky-note") != nullptr);
    REQUIRE(presenter.style("code-block") != nullptr);
    REQUIRE(presenter.style("frame") != nullptr);
}

TEST_CASE("CanvasPresenter: style lookup", "[v20][canvas-presenter]")
{
    EventBus bus;
    CanvasContentPresenter presenter(bus);

    auto* sn = presenter.style("sticky-note");
    REQUIRE(sn != nullptr);
    REQUIRE(sn->has_fill());
    REQUIRE(sn->has_border());
    REQUIRE(sn->opacity == 1.0);
}

TEST_CASE("CanvasPresenter: custom style registration", "[v20][canvas-presenter]")
{
    EventBus bus;
    CanvasContentPresenter presenter(bus);

    int initial = presenter.style_count();

    CanvasObjectStyle custom;
    custom.style_id = "custom-widget";
    custom.border_color = "#ff0000";
    custom.fill_color = "#ffffff";
    custom.text_color = "#000000";
    presenter.register_style(custom);

    REQUIRE(presenter.style_count() == initial + 1);
    REQUIRE(presenter.style("custom-widget") != nullptr);
}

TEST_CASE("CanvasPresenter: visual state tracking", "[v20][canvas-presenter]")
{
    EventBus bus;
    CanvasContentPresenter presenter(bus);

    REQUIRE(presenter.object_visual_state("obj-1") == CanvasObjectVisualState::kNormal);

    presenter.set_object_visual_state("obj-1", CanvasObjectVisualState::kSelected);
    REQUIRE(presenter.object_visual_state("obj-1") == CanvasObjectVisualState::kSelected);
    REQUIRE(presenter.update_count() > 0);
}

TEST_CASE("CanvasPresenter: embedded content types", "[v20][canvas-presenter]")
{
    EventBus bus;
    CanvasContentPresenter presenter(bus);

    REQUIRE(presenter.embedded_content_type("obj-1") == EmbeddedContentType::kPlainText);

    presenter.register_embedded_content("obj-1", EmbeddedContentType::kCodeBlock);
    REQUIRE(presenter.embedded_content_type("obj-1") == EmbeddedContentType::kCodeBlock);
}
