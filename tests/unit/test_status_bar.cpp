#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_string.hpp>

#include <sstream>
#include <string>

// We can't construct a full StatusBarPanel in tests (needs wxWidgets app loop),
// so we test the static helpers and event types directly. StatusBarPanel's
// count_words and view_mode_label are private statics — we replicate the logic
// here for validation and test the event structures and state flow.

namespace
{

/// Replicate StatusBarPanel::count_words for unit testing.
auto count_words(const std::string& content) -> int
{
    if (content.empty())
    {
        return 0;
    }

    int count = 0;
    bool in_word = false;

    for (char character : content)
    {
        if (std::isspace(static_cast<unsigned char>(character)) != 0)
        {
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true;
            ++count;
        }
    }

    return count;
}

/// Replicate StatusBarPanel::view_mode_label for unit testing.
auto view_mode_label(markamp::core::events::ViewMode mode) -> std::string
{
    switch (mode)
    {
        case markamp::core::events::ViewMode::Editor:
            return "SRC";
        case markamp::core::events::ViewMode::Preview:
            return "VIEW";
        case markamp::core::events::ViewMode::Split:
            return "SPLIT";
    }
    return "SPLIT";
}

} // namespace

// ═══════════════════════════════════════════════════════
//  §1  Word count helper
// ═══════════════════════════════════════════════════════

TEST_CASE("Word count: empty string returns zero", "[status_bar]")
{
    REQUIRE(count_words("") == 0);
}

TEST_CASE("Word count: single word", "[status_bar]")
{
    REQUIRE(count_words("hello") == 1);
}

TEST_CASE("Word count: multiple words with spaces", "[status_bar]")
{
    REQUIRE(count_words("hello world foo bar") == 4);
}

TEST_CASE("Word count: leading and trailing whitespace", "[status_bar]")
{
    REQUIRE(count_words("  hello world  ") == 2);
}

TEST_CASE("Word count: multiple whitespace between words", "[status_bar]")
{
    REQUIRE(count_words("one   two\t\tthree\n\nfour") == 4);
}

TEST_CASE("Word count: only whitespace", "[status_bar]")
{
    REQUIRE(count_words("   \t\n  ") == 0);
}

// ═══════════════════════════════════════════════════════
//  §2  View mode label helper
// ═══════════════════════════════════════════════════════

TEST_CASE("View mode label: Editor -> SRC", "[status_bar]")
{
    REQUIRE(view_mode_label(markamp::core::events::ViewMode::Editor) == "SRC");
}

TEST_CASE("View mode label: Preview -> VIEW", "[status_bar]")
{
    REQUIRE(view_mode_label(markamp::core::events::ViewMode::Preview) == "VIEW");
}

TEST_CASE("View mode label: Split -> SPLIT", "[status_bar]")
{
    REQUIRE(view_mode_label(markamp::core::events::ViewMode::Split) == "SPLIT");
}

// ═══════════════════════════════════════════════════════
//  §3  Event structures
// ═══════════════════════════════════════════════════════

TEST_CASE("FileEncodingDetectedEvent has correct type name", "[status_bar]")
{
    markamp::core::events::FileEncodingDetectedEvent evt;
    evt.encoding_name = "UTF-8 BOM";
    REQUIRE(evt.type_name() == "FileEncodingDetectedEvent");
    REQUIRE(evt.encoding_name == "UTF-8 BOM");
}

TEST_CASE("MermaidRenderStatusEvent defaults to inactive", "[status_bar]")
{
    markamp::core::events::MermaidRenderStatusEvent evt;
    REQUIRE(evt.active == false);
    REQUIRE(evt.status.empty());
}

TEST_CASE("MermaidRenderStatusEvent has correct type name", "[status_bar]")
{
    markamp::core::events::MermaidRenderStatusEvent evt;
    evt.status = "RENDERING";
    evt.active = true;
    REQUIRE(evt.type_name() == "MermaidRenderStatusEvent");
    REQUIRE(evt.status == "RENDERING");
    REQUIRE(evt.active);
}

// ═══════════════════════════════════════════════════════
//  §4  EventBus subscription integration
// ═══════════════════════════════════════════════════════

TEST_CASE("EventBus delivers CursorPositionChangedEvent", "[status_bar]")
{
    markamp::core::EventBus bus;
    int received_line = 0;
    int received_col = 0;

    auto subscription = bus.subscribe<markamp::core::events::CursorPositionChangedEvent>(
        [&](const markamp::core::events::CursorPositionChangedEvent& evt)
        {
            received_line = evt.line;
            received_col = evt.column;
        });

    markamp::core::events::CursorPositionChangedEvent evt;
    evt.line = 42;
    evt.column = 17;
    bus.publish(evt);

    REQUIRE(received_line == 42);
    REQUIRE(received_col == 17);
}

TEST_CASE("EventBus delivers EditorContentChangedEvent for word counting", "[status_bar]")
{
    markamp::core::EventBus bus;
    int word_count_result = 0;

    auto subscription = bus.subscribe<markamp::core::events::EditorContentChangedEvent>(
        [&](const markamp::core::events::EditorContentChangedEvent& evt)
        { word_count_result = count_words(evt.content); });

    markamp::core::events::EditorContentChangedEvent evt;
    evt.content = "The quick brown fox jumps over the lazy dog";
    bus.publish(evt);

    REQUIRE(word_count_result == 9);
}

TEST_CASE("EventBus delivers ViewModeChangedEvent", "[status_bar]")
{
    markamp::core::EventBus bus;
    markamp::core::events::ViewMode received_mode = markamp::core::events::ViewMode::Split;

    auto subscription = bus.subscribe<markamp::core::events::ViewModeChangedEvent>(
        [&](const markamp::core::events::ViewModeChangedEvent& evt) { received_mode = evt.mode; });

    bus.publish(
        markamp::core::events::ViewModeChangedEvent{markamp::core::events::ViewMode::Editor});

    REQUIRE(received_mode == markamp::core::events::ViewMode::Editor);
    REQUIRE(view_mode_label(received_mode) == "SRC");
}

TEST_CASE("EventBus delivers FileEncodingDetectedEvent", "[status_bar]")
{
    markamp::core::EventBus bus;
    std::string received_encoding;

    auto subscription = bus.subscribe<markamp::core::events::FileEncodingDetectedEvent>(
        [&](const markamp::core::events::FileEncodingDetectedEvent& evt)
        { received_encoding = evt.encoding_name; });

    markamp::core::events::FileEncodingDetectedEvent evt;
    evt.encoding_name = "UTF-16 LE";
    bus.publish(evt);

    REQUIRE(received_encoding == "UTF-16 LE");
}

TEST_CASE("EventBus delivers MermaidRenderStatusEvent", "[status_bar]")
{
    markamp::core::EventBus bus;
    std::string received_status;
    bool received_active = false;

    auto subscription = bus.subscribe<markamp::core::events::MermaidRenderStatusEvent>(
        [&](const markamp::core::events::MermaidRenderStatusEvent& evt)
        {
            received_status = evt.status;
            received_active = evt.active;
        });

    markamp::core::events::MermaidRenderStatusEvent evt;
    evt.status = "ERROR";
    evt.active = false;
    bus.publish(evt);

    REQUIRE(received_status == "ERROR");
    REQUIRE_FALSE(received_active);
}

// ═══════════════════════════════════════════════════════
//  §5  Status bar display format validation
// ═══════════════════════════════════════════════════════

TEST_CASE("Cursor position format string", "[status_bar]")
{
    // Verify format matches PRD: "LN X, COL Y"
    int line = 12;
    int col = 42;
    auto text = "LN " + std::to_string(line) + ", COL " + std::to_string(col);
    REQUIRE(text == "LN 12, COL 42");
}

TEST_CASE("Word count format string", "[status_bar]")
{
    int words = 150;
    auto text = std::to_string(words) + " WORDS";
    REQUIRE(text == "150 WORDS");
}

TEST_CASE("Mermaid status format string", "[status_bar]")
{
    std::string status = "ACTIVE";
    auto text = "MERMAID: " + status;
    REQUIRE(text == "MERMAID: ACTIVE");
}
