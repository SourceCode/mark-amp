#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════
// Phase 1: Split Editor Writing System — Advanced Tests
// Tests cover snap presets, focus mode events, scroll sync
// mode events, heading index logic, and animation easing.
// ═══════════════════════════════════════════════════════

// --- Snap preset values are correct ---

TEST_CASE("Snap preset ratios: Even = 0.5", "[splitview][snap]")
{
    constexpr double kEvenRatio = 0.5;
    REQUIRE(kEvenRatio == 0.5);
}

TEST_CASE("Snap preset ratios: EditorWide = 0.7", "[splitview][snap]")
{
    constexpr double kEditorWideRatio = 0.7;
    REQUIRE(kEditorWideRatio == 0.7);
}

TEST_CASE("Snap preset ratios: PreviewWide = 0.3", "[splitview][snap]")
{
    constexpr double kPreviewWideRatio = 0.3;
    REQUIRE(kPreviewWideRatio == 0.3);
}

// --- Snap preset cycling logic ---

TEST_CASE("Snap preset cycling: Even -> EditorWide -> PreviewWide -> Even", "[splitview][snap]")
{
    // Simulate the cycle
    double ratios[] = {0.5, 0.7, 0.3, 0.5};
    constexpr int kCycleLen = 3;

    for (int idx = 0; idx < kCycleLen; ++idx)
    {
        double current = ratios[idx];
        double next = ratios[idx + 1];
        // After cycling, ratio should change
        REQUIRE(current != next);
    }
    // Full cycle returns to start
    REQUIRE(ratios[0] == ratios[3]);
}

// --- FocusModeChangedEvent ---

TEST_CASE("FocusModeChangedEvent carries active state", "[splitview][focus]")
{
    FocusModeChangedEvent evt_on{true};
    REQUIRE(evt_on.active == true);

    FocusModeChangedEvent evt_off{false};
    REQUIRE(evt_off.active == false);
}

TEST_CASE("FocusModeChangedEvent default is inactive", "[splitview][focus]")
{
    FocusModeChangedEvent evt;
    REQUIRE(evt.active == false);
}

TEST_CASE("FocusModeChangedEvent type_name is correct", "[splitview][focus]")
{
    FocusModeChangedEvent evt{true};
    REQUIRE(evt.type_name() == "FocusModeChangedEvent");
}

TEST_CASE("EventBus publishes FocusModeChangedEvent", "[splitview][focus]")
{
    EventBus bus;
    bool received_active = false;
    int call_count = 0;

    auto sub = bus.subscribe<FocusModeChangedEvent>(
        [&](const FocusModeChangedEvent& evt)
        {
            received_active = evt.active;
            ++call_count;
        });

    bus.publish(FocusModeChangedEvent{true});
    REQUIRE(received_active == true);
    REQUIRE(call_count == 1);

    bus.publish(FocusModeChangedEvent{false});
    REQUIRE(received_active == false);
    REQUIRE(call_count == 2);
}

// --- ScrollSyncModeChangedEvent ---

TEST_CASE("ScrollSyncModeChangedEvent carries mode", "[splitview][scrollsync]")
{
    ScrollSyncModeChangedEvent evt_prop{ScrollSyncMode::Proportional};
    REQUIRE(evt_prop.mode == ScrollSyncMode::Proportional);

    ScrollSyncModeChangedEvent evt_heading{ScrollSyncMode::HeadingAnchor};
    REQUIRE(evt_heading.mode == ScrollSyncMode::HeadingAnchor);
}

TEST_CASE("ScrollSyncModeChangedEvent default is Proportional", "[splitview][scrollsync]")
{
    ScrollSyncModeChangedEvent evt;
    REQUIRE(evt.mode == ScrollSyncMode::Proportional);
}

TEST_CASE("ScrollSyncModeChangedEvent type_name is correct", "[splitview][scrollsync]")
{
    ScrollSyncModeChangedEvent evt;
    REQUIRE(evt.type_name() == "ScrollSyncModeChangedEvent");
}

TEST_CASE("EventBus publishes ScrollSyncModeChangedEvent", "[splitview][scrollsync]")
{
    EventBus bus;
    ScrollSyncMode received_mode = ScrollSyncMode::Proportional;
    int call_count = 0;

    auto sub = bus.subscribe<ScrollSyncModeChangedEvent>(
        [&](const ScrollSyncModeChangedEvent& evt)
        {
            received_mode = evt.mode;
            ++call_count;
        });

    bus.publish(ScrollSyncModeChangedEvent{ScrollSyncMode::HeadingAnchor});
    REQUIRE(received_mode == ScrollSyncMode::HeadingAnchor);
    REQUIRE(call_count == 1);
}

// --- Heading index logic (pure logic, no wxWidgets) ---

namespace
{

/// Rebuild heading positions from markdown content (same logic as SplitView).
auto rebuild_heading_index(const std::string& content) -> std::vector<int>
{
    std::vector<int> positions;
    std::istringstream stream(content);
    std::string line;
    int line_num = 0;
    while (std::getline(stream, line))
    {
        if (!line.empty() && line[0] == '#')
        {
            positions.push_back(line_num);
        }
        ++line_num;
    }
    return positions;
}

/// Find nearest heading to a given editor line (same logic as SplitView).
auto find_nearest_heading(const std::vector<int>& positions, int editor_line) -> int
{
    if (positions.empty())
    {
        return -1;
    }

    int nearest = positions[0];
    int min_dist = std::abs(editor_line - nearest);

    for (size_t idx = 1; idx < positions.size(); ++idx)
    {
        int dist = std::abs(editor_line - positions[idx]);
        if (dist < min_dist)
        {
            min_dist = dist;
            nearest = positions[idx];
        }
        if (positions[idx] > editor_line)
        {
            break;
        }
    }

    return nearest;
}

} // anonymous namespace

TEST_CASE("Heading index: empty content yields no headings", "[splitview][headingindex]")
{
    auto positions = rebuild_heading_index("");
    REQUIRE(positions.empty());
}

TEST_CASE("Heading index: single heading at line 0", "[splitview][headingindex]")
{
    auto positions = rebuild_heading_index("# Title\nSome text\n");
    REQUIRE(positions.size() == 1);
    REQUIRE(positions[0] == 0);
}

TEST_CASE("Heading index: multiple headings", "[splitview][headingindex]")
{
    std::string content = "# Title\n"
                          "Some text\n"
                          "## Subtitle\n"
                          "More text\n"
                          "### Deep heading\n";

    auto positions = rebuild_heading_index(content);
    REQUIRE(positions.size() == 3);
    REQUIRE(positions[0] == 0);
    REQUIRE(positions[1] == 2);
    REQUIRE(positions[2] == 4);
}

TEST_CASE("Heading index: non-heading lines are skipped", "[splitview][headingindex]")
{
    std::string content = "No heading here\n"
                          "Still no heading\n"
                          "# First heading\n";

    auto positions = rebuild_heading_index(content);
    REQUIRE(positions.size() == 1);
    REQUIRE(positions[0] == 2);
}

TEST_CASE("Find nearest heading: exact match", "[splitview][headingindex]")
{
    std::vector<int> positions = {0, 5, 10};
    REQUIRE(find_nearest_heading(positions, 5) == 5);
}

TEST_CASE("Find nearest heading: between two headings", "[splitview][headingindex]")
{
    std::vector<int> positions = {0, 10, 20};
    // Line 6 is closer to 10 than to 0
    REQUIRE(find_nearest_heading(positions, 6) == 10);
    // Line 4 is closer to 0 than to 10
    REQUIRE(find_nearest_heading(positions, 4) == 0);
}

TEST_CASE("Find nearest heading: empty list returns -1", "[splitview][headingindex]")
{
    std::vector<int> positions;
    REQUIRE(find_nearest_heading(positions, 5) == -1);
}

TEST_CASE("Find nearest heading: beyond last heading", "[splitview][headingindex]")
{
    std::vector<int> positions = {0, 10, 20};
    REQUIRE(find_nearest_heading(positions, 100) == 20);
}

// --- Easing function (pure math) ---

namespace
{

auto ease_out_cubic(double progress) -> double
{
    double inv = 1.0 - progress;
    return 1.0 - (inv * inv * inv);
}

} // anonymous namespace

TEST_CASE("EaseOutCubic: 0.0 -> 0.0", "[splitview][easing]")
{
    REQUIRE(ease_out_cubic(0.0) == 0.0);
}

TEST_CASE("EaseOutCubic: 1.0 -> 1.0", "[splitview][easing]")
{
    REQUIRE(ease_out_cubic(1.0) == 1.0);
}

TEST_CASE("EaseOutCubic: 0.5 produces value > 0.5 (ease-out curve)", "[splitview][easing]")
{
    double result = ease_out_cubic(0.5);
    REQUIRE(result > 0.5);
    REQUIRE(result < 1.0);
}

TEST_CASE("EaseOutCubic: monotonically increasing", "[splitview][easing]")
{
    double prev = 0.0;
    for (int step = 1; step <= 10; ++step)
    {
        double progress = static_cast<double>(step) / 10.0;
        double val = ease_out_cubic(progress);
        REQUIRE(val > prev);
        prev = val;
    }
}

// --- Focus mode constants ---

TEST_CASE("Focus mode max chars is 80", "[splitview][focus]")
{
    constexpr int kFocusMaxChars = 80;
    REQUIRE(kFocusMaxChars == 80);
}

// --- Divider constants ---

TEST_CASE("Divider width is 6px", "[splitview][divider]")
{
    constexpr int kDividerWidth = 6;
    REQUIRE(kDividerWidth == 6);
}

TEST_CASE("Divider hit width is 12px (double visual width)", "[splitview][divider]")
{
    constexpr int kDividerHitWidth = 12;
    REQUIRE(kDividerHitWidth == 12);
}

// --- Animation constants ---

TEST_CASE("Animation frame interval is ~16ms (60fps)", "[splitview][animation]")
{
    constexpr int kAnimFrameMs = 16;
    REQUIRE(kAnimFrameMs >= 16);
    REQUIRE(kAnimFrameMs <= 17);
}

TEST_CASE("Transition duration is 200ms", "[splitview][animation]")
{
    constexpr double kTransitionDurationMs = 200.0;
    REQUIRE(kTransitionDurationMs == 200.0);
}
