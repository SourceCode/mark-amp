#include "MarkAmpApp.h"

#include "core/AppState.h"
#include "core/BuiltInPlugins.h"
#include "core/Command.h"
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
#include "core/Logger.h"
#include "core/MathRenderer.h"
#include "core/MermaidRenderer.h"
#include "core/NotificationService.h"
#include "core/OutputChannelService.h"
#include "core/PluginManager.h"
#include "core/ProgressService.h"
#include "core/QuickPickService.h"
#include "core/RecentWorkspaces.h"
#include "core/SnippetEngine.h"
#include "core/StatusBarItemService.h"
#include "core/TaskRunnerService.h"
#include "core/TerminalService.h"
#include "core/TextEditorService.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"
#include "core/TreeDataProviderRegistry.h"
#include "core/WebviewService.h"
#include "core/WorkspaceService.h"
#include "platform/PlatformAbstraction.h"
#include "ui/MainFrame.h"

#include <spdlog/spdlog.h>

namespace markamp::app
{

// Destructor defined here where all forward-declared types are complete
MarkAmpApp::~MarkAmpApp() = default;

bool MarkAmpApp::OnInit()
{
    if (!wxApp::OnInit())
    {
        return false;
    }

    SetAppName("MarkAmp");
    SetVendorName("MarkAmp");

    // 1. Initialize logging
    core::initLogger();
    MARKAMP_LOG_INFO("MarkAmp v{}.{}.{} starting...",
                     MARKAMP_VERSION_MAJOR,
                     MARKAMP_VERSION_MINOR,
                     MARKAMP_VERSION_PATCH);
    MARKAMP_LOG_INFO(
        "wxWidgets version: {}.{}.{}", wxMAJOR_VERSION, wxMINOR_VERSION, wxRELEASE_NUMBER);
    MARKAMP_LOG_INFO("Platform: {}",
                     wxPlatformInfo::Get().GetOperatingSystemDescription().ToStdString());

    // 2. Create core services
    event_bus_ = std::make_unique<core::EventBus>();
    MARKAMP_LOG_DEBUG("EventBus initialized");

    // 3. Load configuration
    config_ = std::make_unique<core::Config>();
    auto loadResult = config_->load();
    if (!loadResult)
    {
        MARKAMP_LOG_WARN("Config load failed: {}", loadResult.error());
    }
    else
    {
        MARKAMP_LOG_INFO("Configuration loaded");
    }

    // 3b. Initialize recent workspaces
    recent_workspaces_ = std::make_unique<core::RecentWorkspaces>(*config_);
    MARKAMP_LOG_DEBUG("RecentWorkspaces initialized");

    // 4. Initialize app state manager
    state_manager_ = std::make_unique<core::AppStateManager>(*event_bus_);
    MARKAMP_LOG_DEBUG("AppStateManager initialized");

    // 5. Initialize command history
    command_history_ = std::make_unique<core::CommandHistory>();
    MARKAMP_LOG_DEBUG("CommandHistory initialized");

    // 6. Create platform abstraction
    platform_ = platform::create_platform();
    MARKAMP_LOG_DEBUG("Platform abstraction initialized");

    // 7. Initialize theme system
    theme_registry_ = std::make_unique<core::ThemeRegistry>();
    auto themeResult = theme_registry_->initialize();
    if (!themeResult)
    {
        MARKAMP_LOG_WARN("ThemeRegistry init failed: {}", themeResult.error());
    }
    MARKAMP_LOG_DEBUG("ThemeRegistry initialized with {} themes",
                      theme_registry_->list_themes().size());

    theme_engine_ = std::make_unique<core::ThemeEngine>(*event_bus_, *theme_registry_);
    MARKAMP_LOG_DEBUG("ThemeEngine initialized with theme: {}",
                      theme_engine_->current_theme().name);

    // 8. Initialize extension API services
    context_key_service_ = std::make_unique<core::ContextKeyService>();
    output_channel_service_ = std::make_unique<core::OutputChannelService>();
    diagnostics_service_ = std::make_unique<core::DiagnosticsService>();
    decoration_service_ = std::make_unique<core::DecorationService>();
    webview_service_ = std::make_unique<core::WebviewService>();
    file_system_provider_registry_ = std::make_unique<core::FileSystemProviderRegistry>();
    language_provider_registry_ = std::make_unique<core::LanguageProviderRegistry>();
    tree_data_provider_registry_ = std::make_unique<core::TreeDataProviderRegistry>();
    snippet_engine_ = std::make_unique<core::SnippetEngine>();
    workspace_service_ = std::make_unique<core::WorkspaceService>();
    text_editor_service_ = std::make_unique<core::TextEditorService>();
    progress_service_ = std::make_unique<core::ProgressService>();
    extension_event_bus_ = std::make_unique<core::ExtensionEventBus>();
    environment_service_ = std::make_unique<core::EnvironmentService>();
    notification_service_ = std::make_unique<core::NotificationService>(*event_bus_);
    status_bar_item_service_ = std::make_unique<core::StatusBarItemService>();
    input_box_service_ = std::make_unique<core::InputBoxService>();
    quick_pick_service_ = std::make_unique<core::QuickPickService>();
    grammar_engine_ = std::make_unique<core::GrammarEngine>();
    terminal_service_ = std::make_unique<core::TerminalService>();
    task_runner_service_ = std::make_unique<core::TaskRunnerService>();
    MARKAMP_LOG_INFO("Extension API services initialized (21 services)");

    // 9. Initialize plugin system
    feature_registry_ = std::make_unique<core::FeatureRegistry>(*event_bus_, *config_);
    plugin_manager_ = std::make_unique<core::PluginManager>(*event_bus_, *config_);

    // Wire extension services into PluginManager
    core::PluginManager::ExtensionServices ext_services{};
    ext_services.context_key_service = context_key_service_.get();
    ext_services.output_channel_service = output_channel_service_.get();
    ext_services.diagnostics_service = diagnostics_service_.get();
    ext_services.decoration_service = decoration_service_.get();
    ext_services.webview_service = webview_service_.get();
    ext_services.file_system_provider_registry = file_system_provider_registry_.get();
    ext_services.language_provider_registry = language_provider_registry_.get();
    ext_services.tree_data_provider_registry = tree_data_provider_registry_.get();
    ext_services.snippet_engine = snippet_engine_.get();
    ext_services.workspace_service = workspace_service_.get();
    ext_services.text_editor_service = text_editor_service_.get();
    ext_services.progress_service = progress_service_.get();
    ext_services.extension_event_bus = extension_event_bus_.get();
    ext_services.environment_service = environment_service_.get();
    ext_services.notification_service = notification_service_.get();
    ext_services.status_bar_item_service = status_bar_item_service_.get();
    ext_services.input_box_service = input_box_service_.get();
    ext_services.quick_pick_service = quick_pick_service_.get();
    ext_services.grammar_engine = grammar_engine_.get();
    ext_services.terminal_service = terminal_service_.get();
    ext_services.task_runner_service = task_runner_service_.get();
    plugin_manager_->set_extension_services(ext_services);
    plugin_manager_->set_status_bar_service(status_bar_item_service_.get());
    plugin_manager_->set_tree_registry(tree_data_provider_registry_.get());

    core::register_builtin_plugins(*plugin_manager_, *feature_registry_);
    plugin_manager_->activate_all();
    MARKAMP_LOG_INFO("Plugin system initialized: {} plugins, {} features",
                     plugin_manager_->plugin_count(),
                     feature_registry_->feature_count());

    // 10. Initialize Mermaid renderer (before MainFrame so it can be injected)
    mermaid_renderer_ = std::make_shared<core::MermaidRenderer>();
    MARKAMP_LOG_INFO("MermaidRenderer initialized (available: {})",
                     mermaid_renderer_->is_available() ? "yes" : "no");

    // 11. Initialize Math renderer (before MainFrame so it can be injected)
    math_renderer_ = std::make_shared<core::MathRenderer>();
    MARKAMP_LOG_INFO("MathRenderer initialized (available: {})",
                     math_renderer_->is_available() ? "yes" : "no");

    // 12. Create and show the main frame (with frameless custom chrome)
    auto* frame = new ui::MainFrame("MarkAmp",
                                    wxDefaultPosition,
                                    wxSize(kDefaultWidth, kDefaultHeight),
                                    event_bus_.get(),
                                    config_.get(),
                                    recent_workspaces_.get(),
                                    platform_.get(),
                                    theme_engine_.get(),
                                    feature_registry_.get(),
                                    mermaid_renderer_.get(),
                                    math_renderer_.get());

    frame->Show(true);
    SetTopWindow(frame);

    // 12. Publish app ready event
    core::events::AppReadyEvent readyEvent;
    event_bus_->publish(readyEvent);

    // 13. Bind idle handler to drain queued and fast-path EventBus events
    Bind(wxEVT_IDLE, &MarkAmpApp::OnIdle, this);
    MARKAMP_LOG_INFO("MarkAmp initialization complete");

    return true;
}

void MarkAmpApp::OnIdle(wxIdleEvent& event)
{
    if (event_bus_)
    {
        event_bus_->process_queued();
        event_bus_->drain_fast_queue();
    }
    event.Skip();
}

int MarkAmpApp::OnExit()
{
    MARKAMP_LOG_INFO("MarkAmp shutting down...");

    // Publish shutdown event
    if (event_bus_)
    {
        core::events::AppShutdownEvent shutdownEvent;
        event_bus_->publish(shutdownEvent);
    }

    // Save configuration
    if (config_)
    {
        auto saveResult = config_->save();
        if (!saveResult)
        {
            MARKAMP_LOG_WARN("Config save failed: {}", saveResult.error());
        }
    }

    // IMPORTANT: Destroy the top-level window BEFORE resetting core services.
    // wxWidgets UI components (CustomChrome, LayoutManager, etc.) hold RAII
    // Subscription objects whose destructors call back into EventBus.
    // If we reset event_bus_ first, those destructors would access freed memory,
    // causing std::terminate via SIGABRT.
    auto* top = GetTopWindow();
    if (top != nullptr)
    {
        top->Destroy();
        DeletePendingObjects();
    }

    // Clean up in reverse order (safe now that all windows are destroyed)
    if (plugin_manager_)
    {
        plugin_manager_->deactivate_all();
    }
    plugin_manager_.reset();
    feature_registry_.reset();

    // Destroy extension services in reverse instantiation order
    task_runner_service_.reset();
    terminal_service_.reset();
    grammar_engine_.reset();
    quick_pick_service_.reset();
    input_box_service_.reset();
    status_bar_item_service_.reset();
    notification_service_.reset();
    environment_service_.reset();
    extension_event_bus_.reset();
    progress_service_.reset();
    text_editor_service_.reset();
    workspace_service_.reset();
    snippet_engine_.reset();
    tree_data_provider_registry_.reset();
    language_provider_registry_.reset();
    file_system_provider_registry_.reset();
    webview_service_.reset();
    decoration_service_.reset();
    diagnostics_service_.reset();
    output_channel_service_.reset();
    context_key_service_.reset();

    mermaid_renderer_.reset();
    math_renderer_.reset();
    command_history_.reset();
    state_manager_.reset();
    theme_engine_.reset();
    theme_registry_.reset();
    config_.reset();
    platform_.reset();
    event_bus_.reset();

    MARKAMP_LOG_INFO("MarkAmp shutdown complete");
    spdlog::shutdown();
    return wxApp::OnExit();
}

} // namespace markamp::app
