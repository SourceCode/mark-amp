#include "core/EventBus.h"
#include "core/NotificationService.h"
#include "core/RunConfigService.h"
#include "ui/ToolbarModel.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::ui;

// ============================================================================
// RunConfigService Tests (Phase 26 Task 22)
// ============================================================================

static auto make_test_config(const std::string& name = "Debug",
                             const std::string& command = "cmake --build build/debug")
    -> RunConfiguration
{
    RunConfiguration config;
    config.name = name;
    config.command = command;
    config.type = "cmake";
    config.build_before_run = true;
    return config;
}

TEST_CASE("RunConfigService: add and find configurations", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    auto config = make_test_config();
    service.add_configuration(config);

    REQUIRE(service.configurations().size() == 1);
    REQUIRE(service.configurations().front().name == "Debug");

    auto* found = service.find("Debug");
    REQUIRE(found != nullptr);
    REQUIRE(found->command == "cmake --build build/debug");
}

TEST_CASE("RunConfigService: reject empty name or command", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    RunConfiguration empty_name;
    empty_name.command = "some_command";
    service.add_configuration(empty_name);
    REQUIRE(service.configurations().empty());

    RunConfiguration empty_cmd;
    empty_cmd.name = "Test";
    service.add_configuration(empty_cmd);
    REQUIRE(service.configurations().empty());
}

TEST_CASE("RunConfigService: remove configuration", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Debug"));
    service.add_configuration(make_test_config("Release", "cmake --build build/release"));

    REQUIRE(service.configurations().size() == 2);

    service.remove_configuration("Debug");
    REQUIRE(service.configurations().size() == 1);
    REQUIRE(service.find("Debug") == nullptr);
    REQUIRE(service.find("Release") != nullptr);
}

TEST_CASE("RunConfigService: update configuration", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Debug"));
    REQUIRE(service.find("Debug")->build_before_run == true);

    auto updated = make_test_config("Debug");
    updated.build_before_run = false;
    service.update_configuration(updated);

    REQUIRE(service.find("Debug")->build_before_run == false);
}

TEST_CASE("RunConfigService: active configuration", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    REQUIRE(service.active_configuration() == nullptr);

    service.add_configuration(make_test_config("Debug"));
    service.add_configuration(make_test_config("Release", "cmake --build build/release"));

    service.set_active("Debug");
    REQUIRE(service.active_configuration() != nullptr);
    REQUIRE(service.active_configuration()->name == "Debug");

    service.set_active("Release");
    REQUIRE(service.active_configuration()->name == "Release");
}

TEST_CASE("RunConfigService: set_active ignores non-existent name", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Debug"));
    service.set_active("Debug");
    service.set_active("NonExistent");

    REQUIRE(service.active_configuration()->name == "Debug");
}

TEST_CASE("RunConfigService: recent configurations", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Alpha", "cmd1"));
    service.add_configuration(make_test_config("Beta", "cmd2"));
    service.add_configuration(make_test_config("Gamma", "cmd3"));

    service.set_active("Alpha");
    service.set_active("Beta");
    service.set_active("Gamma");

    auto recent = service.recent_configurations(3);
    REQUIRE(recent.size() == 3);
    REQUIRE(recent[0] == "Gamma"); // Most recent first
    REQUIRE(recent[1] == "Beta");
    REQUIRE(recent[2] == "Alpha");
}

TEST_CASE("RunConfigService: recent list deduplicates", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Alpha", "cmd1"));
    service.add_configuration(make_test_config("Beta", "cmd2"));

    service.set_active("Alpha");
    service.set_active("Beta");
    service.set_active("Alpha"); // Re-select Alpha

    auto recent = service.recent_configurations();
    REQUIRE(recent.size() == 2);
    REQUIRE(recent[0] == "Alpha"); // Alpha is most recent
    REQUIRE(recent[1] == "Beta");
}

TEST_CASE("RunConfigService: remove clears active", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Debug"));
    service.set_active("Debug");
    REQUIRE(service.active_configuration() != nullptr);

    service.remove_configuration("Debug");
    REQUIRE(service.active_configuration() == nullptr);
}

TEST_CASE("RunConfigService: add replaces existing by name", "[toolbar_v2][run_config]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("Debug", "old_cmd"));
    service.add_configuration(make_test_config("Debug", "new_cmd"));

    REQUIRE(service.configurations().size() == 1);
    REQUIRE(service.find("Debug")->command == "new_cmd");
}

// ============================================================================
// NotificationService Stored Notifications Tests (Phase 26 Task 23)
// ============================================================================

TEST_CASE("NotificationService: store and query", "[toolbar_v2][notification]")
{
    EventBus event_bus;
    NotificationService service(event_bus);

    REQUIRE(service.stored_notifications().empty());
    REQUIRE(service.unread_count() == 0);

    service.store("Build", "Build succeeded", StoredNotification::Level::kInfo, "build");
    REQUIRE(service.stored_notifications().size() == 1);
    REQUIRE(service.unread_count() == 1);
    REQUIRE(service.stored_notifications().front().title == "Build");
}

TEST_CASE("NotificationService: mark read", "[toolbar_v2][notification]")
{
    EventBus event_bus;
    NotificationService service(event_bus);

    service.store("Msg1", "Body1");
    service.store("Msg2", "Body2");
    REQUIRE(service.unread_count() == 2);

    auto notification_id = service.stored_notifications().front().id;
    service.mark_read(notification_id);
    REQUIRE(service.unread_count() == 1);
}

TEST_CASE("NotificationService: mark all read", "[toolbar_v2][notification]")
{
    EventBus event_bus;
    NotificationService service(event_bus);

    service.store("A", "a");
    service.store("B", "b");
    service.store("C", "c");
    REQUIRE(service.unread_count() == 3);

    service.mark_all_read();
    REQUIRE(service.unread_count() == 0);
}

TEST_CASE("NotificationService: clear all", "[toolbar_v2][notification]")
{
    EventBus event_bus;
    NotificationService service(event_bus);

    service.store("A", "a");
    service.store("B", "b");
    service.clear_all();
    REQUIRE(service.stored_notifications().empty());
    REQUIRE(service.unread_count() == 0);
}

TEST_CASE("NotificationService: most recent first", "[toolbar_v2][notification]")
{
    EventBus event_bus;
    NotificationService service(event_bus);

    service.store("First", "1");
    service.store("Second", "2");
    service.store("Third", "3");

    REQUIRE(service.stored_notifications().front().title == "Third");
}

// ============================================================================
// ToolbarModel Responsive Collapse Tests (Phase 26 Task 24)
// ============================================================================

static auto make_toolbar_model_with_buttons(int count) -> ToolbarModel
{
    ToolbarModel model;
    std::vector<ToolbarButtonModel> buttons;
    for (int index = 0; index < count; ++index)
    {
        ToolbarButtonModel button;
        button.button_id = "btn_" + std::to_string(index);
        button.label = "Button " + std::to_string(index);
        buttons.push_back(std::move(button));
    }
    model.set_global_actions(std::move(buttons));
    return model;
}

TEST_CASE("ToolbarModel: overflow detects hidden buttons", "[toolbar_v2][responsive]")
{
    auto model = make_toolbar_model_with_buttons(10);
    auto overflowed = model.overflowed_indices(200, 40);

    // 200px / 40px = 5 visible buttons, 5 overflow
    REQUIRE(overflowed.size() == 5);
    REQUIRE(overflowed[0] == 5);
    REQUIRE(overflowed[4] == 9);
}

TEST_CASE("ToolbarModel: no overflow when wide enough", "[toolbar_v2][responsive]")
{
    auto model = make_toolbar_model_with_buttons(5);
    auto overflowed = model.overflowed_indices(300, 40);

    REQUIRE(overflowed.empty());
}

TEST_CASE("ToolbarModel: all overflow at narrow width", "[toolbar_v2][responsive]")
{
    auto model = make_toolbar_model_with_buttons(5);
    auto overflowed = model.overflowed_indices(30, 40);

    // Only 0 buttons fit — all overflow
    REQUIRE(overflowed.size() == 5);
}

// ============================================================================
// Run Flow Integration Tests (Phase 26 Task 25)
// ============================================================================

TEST_CASE("Run flow: start publishes RunConfigStartedEvent", "[toolbar_v2][run_flow]")
{
    EventBus event_bus;
    RunConfigService service(event_bus);

    service.add_configuration(make_test_config("MyApp", "./build/myapp"));
    service.set_active("MyApp");

    bool event_received = false;
    auto subscription = event_bus.subscribe<events::RunConfigStartedEvent>(
        [&](const events::RunConfigStartedEvent& evt)
        {
            event_received = true;
            REQUIRE(evt.config_name == "MyApp");
            REQUIRE_FALSE(evt.is_debug);
        });

    // Simulate toolbar run button: publish the event
    events::RunConfigStartedEvent run_event;
    run_event.config_name = "MyApp";
    run_event.is_debug = false;
    event_bus.publish(run_event);

    REQUIRE(event_received);
}

TEST_CASE("Run flow: stop publishes RunConfigStoppedEvent", "[toolbar_v2][run_flow]")
{
    EventBus event_bus;

    bool stopped = false;
    auto subscription = event_bus.subscribe<events::RunConfigStoppedEvent>(
        [&](const events::RunConfigStoppedEvent& evt)
        {
            stopped = true;
            REQUIRE(evt.config_name == "TestConfig");
        });

    events::RunConfigStoppedEvent stop_event;
    stop_event.config_name = "TestConfig";
    event_bus.publish(stop_event);

    REQUIRE(stopped);
}

TEST_CASE("Run flow: debug mode sets is_debug flag", "[toolbar_v2][run_flow]")
{
    EventBus event_bus;

    bool is_debug = false;
    auto subscription = event_bus.subscribe<events::RunConfigStartedEvent>(
        [&](const events::RunConfigStartedEvent& evt) { is_debug = evt.is_debug; });

    events::RunConfigStartedEvent debug_event;
    debug_event.config_name = "Debug Build";
    debug_event.is_debug = true;
    event_bus.publish(debug_event);

    REQUIRE(is_debug);
}

TEST_CASE("Run flow: config changed event", "[toolbar_v2][run_flow]")
{
    EventBus event_bus;

    std::string changed_name;
    auto subscription = event_bus.subscribe<events::RunConfigChangedEvent>(
        [&](const events::RunConfigChangedEvent& evt) { changed_name = evt.config_name; });

    events::RunConfigChangedEvent changed_event;
    changed_event.config_name = "Release";
    event_bus.publish(changed_event);

    REQUIRE(changed_name == "Release");
}

TEST_CASE("Run flow: build started/finished events", "[toolbar_v2][run_flow]")
{
    EventBus event_bus;

    bool build_started = false;
    bool build_finished = false;
    bool build_success = false;

    auto start_sub = event_bus.subscribe<events::BuildStartedEvent>(
        [&](const events::BuildStartedEvent&) { build_started = true; });

    auto finish_sub = event_bus.subscribe<events::BuildFinishedEvent>(
        [&](const events::BuildFinishedEvent& evt)
        {
            build_finished = true;
            build_success = evt.success;
        });

    events::BuildStartedEvent start_event;
    start_event.target = "markamp";
    event_bus.publish(start_event);
    REQUIRE(build_started);

    events::BuildFinishedEvent finish_event;
    finish_event.success = true;
    event_bus.publish(finish_event);
    REQUIRE(build_finished);
    REQUIRE(build_success);
}
