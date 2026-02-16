/// test_deferred_loading.cpp — Deferred theme & extension loading tests
///
/// Phase 13: Validates the deferred loading event sequence —
/// built-in themes/plugins load synchronously, user themes and extensions
/// load asynchronously after StartupDeferralEvent.

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/LazyService.h"

#include <catch2/catch_test_macros.hpp>

#include <string>
#include <vector>

namespace
{

/// Tracks the sequence of events received during deferred loading.
struct DeferralTracker
{
    std::vector<std::string> event_log;

    void on_deferral(const markamp::core::events::StartupDeferralEvent& /*event*/)
    {
        event_log.emplace_back("StartupDeferralEvent");
    }

    void on_user_themes(const markamp::core::events::UserThemesLoadedEvent& event)
    {
        event_log.push_back("UserThemesLoadedEvent:" + std::to_string(event.theme_count));
    }

    void on_extensions(const markamp::core::events::ExtensionsScanCompleteEvent& event)
    {
        event_log.push_back("ExtensionsScanCompleteEvent:" + std::to_string(event.extension_count) +
                            "/" + std::to_string(event.activated_count));
    }
};

} // namespace

TEST_CASE("Deferred loading: StartupDeferralEvent is publishable", "[deferred]")
{
    markamp::core::EventBus bus;
    bool received = false;

    auto sub = bus.subscribe<markamp::core::events::StartupDeferralEvent>(
        [&received](const markamp::core::events::StartupDeferralEvent& /*event*/)
        { received = true; });

    bus.publish(markamp::core::events::StartupDeferralEvent{});
    REQUIRE(received);
}

TEST_CASE("Deferred loading: UserThemesLoadedEvent carries theme count", "[deferred]")
{
    markamp::core::EventBus bus;
    int received_count = -1;

    auto sub = bus.subscribe<markamp::core::events::UserThemesLoadedEvent>(
        [&received_count](const markamp::core::events::UserThemesLoadedEvent& event)
        { received_count = event.theme_count; });

    markamp::core::events::UserThemesLoadedEvent event;
    event.theme_count = 42;
    bus.publish(event);

    REQUIRE(received_count == 42);
}

TEST_CASE("Deferred loading: ExtensionsScanCompleteEvent carries counts", "[deferred]")
{
    markamp::core::EventBus bus;
    int ext_count = -1;
    int act_count = -1;

    auto sub = bus.subscribe<markamp::core::events::ExtensionsScanCompleteEvent>(
        [&ext_count, &act_count](const markamp::core::events::ExtensionsScanCompleteEvent& event)
        {
            ext_count = event.extension_count;
            act_count = event.activated_count;
        });

    markamp::core::events::ExtensionsScanCompleteEvent event;
    event.extension_count = 15;
    event.activated_count = 12;
    bus.publish(event);

    REQUIRE(ext_count == 15);
    REQUIRE(act_count == 12);
}

TEST_CASE("Deferred loading: event sequence tracking", "[deferred]")
{
    markamp::core::EventBus bus;
    DeferralTracker tracker;

    auto sub1 = bus.subscribe<markamp::core::events::StartupDeferralEvent>(
        [&tracker](const markamp::core::events::StartupDeferralEvent& event)
        { tracker.on_deferral(event); });
    auto sub2 = bus.subscribe<markamp::core::events::UserThemesLoadedEvent>(
        [&tracker](const markamp::core::events::UserThemesLoadedEvent& event)
        { tracker.on_user_themes(event); });
    auto sub3 = bus.subscribe<markamp::core::events::ExtensionsScanCompleteEvent>(
        [&tracker](const markamp::core::events::ExtensionsScanCompleteEvent& event)
        { tracker.on_extensions(event); });

    // Simulate startup sequence:
    // 1. Built-in themes/plugins already loaded (not tested via events)
    // 2. First frame rendered → publish StartupDeferralEvent
    bus.publish(markamp::core::events::StartupDeferralEvent{});

    // 3. Background theme scan completes
    markamp::core::events::UserThemesLoadedEvent theme_event;
    theme_event.theme_count = 10;
    bus.publish(theme_event);

    // 4. Extension scanning completes
    markamp::core::events::ExtensionsScanCompleteEvent ext_event;
    ext_event.extension_count = 8;
    ext_event.activated_count = 7;
    bus.publish(ext_event);

    REQUIRE(tracker.event_log.size() == 3);
    REQUIRE(tracker.event_log[0] == "StartupDeferralEvent");
    REQUIRE(tracker.event_log[1] == "UserThemesLoadedEvent:10");
    REQUIRE(tracker.event_log[2] == "ExtensionsScanCompleteEvent:8/7");
}

TEST_CASE("Deferred loading: lazy service integrates with deferral", "[deferred]")
{
    // Verify that a LazyService can be triggered by StartupDeferralEvent
    markamp::core::EventBus bus;

    struct MockThemeScanner
    {
        bool scan_started{false};
        void start_scan()
        {
            scan_started = true;
        }
    };

    markamp::core::LazyService<MockThemeScanner> lazy_scanner{
        []() { return std::make_unique<MockThemeScanner>(); }};

    REQUIRE_FALSE(lazy_scanner.is_created());

    // Subscribe to deferral event to trigger lazy service
    auto sub = bus.subscribe<markamp::core::events::StartupDeferralEvent>(
        [&lazy_scanner](const markamp::core::events::StartupDeferralEvent& /*event*/)
        {
            auto* scanner = lazy_scanner.get_or_create();
            scanner->start_scan();
        });

    bus.publish(markamp::core::events::StartupDeferralEvent{});

    REQUIRE(lazy_scanner.is_created());
    REQUIRE(lazy_scanner.get()->scan_started);
}
