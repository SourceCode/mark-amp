// ============================================================================
// File: tests/unit/test_phase14_canvas_extensibility.cpp
// Phase 14: Canvas Extensibility & App Widgets — unit tests
// ============================================================================
#include "canvas/CanvasTemplateEngine.h"
#include "canvas/CanvasToolHost.h"
#include "canvas/CustomObjectTypeRegistry.h"
#include "canvas/WidgetLifecycleManager.h"
#include "canvas/WidgetMessageBus.h"
#include "canvas/WidgetPermissionGuard.h"
#include "canvas/WidgetRenderer.h"
#include "core/EventBus.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::canvas;
using namespace markamp::core;

// ============================================================================
// Helpers
// ============================================================================

namespace
{

/// Trivial CanvasObject stub for factory testing.
class StubCanvasObject : public CanvasObject
{
public:
    StubCanvasObject()
        : CanvasObject(CanvasObjectType::Shape)
    {
    }

    [[nodiscard]] auto local_bounds() const -> AABB override
    {
        return {0.0, 0.0, 100.0, 80.0};
    }

    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override
    {
        return std::make_unique<StubCanvasObject>();
    }
};

} // anonymous namespace

// ============================================================================
// CustomObjectTypeRegistry
// ============================================================================

TEST_CASE("CustomObjectTypeRegistry — register and query", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    REQUIRE(registry.type_count() == 0);

    CustomObjectTypeDescriptor desc;
    desc.type_id = "ext.kanban-card";
    desc.display_name = "Kanban Card";
    desc.icon = "kanban.png";
    desc.extension_id = "ext-kanban";
    desc.category = "data";
    desc.factory = [](Point2D /*pos*/, Size2D /*sz*/)
    { return std::make_unique<StubCanvasObject>(); };

    auto result = registry.register_type(desc);
    REQUIRE(result.success);
    REQUIRE(registry.type_count() == 1);
    REQUIRE(registry.is_registered("ext.kanban-card"));
}

TEST_CASE("CustomObjectTypeRegistry — duplicate rejection", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    CustomObjectTypeDescriptor desc;
    desc.type_id = "ext.card";
    desc.display_name = "Card";
    desc.extension_id = "ext-a";
    desc.factory = [](Point2D, Size2D) { return std::make_unique<StubCanvasObject>(); };

    REQUIRE(registry.register_type(desc).success);
    REQUIRE_FALSE(registry.register_type(desc).success); // duplicate
}

TEST_CASE("CustomObjectTypeRegistry — create object via factory", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    CustomObjectTypeDescriptor desc;
    desc.type_id = "ext.widget";
    desc.display_name = "Widget";
    desc.extension_id = "ext-w";
    desc.factory = [](Point2D, Size2D) { return std::make_unique<StubCanvasObject>(); };

    registry.register_type(desc);
    auto obj = registry.create_object("ext.widget");
    REQUIRE(obj != nullptr);
}

TEST_CASE("CustomObjectTypeRegistry — unregister extension types", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    for (int idx = 0; idx < 3; ++idx)
    {
        CustomObjectTypeDescriptor desc;
        desc.type_id = "ext.type-" + std::to_string(idx);
        desc.display_name = "Type " + std::to_string(idx);
        desc.extension_id = "ext-bulk";
        desc.factory = [](Point2D, Size2D) { return std::make_unique<StubCanvasObject>(); };
        registry.register_type(desc);
    }

    REQUIRE(registry.type_count() == 3);
    auto removed = registry.unregister_extension("ext-bulk");
    REQUIRE(removed == 3);
    REQUIRE(registry.type_count() == 0);
}

TEST_CASE("CustomObjectTypeRegistry — category query", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    CustomObjectTypeDescriptor desc_a;
    desc_a.type_id = "a";
    desc_a.display_name = "A";
    desc_a.category = "shapes";
    desc_a.extension_id = "ext";
    desc_a.factory = [](Point2D, Size2D) { return std::make_unique<StubCanvasObject>(); };

    CustomObjectTypeDescriptor desc_b;
    desc_b.type_id = "b";
    desc_b.display_name = "B";
    desc_b.category = "data";
    desc_b.extension_id = "ext";
    desc_b.factory = [](Point2D, Size2D) { return std::make_unique<StubCanvasObject>(); };

    registry.register_type(desc_a);
    registry.register_type(desc_b);

    auto shapes = registry.types_in_category("shapes");
    REQUIRE(shapes.size() == 1);
    REQUIRE(shapes[0]->type_id == "a");
}

TEST_CASE("CustomObjectTypeRegistry — null factory rejected", "[phase14][registry]")
{
    EventBus bus;
    CustomObjectTypeRegistry registry(bus);

    CustomObjectTypeDescriptor desc;
    desc.type_id = "no-factory";
    desc.display_name = "NoFactory";
    desc.extension_id = "ext";
    // factory is null (default)

    auto result = registry.register_type(desc);
    REQUIRE_FALSE(result.success);
}

// ============================================================================
// CanvasToolHost
// ============================================================================

TEST_CASE("CanvasToolHost — register and activate tool", "[phase14][toolhost]")
{
    EventBus bus;
    CanvasToolHost host(bus);

    HostedTool tool;
    tool.tool_id = "select";
    tool.label = "Select";
    tool.category = "core";
    tool.is_built_in = true;
    tool.priority = 0;

    REQUIRE(host.register_tool(tool));
    REQUIRE(host.tool_count() == 1);

    auto result = host.activate_tool("select");
    REQUIRE(result.success);
    REQUIRE(host.active_tool_id() == "select");
}

TEST_CASE("CanvasToolHost — activation deactivates previous tool", "[phase14][toolhost]")
{
    EventBus bus;
    CanvasToolHost host(bus);

    bool tool_a_active = false;
    bool tool_b_active = false;

    HostedTool tool_a;
    tool_a.tool_id = "tool-a";
    tool_a.label = "Tool A";
    tool_a.category = "draw";
    tool_a.on_activation = [&](bool active) { tool_a_active = active; };

    HostedTool tool_b;
    tool_b.tool_id = "tool-b";
    tool_b.label = "Tool B";
    tool_b.category = "draw";
    tool_b.on_activation = [&](bool active) { tool_b_active = active; };

    host.register_tool(tool_a);
    host.register_tool(tool_b);

    host.activate_tool("tool-a");
    REQUIRE(tool_a_active);

    auto result = host.activate_tool("tool-b");
    REQUIRE(result.success);
    REQUIRE(result.deactivated_tool_id == "tool-a");
    REQUIRE_FALSE(tool_a_active);
    REQUIRE(tool_b_active);
}

TEST_CASE("CanvasToolHost — tools in category sorted by priority", "[phase14][toolhost]")
{
    EventBus bus;
    CanvasToolHost host(bus);

    HostedTool low;
    low.tool_id = "low";
    low.label = "Low";
    low.category = "draw";
    low.priority = 10;

    HostedTool high;
    high.tool_id = "high";
    high.label = "High";
    high.category = "draw";
    high.priority = 1;

    host.register_tool(low);
    host.register_tool(high);

    auto tools = host.tools_in_category("draw");
    REQUIRE(tools.size() == 2);
    REQUIRE(tools[0]->tool_id == "high"); // Lower priority = first
    REQUIRE(tools[1]->tool_id == "low");
}

TEST_CASE("CanvasToolHost — tool categories", "[phase14][toolhost]")
{
    EventBus bus;
    CanvasToolHost host(bus);

    HostedTool tool_a;
    tool_a.tool_id = "a";
    tool_a.label = "A";
    tool_a.category = "draw";

    HostedTool tool_b;
    tool_b.tool_id = "b";
    tool_b.label = "B";
    tool_b.category = "annotate";

    host.register_tool(tool_a);
    host.register_tool(tool_b);

    auto categories = host.tool_categories();
    REQUIRE(categories.size() == 2);
}

TEST_CASE("CanvasToolHost — extension tool count", "[phase14][toolhost]")
{
    EventBus bus;
    CanvasToolHost host(bus);

    HostedTool builtin;
    builtin.tool_id = "select";
    builtin.label = "Select";
    builtin.category = "core";
    builtin.is_built_in = true;

    HostedTool extension;
    extension.tool_id = "ext-laser";
    extension.label = "Laser";
    extension.category = "annotate";
    extension.extension_id = "ext-tools";
    extension.is_built_in = false;

    host.register_tool(builtin);
    host.register_tool(extension);

    REQUIRE(host.tool_count() == 2);
    REQUIRE(host.extension_tool_count() == 1);
}

// ============================================================================
// WidgetRenderer
// ============================================================================

TEST_CASE("WidgetRenderer — compute layout regions", "[phase14][renderer]")
{
    WidgetRenderer renderer;

    WidgetRenderContext ctx;
    ctx.bounds = {0.0, 0.0, 200.0, 150.0};
    ctx.zoom_level = 1.0;
    ctx.style = WidgetRenderStyle::kCard;

    auto layout = renderer.compute_layout(ctx);

    REQUIRE(layout.header.region_width == 200.0);
    REQUIRE(layout.content.region_width == 200.0);
    REQUIRE(layout.status_bar.region_width == 200.0);
}

TEST_CASE("WidgetRenderer — style names", "[phase14][renderer]")
{
    REQUIRE(WidgetRenderer::style_name(WidgetRenderStyle::kCard) == "card");
    REQUIRE(WidgetRenderer::style_name(WidgetRenderStyle::kCompact) == "compact");
    REQUIRE(WidgetRenderer::style_name(WidgetRenderStyle::kMinimal) == "minimal");
    REQUIRE(WidgetRenderer::style_name(WidgetRenderStyle::kFullscreen) == "fullscreen");
}

TEST_CASE("WidgetRenderer — render widget description", "[phase14][renderer]")
{
    WidgetRenderer renderer;
    AppWidgetObject widget;
    widget.set_widget_id("w1");
    widget.set_app_id("app-test");

    WidgetRenderContext ctx;
    ctx.bounds = {10.0, 20.0, 200.0, 150.0};
    ctx.is_selected = true;

    auto desc = renderer.render_widget(widget, ctx);
    REQUIRE(desc.find("w1") != std::string::npos);
    REQUIRE(desc.find("SELECTED") != std::string::npos);
}

TEST_CASE("WidgetRenderer — render placeholder", "[phase14][renderer]")
{
    WidgetRenderer renderer;
    WidgetRenderContext ctx;
    ctx.bounds = {0.0, 0.0, 200.0, 100.0};

    auto desc = renderer.render_placeholder(ctx);
    REQUIRE(desc.find("Placeholder") != std::string::npos);
    REQUIRE(desc.find("loading") != std::string::npos);
}

TEST_CASE("WidgetRenderer — render error state", "[phase14][renderer]")
{
    WidgetRenderer renderer;
    WidgetRenderContext ctx;
    ctx.bounds = {0.0, 0.0, 200.0, 100.0};

    auto desc = renderer.render_error_state("Connection lost", ctx);
    REQUIRE(desc.find("Error") != std::string::npos);
    REQUIRE(desc.find("Connection lost") != std::string::npos);
}

// ============================================================================
// WidgetLifecycleManager
// ============================================================================

TEST_CASE("WidgetLifecycleManager — track and initialize", "[phase14][lifecycle]")
{
    EventBus bus;
    WidgetLifecycleManager manager(bus);

    REQUIRE(manager.track_widget("w1"));
    REQUIRE(manager.tracked_count() == 1);

    auto state = manager.widget_state("w1");
    REQUIRE(state != nullptr);
    REQUIRE(state->state == WidgetLifecycleState::kUninitialized);

    auto result = manager.initialize_widget("w1");
    REQUIRE(result.success);
    REQUIRE(result.current_state == WidgetLifecycleState::kInitializing);
}

TEST_CASE("WidgetLifecycleManager — full lifecycle", "[phase14][lifecycle]")
{
    EventBus bus;
    WidgetLifecycleManager manager(bus);

    manager.track_widget("w1");
    REQUIRE(manager.initialize_widget("w1").success);
    REQUIRE(manager.activate_widget("w1").success);
    REQUIRE(manager.active_count() == 1);
    REQUIRE(manager.suspend_widget("w1").success);
    REQUIRE(manager.active_count() == 0);
    REQUIRE(manager.activate_widget("w1").success); // resume
    REQUIRE(manager.destroy_widget("w1").success);
}

TEST_CASE("WidgetLifecycleManager — invalid transitions rejected", "[phase14][lifecycle]")
{
    EventBus bus;
    WidgetLifecycleManager manager(bus);

    manager.track_widget("w1");

    // Cannot skip to Active from Uninitialized
    auto result = manager.activate_widget("w1");
    REQUIRE_FALSE(result.success);
}

TEST_CASE("WidgetLifecycleManager — state names", "[phase14][lifecycle]")
{
    REQUIRE(WidgetLifecycleManager::state_name(WidgetLifecycleState::kUninitialized) ==
            "uninitialized");
    REQUIRE(WidgetLifecycleManager::state_name(WidgetLifecycleState::kActive) == "active");
    REQUIRE(WidgetLifecycleManager::state_name(WidgetLifecycleState::kError) == "error");
    REQUIRE(WidgetLifecycleManager::state_name(WidgetLifecycleState::kDestroyed) == "destroyed");
}

TEST_CASE("WidgetLifecycleManager — lifecycle callback fires", "[phase14][lifecycle]")
{
    EventBus bus;
    WidgetLifecycleManager manager(bus);

    int callback_count = 0;
    manager.set_on_state_change([&](const std::string& /*wid*/,
                                    WidgetLifecycleState /*old_st*/,
                                    WidgetLifecycleState /*new_st*/) { ++callback_count; });

    manager.track_widget("w1");
    manager.initialize_widget("w1");
    manager.activate_widget("w1");

    REQUIRE(callback_count == 2);
}

// ============================================================================
// WidgetPermissionGuard
// ============================================================================

TEST_CASE("WidgetPermissionGuard — declared permissions granted", "[phase14][permissions]")
{
    EventBus bus;
    WidgetPermissionGuard guard(bus);

    guard.register_widget("w1", {WidgetPermission::kReadData, WidgetPermission::kWriteData});

    REQUIRE(guard.check_permission("w1", WidgetPermission::kReadData) ==
            PermissionCheckResult::kGranted);
    REQUIRE(guard.check_permission("w1", WidgetPermission::kWriteData) ==
            PermissionCheckResult::kGranted);
}

TEST_CASE("WidgetPermissionGuard — undeclared permissions denied", "[phase14][permissions]")
{
    EventBus bus;
    WidgetPermissionGuard guard(bus);

    guard.register_widget("w1", {WidgetPermission::kReadData});

    REQUIRE(guard.check_permission("w1", WidgetPermission::kNetworkAccess) ==
            PermissionCheckResult::kDenied);
}

TEST_CASE("WidgetPermissionGuard — runtime grant and revoke", "[phase14][permissions]")
{
    EventBus bus;
    WidgetPermissionGuard guard(bus);

    guard.register_widget("w1", {});

    REQUIRE(guard.check_permission("w1", WidgetPermission::kReadData) ==
            PermissionCheckResult::kDenied);

    guard.grant_permission("w1", WidgetPermission::kReadData);
    REQUIRE(guard.check_permission("w1", WidgetPermission::kReadData) ==
            PermissionCheckResult::kGranted);

    guard.revoke_permission("w1", WidgetPermission::kReadData);
    REQUIRE(guard.check_permission("w1", WidgetPermission::kReadData) ==
            PermissionCheckResult::kDenied);
}

TEST_CASE("WidgetPermissionGuard — audit log records checks", "[phase14][permissions]")
{
    EventBus bus;
    WidgetPermissionGuard guard(bus);

    guard.register_widget("w1", {WidgetPermission::kReadData});
    guard.check_permission("w1", WidgetPermission::kReadData);
    guard.check_permission("w1", WidgetPermission::kNetworkAccess);

    auto log = guard.audit_log();
    REQUIRE(log.size() == 2);
}

TEST_CASE("WidgetPermissionGuard — permission names", "[phase14][permissions]")
{
    REQUIRE(WidgetPermissionGuard::permission_name(WidgetPermission::kReadData) == "read_data");
    REQUIRE(WidgetPermissionGuard::permission_name(WidgetPermission::kCanvasModify) ==
            "canvas_modify");
}

// ============================================================================
// CanvasTemplateEngine
// ============================================================================

TEST_CASE("CanvasTemplateEngine — register and find template", "[phase14][templates]")
{
    EventBus bus;
    CanvasTemplateEngine engine(bus);

    CanvasTemplate tmpl;
    tmpl.template_id = "brainstorm-1";
    tmpl.name = "Basic Brainstorm";
    tmpl.category = "brainstorming";
    tmpl.board_json = R"({"objects": [{"type": "sticky"}]})";
    tmpl.tags = {"brainstorm", "quick"};

    REQUIRE(engine.register_template(tmpl));
    REQUIRE(engine.template_count() == 1);

    auto found = engine.find_template("brainstorm-1");
    REQUIRE(found != nullptr);
    REQUIRE(found->name == "Basic Brainstorm");
}

TEST_CASE("CanvasTemplateEngine — apply template", "[phase14][templates]")
{
    EventBus bus;
    CanvasTemplateEngine engine(bus);

    CanvasTemplate tmpl;
    tmpl.template_id = "retro-1";
    tmpl.name = "Retrospective";
    tmpl.category = "retrospective";
    tmpl.board_json = R"({"columns": [{"title": "Went well"}, {"title": "Improve"}]})";

    engine.register_template(tmpl);

    auto result = engine.apply_template("retro-1");
    REQUIRE(result.success);
    REQUIRE(result.objects_created > 0);
}

TEST_CASE("CanvasTemplateEngine — search by name", "[phase14][templates]")
{
    EventBus bus;
    CanvasTemplateEngine engine(bus);

    CanvasTemplate tmpl_a;
    tmpl_a.template_id = "a";
    tmpl_a.name = "Sprint Planning Board";
    tmpl_a.category = "planning";

    CanvasTemplate tmpl_b;
    tmpl_b.template_id = "b";
    tmpl_b.name = "Brainstorm Canvas";
    tmpl_b.category = "brainstorming";

    engine.register_template(tmpl_a);
    engine.register_template(tmpl_b);

    auto results = engine.search_templates("sprint");
    REQUIRE(results.size() == 1);
    REQUIRE(results[0]->name == "Sprint Planning Board");
}

TEST_CASE("CanvasTemplateEngine — categories", "[phase14][templates]")
{
    EventBus bus;
    CanvasTemplateEngine engine(bus);

    CanvasTemplate tmpl_a;
    tmpl_a.template_id = "a";
    tmpl_a.name = "A";
    tmpl_a.category = "planning";

    CanvasTemplate tmpl_b;
    tmpl_b.template_id = "b";
    tmpl_b.name = "B";
    tmpl_b.category = "brainstorming";

    engine.register_template(tmpl_a);
    engine.register_template(tmpl_b);

    auto cats = engine.template_categories();
    REQUIRE(cats.size() == 2);
}

TEST_CASE("CanvasTemplateEngine — unregister extension templates", "[phase14][templates]")
{
    EventBus bus;
    CanvasTemplateEngine engine(bus);

    CanvasTemplate tmpl;
    tmpl.template_id = "ext-t1";
    tmpl.name = "ExtTemplate";
    tmpl.extension_id = "ext-a";

    engine.register_template(tmpl);
    REQUIRE(engine.template_count() == 1);

    auto removed = engine.unregister_extension_templates("ext-a");
    REQUIRE(removed == 1);
    REQUIRE(engine.template_count() == 0);
}

// ============================================================================
// WidgetMessageBus
// ============================================================================

TEST_CASE("WidgetMessageBus — send and receive message", "[phase14][messaging]")
{
    EventBus bus;
    WidgetMessageBus msg_bus(bus);

    std::string received_payload;
    msg_bus.subscribe("w2",
                      "data.update",
                      [&](const WidgetMessage& msg) { received_payload = msg.payload_json; });

    WidgetMessage msg;
    msg.source_widget_id = "w1";
    msg.target_widget_id = "w2";
    msg.message_type = "data.update";
    msg.payload_json = R"({"key": "value"})";

    REQUIRE(msg_bus.send_message(msg));
    REQUIRE(received_payload == R"({"key": "value"})");
}

TEST_CASE("WidgetMessageBus — broadcast to all subscribers", "[phase14][messaging]")
{
    EventBus bus;
    WidgetMessageBus msg_bus(bus);

    int receive_count = 0;
    msg_bus.subscribe("w2", "*", [&](const WidgetMessage& /*msg*/) { ++receive_count; });
    msg_bus.subscribe("w3", "*", [&](const WidgetMessage& /*msg*/) { ++receive_count; });

    WidgetMessage msg;
    msg.source_widget_id = "w1";
    msg.message_type = "notification";

    auto delivered = msg_bus.broadcast(msg);
    REQUIRE(delivered == 2);
    REQUIRE(receive_count == 2);
}

TEST_CASE("WidgetMessageBus — pattern matching with prefix wildcard", "[phase14][messaging]")
{
    EventBus bus;
    WidgetMessageBus msg_bus(bus);

    std::string received_type;
    msg_bus.subscribe(
        "w2", "data.*", [&](const WidgetMessage& msg) { received_type = msg.message_type; });

    WidgetMessage msg;
    msg.source_widget_id = "w1";
    msg.target_widget_id = "w2";
    msg.message_type = "data.update";

    REQUIRE(msg_bus.send_message(msg));
    REQUIRE(received_type == "data.update");
}

TEST_CASE("WidgetMessageBus — unsubscribe widget", "[phase14][messaging]")
{
    EventBus bus;
    WidgetMessageBus msg_bus(bus);

    msg_bus.subscribe("w1", "*", [](const WidgetMessage&) {});
    msg_bus.subscribe("w1", "specific", [](const WidgetMessage&) {});

    REQUIRE(msg_bus.subscription_count() == 2);

    auto removed = msg_bus.unsubscribe_widget("w1");
    REQUIRE(removed == 2);
    REQUIRE(msg_bus.subscription_count() == 0);
}

TEST_CASE("WidgetMessageBus — queue overflow protection", "[phase14][messaging]")
{
    EventBus bus;
    WidgetMessageBus msg_bus(bus, 2); // Max 2 delivered messages

    msg_bus.subscribe("w2", "*", [](const WidgetMessage&) {});

    WidgetMessage msg;
    msg.source_widget_id = "w1";
    msg.target_widget_id = "w2";
    msg.message_type = "test";

    REQUIRE(msg_bus.send_message(msg));       // 1
    REQUIRE(msg_bus.send_message(msg));       // 2
    REQUIRE_FALSE(msg_bus.send_message(msg)); // overflow

    REQUIRE(msg_bus.messages_delivered() == 2);
    REQUIRE(msg_bus.messages_dropped() == 1);
}
