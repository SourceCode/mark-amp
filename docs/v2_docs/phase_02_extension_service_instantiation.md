# Phase 02: Extension Service Instantiation & Wiring

**Priority:** Critical
**Estimated Scope:** ~8 files affected
**Dependencies:** Phase 01

## Objective

Instantiate all 15 P1-P4 extension services in the application lifecycle and store them as owned members, making them available for injection into PluginContext during plugin activation.

## Background/Context

PluginContext.h declares 15 service pointers that are NEVER instantiated anywhere in the application:

- `SnippetEngine*` (P1)
- `WorkspaceService*` (P2)
- `TextEditorService*` (P2)
- `ProgressService*` (P2)
- `ExtensionEventBus*` (P2/P3)
- `EnvironmentService*` (P3)
- `GrammarEngine*` (P3)
- `TerminalService*` (P4)
- `TaskRunnerService*` (P4)
- `NotificationServiceAPI*` (needs EventBus)
- `StatusBarItemService*`
- `InputBoxService*`
- `QuickPickService*`
- `ContextKeyService*`
- Also: `OutputChannelService*`, `DiagnosticsService*`, `TreeDataProviderRegistry*`, `WebviewService*`, `DecorationService*`, `FileSystemProviderRegistry*`, `LanguageProviderRegistry*`

All of these have working implementations with test coverage, but `MarkAmpApp::OnInit()` only creates EventBus, Config, ThemeRegistry, ThemeEngine, FeatureRegistry, PluginManager, and MermaidRenderer. The extension services are orphaned.

## Detailed Tasks

### Task 1: Add service member declarations to MarkAmpApp

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.h`

Add after the existing `plugin_manager_` member (line ~59):
```cpp
// Extension API services (injected into PluginContext)
std::unique_ptr<core::ContextKeyService> context_key_service_;
std::unique_ptr<core::OutputChannelService> output_channel_service_;
std::unique_ptr<core::DiagnosticsService> diagnostics_service_;
std::unique_ptr<core::TreeDataProviderRegistry> tree_data_provider_registry_;
std::unique_ptr<core::WebviewService> webview_service_;
std::unique_ptr<core::DecorationService> decoration_service_;
std::unique_ptr<core::FileSystemProviderRegistry> file_system_provider_registry_;
std::unique_ptr<core::LanguageProviderRegistry> language_provider_registry_;
std::unique_ptr<core::SnippetEngine> snippet_engine_;
std::unique_ptr<core::WorkspaceService> workspace_service_;
std::unique_ptr<core::TextEditorService> text_editor_service_;
std::unique_ptr<core::ProgressService> progress_service_;
std::unique_ptr<core::ExtensionEventBus> extension_event_bus_;
std::unique_ptr<core::EnvironmentService> environment_service_;
std::unique_ptr<core::GrammarEngine> grammar_engine_;
std::unique_ptr<core::TerminalService> terminal_service_;
std::unique_ptr<core::TaskRunnerService> task_runner_service_;
std::unique_ptr<core::NotificationServiceAPI> notification_service_api_;
std::unique_ptr<core::StatusBarItemService> status_bar_item_service_;
std::unique_ptr<core::InputBoxService> input_box_service_;
std::unique_ptr<core::QuickPickService> quick_pick_service_;
```

Add forward declarations at top of MarkAmpApp.h:
```cpp
namespace markamp::core {
class ContextKeyService;
class OutputChannelService;
class DiagnosticsService;
class TreeDataProviderRegistry;
class WebviewService;
class DecorationService;
class FileSystemProviderRegistry;
class LanguageProviderRegistry;
class SnippetEngine;
class WorkspaceService;
class TextEditorService;
class ProgressService;
class ExtensionEventBus;
class EnvironmentService;
class GrammarEngine;
class TerminalService;
class TaskRunnerService;
class NotificationServiceAPI;
class StatusBarItemService;
class InputBoxService;
class QuickPickService;
}
```

### Task 2: Instantiate services in MarkAmpApp::OnInit()

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Add includes at top:
```cpp
#include "core/ContextKeyService.h"
#include "core/OutputChannelService.h"
#include "core/DiagnosticsService.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"
#include "core/DecorationService.h"
#include "core/FileSystemProviderRegistry.h"
#include "core/LanguageProviderRegistry.h"
#include "core/SnippetEngine.h"
#include "core/WorkspaceService.h"
#include "core/TextEditorService.h"
#include "core/ProgressService.h"
#include "core/ExtensionEvents.h"
#include "core/EnvironmentService.h"
#include "core/GrammarEngine.h"
#include "core/TerminalService.h"
#include "core/TaskRunnerService.h"
#include "core/NotificationServiceAPI.h"
#include "core/StatusBarItemService.h"
#include "core/InputBoxService.h"
#include "core/QuickPickService.h"
```

Insert after step 7 (ThemeEngine initialization) and before step 8 (plugin system):
```cpp
// 7b. Initialize extension API services
context_key_service_ = std::make_unique<core::ContextKeyService>();
output_channel_service_ = std::make_unique<core::OutputChannelService>();
diagnostics_service_ = std::make_unique<core::DiagnosticsService>();
tree_data_provider_registry_ = std::make_unique<core::TreeDataProviderRegistry>();
webview_service_ = std::make_unique<core::WebviewService>();
decoration_service_ = std::make_unique<core::DecorationService>();
file_system_provider_registry_ = std::make_unique<core::FileSystemProviderRegistry>();
language_provider_registry_ = std::make_unique<core::LanguageProviderRegistry>();
snippet_engine_ = std::make_unique<core::SnippetEngine>();
workspace_service_ = std::make_unique<core::WorkspaceService>();
text_editor_service_ = std::make_unique<core::TextEditorService>();
progress_service_ = std::make_unique<core::ProgressService>();
extension_event_bus_ = std::make_unique<core::ExtensionEventBus>();
environment_service_ = std::make_unique<core::EnvironmentService>();
grammar_engine_ = std::make_unique<core::GrammarEngine>();
terminal_service_ = std::make_unique<core::TerminalService>();
task_runner_service_ = std::make_unique<core::TaskRunnerService>();
notification_service_api_ = std::make_unique<core::NotificationServiceAPI>(*event_bus_);
status_bar_item_service_ = std::make_unique<core::StatusBarItemService>();
input_box_service_ = std::make_unique<core::InputBoxService>();
quick_pick_service_ = std::make_unique<core::QuickPickService>();
MARKAMP_LOG_INFO("Extension API services initialized (21 services)");
```

### Task 3: Wire services into PluginManager

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

After creating plugin_manager_ (step 8), wire the Tier 3 services:
```cpp
plugin_manager_->set_status_bar_service(status_bar_item_service_.get());
plugin_manager_->set_theme_registry(theme_registry_.get());
plugin_manager_->set_tree_registry(tree_data_provider_registry_.get());
```

### Task 4: Destroy services in reverse order in OnExit()

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

Add before `plugin_manager_.reset()` in OnExit():
```cpp
// Destroy extension services in reverse creation order
quick_pick_service_.reset();
input_box_service_.reset();
status_bar_item_service_.reset();
notification_service_api_.reset();
task_runner_service_.reset();
terminal_service_.reset();
grammar_engine_.reset();
environment_service_.reset();
extension_event_bus_.reset();
progress_service_.reset();
text_editor_service_.reset();
workspace_service_.reset();
snippet_engine_.reset();
language_provider_registry_.reset();
file_system_provider_registry_.reset();
decoration_service_.reset();
webview_service_.reset();
tree_data_provider_registry_.reset();
diagnostics_service_.reset();
output_channel_service_.reset();
context_key_service_.reset();
```

### Task 5: Fix duplicate step 10 comment

**File:** `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

At line 118, the MermaidRenderer initialization is labeled "step 10". At line 124, AppReadyEvent publishing is also labeled "step 10". Fix to sequential numbering (11, 12, etc.).

## Acceptance Criteria

1. All 21 extension services are instantiated during OnInit()
2. Services are destroyed in proper reverse order during OnExit()
3. No memory leaks (services are owned by unique_ptr)
4. Application compiles and runs without errors
5. All existing tests still pass
6. PluginManager has status_bar_service, theme_registry, and tree_registry set

## Testing Requirements

- Application starts and shuts down cleanly with all services initialized
- Add a log verification test or manual check that all 21 "initialized" log lines appear
- Existing 21+ test targets all pass
