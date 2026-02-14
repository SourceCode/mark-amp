#pragma once

#include "core/RecentWorkspaces.h"
#include "platform/PlatformAbstraction.h"

#include <wx/app.h>

#include <memory>

namespace markamp::core
{
class EventBus;
class Config;
class AppStateManager;
class RecentWorkspaces;
class CommandHistory;
class MermaidRenderer;
class MathRenderer;
class ThemeRegistry;
class ThemeEngine;
class PluginManager;
class FeatureRegistry;

// Extension API services (P1–P4)
class ContextKeyService;
class OutputChannelService;
class DiagnosticsService;
class DecorationService;
class WebviewService;
class FileSystemProviderRegistry;
class LanguageProviderRegistry;
class TreeDataProviderRegistry;
class SnippetEngine;
class WorkspaceService;
class TextEditorService;
class ProgressService;
class ExtensionEventBus;
class EnvironmentService;
class NotificationService;
class StatusBarItemService;
class InputBoxService;
class QuickPickService;
class GrammarEngine;
class TerminalService;
class TaskRunnerService;
} // namespace markamp::core

namespace markamp::platform
{
class PlatformAbstraction;
} // namespace markamp::platform

namespace markamp::app
{

class MarkAmpApp : public wxApp
{
public:
    bool OnInit() override;
    int OnExit() override;

    /// Idle handler — drains queued and fast-path EventBus events.
    void OnIdle(wxIdleEvent& event);

    // Destructor must be declared here, defined in .cpp where types are complete
    ~MarkAmpApp() override;

    // Application-wide settings
    static constexpr int kDefaultWidth = 1280;
    static constexpr int kDefaultHeight = 800;
    static constexpr int kMinWidth = 800;
    static constexpr int kMinHeight = 600;

private:
    // Core services (owned by the app, lifetime-managed)
    std::unique_ptr<core::EventBus> event_bus_;
    std::unique_ptr<core::Config> config_;
    std::unique_ptr<core::RecentWorkspaces> recent_workspaces_;
    std::unique_ptr<core::AppStateManager> state_manager_;
    std::unique_ptr<core::CommandHistory> command_history_;
    std::unique_ptr<platform::PlatformAbstraction> platform_;
    std::unique_ptr<core::ThemeRegistry> theme_registry_;
    std::unique_ptr<core::ThemeEngine> theme_engine_;
    std::shared_ptr<core::MermaidRenderer> mermaid_renderer_;
    std::shared_ptr<core::MathRenderer> math_renderer_;
    std::unique_ptr<core::FeatureRegistry> feature_registry_;
    std::unique_ptr<core::PluginManager> plugin_manager_;

    // Extension API services (P1–P4, owned by the app)
    std::unique_ptr<core::ContextKeyService> context_key_service_;
    std::unique_ptr<core::OutputChannelService> output_channel_service_;
    std::unique_ptr<core::DiagnosticsService> diagnostics_service_;
    std::unique_ptr<core::DecorationService> decoration_service_;
    std::unique_ptr<core::WebviewService> webview_service_;
    std::unique_ptr<core::FileSystemProviderRegistry> file_system_provider_registry_;
    std::unique_ptr<core::LanguageProviderRegistry> language_provider_registry_;
    std::unique_ptr<core::TreeDataProviderRegistry> tree_data_provider_registry_;
    std::unique_ptr<core::SnippetEngine> snippet_engine_;
    std::unique_ptr<core::WorkspaceService> workspace_service_;
    std::unique_ptr<core::TextEditorService> text_editor_service_;
    std::unique_ptr<core::ProgressService> progress_service_;
    std::unique_ptr<core::ExtensionEventBus> extension_event_bus_;
    std::unique_ptr<core::EnvironmentService> environment_service_;
    std::unique_ptr<core::NotificationService> notification_service_;
    std::unique_ptr<core::StatusBarItemService> status_bar_item_service_;
    std::unique_ptr<core::InputBoxService> input_box_service_;
    std::unique_ptr<core::QuickPickService> quick_pick_service_;
    std::unique_ptr<core::GrammarEngine> grammar_engine_;
    std::unique_ptr<core::TerminalService> terminal_service_;
    std::unique_ptr<core::TaskRunnerService> task_runner_service_;
};

} // namespace markamp::app
