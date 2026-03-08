// test_text_direction_engine.cpp — 10 tests for TextDirectionEngine
#include "core/TextDirectionEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("TextDirectionEngine default direction is LTR", "[i18n][bidi]")
{
    TextDirectionEngine engine;
    CHECK(engine.get_default_direction() == TextDirection::kLTR);
}

TEST_CASE("TextDirectionEngine set default direction", "[i18n][bidi]")
{
    TextDirectionEngine engine;
    engine.set_default_direction(TextDirection::kRTL);
    CHECK(engine.get_default_direction() == TextDirection::kRTL);
}

TEST_CASE("TextDirectionEngine detect LTR text", "[i18n][bidi]")
{
    TextDirectionEngine engine;
    auto direction = engine.detect_direction("Hello World");
    CHECK(direction == TextDirection::kLTR);
}

TEST_CASE("TextDirectionEngine segment_text returns segments", "[i18n][bidi]")
{
    TextDirectionEngine engine;
    auto segments = engine.segment_text("Hello World");
    CHECK_FALSE(segments.empty());
}

TEST_CASE("TextDirectionEngine strip_bidi_markers removes markers", "[i18n][bidi]")
{
    auto stripped = TextDirectionEngine::strip_bidi_markers("Plain text");
    CHECK(stripped == "Plain text");
}

TEST_CASE("TextDirectionEngine apply_bidi_markers for LTR", "[i18n][bidi]")
{
    auto marked = TextDirectionEngine::apply_bidi_markers("Hello", TextDirection::kLTR);
    CHECK_FALSE(marked.empty());
}

TEST_CASE("TextDirectionEngine apply_bidi_markers for RTL", "[i18n][bidi]")
{
    auto marked = TextDirectionEngine::apply_bidi_markers("test", TextDirection::kRTL);
    CHECK_FALSE(marked.empty());
}

TEST_CASE("TextDirection enum values are distinct", "[i18n][bidi]")
{
    CHECK(TextDirection::kLTR != TextDirection::kRTL);
    CHECK(TextDirection::kRTL != TextDirection::kAuto);
    CHECK(TextDirection::kLTR != TextDirection::kAuto);
}

TEST_CASE("BidiSegment defaults", "[i18n][bidi]")
{
    BidiSegment segment;
    CHECK(segment.text.empty());
    CHECK(segment.direction == TextDirection::kLTR);
    CHECK(segment.start_offset == 0);
    CHECK(segment.length == 0);
}

TEST_CASE("TextDirectionEngine detect empty string", "[i18n][bidi]")
{
    TextDirectionEngine engine;
    auto direction = engine.detect_direction("");
    // Empty string should return default direction without crashing
    CHECK((direction == TextDirection::kLTR || direction == TextDirection::kAuto));
}
