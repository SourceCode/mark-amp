/// EventTypeId.h — Phase 20: Compile-time event type registry
///
/// Provides O(1) numeric event IDs for flat-array subscriber lookup,
/// eliminating the unordered_map<type_index> overhead in EventBus.
///
/// Pattern implemented: #5 Skip map lookup for events, O(1) subscriber array

#pragma once

#include <array>
#include <cstdint>
#include <string_view>

namespace markamp::core
{

/// Maximum number of event type IDs supported. Must be >= total event types.
inline constexpr std::size_t kMaxEventTypes = 512;

/// Compile-time event type IDs for O(1) array indexing.
/// High-frequency events are listed first for cache locality.
enum class EventTypeId : uint16_t
{
    // ── High-frequency editing events ──
    EditorContentChanged = 0,
    CursorPositionChanged = 1,
    EditorScrollChanged = 2,
    EditorStatsChanged = 3,
    FileContentChanged = 4,

    // ── File lifecycle events ──
    FileOpened = 5,
    FileSaved = 6,
    ActiveFileChanged = 7,
    FileReloadRequest = 8,
    FileEncodingDetected = 9,

    // ── View events ──
    ViewModeChanged = 10,
    SidebarToggle = 11,
    UIScaleChanged = 12,
    FocusModeChanged = 13,
    ScrollSyncModeChanged = 14,

    // ── Tab events ──
    TabSwitched = 15,
    TabCloseRequest = 16,
    TabSaveRequest = 17,
    TabSaveAsRequest = 18,

    // ── Theme events ──
    ThemeChanged = 19,
    UserThemesLoaded = 20,

    // ── Plugin events ──
    PluginActivated = 21,
    PluginDeactivated = 22,
    FeatureToggled = 23,
    ExtensionInstalled = 24,
    ExtensionUninstalled = 25,
    ExtensionEnablementChanged = 26,
    ExtensionsScanComplete = 27,

    // ── App lifecycle ──
    AppReady = 28,
    AppShutdown = 29,
    StartupDeferral = 30,

    // ── Settings events ──
    SettingChanged = 31,
    SettingsBatchChanged = 32,
    SettingsOpenRequest = 33,

    // ── Notification events ──
    Notification = 34,
    ActivityBarSelection = 35,

    // ── Search events ──
    FindRequest = 36,
    ReplaceRequest = 37,

    // ── Canvas events ──
    CanvasViewportChanged = 38,
    CanvasObjectAdded = 39,
    CanvasObjectRemoved = 40,
    CanvasObjectModified = 41,
    CanvasToolChanged = 42,
    CanvasSelectionChanged = 43,

    // ── Notebook/kernel events ──
    KernelStarted = 44,
    KernelStopped = 45,
    CellExecutionCompleted = 46,
    CellOutput = 47,

    // ── Git events ──
    GitRepoOpened = 48,
    GitStatusChanged = 49,
    GitCommitCreated = 50,

    // ── Rendering events ──
    MermaidRenderStatus = 51,

    // ── Reserved for user-defined / dynamic events ──
    _ReservedStart = 128,

    // Sentinel
    _Count = kMaxEventTypes,
};

/// O(1) name lookup for known event type IDs.
[[nodiscard]] inline auto event_type_name(EventTypeId id) -> std::string_view
{
    // Only the most common IDs get names; others return "Unknown".
    static constexpr std::array<std::string_view, 52> kNames = {{
        "EditorContentChanged",       // 0
        "CursorPositionChanged",      // 1
        "EditorScrollChanged",        // 2
        "EditorStatsChanged",         // 3
        "FileContentChanged",         // 4
        "FileOpened",                 // 5
        "FileSaved",                  // 6
        "ActiveFileChanged",          // 7
        "FileReloadRequest",          // 8
        "FileEncodingDetected",       // 9
        "ViewModeChanged",            // 10
        "SidebarToggle",              // 11
        "UIScaleChanged",             // 12
        "FocusModeChanged",           // 13
        "ScrollSyncModeChanged",      // 14
        "TabSwitched",                // 15
        "TabCloseRequest",            // 16
        "TabSaveRequest",             // 17
        "TabSaveAsRequest",           // 18
        "ThemeChanged",               // 19
        "UserThemesLoaded",           // 20
        "PluginActivated",            // 21
        "PluginDeactivated",          // 22
        "FeatureToggled",             // 23
        "ExtensionInstalled",         // 24
        "ExtensionUninstalled",       // 25
        "ExtensionEnablementChanged", // 26
        "ExtensionsScanComplete",     // 27
        "AppReady",                   // 28
        "AppShutdown",                // 29
        "StartupDeferral",            // 30
        "SettingChanged",             // 31
        "SettingsBatchChanged",       // 32
        "SettingsOpenRequest",        // 33
        "Notification",               // 34
        "ActivityBarSelection",       // 35
        "FindRequest",                // 36
        "ReplaceRequest",             // 37
        "CanvasViewportChanged",      // 38
        "CanvasObjectAdded",          // 39
        "CanvasObjectRemoved",        // 40
        "CanvasObjectModified",       // 41
        "CanvasToolChanged",          // 42
        "CanvasSelectionChanged",     // 43
        "KernelStarted",              // 44
        "KernelStopped",              // 45
        "CellExecutionCompleted",     // 46
        "CellOutput",                 // 47
        "GitRepoOpened",              // 48
        "GitStatusChanged",           // 49
        "GitCommitCreated",           // 50
        "MermaidRenderStatus",        // 51
    }};

    auto index = static_cast<std::size_t>(id);
    if (index < kNames.size())
    {
        return kNames[index];
    }
    return "Unknown";
}

} // namespace markamp::core
