/// @file test_v20_visual_integration.cpp
/// @brief V20 Phase 09 – Visual system integration tests.

#include "core/ShellVisualSystem.h"
#include "core/IconSemanticMapper.h"
#include "core/AccessibilityAuditGate.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

TEST_CASE("VisualInteg: shell and icon systems consistent", "[v20][visual-integ]")
{
    EventBus bus;
    ShellVisualSystem shell(bus);
    IconSemanticMapper icons(bus);

    // Shell chrome has defined layers
    auto chrome = shell.all_chrome();
    REQUIRE(chrome.size() >= 8);

    // All icons have labels
    auto unlabeled = icons.unlabeled_icons();
    REQUIRE(unlabeled.empty());
}

TEST_CASE("VisualInteg: accessibility gates with complete system", "[v20][visual-integ]")
{
    EventBus bus;
    ShellVisualSystem shell(bus);
    IconSemanticMapper icons(bus);
    AccessibilityAuditGate gate(bus);

    // Verify all default icons have labels (a11y pass)
    for (const auto& ic : icons.all_icons())
    {
        if (!ic.has_label())
        {
            A11yFinding f;
            f.id = "unlabeled-" + ic.action_id;
            f.surface = "icon";
            f.description = "Icon missing label: " + ic.action_id;
            f.severity = A11ySeverity::kError;
            f.category = A11yCategory::kLabel;
            gate.record_finding(f);
        }
    }
    REQUIRE(gate.gates_pass());
}

TEST_CASE("VisualInteg: event flow across visual system", "[v20][visual-integ]")
{
    EventBus bus;
    int chrome_events = 0;
    int icon_events = 0;
    int a11y_events = 0;

    auto sub1 = bus.subscribe<events::ShellChromeRegisteredEvent>(
        [&](const events::ShellChromeRegisteredEvent&) { ++chrome_events; });
    auto sub2 = bus.subscribe<events::IconMappingRegisteredEvent>(
        [&](const events::IconMappingRegisteredEvent&) { ++icon_events; });
    auto sub3 = bus.subscribe<events::A11yFindingRecordedEvent>(
        [&](const events::A11yFindingRecordedEvent&) { ++a11y_events; });

    ShellVisualSystem shell(bus);
    IconSemanticMapper icons(bus);
    AccessibilityAuditGate gate(bus);

    REQUIRE(chrome_events >= 8);
    REQUIRE(icon_events >= 17);
    REQUIRE(a11y_events == 0);

    A11yFinding f;
    f.id = "test";
    f.severity = A11ySeverity::kInfo;
    gate.record_finding(f);
    REQUIRE(a11y_events == 1);
}
