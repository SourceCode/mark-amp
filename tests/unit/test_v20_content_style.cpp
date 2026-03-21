/// @file test_v20_content_style.cpp
/// @brief V20 Phase 08 – ContentStyleSystem unit tests.

#include "core/ContentStyleSystem.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("ContentStyle: default tokens registered", "[v20][content-style]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    REQUIRE(css.total_tokens() > 0);
    REQUIRE(css.all_spacing().size() >= 7);
    REQUIRE(css.all_typography().size() >= 7);
    REQUIRE(css.all_feedback_styles().size() >= 6);
}

TEST_CASE("ContentStyle: spacing lookup", "[v20][content-style]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    auto sm = css.spacing("spacing.sm");
    REQUIRE(sm != nullptr);
    REQUIRE(sm->value_px == 8);
    REQUIRE(sm->scaled(2.0) == 16);

    REQUIRE(css.spacing("nonexistent") == nullptr);
}

TEST_CASE("ContentStyle: typography lookup", "[v20][content-style]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    auto body = css.typography("type.body");
    REQUIRE(body != nullptr);
    REQUIRE(body->font_size_px == 13);
    REQUIRE(body->line_height_px == 20);
    REQUIRE(body->line_height_ratio() > 1.0);

    auto code = css.typography("type.code");
    REQUIRE(code != nullptr);
    REQUIRE(code->font_role == FontRole::kMonospace);
}

TEST_CASE("ContentStyle: feedback style lookup", "[v20][content-style]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    auto error = css.feedback_style("feedback.error");
    REQUIRE(error != nullptr);
    REQUIRE(error->is_underline);

    auto selection = css.feedback_style("feedback.selection");
    REQUIRE(selection != nullptr);
    REQUIRE(selection->is_highlight);
}

TEST_CASE("ContentStyle: custom token registration", "[v20][content-style]")
{
    EventBus bus;
    ContentStyleSystem css(bus);

    int initial = css.total_tokens();

    SpacingToken custom;
    custom.name = "spacing.custom";
    custom.value_px = 42;
    css.register_spacing(custom);

    REQUIRE(css.total_tokens() == initial + 1);
    REQUIRE(css.spacing("spacing.custom")->value_px == 42);
}
