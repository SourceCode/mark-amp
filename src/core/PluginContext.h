#pragma once

#include "ContextKeyService.h"
#include "DecorationService.h"
#include "DiagnosticsService.h"
#include "FileSystemProviderRegistry.h"
#include "InputBoxService.h"
#include "LanguageProviderRegistry.h"
#include "NotificationService.h"
#include "OutputChannelService.h"
#include "QuickPickService.h"
#include "StatusBarItemService.h"
#include "TreeDataProviderRegistry.h"
#include "WebviewService.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class FeatureRegistry;

// Forward declarations for P1–P4 services
class SnippetEngine;
class WorkspaceService;
class TextEditorService;
class ProgressService;
class ExtensionEventBus;
class EnvironmentService;
class GrammarEngine;
class TerminalService;
class TaskRunnerService;

/// Unified plugin context providing access to all VS Code-equivalent API services.
/// This is the single source of truth passed to plugins during activation, giving
/// them access to the full extension API surface.
///
/// Combines the original IPlugin.h PluginContext (EventBus, Config, command handler)
/// with the enhanced service-rich context and all P1–P4 extension services.
struct PluginContext
{
    // ── Extension Identity & Paths ──

    /// Unique extension ID (publisher.name format).
    std::string extension_id;

    /// Absolute path to the extension's installation directory.
    std::string extension_path;

    /// Path to the extension's global storage directory.
    std::string global_storage_path;

    /// Path to the extension's workspace storage directory.
    std::string workspace_storage_path;

    /// Path to the extension's log directory.
    std::string log_path;

    // ── Core Application Services (from original IPlugin.h context) ──

    /// Event bus for subscribing to application-wide events.
    EventBus* event_bus{nullptr};

    /// Application configuration (read/write settings).
    Config* config{nullptr};

    /// Feature registry for toggling features.
    FeatureRegistry* feature_registry{nullptr};

    /// Register a command handler. The plugin manager will wire this to
    /// the command palette and shortcut manager based on the manifest.
    std::function<void(const std::string& command_id, std::function<void()> handler)>
        register_command_handler;

    /// Execute a registered command by ID. Returns true if found and executed.
    /// Mirrors VS Code's `commands.executeCommand()`.
    std::function<bool(const std::string& command_id)> execute_command;

    /// Get a list of all registered command IDs.
    /// Mirrors VS Code's `commands.getCommands()`.
    std::function<std::vector<std::string>()> get_commands;

    /// Per-workspace key-value state (survives across sessions, scoped to workspace).
    std::unordered_map<std::string, std::string> workspace_state;

    /// Global key-value state (survives across sessions, shared across workspaces).
    std::unordered_map<std::string, std::string> global_state;

    // ── Extension API Services ──

    ContextKeyService* context_key_service{nullptr};
    OutputChannelService* output_channel_service{nullptr};
    DiagnosticsService* diagnostics_service{nullptr};
    TreeDataProviderRegistry* tree_data_provider_registry{nullptr};
    WebviewService* webview_service{nullptr};
    DecorationService* decoration_service{nullptr};
    FileSystemProviderRegistry* file_system_provider_registry{nullptr};
    LanguageProviderRegistry* language_provider_registry{nullptr};

    // ── P1: Snippet Engine ──

    SnippetEngine* snippet_engine{nullptr};

    // ── P2: Workspace & Editor Services ──

    WorkspaceService* workspace_service{nullptr};
    TextEditorService* text_editor_service{nullptr};
    ProgressService* progress_service{nullptr};

    // ── P2+P3: Events & Environment ──

    ExtensionEventBus* extension_event_bus{nullptr};
    EnvironmentService* environment_service{nullptr};

    // ── Extension-Facing Window Services ──

    NotificationService* notification_service{nullptr};
    StatusBarItemService* status_bar_item_service{nullptr};
    InputBoxService* input_box_service{nullptr};
    QuickPickService* quick_pick_service{nullptr};

    // ── P3+P4: Stubs (N/A for Markdown editor) ──

    GrammarEngine* grammar_engine{nullptr};
    TerminalService* terminal_service{nullptr};
    TaskRunnerService* task_runner_service{nullptr};
};

} // namespace markamp::core
