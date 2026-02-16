#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace markamp::core
{

/// Enumerates all discrete startup phases in MarkAmpApp::OnInit().
/// Used by StartupTimer checkpoints and Tracy zone instrumentation
/// to produce a detailed breakdown of application startup time.
///
/// Phase 08: Fine-grained startup profiling for cold-start < 150ms target.
enum class StartupPhase : uint8_t
{
    // Core infrastructure
    LoggerInit = 0,
    EventBusCreation,
    ConfigLoad,
    RecentWorkspaces,
    AppStateManager,
    CommandHistory,

    // Platform + theming
    PlatformCreation,
    ThemeRegistryScan,
    ThemeEngineInit,

    // Extension services (21 individual services)
    ContextKeyService,
    OutputChannelService,
    DiagnosticsService,
    DecorationService,
    WebviewService,
    FileSystemProviderRegistry,
    LanguageProviderRegistry,
    TreeDataProviderRegistry,
    SnippetEngine,
    WorkspaceService,
    TextEditorService,
    ProgressService,
    ExtensionEventBus,
    EnvironmentService,
    NotificationService,
    StatusBarItemService,
    InputBoxService,
    QuickPickService,
    GrammarEngine,
    TerminalService,
    TaskRunnerService,

    // Plugin system
    FeatureRegistryInit,
    PluginManagerCreation,
    BuiltInPluginRegistration,
    PluginManagerActivation,

    // Rendering and GUI
    MermaidRendererInit,
    MathRendererInit,
    MainFrameCreation,
    FirstShow,
    FirstFrameRendered,

    // Sentinel — must be last
    Count
};

/// Compile-time lookup table of human-readable names for each startup phase.
/// Index by static_cast<std::size_t>(StartupPhase::X).
inline constexpr std::array<std::string_view, static_cast<std::size_t>(StartupPhase::Count)>
    kStartupPhaseNames = {{
        "LoggerInit",
        "EventBusCreation",
        "ConfigLoad",
        "RecentWorkspaces",
        "AppStateManager",
        "CommandHistory",
        "PlatformCreation",
        "ThemeRegistryScan",
        "ThemeEngineInit",
        "ContextKeyService",
        "OutputChannelService",
        "DiagnosticsService",
        "DecorationService",
        "WebviewService",
        "FileSystemProviderRegistry",
        "LanguageProviderRegistry",
        "TreeDataProviderRegistry",
        "SnippetEngine",
        "WorkspaceService",
        "TextEditorService",
        "ProgressService",
        "ExtensionEventBus",
        "EnvironmentService",
        "NotificationService",
        "StatusBarItemService",
        "InputBoxService",
        "QuickPickService",
        "GrammarEngine",
        "TerminalService",
        "TaskRunnerService",
        "FeatureRegistryInit",
        "PluginManagerCreation",
        "BuiltInPluginRegistration",
        "PluginManagerActivation",
        "MermaidRendererInit",
        "MathRendererInit",
        "MainFrameCreation",
        "FirstShow",
        "FirstFrameRendered",
    }};

/// Get the human-readable name of a startup phase.
[[nodiscard]] constexpr auto phase_name(StartupPhase phase) -> std::string_view
{
    const auto index = static_cast<std::size_t>(phase);
    if (index < kStartupPhaseNames.size())
    {
        return kStartupPhaseNames[index];
    }
    return "Unknown";
}

} // namespace markamp::core
