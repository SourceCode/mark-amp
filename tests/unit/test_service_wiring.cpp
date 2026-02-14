/// @file test_service_wiring.cpp
/// Integration test verifying all services wire correctly via PluginManager.

#include "core/BuiltInPlugins.h"
#include "core/Config.h"
#include "core/ContextKeyService.h"
#include "core/DecorationService.h"
#include "core/DiagnosticsService.h"
#include "core/EnvironmentService.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ExtensionEvents.h"
#include "core/FeatureRegistry.h"
#include "core/FileSystemProviderRegistry.h"
#include "core/GrammarEngine.h"
#include "core/InputBoxService.h"
#include "core/LanguageProviderRegistry.h"
#include "core/OutputChannelService.h"
#include "core/PluginContext.h"
#include "core/PluginManager.h"
#include "core/ProgressService.h"
#include "core/QuickPickService.h"
#include "core/SnippetEngine.h"
#include "core/StatusBarItemService.h"
#include "core/TaskRunnerService.h"
#include "core/TerminalService.h"
#include "core/TextEditorService.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"
#include "core/WorkspaceService.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

TEST_CASE("ServiceWiring: all extension services instantiate", "[integration][wiring]")
{
    // Core services
    ContextKeyService context_keys;
    OutputChannelService output_channels;
    DiagnosticsService diagnostics;
    TreeDataProviderRegistry tree_registry;
    WebviewService webview;
    DecorationService decorations;
    FileSystemProviderRegistry fs_registry;
    LanguageProviderRegistry lang_registry;
    StatusBarItemService status_bar;

    // P1-P4 services
    SnippetEngine snippets;
    WorkspaceService workspace;
    TextEditorService text_editor;
    ProgressService progress;
    ExtensionEventBus ext_events;
    EnvironmentService environment;
    GrammarEngine grammar;
    TerminalService terminal;
    TaskRunnerService task_runner;
    InputBoxService input_box;
    QuickPickService quick_pick;

    // Verify all instantiate without crashing
    REQUIRE(diagnostics.total_count() == 0);
    REQUIRE(output_channels.channel_names().empty());
    REQUIRE(tree_registry.view_ids().empty());
    REQUIRE(snippets.count() == 0);
    REQUIRE(workspace.workspace_folders().empty());
    REQUIRE(text_editor.active_editor() == nullptr);
    REQUIRE(environment.app_name() == "MarkAmp");
}

TEST_CASE("ServiceWiring: PluginManager receives all ExtensionServices", "[integration][wiring]")
{
    EventBus bus;
    Config config;
    PluginManager pm(bus, config);

    // Create services
    ContextKeyService context_keys;
    OutputChannelService output_channels;
    DiagnosticsService diagnostics;
    TreeDataProviderRegistry tree_registry;
    WebviewService webview;
    DecorationService decorations;
    FileSystemProviderRegistry fs_registry;
    LanguageProviderRegistry lang_registry;
    SnippetEngine snippets;
    WorkspaceService workspace;
    TextEditorService text_editor;
    ProgressService progress;
    EnvironmentService environment;
    GrammarEngine grammar;
    TerminalService terminal;
    TaskRunnerService task_runner;
    InputBoxService input_box;
    QuickPickService quick_pick;

    // Wire all services
    PluginManager::ExtensionServices ext_svc;
    ext_svc.context_key_service = &context_keys;
    ext_svc.output_channel_service = &output_channels;
    ext_svc.diagnostics_service = &diagnostics;
    ext_svc.tree_data_provider_registry = &tree_registry;
    ext_svc.webview_service = &webview;
    ext_svc.decoration_service = &decorations;
    ext_svc.file_system_provider_registry = &fs_registry;
    ext_svc.language_provider_registry = &lang_registry;
    ext_svc.snippet_engine = &snippets;
    ext_svc.workspace_service = &workspace;
    ext_svc.text_editor_service = &text_editor;
    ext_svc.progress_service = &progress;
    ext_svc.environment_service = &environment;
    ext_svc.grammar_engine = &grammar;
    ext_svc.terminal_service = &terminal;
    ext_svc.task_runner_service = &task_runner;
    ext_svc.input_box_service = &input_box;
    ext_svc.quick_pick_service = &quick_pick;

    pm.set_extension_services(ext_svc);

    // Register built-in plugins
    FeatureRegistry features(bus, config);
    register_builtin_plugins(pm, features);

    // activate_all should work without crashing
    pm.activate_all();

    // Verify plugins are registered
    REQUIRE(pm.plugin_count() == 7);
}

TEST_CASE("ServiceWiring: EventBus cross-service event delivery", "[integration][wiring]")
{
    EventBus bus;
    Config config;
    FeatureRegistry features(bus, config);

    // Subscribe to feature toggle events
    std::string toggled_id;
    bool toggled_enabled = true;
    auto sub = bus.subscribe<FeatureToggledEvent>(
        [&](const FeatureToggledEvent& evt)
        {
            toggled_id = evt.feature_id;
            toggled_enabled = evt.enabled;
        });

    // Register and disable a feature — should fire event through EventBus
    features.register_feature({"test-feature", "Test Feature", "Test", true});
    features.disable("test-feature");

    REQUIRE(toggled_id == "test-feature");
    REQUIRE(toggled_enabled == false);
}
