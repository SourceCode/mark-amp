#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/FocusManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp;
using namespace markamp::ui;
using namespace markamp::core;

TEST_CASE("Keyboard-Only Mode Detector", "[phase05][accessibility]")
{
    EventBus event_bus;

    // Reset state before test
    FocusManager::get().set_keyboard_mode_active(false);

    SECTION("Keyboard mode defaults to disabled")
    {
        REQUIRE_FALSE(FocusManager::get().is_keyboard_mode_active());
    }

    SECTION("set_keyboard_mode_active triggers event and updates state")
    {
        bool event_fired = false;
        bool reported_state = false;

        auto sub = event_bus.subscribe<events::KeyboardModeChangedEvent>(
            [&](const events::KeyboardModeChangedEvent& evt)
            {
                event_fired = true;
                reported_state = evt.is_keyboard_only;
            });

        // Set state by calling FocusManager inside the "Application Filter"
        FocusManager::get().set_keyboard_mode_active(true);
        event_bus.publish(events::KeyboardModeChangedEvent(true));

        REQUIRE(FocusManager::get().is_keyboard_mode_active());
        REQUIRE(event_fired);
        REQUIRE(reported_state);

        // Resetting fires another event
        event_fired = false;
        FocusManager::get().set_keyboard_mode_active(false);
        event_bus.publish(events::KeyboardModeChangedEvent(false));

        REQUIRE_FALSE(FocusManager::get().is_keyboard_mode_active());
        REQUIRE(event_fired);
        REQUIRE_FALSE(reported_state);
    }
}
