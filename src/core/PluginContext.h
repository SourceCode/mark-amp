#pragma once

// Phase 15: Forward declarations replace direct includes.
// PluginContext stores only T* pointers, so incomplete types are sufficient.
// This reduces the include chain from 12 headers to zero,
// speeding up incremental builds for any file that includes PluginContext.h.

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class FeatureRegistry;
class TagService;
class SearchEngine;
class GraphEngine;
class DailyNoteService;
class NoteTemplateEngine;
class EmbedResolver;
class LinkSuggestionService;
class OutlinePanelController;

// Forward declarations for extension API services (Phase 15: replaced #includes)
class ContextKeyService;
class OutputChannelService;
class DiagnosticsService;
class TreeDataProviderRegistry;
class WebviewService;
class DecorationService;
class FileSystemProviderRegistry;
class LanguageProviderRegistry;
class NotificationService;
class StatusBarItemService;
class InputBoxService;
class QuickPickService;

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
class VaultService;
class BacklinkIndex;

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

    // ── V4 Phase 02: Vault Management ──

    VaultService* vault_service{nullptr};

    // ── V4 Phase 03: Backlink Index ──

    BacklinkIndex* backlink_index{nullptr};

    // ── V4 Phase 04: Tag System ──

    TagService* tag_service{nullptr};

    // ── V4 Phase 05: Search Engine ──

    SearchEngine* search_engine{nullptr};

    // ── V4 Phase 09: Daily Notes ──

    DailyNoteService* daily_note_service{nullptr};

    // ── V4 Phase 10: Embed Resolver ──

    EmbedResolver* embed_resolver{nullptr};

    // ── V4 Phase 11: Link Suggestion Service ──

    LinkSuggestionService* link_suggestion_service{nullptr};

    // ── V4 Phase 12: Outline Panel Controller ──

    OutlinePanelController* outline_panel_controller{nullptr};

    // ── V4 Phase 14: Note Template Engine ──

    NoteTemplateEngine* note_template_engine{nullptr};
};

} // namespace markamp::core
