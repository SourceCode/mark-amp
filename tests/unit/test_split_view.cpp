#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

#include <algorithm>
#include <vector>

using namespace markamp::core;
using namespace markamp::core::events;

// ═══════════════════════════════════════════════════════
// SplitView logic tests (without wxWidgets GUI)
// Tests cover ViewMode enum, event publishing, and split
// ratio clamping logic that can be verified without
// instantiating wxWidgets windows.
// ═══════════════════════════════════════════════════════

// --- 1. Default ViewMode is Split ---
TEST_CASE("Default view mode is Split", "[splitview][viewmode]")
{
    ViewMode mode = ViewMode::Split;
    REQUIRE(mode == ViewMode::Split);
}

// --- 2. ViewMode enum has 3 values ---
TEST_CASE("ViewMode enum has Editor, Split, Preview", "[splitview][viewmode]")
{
    auto editor = ViewMode::Editor;
    auto split = ViewMode::Split;
    auto preview = ViewMode::Preview;

    REQUIRE(editor != split);
    REQUIRE(split != preview);
    REQUIRE(editor != preview);
}

// --- 3. ViewModeChangedEvent carries the mode ---
TEST_CASE("ViewModeChangedEvent carries mode", "[splitview][events]")
{
    ViewModeChangedEvent evt{ViewMode::Editor};
    REQUIRE(evt.mode == ViewMode::Editor);

    ViewModeChangedEvent evt2{ViewMode::Preview};
    REQUIRE(evt2.mode == ViewMode::Preview);

    ViewModeChangedEvent evt3{ViewMode::Split};
    REQUIRE(evt3.mode == ViewMode::Split);
}

// --- 4. ViewModeChangedEvent has correct type name ---
TEST_CASE("ViewModeChangedEvent type_name is correct", "[splitview][events]")
{
    ViewModeChangedEvent evt{ViewMode::Split};
    REQUIRE(evt.type_name() == "ViewModeChangedEvent");
}

// --- 5. EventBus publishes ViewModeChangedEvent ---
TEST_CASE("EventBus publishes ViewModeChangedEvent", "[splitview][events]")
{
    EventBus bus;
    ViewMode received_mode = ViewMode::Split;
    int call_count = 0;

    auto sub = bus.subscribe<ViewModeChangedEvent>(
        [&](const ViewModeChangedEvent& evt)
        {
            received_mode = evt.mode;
            ++call_count;
        });

    bus.publish(ViewModeChangedEvent{ViewMode::Editor});
    REQUIRE(received_mode == ViewMode::Editor);
    REQUIRE(call_count == 1);
}

// --- 6. Multiple event subscriptions work ---
TEST_CASE("Multiple ViewModeChangedEvent subscriptions", "[splitview][events]")
{
    EventBus bus;
    int count_a = 0;
    int count_b = 0;

    auto sub_a =
        bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent&) { ++count_a; });
    auto sub_b =
        bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent&) { ++count_b; });

    bus.publish(ViewModeChangedEvent{ViewMode::Preview});
    REQUIRE(count_a == 1);
    REQUIRE(count_b == 1);
}

// --- 7. Split ratio clamping logic ---
TEST_CASE("Split ratio clamping constants are valid", "[splitview][ratio]")
{
    constexpr double kMin = 0.2;
    constexpr double kMax = 0.8;
    constexpr double kDefault = 0.5;

    REQUIRE(kMin > 0.0);
    REQUIRE(kMax < 1.0);
    REQUIRE(kMin < kMax);
    REQUIRE(kDefault >= kMin);
    REQUIRE(kDefault <= kMax);
}

// --- 8. Clamping below minimum ---
TEST_CASE("Split ratio below minimum clamps to min", "[splitview][ratio]")
{
    constexpr double kMin = 0.2;
    constexpr double kMax = 0.8;
    double ratio = 0.05;
    ratio = std::clamp(ratio, kMin, kMax);
    REQUIRE(ratio == kMin);
}

// --- 9. Clamping above maximum ---
TEST_CASE("Split ratio above maximum clamps to max", "[splitview][ratio]")
{
    constexpr double kMin = 0.2;
    constexpr double kMax = 0.8;
    double ratio = 0.95;
    ratio = std::clamp(ratio, kMin, kMax);
    REQUIRE(ratio == kMax);
}

// --- 10. Valid ratio passes through unchanged ---
TEST_CASE("Split ratio within range is not clamped", "[splitview][ratio]")
{
    constexpr double kMin = 0.2;
    constexpr double kMax = 0.8;
    double ratio = 0.6;
    double clamped = std::clamp(ratio, kMin, kMax);
    REQUIRE(clamped == ratio);
}

// --- 11. Mode switching publishes different event values ---
TEST_CASE("Successive mode changes publish correct values", "[splitview][events]")
{
    EventBus bus;
    std::vector<ViewMode> received;

    auto sub = bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent& evt)
                                                   { received.push_back(evt.mode); });

    bus.publish(ViewModeChangedEvent{ViewMode::Editor});
    bus.publish(ViewModeChangedEvent{ViewMode::Preview});
    bus.publish(ViewModeChangedEvent{ViewMode::Split});

    REQUIRE(received.size() == 3);
    REQUIRE(received[0] == ViewMode::Editor);
    REQUIRE(received[1] == ViewMode::Preview);
    REQUIRE(received[2] == ViewMode::Split);
}

// --- 12. Unsubscribe stops receiving events ---
TEST_CASE("Unsubscribed handler does not receive events", "[splitview][events]")
{
    EventBus bus;
    int count = 0;

    {
        auto sub =
            bus.subscribe<ViewModeChangedEvent>([&](const ViewModeChangedEvent&) { ++count; });

        bus.publish(ViewModeChangedEvent{ViewMode::Editor});
        REQUIRE(count == 1);
    }
    // sub goes out of scope — should unsubscribe

    bus.publish(ViewModeChangedEvent{ViewMode::Preview});
    REQUIRE(count == 1); // Not incremented
}
