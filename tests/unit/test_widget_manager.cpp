/// @file test_widget_manager.cpp
/// @brief V4 Phase 33 – WidgetManager tests.

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/KernelManager.h"
#include "core/WidgetManager.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;

// ============================================================================
// Test fixture helper
// ============================================================================

struct WidgetFixture
{
    EventBus event_bus;
    Config config;
    KernelManager kernel_manager{event_bus, config};
    WidgetManager manager{event_bus, kernel_manager};

    void open_slider()
    {
        CommMessage msg;
        msg.comm_id = "comm-1";
        msg.target_name = "jupyter.widget";
        msg.msg_type = "comm_open";
        msg.data_json =
            R"({"widget_id":"w1","widget_type":"IntSlider","value":50,"min":0,"max":100,"description":"Speed"})";
        manager.handle_comm_open(msg);
    }
};

// ============================================================================
// comm_open
// ============================================================================

TEST_CASE("WidgetManager handles comm_open to create widget", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    REQUIRE(ctx.manager.widget_count() == 1);
    auto widget = ctx.manager.get_widget("w1");
    REQUIRE(widget.has_value());
    REQUIRE(widget->widget_type == "IntSlider");
    REQUIRE(widget->get_int("value") == 50);
    REQUIRE(widget->get_int("min") == 0);
    REQUIRE(widget->get_int("max") == 100);
    REQUIRE(widget->get_string("description") == "Speed");
}

// ============================================================================
// comm_msg (state update)
// ============================================================================

TEST_CASE("WidgetManager handles comm_msg to update state", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    CommMessage update;
    update.comm_id = "comm-1";
    update.msg_type = "comm_msg";
    update.data_json = R"({"value":75})";
    ctx.manager.handle_comm_msg(update);

    auto widget = ctx.manager.get_widget("w1");
    REQUIRE(widget.has_value());
    REQUIRE(widget->get_int("value") == 75);
}

// ============================================================================
// comm_close
// ============================================================================

TEST_CASE("WidgetManager handles comm_close to destroy widget", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    CommMessage close;
    close.comm_id = "comm-1";
    close.msg_type = "comm_close";
    ctx.manager.handle_comm_close(close);

    REQUIRE(ctx.manager.widget_count() == 0);
    REQUIRE_FALSE(ctx.manager.get_widget("w1").has_value());
}

// ============================================================================
// Frontend -> Kernel state update
// ============================================================================

TEST_CASE("WidgetManager sends state update from frontend", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    ctx.manager.send_state_update("w1", "value", 99);

    auto widget = ctx.manager.get_widget("w1");
    REQUIRE(widget->get_int("value") == 99);
    REQUIRE(ctx.manager.last_outbound_message().find("99") != std::string::npos);
}

// ============================================================================
// Callbacks
// ============================================================================

TEST_CASE("WidgetManager fires state change callback", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    bool callback_fired = false;
    int new_value = 0;
    ctx.manager.on_state_change("w1",
                                [&](const WidgetState& state)
                                {
                                    callback_fired = true;
                                    new_value = state.get_int("value");
                                });

    ctx.manager.send_state_update("w1", "value", 42);
    REQUIRE(callback_fired);
    REQUIRE(new_value == 42);
}

// ============================================================================
// Active widgets
// ============================================================================

TEST_CASE("WidgetManager lists active widgets", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();

    // Open a second widget.
    CommMessage msg2;
    msg2.comm_id = "comm-2";
    msg2.msg_type = "comm_open";
    msg2.data_json = R"({"widget_id":"w2","widget_type":"Dropdown"})";
    ctx.manager.handle_comm_open(msg2);

    auto widgets = ctx.manager.active_widgets();
    REQUIRE(widgets.size() == 2);
}

// ============================================================================
// Clear all
// ============================================================================

TEST_CASE("WidgetManager clears all widgets on kernel restart", "[phase33][widget]")
{
    WidgetFixture ctx;
    ctx.open_slider();
    ctx.manager.clear_all();

    REQUIRE(ctx.manager.widget_count() == 0);
}

// ============================================================================
// WidgetState accessors
// ============================================================================

TEST_CASE("WidgetState get_bool accessor works", "[phase33][widget]")
{
    WidgetState state;
    state.properties["visible"] = true;
    state.properties["disabled"] = false;

    REQUIRE(state.get_bool("visible") == true);
    REQUIRE(state.get_bool("disabled") == false);
    REQUIRE(state.get_bool("nonexistent") == false);
}

TEST_CASE("WidgetState get_double accessor works", "[phase33][widget]")
{
    WidgetState state;
    state.properties["step"] = 0.5;

    REQUIRE(state.get_double("step") == 0.5);
    REQUIRE(state.get_double("nonexistent") == 0.0);
}

TEST_CASE("WidgetManager ignores comm_msg for unknown comm_id", "[phase33][widget]")
{
    WidgetFixture ctx;

    CommMessage msg;
    msg.comm_id = "unknown-comm";
    msg.msg_type = "comm_msg";
    msg.data_json = R"({"value":1})";
    ctx.manager.handle_comm_msg(msg);

    REQUIRE(ctx.manager.widget_count() == 0);
}
