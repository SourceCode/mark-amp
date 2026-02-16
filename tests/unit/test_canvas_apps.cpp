#include "canvas/AppWidgetObject.h"
#include "canvas/CanvasTypes.h"
#include "core/CanvasAppManifest.h"
#include "core/CanvasIntegrationService.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionManifest.h"
#include "core/ExtensionSandbox.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::canvas;

// ============================================================================
// Section 1: Marketplace Events
// ============================================================================

TEST_CASE("Canvas marketplace events compile and construct", "[canvas_apps][events]")
{
    SECTION("CanvasAppRegisteredEvent")
    {
        events::CanvasAppRegisteredEvent evt;
        evt.app_id = "app_1";
        evt.app_name = "Test App";
        evt.extension_id = "test.ext";
        CHECK(evt.app_id == "app_1");
        CHECK(evt.app_name == "Test App");
        CHECK(evt.extension_id == "test.ext");
    }

    SECTION("CanvasWidgetInsertedEvent")
    {
        events::CanvasWidgetInsertedEvent evt;
        evt.widget_id = "w1";
        evt.app_id = "app_1";
        evt.object_id = "obj_42";
        CHECK(evt.widget_id == "w1");
        CHECK(evt.object_id == "obj_42");
    }

    SECTION("CanvasWidgetSyncStartedEvent")
    {
        events::CanvasWidgetSyncStartedEvent evt;
        evt.widget_id = "w1";
        evt.provider_id = "prov_1";
        CHECK(evt.provider_id == "prov_1");
    }

    SECTION("CanvasWidgetSyncCompletedEvent")
    {
        events::CanvasWidgetSyncCompletedEvent evt;
        evt.items_synced = 42;
        CHECK(evt.items_synced == 42);
    }

    SECTION("CanvasWidgetSyncFailedEvent")
    {
        events::CanvasWidgetSyncFailedEvent evt;
        evt.error_message = "timeout";
        CHECK(evt.error_message == "timeout");
    }

    SECTION("CanvasWidgetPermissionDeniedEvent")
    {
        events::CanvasWidgetPermissionDeniedEvent evt;
        evt.requested_scope = "canvas.write";
        evt.denied_reason = "not granted";
        CHECK(evt.requested_scope == "canvas.write");
    }
}

// ============================================================================
// Section 2: Canvas Permissions
// ============================================================================

TEST_CASE("Canvas permission scopes in ExtensionSandbox", "[canvas_apps][permissions]")
{
    ExtensionSandbox sandbox;

    SECTION("Canvas scopes round-trip through from_string/to_string")
    {
        auto read = ExtensionSandbox::from_string("canvas.read");
        CHECK(read == ExtensionPermission::kCanvasRead);
        CHECK(ExtensionSandbox::to_string(read) == "canvas.read");

        auto write = ExtensionSandbox::from_string("canvas.write");
        CHECK(write == ExtensionPermission::kCanvasWrite);
        CHECK(ExtensionSandbox::to_string(write) == "canvas.write");

        auto sel = ExtensionSandbox::from_string("canvas.selection");
        CHECK(sel == ExtensionPermission::kCanvasSelection);

        auto com = ExtensionSandbox::from_string("canvas.comments");
        CHECK(com == ExtensionPermission::kCanvasComments);

        auto tmpl = ExtensionSandbox::from_string("canvas.templates");
        CHECK(tmpl == ExtensionPermission::kCanvasTemplates);

        auto net = ExtensionSandbox::from_string("canvas.network");
        CHECK(net == ExtensionPermission::kCanvasNetwork);
    }

    SECTION("Grant and check canvas permissions")
    {
        sandbox.set_permissions(
            "test.ext", {ExtensionPermission::kCanvasRead, ExtensionPermission::kCanvasWrite});

        CHECK(sandbox.has_permission("test.ext", ExtensionPermission::kCanvasRead));
        CHECK(sandbox.has_permission("test.ext", ExtensionPermission::kCanvasWrite));
        CHECK_FALSE(sandbox.has_permission("test.ext", ExtensionPermission::kCanvasNetwork));
    }
}

// ============================================================================
// Section 3: CanvasAppManifest
// ============================================================================

TEST_CASE("CanvasAppManifest structure", "[canvas_apps][manifest]")
{
    CanvasAppManifest manifest;

    SECTION("Empty manifest")
    {
        CHECK(manifest.is_empty());
        CHECK(manifest.contribution_count() == 0);
    }

    SECTION("Manifest with contributions")
    {
        manifest.tools.push_back({"tool_1", "Laser Pointer", "/icons/laser.png", "annotate", 0});
        manifest.widgets.push_back({"widget_1",
                                    "Data Table",
                                    "Shows tabular data",
                                    "/icons/table.png",
                                    "sql_provider",
                                    "{}",
                                    300,
                                    200,
                                    true});
        manifest.templates.push_back({"tmpl_1",
                                      "Brainstorm",
                                      "Group brainstorming board",
                                      "brainstorming",
                                      "extension",
                                      "{}"});
        manifest.inspectors.push_back({"insp_1", "Style Inspector", "*", 0});
        manifest.actions.push_back({"act_1", "Export Selection", "", "selection", "", 0});

        CHECK_FALSE(manifest.is_empty());
        CHECK(manifest.contribution_count() == 5);
    }

    SECTION("ExtensionManifest has canvas_app field")
    {
        ExtensionManifest ext;
        ext.name = "test-ext";
        ext.version = "1.0.0";
        ext.publisher = "testpub";
        ext.canvas_app.tools.push_back({"t1", "Tool 1", "", "draw", 0});
        CHECK_FALSE(ext.canvas_app.is_empty());
        CHECK(ext.canvas_app.contribution_count() == 1);
    }
}

// ============================================================================
// Section 4: AppWidgetObject
// ============================================================================

TEST_CASE("AppWidgetObject canvas object", "[canvas_apps][widget]")
{
    AppWidgetObject widget;

    SECTION("Default state")
    {
        CHECK(widget.type_name() == "app_widget");
        CHECK(widget.widget_width() == 200);
        CHECK(widget.widget_height() == 150);
        CHECK(widget.sync_status() == WidgetSyncStatus::kIdle);
        CHECK(widget.bindings().empty());
    }

    SECTION("Setters and getters")
    {
        widget.set_widget_id("w1");
        widget.set_app_id("app_1");
        widget.set_provider_id("prov_1");
        widget.set_state_json("{\"count\": 0}");
        widget.set_config_json("{\"refresh\": 30}");
        widget.set_widget_size(400, 300);
        widget.set_sync_status(WidgetSyncStatus::kSyncing);

        CHECK(widget.widget_id() == "w1");
        CHECK(widget.app_id() == "app_1");
        CHECK(widget.provider_id() == "prov_1");
        CHECK(widget.state_json() == "{\"count\": 0}");
        CHECK(widget.config_json() == "{\"refresh\": 30}");
        CHECK(widget.widget_width() == 400);
        CHECK(widget.widget_height() == 300);
        CHECK(widget.sync_status() == WidgetSyncStatus::kSyncing);
    }

    SECTION("local_bounds matches dimensions")
    {
        widget.set_widget_size(250, 180);
        auto bounds = widget.local_bounds();
        CHECK(bounds.min_x == 0.0);
        CHECK(bounds.min_y == 0.0);
        CHECK(bounds.max_x == 250.0);
        CHECK(bounds.max_y == 180.0);
    }

    SECTION("clone deep copies")
    {
        widget.set_widget_id("w1");
        widget.set_app_id("app_1");
        widget.set_state_json("{\"data\": true}");
        widget.add_binding({"title", "issues[0].title", ""});

        auto cloned = widget.clone();
        auto* cloned_widget = dynamic_cast<AppWidgetObject*>(cloned.get());
        REQUIRE(cloned_widget != nullptr);
        CHECK(cloned_widget->widget_id() == "w1");
        CHECK(cloned_widget->app_id() == "app_1");
        CHECK(cloned_widget->state_json() == "{\"data\": true}");
        CHECK(cloned_widget->bindings().size() == 1);
        CHECK(cloned_widget->bindings()[0].field_name == "title");
    }

    SECTION("Data bindings")
    {
        widget.add_binding({"title", "data.title", ""});
        widget.add_binding({"count", "data.count", "parseInt"});
        CHECK(widget.bindings().size() == 2);

        widget.clear_bindings();
        CHECK(widget.bindings().empty());
    }

    SECTION("CanvasObjectType is AppWidget")
    {
        CHECK(widget.type() == CanvasObjectType::AppWidget);
    }
}

// ============================================================================
// Section 5: CanvasIntegrationService
// ============================================================================

TEST_CASE("CanvasIntegrationService lifecycle", "[canvas_apps][integration]")
{
    EventBus bus;
    ExtensionSandbox sandbox;
    CanvasIntegrationService service(bus, sandbox);

    SECTION("Register and find apps")
    {
        CanvasAppManifest manifest;
        manifest.tools.push_back({"t1", "Tool", "", "draw", 0});

        auto app_id = service.register_app("ext_1", manifest);
        CHECK_FALSE(app_id.empty());
        CHECK(service.app_count() == 1);

        auto* app = service.find_app(app_id);
        REQUIRE(app != nullptr);
        CHECK(app->extension_id == "ext_1");
        CHECK(app->manifest.tools.size() == 1);
        CHECK(app->is_active);
    }

    SECTION("Unregister app")
    {
        auto app_id = service.register_app("ext_1", {});
        CHECK(service.app_count() == 1);

        service.unregister_app(app_id);
        CHECK(service.app_count() == 0);
        CHECK(service.find_app(app_id) == nullptr);
    }

    SECTION("Sync lifecycle")
    {
        auto job_id = service.sync_widget("w1", "prov_1");
        CHECK_FALSE(job_id.empty());

        auto* job = service.get_sync_job(job_id);
        REQUIRE(job != nullptr);
        CHECK(job->status == SyncJobStatus::kRunning);
        CHECK(job->widget_id == "w1");
        CHECK(service.active_sync_count() == 1);

        service.complete_sync(job_id, 10);
        job = service.get_sync_job(job_id);
        CHECK(job->status == SyncJobStatus::kCompleted);
        CHECK(job->items_synced == 10);
        CHECK(service.active_sync_count() == 0);
    }

    SECTION("Sync failure")
    {
        auto job_id = service.sync_widget("w2", "prov_2");
        service.fail_sync(job_id, "connection refused");

        auto* job = service.get_sync_job(job_id);
        CHECK(job->status == SyncJobStatus::kFailed);
        CHECK(job->error_message == "connection refused");
    }

    SECTION("Permission check delegates to sandbox")
    {
        sandbox.set_permissions(
            "ext_1", {ExtensionPermission::kCanvasRead, ExtensionPermission::kCanvasWrite});

        CHECK(service.check_permission("ext_1", ExtensionPermission::kCanvasRead));
        CHECK(service.check_permission("ext_1", ExtensionPermission::kCanvasWrite));
        CHECK_FALSE(service.check_permission("ext_1", ExtensionPermission::kCanvasNetwork));
        CHECK_FALSE(service.check_permission("ext_2", ExtensionPermission::kCanvasRead));
    }
}

// ============================================================================
// Section 6: Event bus integration
// ============================================================================

TEST_CASE("CanvasIntegrationService publishes events", "[canvas_apps][events]")
{
    EventBus bus;
    ExtensionSandbox sandbox;
    CanvasIntegrationService service(bus, sandbox);

    SECTION("Register app emits CanvasAppRegisteredEvent")
    {
        bool received = false;
        auto sub = bus.subscribe<events::CanvasAppRegisteredEvent>(
            [&](const events::CanvasAppRegisteredEvent& evt)
            {
                received = true;
                CHECK(evt.extension_id == "ext_1");
            });

        service.register_app("ext_1", {});
        CHECK(received);
    }

    SECTION("Sync start emits CanvasWidgetSyncStartedEvent")
    {
        bool received = false;
        auto sub = bus.subscribe<events::CanvasWidgetSyncStartedEvent>(
            [&](const events::CanvasWidgetSyncStartedEvent& evt)
            {
                received = true;
                CHECK(evt.widget_id == "w1");
            });

        service.sync_widget("w1", "prov_1");
        CHECK(received);
    }

    SECTION("Sync complete emits CanvasWidgetSyncCompletedEvent")
    {
        bool received = false;
        auto sub = bus.subscribe<events::CanvasWidgetSyncCompletedEvent>(
            [&](const events::CanvasWidgetSyncCompletedEvent& evt)
            {
                received = true;
                CHECK(evt.items_synced == 5);
            });

        auto job_id = service.sync_widget("w1", "prov_1");
        service.complete_sync(job_id, 5);
        CHECK(received);
    }

    SECTION("Sync failure emits CanvasWidgetSyncFailedEvent")
    {
        bool received = false;
        auto sub = bus.subscribe<events::CanvasWidgetSyncFailedEvent>(
            [&](const events::CanvasWidgetSyncFailedEvent& evt)
            {
                received = true;
                CHECK(evt.error_message == "timeout");
            });

        auto job_id = service.sync_widget("w1", "prov_1");
        service.fail_sync(job_id, "timeout");
        CHECK(received);
    }
}
