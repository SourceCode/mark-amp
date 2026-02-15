/// @file test_presentation.cpp
/// @brief V4 Phase 27 – Presentation Engine unit tests.

#include "core/EventBus.h"
#include "ui/PresentationEngine.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

struct PresentationFixture
{
    EventBus event_bus;
    PresentationEngine engine{event_bus};
};

TEST_CASE("PresentationEngine: parse slides by separator", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "# Slide 1\nContent 1\n---\n# Slide 2\nContent 2\n---\n# Slide "
                           "3\nContent 3\n---\nFinal content";

    fixture.engine.load_document(markdown);
    REQUIRE(fixture.engine.slide_count() == 4);
}

TEST_CASE("PresentationEngine: parse slides by heading", "[presentation]")
{
    PresentationFixture fixture;
    PresentationConfig config;
    config.use_heading_breaks = true;
    config.heading_break_level = 1;
    config.slide_separator = "---";

    std::string markdown =
        "# First Slide\nContent 1\n# Second Slide\nContent 2\n# Third Slide\nContent 3";
    fixture.engine.load_document(markdown, config);
    REQUIRE(fixture.engine.slide_count() == 3);
}

TEST_CASE("PresentationEngine: slide title extraction", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "# My Title\nSome body text\n---\n## Sub heading\nMore content";
    fixture.engine.load_document(markdown);

    REQUIRE(fixture.engine.get_slide(0).title == "My Title");
    REQUIRE(fixture.engine.get_slide(1).title == "Sub heading");
}

TEST_CASE("PresentationEngine: speaker notes extraction", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "# Slide 1\nContent\n<!-- notes -->\nThese are speaker notes\n---\n# "
                           "Slide 2\nNo notes here";

    fixture.engine.load_document(markdown);
    REQUIRE(fixture.engine.slide_count() == 2);
    REQUIRE(fixture.engine.get_slide(0).speaker_notes == "These are speaker notes");
    REQUIRE(fixture.engine.get_slide(1).speaker_notes.empty());
}

TEST_CASE("PresentationEngine: next/previous navigation", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "Slide 0\n---\nSlide 1\n---\nSlide 2";
    fixture.engine.load_document(markdown);

    REQUIRE(fixture.engine.current_slide() == 0);
    fixture.engine.next_slide();
    REQUIRE(fixture.engine.current_slide() == 1);
    fixture.engine.previous_slide();
    REQUIRE(fixture.engine.current_slide() == 0);
}

TEST_CASE("PresentationEngine: bounds checking", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "Slide 0\n---\nSlide 1\n---\nSlide 2";
    fixture.engine.load_document(markdown);

    // Previous at start stays at 0.
    fixture.engine.previous_slide();
    REQUIRE(fixture.engine.current_slide() == 0);

    // Go to last slide.
    fixture.engine.go_to_slide(2);
    REQUIRE(fixture.engine.current_slide() == 2);

    // Next at end stays at last.
    fixture.engine.next_slide();
    REQUIRE(fixture.engine.current_slide() == 2);
}

TEST_CASE("PresentationEngine: slide count", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "A\n---\nB\n---\nC\n---\nD\n---\nE";
    fixture.engine.load_document(markdown);
    REQUIRE(fixture.engine.slide_count() == 5);
}

TEST_CASE("PresentationEngine: go_to_slide", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "S0\n---\nS1\n---\nS2\n---\nS3";
    fixture.engine.load_document(markdown);

    fixture.engine.go_to_slide(2);
    REQUIRE(fixture.engine.current_slide() == 2);

    // Out of bounds clamped.
    fixture.engine.go_to_slide(100);
    REQUIRE(fixture.engine.current_slide() == 3);

    fixture.engine.go_to_slide(-5);
    REQUIRE(fixture.engine.current_slide() == 0);
}

TEST_CASE("PresentationEngine: progress calculation", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "S0\n---\nS1\n---\nS2\n---\nS3";
    fixture.engine.load_document(markdown);

    // At slide 0 of 4: progress = 0/3 = 0.0
    REQUIRE(fixture.engine.progress() == 0.0);

    fixture.engine.go_to_slide(3);
    // At slide 3 of 4: progress = 3/3 = 1.0
    REQUIRE(fixture.engine.progress() == 1.0);

    fixture.engine.go_to_slide(1);
    // At slide 1 of 4: progress ≈ 0.333
    REQUIRE(fixture.engine.progress() > 0.3);
    REQUIRE(fixture.engine.progress() < 0.4);
}

TEST_CASE("PresentationEngine: start/stop", "[presentation]")
{
    PresentationFixture fixture;
    std::string markdown = "S0\n---\nS1";
    fixture.engine.load_document(markdown);

    REQUIRE_FALSE(fixture.engine.is_presenting());

    fixture.engine.start();
    REQUIRE(fixture.engine.is_presenting());
    REQUIRE(fixture.engine.current_slide() == 0);

    fixture.engine.stop();
    REQUIRE_FALSE(fixture.engine.is_presenting());
}
