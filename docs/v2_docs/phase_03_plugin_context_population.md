# Phase 03: PluginContext Full Population

**Priority:** Critical
**Estimated Scope:** ~4 files affected
**Dependencies:** Phase 02

## Objective

Ensure that when PluginManager::activate_plugin() creates a PluginContext, ALL service pointers are populated from the app-owned service instances -- not just event_bus, config, and register_command_handler.

## Background/Context

Currently in `PluginManager::activate_plugin()` (line 216-228 of PluginManager.cpp), the PluginContext is created with only 3 fields set:
```cpp
PluginContext ctx;
ctx.event_bus = &event_bus_;
ctx.config = &config_;
ctx.register_command_handler = [&entry = *entry_it](...) { ... };
```

The remaining ~25 fields (all service pointers, extension_path, storage paths, execute_command, get_commands) are left as nullptr/empty. This means every plugin that tries to use any API service crashes or gets null.

## Detailed Tasks

### Task 1: Add service pointers to PluginManager

**File:** `/Users/ryanrentfro/code/markamp/src/core/PluginManager.h`

Add private member pointers for all services. Use a struct to group them:
```cpp
/// All extension API service pointers, set by the application before activation.
struct ExtensionServices
{
    ContextKeyService* context_key_service{nullptr};
    OutputChannelService* output_channel_service{nullptr};
    DiagnosticsService* diagnostics_service{nullptr};
    TreeDataProviderRegistry* tree_data_provider_registry{nullptr};
    WebviewService* webview_service{nullptr};
    DecorationService* decoration_service{nullptr};
    FileSystemProviderRegistry* file_system_provider_registry{nullptr};
    LanguageProviderRegistry* language_provider_registry{nullptr};
    SnippetEngine* snippet_engine{nullptr};
    WorkspaceService* workspace_service{nullptr};
    TextEditorService* text_editor_service{nullptr};
    ProgressService* progress_service{nullptr};
    ExtensionEventBus* extension_event_bus{nullptr};
    EnvironmentService* environment_service{nullptr};
    GrammarEngine* grammar_engine{nullptr};
    TerminalService* terminal_service{nullptr};
    TaskRunnerService* task_runner_service{nullptr};
    NotificationServiceAPI* notification_service{nullptr};
    StatusBarItemService* status_bar_item_service{nullptr};
    InputBoxService* input_box_service{nullptr};
    QuickPickService* quick_pick_service{nullptr};
};

ExtensionServices ext_services_;
```

Add a public setter:
```cpp
/// Set all extension API services at once. Call from MarkAmpApp after creating services.
void set_extension_services(const ExtensionServices& services) { ext_services_ = services; }
```

### Task 2: Populate PluginContext in activate_plugin()

**File:** `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`

In `activate_plugin()`, after creating the PluginContext (around line 216), add:
```cpp
// Populate extension API services
ctx.context_key_service = ext_services_.context_key_service;
ctx.output_channel_service = ext_services_.output_channel_service;
ctx.diagnostics_service = ext_services_.diagnostics_service;
ctx.tree_data_provider_registry = ext_services_.tree_data_provider_registry;
ctx.webview_service = ext_services_.webview_service;
ctx.decoration_service = ext_services_.decoration_service;
ctx.file_system_provider_registry = ext_services_.file_system_provider_registry;
ctx.language_provider_registry = ext_services_.language_provider_registry;
ctx.snippet_engine = ext_services_.snippet_engine;
ctx.workspace_service = ext_services_.workspace_service;
ctx.text_editor_service = ext_services_.text_editor_service;
ctx.progress_service = ext_services_.progress_service;
ctx.extension_event_bus = ext_services_.extension_event_bus;
ctx.environment_service = ext_services_.environment_service;
ctx.grammar_engine = ext_services_.grammar_engine;
ctx.terminal_service = ext_services_.terminal_service;
ctx.task_runner_service = ext_services_.task_runner_service;
ctx.notification_service = ext_services_.notification_service;
ctx.status_bar_item_service = ext_services_.status_bar_item_service;
ctx.input_box_service = ext_services_.input_box_service;
ctx.quick_pick_service = ext_services_.quick_pick_service;
```

Also populate `execute_command` and `get_commands`:
```cpp
ctx.execute_command = [this](const std::string& command_id) -> bool {
    for (auto& entry : plugins_) {
        auto handler_it = entry.command_handlers.find(command_id);
        if (handler_it != entry.command_handlers.end()) {
            handler_it->second();
            return true;
        }
    }
    return false;
};

ctx.get_commands = [this]() -> std::vector<std::string> {
    std::vector<std::string> commands;
    for (const auto& entry : plugins_) {
        for (const auto& [cmd_id, handler] : entry.command_handlers) {
            commands.push_back(cmd_id);
        }
    }
    return commands;
};
```

Also populate extension identity fields if manifest is available:
```cpp
if (entry_it->ext_manifest.has_value()) {
    ctx.extension_id = entry_it->ext_manifest->id;
    // Extension paths would be set by the scanner
}
```

### Task 3: Wire services in MarkAmpApp

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

After creating all services (from Phase 02) and before `plugin_manager_->activate_all()`:
```cpp
core::PluginManager::ExtensionServices ext_svcs;
ext_svcs.context_key_service = context_key_service_.get();
ext_svcs.output_channel_service = output_channel_service_.get();
ext_svcs.diagnostics_service = diagnostics_service_.get();
ext_svcs.tree_data_provider_registry = tree_data_provider_registry_.get();
ext_svcs.webview_service = webview_service_.get();
ext_svcs.decoration_service = decoration_service_.get();
ext_svcs.file_system_provider_registry = file_system_provider_registry_.get();
ext_svcs.language_provider_registry = language_provider_registry_.get();
ext_svcs.snippet_engine = snippet_engine_.get();
ext_svcs.workspace_service = workspace_service_.get();
ext_svcs.text_editor_service = text_editor_service_.get();
ext_svcs.progress_service = progress_service_.get();
ext_svcs.extension_event_bus = extension_event_bus_.get();
ext_svcs.environment_service = environment_service_.get();
ext_svcs.grammar_engine = grammar_engine_.get();
ext_svcs.terminal_service = terminal_service_.get();
ext_svcs.task_runner_service = task_runner_service_.get();
ext_svcs.notification_service = notification_service_api_.get();
ext_svcs.status_bar_item_service = status_bar_item_service_.get();
ext_svcs.input_box_service = input_box_service_.get();
ext_svcs.quick_pick_service = quick_pick_service_.get();
plugin_manager_->set_extension_services(ext_svcs);
```

### Task 4: Update existing tests

**File:** `/Users/ryanrentfro/code/markamp/tests/unit/test_plugin_manager_v2.cpp`

Update test setup to use the new `set_extension_services()` API if any tests verify context population.

## Acceptance Criteria

1. Every field in PluginContext is non-null when a plugin's activate() is called
2. execute_command() and get_commands() work across all registered plugins
3. Built-in plugins receive a fully-populated context
4. All existing tests pass
5. No nullptr service pointers in any activated plugin's context

## Testing Requirements

- Add test case to `test_plugin_manager_v2.cpp` verifying all context fields are set
- Add test case verifying execute_command() can invoke cross-plugin commands
- Existing test suite passes
