#include "core/ContextKeyService.h"
#include "core/DecorationService.h"
#include "core/DiagnosticsService.h"
#include "core/ExtensionHostRecovery.h"
#include "core/ExtensionRecommendations.h"
#include "core/ExtensionSandbox.h"
#include "core/ExtensionTelemetry.h"
#include "core/FileSystemProviderRegistry.h"
#include "core/LanguageProviderRegistry.h"
#include "core/OutputChannelService.h"
#include "core/PluginContext.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"
#include "core/WhenClause.h"
#include "ui/OutputPanel.h"
#include "ui/ProblemsPanel.h"
#include "ui/TreeViewHost.h"
#include "ui/WalkthroughPanel.h"
#include "ui/WebviewHostPanel.h"

#include <catch2/catch_test_macros.hpp>

#include <chrono>
#include <stdexcept>
#include <string>
#include <vector>

using namespace markamp::core;
using namespace markamp::ui;

// ══════════════════════════════════════════
// Integration: Full Extension Lifecycle
// ══════════════════════════════════════════

TEST_CASE("Integration: extension activation round-trip", "[integration]")
{
    // 1. Set up all services (simulating what PluginManager would do)
    ContextKeyService ctx_keys;
    OutputChannelService output;
    DiagnosticsService diagnostics;
    TreeDataProviderRegistry tree_reg;
    WebviewService webviews;
    DecorationService decorations;
    FileSystemProviderRegistry fs_reg;
    LanguageProviderRegistry lang_reg;

    // 2. Create plugin context
    PluginContext plugin_ctx{
        .extension_id = "publisher.my-extension",
        .extension_path = "/extensions/my-extension",
        .global_storage_path = "/storage/global",
        .workspace_storage_path = "/storage/workspace",
        .log_path = "/logs",
        .context_key_service = &ctx_keys,
        .output_channel_service = &output,
        .diagnostics_service = &diagnostics,
        .tree_data_provider_registry = &tree_reg,
        .webview_service = &webviews,
        .decoration_service = &decorations,
        .file_system_provider_registry = &fs_reg,
        .language_provider_registry = &lang_reg,
    };

    // 3. Simulate extension activate()
    plugin_ctx.context_key_service->set_context("myExt.activated", true);
    auto* channel = plugin_ctx.output_channel_service->create_channel("My Extension");
    channel->append_line("Extension activated successfully");

    // 4. Verify context keys
    REQUIRE(plugin_ctx.context_key_service->get_bool("myExt.activated"));

    // 5. Verify output channel content
    REQUIRE(channel->content() == "Extension activated successfully\n");

    // 6. Set some diagnostics
    plugin_ctx.diagnostics_service->set(
        "file:///test.md",
        {{.range = {}, .message = "Missing heading", .severity = DiagnosticSeverity::kWarning}});

    // 7. Create a webview
    auto* panel = plugin_ctx.webview_service->create_panel(
        "myExt.preview", "Preview", {.enable_scripts = true});
    panel->set_html("<h1>Hello from extension</h1>");

    // 8. Verify diagnostics
    REQUIRE(plugin_ctx.diagnostics_service->total_count() == 1);
    REQUIRE(panel->html() == "<h1>Hello from extension</h1>");
}

TEST_CASE("Integration: when-clause with context keys round-trip", "[integration]")
{
    ContextKeyService ctx_keys;
    ctx_keys.set_context("editorLangId", std::string("markdown"));
    ctx_keys.set_context("editorHasSelection", true);

    auto expr = WhenClauseParser::parse("editorLangId == 'markdown' && editorHasSelection");
    REQUIRE(expr != nullptr);

    REQUIRE(WhenClauseEvaluator::evaluate(expr, ctx_keys));

    // Change context
    ctx_keys.set_context("editorHasSelection", false);
    REQUIRE_FALSE(WhenClauseEvaluator::evaluate(expr, ctx_keys));
}

TEST_CASE("Integration: output channel → OutputPanel", "[integration]")
{
    OutputChannelService output_svc;
    output_svc.create_channel("Build");
    output_svc.get_channel("Build")->append_line("Compiling...");
    output_svc.create_channel("Git");
    output_svc.get_channel("Git")->append_line("Fetching...");

    OutputPanel panel;
    panel.set_service(&output_svc);

    REQUIRE(panel.channel_names().size() == 2);
    panel.set_active_channel("Build");
    REQUIRE(panel.active_content() == "Compiling...\n");
}

TEST_CASE("Integration: diagnostics → ProblemsPanel", "[integration]")
{
    DiagnosticsService diag_svc;
    diag_svc.set("file:///a.md",
                 {{.range = {}, .message = "Error", .severity = DiagnosticSeverity::kError}});
    diag_svc.set("file:///b.md",
                 {{.range = {}, .message = "Warning", .severity = DiagnosticSeverity::kWarning},
                  {.range = {}, .message = "Info", .severity = DiagnosticSeverity::kInformation}});

    ProblemsPanel panel;
    panel.set_service(&diag_svc);

    REQUIRE(panel.error_count() == 1);
    REQUIRE(panel.warning_count() == 1);
    REQUIRE(panel.info_count() == 1);

    auto all_problems = panel.problems();
    REQUIRE(all_problems.size() == 3);
}

TEST_CASE("Integration: webview → WebviewHostPanel", "[integration]")
{
    WebviewService web_svc;
    web_svc.create_panel("myExt.preview", "Preview", {.enable_scripts = true});
    web_svc.get_panel("myExt.preview")->set_html("<p>Test</p>");

    WebviewHostPanel host;
    host.set_service(&web_svc);
    host.set_active_panel("myExt.preview");

    REQUIRE(host.active_html() == "<p>Test</p>");
    REQUIRE(host.scripts_enabled());

    std::string received;
    web_svc.get_panel("myExt.preview")
        ->on_did_receive_message([&received](const std::string& msg) { received = msg; });
    host.post_message(R"({"cmd":"refresh"})");
    REQUIRE(received == R"({"cmd":"refresh"})");
}

TEST_CASE("Integration: crash isolation", "[integration]")
{
    ExtensionHostRecovery recovery(2);

    // First error
    bool result =
        recovery.execute_safely("bad.extension", []() { throw std::runtime_error("crash 1"); });
    REQUIRE_FALSE(result);
    REQUIRE(recovery.get_errors("bad.extension").size() == 1);
    REQUIRE_FALSE(recovery.is_disabled("bad.extension"));

    // Second error → auto-disable
    result =
        recovery.execute_safely("bad.extension", []() { throw std::runtime_error("crash 2"); });
    REQUIRE_FALSE(result);
    REQUIRE(recovery.is_disabled("bad.extension"));

    // Further calls are blocked
    auto blocked = recovery.execute_safely("bad.extension", []() {});
    REQUIRE_FALSE(blocked);
}

TEST_CASE("Integration: extension telemetry", "[integration]")
{
    ExtensionTelemetry telemetry;
    telemetry.record_activation("my.extension", std::chrono::milliseconds(150));
    telemetry.record_api_call("my.extension");
    telemetry.record_api_call("my.extension");
    telemetry.record_command("my.extension");
    telemetry.record_error("my.extension");

    const auto* data = telemetry.get_telemetry("my.extension");
    REQUIRE(data != nullptr);
    REQUIRE(data->activation_time == std::chrono::milliseconds(150));
    REQUIRE(data->api_call_count == 2);
    REQUIRE(data->command_execution_count == 1);
    REQUIRE(data->error_count == 1);
}

TEST_CASE("Integration: extension sandbox permissions", "[integration]")
{
    ExtensionSandbox sandbox;
    sandbox.set_permissions("my.extension",
                            {ExtensionPermission::kFilesystem, ExtensionPermission::kClipboard});

    REQUIRE(sandbox.has_permission("my.extension", ExtensionPermission::kFilesystem));
    REQUIRE(sandbox.has_permission("my.extension", ExtensionPermission::kClipboard));
    REQUIRE_FALSE(sandbox.has_permission("my.extension", ExtensionPermission::kNetwork));

    REQUIRE(ExtensionSandbox::to_string(ExtensionPermission::kFilesystem) == "filesystem");
    REQUIRE(ExtensionSandbox::from_string("network") == ExtensionPermission::kNetwork);
}

TEST_CASE("Integration: extension recommendations JSON", "[integration]")
{
    ExtensionRecommendations recs;
    nlohmann::json json_data = {{"recommendations", {"publisher.ext1", "publisher.ext2"}},
                                {"unwantedRecommendations", {"publisher.bad"}}};
    recs.load_from_json(json_data);

    REQUIRE(recs.recommended().size() == 2);
    REQUIRE(recs.is_recommended("publisher.ext1"));
    REQUIRE(recs.is_unwanted("publisher.bad"));
    REQUIRE_FALSE(recs.is_recommended("publisher.bad"));
}

TEST_CASE("Integration: walkthrough completion tracking", "[integration]")
{
    WalkthroughPanel panel;
    std::vector<ExtensionWalkthrough> walkthroughs = {
        {.walkthrough_id = "myExt.gettingStarted",
         .title = "Get Started",
         .steps =
             {
                 {.step_id = "step1", .title = "Install"},
                 {.step_id = "step2", .title = "Configure"},
                 {.step_id = "step3", .title = "Run"},
             }},
    };
    panel.set_walkthroughs(std::move(walkthroughs));

    REQUIRE(panel.active_walkthrough() == "myExt.gettingStarted");
    REQUIRE(panel.completion_progress("myExt.gettingStarted") == 0.0);

    panel.complete_step("myExt.gettingStarted", "step1");
    REQUIRE(panel.is_step_completed("myExt.gettingStarted", "step1"));
    REQUIRE_FALSE(panel.is_step_completed("myExt.gettingStarted", "step2"));

    // 1 out of 3 done → ~0.333
    double progress = panel.completion_progress("myExt.gettingStarted");
    REQUIRE(progress > 0.3);
    REQUIRE(progress < 0.4);

    panel.complete_step("myExt.gettingStarted", "step2");
    panel.complete_step("myExt.gettingStarted", "step3");
    REQUIRE(panel.completion_progress("myExt.gettingStarted") == 1.0);

    panel.reset_walkthrough("myExt.gettingStarted");
    REQUIRE(panel.completion_progress("myExt.gettingStarted") == 0.0);
}
