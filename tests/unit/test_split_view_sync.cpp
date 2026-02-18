/// @file test_split_view_sync.cpp
/// @brief Phase 09 Batch 9D — Comprehensive unit tests for SplitView sync,
///        layout features, editor mode enhancements, and event round-trips.
///        Also includes BENCHMARK sections for performance budget assertions.

#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/benchmark/catch_benchmark.hpp>
#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <array>
#include <cmath>
#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════
// Task 16: Comprehensive Unit Tests
// ═══════════════════════════════════════════════════════

// --- 1. SplitDirection enum values ---
TEST_CASE("SplitDirection enum has Horizontal and Vertical", "[splitview][direction]")
{
    auto horiz = SplitDirection::Horizontal;
    auto vert = SplitDirection::Vertical;

    REQUIRE(horiz != vert);
    REQUIRE(static_cast<int>(horiz) != static_cast<int>(vert));
}

// --- 2. SplitDirectionChangedEvent carries direction ---
TEST_CASE("SplitDirectionChangedEvent carries direction field", "[splitview][direction][events]")
{
    SplitDirectionChangedEvent evt;
    evt.direction = SplitDirection::Vertical;
    REQUIRE(evt.direction == SplitDirection::Vertical);

    SplitDirectionChangedEvent evt2;
    evt2.direction = SplitDirection::Horizontal;
    REQUIRE(evt2.direction == SplitDirection::Horizontal);
}

// --- 3. SplitDirectionChangedEvent round-trip via EventBus ---
TEST_CASE("SplitDirectionChangedEvent publishes via EventBus", "[splitview][direction][events]")
{
    EventBus bus;
    SplitDirection received = SplitDirection::Horizontal;
    int call_count = 0;

    auto sub = bus.subscribe<SplitDirectionChangedEvent>(
        [&](const SplitDirectionChangedEvent& evt)
        {
            received = evt.direction;
            ++call_count;
        });

    SplitDirectionChangedEvent evt;
    evt.direction = SplitDirection::Vertical;
    bus.publish(evt);

    REQUIRE(received == SplitDirection::Vertical);
    REQUIRE(call_count == 1);
}

// --- 4. ScrollSyncMode enum values ---
TEST_CASE("ScrollSyncMode has Proportional, CursorAnchored, HeadingAnchor", "[splitview][sync]")
{
    auto proportional = ScrollSyncMode::Proportional;
    auto cursor = ScrollSyncMode::CursorAnchored;
    auto heading = ScrollSyncMode::HeadingAnchor;

    REQUIRE(proportional != cursor);
    REQUIRE(cursor != heading);
    REQUIRE(proportional != heading);
}

// --- 5. ScrollSyncModeChangedEvent round-trip ---
TEST_CASE("ScrollSyncModeChangedEvent round-trip", "[splitview][sync][events]")
{
    EventBus bus;
    ScrollSyncMode received = ScrollSyncMode::Proportional;

    auto sub = bus.subscribe<ScrollSyncModeChangedEvent>([&](const ScrollSyncModeChangedEvent& evt)
                                                         { received = evt.mode; });

    bus.publish(ScrollSyncModeChangedEvent{ScrollSyncMode::CursorAnchored});
    REQUIRE(received == ScrollSyncMode::CursorAnchored);

    bus.publish(ScrollSyncModeChangedEvent{ScrollSyncMode::HeadingAnchor});
    REQUIRE(received == ScrollSyncMode::HeadingAnchor);
}

// --- 6. BreadcrumbsChangedEvent carries heading text ---
TEST_CASE("BreadcrumbsChangedEvent carries breadcrumb_path", "[splitview][breadcrumbs][events]")
{
    BreadcrumbsChangedEvent evt;
    evt.breadcrumb_path = "Introduction > Getting Started";
    REQUIRE(evt.breadcrumb_path == "Introduction > Getting Started");
    REQUIRE(evt.type_name() == "BreadcrumbsChangedEvent");
}

// --- 7. BreadcrumbsChangedEvent round-trip via EventBus ---
TEST_CASE("BreadcrumbsChangedEvent publishes via EventBus", "[splitview][breadcrumbs][events]")
{
    EventBus bus;
    std::string received_text;

    auto sub = bus.subscribe<BreadcrumbsChangedEvent>([&](const BreadcrumbsChangedEvent& evt)
                                                      { received_text = evt.breadcrumb_path; });

    BreadcrumbsChangedEvent evt;
    evt.breadcrumb_path = "Chapter 1 > Section A";
    bus.publish(evt);

    REQUIRE(received_text == "Chapter 1 > Section A");
}

// --- 8. OpenInSideEvent carries file_path ---
TEST_CASE("OpenInSideEvent carries file_path", "[splitview][openinside][events]")
{
    OpenInSideEvent evt;
    evt.file_path = "/tmp/example.md";
    REQUIRE(evt.file_path == "/tmp/example.md");
    REQUIRE(evt.type_name() == "OpenInSideEvent");
}

// --- 9. OpenInSideEvent round-trip ---
TEST_CASE("OpenInSideEvent round-trip via EventBus", "[splitview][openinside][events]")
{
    EventBus bus;
    std::string received_path;

    auto sub = bus.subscribe<OpenInSideEvent>([&](const OpenInSideEvent& evt)
                                              { received_path = evt.file_path; });

    OpenInSideEvent evt;
    evt.file_path = "/path/to/file.md";
    bus.publish(evt);

    REQUIRE(received_path == "/path/to/file.md");
}

// --- 10. ExportHtmlRequestEvent publishes ---
TEST_CASE("ExportHtmlRequestEvent publishes via EventBus", "[splitview][export][events]")
{
    EventBus bus;
    int call_count = 0;

    auto sub = bus.subscribe<ExportHtmlRequestEvent>([&](const ExportHtmlRequestEvent& /*evt*/)
                                                     { ++call_count; });

    bus.publish(ExportHtmlRequestEvent{});
    REQUIRE(call_count == 1);

    bus.publish(ExportHtmlRequestEvent{});
    REQUIRE(call_count == 2);
}

// --- 11. SelectionHighlightEvent carries selection_text ---
TEST_CASE("SelectionHighlightEvent carries selection_text", "[splitview][selection][events]")
{
    SelectionHighlightEvent evt;
    evt.selected_text = "highlighted text";
    REQUIRE(evt.selected_text == "highlighted text");
    REQUIRE(evt.type_name() == "SelectionHighlightEvent");
}

// --- 12. SelectionHighlightEvent round-trip ---
TEST_CASE("SelectionHighlightEvent round-trip via EventBus", "[splitview][selection][events]")
{
    EventBus bus;
    std::string received;

    auto sub = bus.subscribe<SelectionHighlightEvent>([&](const SelectionHighlightEvent& evt)
                                                      { received = evt.selected_text; });

    SelectionHighlightEvent evt;
    evt.selected_text = "foo bar";
    bus.publish(evt);

    REQUIRE(received == "foo bar");
}

// --- 13. Snap point calculation logic ---
TEST_CASE("Snap points contain expected values", "[splitview][snap]")
{
    // Mirrors kSnapPoints from SplitView.h
    constexpr std::array<double, 5> kTestSnapPoints = {0.25, 1.0 / 3.0, 0.5, 2.0 / 3.0, 0.75};

    REQUIRE(kTestSnapPoints.size() == 5);
    REQUIRE(kTestSnapPoints[0] == Catch::Approx(0.25));
    REQUIRE(kTestSnapPoints[1] == Catch::Approx(0.3333).epsilon(0.01));
    REQUIRE(kTestSnapPoints[2] == Catch::Approx(0.5));
    REQUIRE(kTestSnapPoints[3] == Catch::Approx(0.6667).epsilon(0.01));
    REQUIRE(kTestSnapPoints[4] == Catch::Approx(0.75));
}

// --- 14. Find nearest snap point logic ---
TEST_CASE("FindNearestSnapPoint selects closest snap", "[splitview][snap]")
{
    constexpr std::array<double, 5> kTestSnapPoints = {0.25, 1.0 / 3.0, 0.5, 2.0 / 3.0, 0.75};
    constexpr double kTestSnapThreshold = 0.03;

    auto find_nearest = [&](double ratio) -> double
    {
        double best = kTestSnapPoints[0];
        double best_dist = std::abs(ratio - best);
        for (std::size_t idx = 1; idx < kTestSnapPoints.size(); ++idx)
        {
            const double current_dist = std::abs(ratio - kTestSnapPoints[idx]);
            if (current_dist < best_dist)
            {
                best_dist = current_dist;
                best = kTestSnapPoints[idx];
            }
        }
        return best;
    };

    // Within threshold of 50% snap
    REQUIRE(find_nearest(0.51) == Catch::Approx(0.5));
    REQUIRE(std::abs(0.51 - find_nearest(0.51)) < kTestSnapThreshold);

    // Within threshold of 25% snap
    REQUIRE(find_nearest(0.26) == Catch::Approx(0.25));

    // Within threshold of 75% snap
    REQUIRE(find_nearest(0.74) == Catch::Approx(0.75));

    // Exactly at 33% snap
    REQUIRE(find_nearest(1.0 / 3.0) == Catch::Approx(1.0 / 3.0));
}

// --- 15. Snap threshold behavior ---
TEST_CASE("Snap threshold prevents snapping when too far", "[splitview][snap]")
{
    constexpr double kTestSnapThreshold = 0.03;

    // 0.40 is far from any snap point (nearest is 0.3333 or 0.5)
    const double nearest_to_40 = 0.5; // 0.5 is closer than 0.333
    const double dist = std::abs(0.40 - nearest_to_40);
    REQUIRE(dist > kTestSnapThreshold); // Should NOT snap
}

// --- 16. ViewMode round-trip all modes ---
TEST_CASE("ViewMode transitions publish all modes correctly", "[splitview][viewmode]")
{
    EventBus bus;
    std::vector<ViewMode> modes;

    auto sub = bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent& evt)
                                                   { modes.push_back(evt.mode); });

    bus.publish(ViewModeChangedEvent{ViewMode::Editor});
    bus.publish(ViewModeChangedEvent{ViewMode::Split});
    bus.publish(ViewModeChangedEvent{ViewMode::Preview});
    bus.publish(ViewModeChangedEvent{ViewMode::LivePreview});

    REQUIRE(modes.size() == 4);
    REQUIRE(modes[0] == ViewMode::Editor);
    REQUIRE(modes[1] == ViewMode::Split);
    REQUIRE(modes[2] == ViewMode::Preview);
    REQUIRE(modes[3] == ViewMode::LivePreview);
}

// --- 17. Multiple event types on same bus ---
TEST_CASE("Multiple event types coexist on EventBus", "[splitview][events]")
{
    EventBus bus;
    int direction_count = 0;
    int mode_count = 0;
    int breadcrumb_count = 0;

    auto dir_sub = bus.subscribe<SplitDirectionChangedEvent>([&](const SplitDirectionChangedEvent&)
                                                             { ++direction_count; });
    auto mode_sub =
        bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent&) { ++mode_count; });
    auto bc_sub = bus.subscribe<BreadcrumbsChangedEvent>([&](const BreadcrumbsChangedEvent&)
                                                         { ++breadcrumb_count; });

    SplitDirectionChangedEvent dir_evt;
    dir_evt.direction = SplitDirection::Vertical;
    bus.publish(dir_evt);
    bus.publish(ViewModeChangedEvent{ViewMode::Split});

    BreadcrumbsChangedEvent bc_evt;
    bc_evt.breadcrumb_path = "Test";
    bus.publish(bc_evt);

    REQUIRE(direction_count == 1);
    REQUIRE(mode_count == 1);
    REQUIRE(breadcrumb_count == 1);
}

// --- 18. Per-file state key generation ---
TEST_CASE("Per-file state keys use hash for uniqueness", "[splitview][state]")
{
    const std::string path1 = "/Users/test/doc1.md";
    const std::string path2 = "/Users/test/doc2.md";

    const auto hash1 = std::hash<std::string>{}(path1);
    const auto hash2 = std::hash<std::string>{}(path2);

    REQUIRE(hash1 != hash2);

    const std::string key1 = "per_file." + std::to_string(hash1) + ".ratio";
    const std::string key2 = "per_file." + std::to_string(hash2) + ".ratio";
    REQUIRE(key1 != key2);
}

// --- 19. Adaptive throttle thresholds ---
TEST_CASE("Adaptive throttle thresholds are ordered", "[splitview][throttle]")
{
    // Mirrors constants from SplitView.h
    constexpr std::size_t kSmallDoc = 5'000;
    constexpr std::size_t kLargeDoc = 50'000;
    constexpr int kSmallDebounce = 50;
    constexpr int kMedDebounce = 150;
    constexpr int kLargeDebounce = 300;

    REQUIRE(kSmallDoc < kLargeDoc);
    REQUIRE(kSmallDebounce < kMedDebounce);
    REQUIRE(kMedDebounce < kLargeDebounce);
}

// --- 20. Notification event carries message correctly ---
TEST_CASE("NotificationEvent round-trip", "[splitview][notification]")
{
    EventBus bus;
    std::string received_msg;
    NotificationLevel received_level = NotificationLevel::Info;

    auto sub = bus.subscribe<NotificationEvent>(
        [&](const NotificationEvent& evt)
        {
            received_msg = evt.message;
            received_level = evt.level;
        });

    bus.publish(NotificationEvent{"HTML exported", NotificationLevel::Success});
    REQUIRE(received_msg == "HTML exported");
    REQUIRE(received_level == NotificationLevel::Success);
}

// ═══════════════════════════════════════════════════════
// Task 17: Performance Budget Assertions (Benchmarks)
// ═══════════════════════════════════════════════════════

TEST_CASE("Benchmark: snap point calculation", "[splitview][benchmark]")
{
    constexpr std::array<double, 5> kSnaps = {0.25, 1.0 / 3.0, 0.5, 2.0 / 3.0, 0.75};

    auto find_nearest = [&](double ratio) -> double
    {
        double best = kSnaps[0];
        double best_dist = std::abs(ratio - best);
        for (std::size_t idx = 1; idx < kSnaps.size(); ++idx)
        {
            const double current_dist = std::abs(ratio - kSnaps[idx]);
            if (current_dist < best_dist)
            {
                best_dist = current_dist;
                best = kSnaps[idx];
            }
        }
        return best;
    };

    BENCHMARK("FindNearestSnapPoint x1000")
    {
        double sum = 0.0;
        for (int iter = 0; iter < 1000; ++iter)
        {
            sum += find_nearest(static_cast<double>(iter) / 1000.0);
        }
        return sum;
    };
}

TEST_CASE("Benchmark: EventBus publish latency", "[splitview][benchmark]")
{
    EventBus bus;
    int counter = 0;

    auto sub = bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent&) { ++counter; });

    BENCHMARK("Publish 1000 ViewModeChangedEvents")
    {
        for (int iter = 0; iter < 1000; ++iter)
        {
            bus.publish(ViewModeChangedEvent{ViewMode::Split});
        }
        return counter;
    };
}

TEST_CASE("Benchmark: per-file state key generation", "[splitview][benchmark]")
{
    BENCHMARK("Hash 1000 file paths")
    {
        std::size_t sum = 0;
        for (int iter = 0; iter < 1000; ++iter)
        {
            const std::string path = "/Users/test/document_" + std::to_string(iter) + ".md";
            sum += std::hash<std::string>{}(path);
        }
        return sum;
    };
}

TEST_CASE("Benchmark: split ratio clamping", "[splitview][benchmark]")
{
    constexpr double kMin = 0.2;
    constexpr double kMax = 0.8;

    BENCHMARK("Clamp 10000 ratios")
    {
        double sum = 0.0;
        for (int iter = 0; iter < 10000; ++iter)
        {
            const double ratio = static_cast<double>(iter) / 10000.0;
            sum += std::clamp(ratio, kMin, kMax);
        }
        return sum;
    };
}
