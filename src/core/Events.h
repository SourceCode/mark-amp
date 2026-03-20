#pragma once

#include "EventBus.h"
#include "SurfaceLink.h"

#include <string>
#include <vector>

namespace markamp::core::events
{

// ============================================================================
// EVENT TYPE TABLE OF CONTENTS — Deduplication Index
// ============================================================================
//
// Before adding a new event type, search this TOC to ensure no duplicate
// exists. Categories are listed in file order with approximate line numbers.
//
//   Section                            | Events        | ~Line
//   -----------------------------------|---------------|------
//   Theme                              | 1 event       |   50
//   File                               | 4 events      |   59
//   View / Layout                      | 3 events      |   81
//   Editor                             | 3 events      |  114
//   Application lifecycle              | 3 events      |  135
//   Scroll sync                        | 3 events      |  142
//   Status bar                         | 2 events      |  167
//   Accessibility                      | 2 events      |  180
//   Startup / Workspace                | 2 events      |  192
//   Focus mode                         | 2 events      |  205
//   Theme gallery                      | 1 event       |  219
//   Tab                                | 10 events     |  225
//   Settings                           | 14 events     |  281
//   Plugin lifecycle                   | 2 events      |  375
//   Feature toggle                     | 2 events      |  387
//   Extension management               | 5 events      |  403
//   Notification                       | 6 events      |  424
//   Activity bar                       | 5 events      |  452
//   Workspace management               | 1 event       |  478
//   R6-R19 Editor actions              | ~60 events    |  485
//   InputBox & QuickPick (Phase 07)    | 5 events      |  667
//   V3 Block/Notebook lifecycle        | ~80 events    |  685
//   V3 Attribute View                  | ~30 events    | 1046
//   V3 FSRS/Deck/Review/Knowledge     | ~30 events    | 1216
//   V4 Document/Vault/Graph            | ~80 events    | 1352
//   V5 Canvas                          | ~20 events    | 2003
//   V6 Deferred loading                | 3 events      | 2206
//   V8 Collaboration/Marketplace       | ~40 events    | 2076
//   V8 Navigation/Tool Windows         | ~25 events    | 2265
//   V8 FX Engine                       | ~10 events    | 2397
//   V11 Node Editor                    | ~20 events    | 4104
//
// Total: ~470+ unique event types across ~100 sections.
// Rule: always grep before adding. No event name may appear twice.
// ============================================================================

// ============================================================================
// Event Declaration Macros
// ============================================================================
//
// Naming Convention:
//   - Notification events:   <Subject><Verb>Event         (e.g. ThemeChangedEvent)
//   - Request events:        <Action>RequestEvent          (e.g. FindRequestEvent)
//   - Toggle request events: Toggle<Feature>RequestEvent   (e.g. ToggleMinimapRequestEvent)
//
// Usage:
//   MARKAMP_DECLARE_EVENT(MySimpleEvent)          — fieldless event, 1 line
//   MARKAMP_DECLARE_EVENT_WITH_FIELDS(MyEvent)    — event with fields, use { } block after
//

// Declares a fieldless event struct with automatic type_name() override.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_DECLARE_EVENT(EventName)                                                           \
    struct EventName : Event                                                                       \
    {                                                                                              \
        [[nodiscard]] auto type_name() const -> std::string_view override                          \
        {                                                                                          \
            return #EventName;                                                                     \
        }                                                                                          \
    }

// Begins a struct with fields. Must be closed with MARKAMP_DECLARE_EVENT_END.
// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_DECLARE_EVENT_WITH_FIELDS(EventName)                                               \
    struct EventName : Event                                                                       \
    {                                                                                              \
        [[nodiscard]] auto type_name() const -> std::string_view override                          \
        {                                                                                          \
            return #EventName;                                                                     \
        }

// NOLINTNEXTLINE(cppcoreguidelines-macro-usage)
#define MARKAMP_DECLARE_EVENT_END }

// ============================================================================
// Theme events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeChangedEvent)
std::string theme_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemePreviewRequestEvent)
std::string theme_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionThemeDiscoveredEvent)
std::string theme_id;
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeTokenEditedEvent)
std::string token;
std::string old_color;
std::string new_color;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeUndoRequestEvent)
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Design System events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DensityProfileChangedEvent)
int new_profile{0}; // DensityProfile as int
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TypographyScaleChangedEvent)
std::string base_sans;
std::string base_mono;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ElevationModeChangedEvent)
bool dark_mode{true};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// File events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileOpenedEvent)
std::string file_path;
std::string content;
int group_id{-1};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileContentChangedEvent)
std::string file_id;
std::string new_content;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileSavedEvent)
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActiveFileChangedEvent)
std::string file_id;
int group_id{-1};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenDiffRequestEvent)
std::string left_path;
std::string right_path;
std::string left_content;
std::string right_content;
std::string title;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// View events
// ============================================================================

enum class ViewMode
{
    Editor,
    Preview,
    Split,
    LivePreview
};

enum class WrapMode
{
    None,
    Word,
    Character
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewModeChangedEvent)
ViewMode mode{ViewMode::Split};

ViewModeChangedEvent() = default;
explicit ViewModeChangedEvent(ViewMode m)
    : mode(m)
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyboardModeChangedEvent)
bool is_keyboard_only{false};

KeyboardModeChangedEvent() = default;
explicit KeyboardModeChangedEvent(bool keyboard_only)
    : is_keyboard_only(keyboard_only)
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarToggleEvent)
bool visible;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(SidebarFocusRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ZenModeChangedEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationModeChangedEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Editor events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CursorPositionChangedEvent)
int line;
int column;
int selection_length{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorContentChangedEvent)
std::string content;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorStatsChangedEvent)
int word_count{0};
int char_count{0};
int line_count{0};
int selection_length{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Application events
// ============================================================================

MARKAMP_DECLARE_EVENT(AppReadyEvent);
MARKAMP_DECLARE_EVENT(AppShutdownEvent);
MARKAMP_DECLARE_EVENT(AppActivatedEvent);

// ============================================================================
// Scroll sync events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorScrollChangedEvent)
double scroll_fraction{0.0}; // 0.0 = top, 1.0 = bottom
MARKAMP_DECLARE_EVENT_END;

enum class ScrollSyncMode
{
    Proportional,
    HeadingAnchor,
    CursorAnchored // V8 Phase 9: sync preview to cursor line
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ScrollSyncModeChangedEvent)
ScrollSyncMode mode{ScrollSyncMode::Proportional};

ScrollSyncModeChangedEvent() = default;
explicit ScrollSyncModeChangedEvent(ScrollSyncMode sync_mode)
    : mode(sync_mode)
{
}
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 09: Split view & editor-preview sync events
// ============================================================================

// Task 4: Breadcrumbs sync — heading hierarchy path
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BreadcrumbsChangedEvent)
std::string breadcrumb_path; // e.g. "doc > ## Section > ### Subsection"
int heading_line{0};         // editor line of innermost heading
MARKAMP_DECLARE_EVENT_END;

// Task 2: Selection mirroring — editor selection text for preview highlight
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionHighlightEvent)
std::string selected_text;
bool clear{false}; // true = clear highlight
MARKAMP_DECLARE_EVENT_END;

// Task 9: Open in side command
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenInSideEvent)
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

// Task 13: Export HTML request
MARKAMP_DECLARE_EVENT(ExportHtmlRequestEvent);

// Task 6: Split direction changed
enum class SplitDirection
{
    Horizontal, // editor left, preview right
    Vertical    // editor top, preview bottom
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SplitDirectionChangedEvent)
SplitDirection direction{SplitDirection::Horizontal};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 10: Panel Area (Bottom) Redesign events
// ============================================================================

enum class BadgeState
{
    kNone,
    kInfo,
    kWarning,
    kError,
    kSuccess,
    kDot
};

MARKAMP_DECLARE_EVENT(PanelAreaTabsChangedEvent);
MARKAMP_DECLARE_EVENT(PanelAreaBadgeChangedEvent);
MARKAMP_DECLARE_EVENT(ToggleBottomPanelRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelAreaMaximizeToggledEvent)
bool is_maximized{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Status bar events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileEncodingDetectedEvent)
std::string encoding_name; // e.g. "UTF-8", "UTF-8 BOM", "ASCII"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MermaidRenderStatusEvent)
std::string status; // "ACTIVE", "INACTIVE", "RENDERING", "ERROR"
bool active{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Accessibility events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(UIScaleChangedEvent)
float scale_factor{1.0F};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(InputModeChangedEvent)
bool using_keyboard{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(A11yHighContrastToggledEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(A11yReducedMotionToggledEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(A11yAuditCompletedEvent)
int critical_count{0};
int warning_count{0};
int info_count{0};
bool passed{true};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChromeFocusChangedEvent)
int zone_id{0}; // Cast from FocusZoneId
int item_index{-1};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(A11yFocusChangedEvent)
std::string element_id;
std::string element_label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(A11yAnnouncementEvent)
std::string message;
std::string priority; // "polite" or "assertive"
MARKAMP_DECLARE_EVENT_END;

// ── V9 Phase 32: Platform-Specific Optimization ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PlatformDarkModeChangedEvent)
bool is_dark{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PlatformDpiChangedEvent)
double scale_factor{1.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileWatchChangeEvent)
std::string path;
std::string change_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrashReportAvailableEvent)
std::string report_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoUpdateAvailableEvent)
std::string version;
std::string changelog_url;
MARKAMP_DECLARE_EVENT_END;

// ── V9 Phase 33: Structured Logging & Observability events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LogLevelChangedEvent)
std::string module_name;
std::string new_level;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(HealthStatusChangedEvent)
std::string subsystem;
std::string status;
std::string message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MetricAlertEvent)
std::string metric_name;
double value{0.0};
double threshold{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticReportGeneratedEvent)
std::string report_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlowSpanDetectedEvent)
std::string span_name;
int64_t duration_us{0};
int64_t threshold_us{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Startup / Workspace events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenFolderRequestEvent)
// If empty, prompt user. If set, open directly.
std::string path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceOpenRequestEvent)
std::string path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Focus mode events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FocusModeChangedEvent)
bool active{false};

FocusModeChangedEvent() = default;
explicit FocusModeChangedEvent(bool is_active)
    : active(is_active)
{
}
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Theme gallery
// ============================================================================

MARKAMP_DECLARE_EVENT(ThemeGalleryRequestEvent);

// ============================================================================
// Tab events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabSwitchedEvent)
std::string file_path;

TabSwitchedEvent() = default;
explicit TabSwitchedEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabCloseRequestEvent)
std::string file_path;

TabCloseRequestEvent() = default;
explicit TabCloseRequestEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabDropRequestEvent)
std::string file_path;
int source_tabbar_id{-1};
int target_tabbar_id{-1};
int insert_index{-1};

TabDropRequestEvent() = default;
explicit TabDropRequestEvent(std::string path, int source, int target, int index = -1)
    : file_path(std::move(path))
    , source_tabbar_id(source)
    , target_tabbar_id(target)
    , insert_index(index)
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabSaveRequestEvent)
std::string file_path;

TabSaveRequestEvent() = default;
explicit TabSaveRequestEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabSaveAsRequestEvent)
std::string file_path;

TabSaveAsRequestEvent() = default;
explicit TabSaveAsRequestEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileReloadRequestEvent)
std::string file_path;

FileReloadRequestEvent() = default;
explicit FileReloadRequestEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GoToLineRequestEvent);

// ============================================================================
// Settings events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsOpenRequestEvent)
std::string setting_id; ///< Deep-link to specific setting (optional)
std::string query;      ///< Pre-fill search query (optional)
std::string scope;      ///< Target scope: "application", "workspace", "project" (optional)
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingChangedEvent)
std::string key;
std::string value;

SettingChangedEvent() = default;
SettingChangedEvent(std::string key_name, std::string new_value)
    : key(std::move(key_name))
    , value(std::move(new_value))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsBatchChangedEvent)
std::vector<std::string> changed_keys;
MARKAMP_DECLARE_EVENT_END;

// ── Batch 10: Settings profile + validation events (#59-60) ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsProfileChangedEvent)
std::string profile_name;

SettingsProfileChangedEvent() = default;
explicit SettingsProfileChangedEvent(std::string name)
    : profile_name(std::move(name))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsValidationErrorEvent)
std::string setting_id;
std::string message;

SettingsValidationErrorEvent() = default;
SettingsValidationErrorEvent(std::string id_val, std::string msg)
    : setting_id(std::move(id_val))
    , message(std::move(msg))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsCategoryChangedEvent)
std::string category;

SettingsCategoryChangedEvent() = default;
explicit SettingsCategoryChangedEvent(std::string cat)
    : category(std::move(cat))
{
}
MARKAMP_DECLARE_EVENT_END;

// Batch 10: Additional Preferences events

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsResetEvent)
std::string setting_id; ///< Empty = reset all

SettingsResetEvent() = default;
explicit SettingsResetEvent(std::string sid)
    : setting_id(std::move(sid))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsSearchEvent)
std::string query;

SettingsSearchEvent() = default;
explicit SettingsSearchEvent(std::string search_query)
    : query(std::move(search_query))
{
}
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsImportedEvent)
std::string file_path;
int setting_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsExportedEvent)
std::string file_path;
int setting_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(PreferencesOpenedEvent);

// ============================================================================
// Plugin events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginActivatedEvent)
std::string plugin_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginDeactivatedEvent)
std::string plugin_id;
MARKAMP_DECLARE_EVENT_END;

// V9 Phase 04 Task 15: Plugin error telemetry
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginErrorEvent)
std::string plugin_id;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Feature toggle events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureToggledEvent)
std::string feature_id;
bool enabled{false};

FeatureToggledEvent() = default;
FeatureToggledEvent(std::string id, bool state)
    : feature_id(std::move(id))
    , enabled(state)
{
}
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Extension management events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionInstalledEvent)
std::string extension_id;
std::string version;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionUninstalledEvent)
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionEnablementChangedEvent)
std::string extension_id;
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(ShowExtensionsBrowserRequestEvent);
MARKAMP_DECLARE_EVENT(ShowExplorerRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelHeaderActionEvent)
std::string action_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelContextMenuEvent)
std::string panel_title;
int screen_x{0};
int screen_y{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 19: Debug events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugStateChangedEvent)
std::string session_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugStoppedEvent)
std::string file;
int line{0};
std::string reason; // "breakpoint", "step", "exception"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugOutputEvent)
std::string category; // "console", "stdout", "stderr"
std::string text;
MARKAMP_DECLARE_EVENT_END;

// ── Phase 20: Extension lifecycle events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionEnabledEvent)
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionDisabledEvent)
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionInstallProgressEvent)
std::string extension_id;
int percent{0};
std::string status; // "downloading", "installing", "complete", "error"
MARKAMP_DECLARE_EVENT_END;

// ── Phase 21: Terminal events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalCreatedEvent)
int terminal_id{0};
std::string name;
std::string shell_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDestroyedEvent)
int terminal_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDataOutputEvent)
int terminal_id{0};
std::string data;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDataInputEvent)
int terminal_id{0};
std::string data;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalTitleChangedEvent)
int terminal_id{0};
std::string title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalCwdChangedEvent)
int terminal_id{0};
std::string cwd;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalBellEvent)
int terminal_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalResizedEvent)
int terminal_id{0};
int cols{80};
int rows{24};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Notification events
// ============================================================================

enum class NotificationLevel
{
    Info,
    Warning,
    Error,
    Success
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotificationEvent)
std::string message;
NotificationLevel level{NotificationLevel::Info};
int duration_ms{3000}; // Auto-dismiss duration, 0 = sticky

NotificationEvent() = default;
explicit NotificationEvent(std::string msg,
                           NotificationLevel lvl = NotificationLevel::Info,
                           int dur = 3000)
    : message(std::move(msg))
    , level(lvl)
    , duration_ms(dur)
{
}
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Activity bar events
// ============================================================================

namespace ActivityBarItemId
{
// Standard activity bar items mirroring the core SidebarModes
constexpr const char* kFileExplorer = "workbench.view.explorer";
constexpr const char* kSearch = "workbench.view.search";
constexpr const char* kSettings = "workbench.view.settings";
constexpr const char* kThemes = "workbench.view.themes";
constexpr const char* kExtensions = "workbench.view.extensions";
constexpr const char* kNotebooks = "workbench.view.notebooks";
constexpr const char* kCanvas = "workbench.view.canvas";
constexpr const char* kGraph = "workbench.view.graph";
constexpr const char* kAI = "workbench.view.ai";
constexpr const char* kFlashcards = "workbench.view.flashcards";
constexpr const char* kGit = "workbench.view.git";
constexpr const char* kTasks = "workbench.view.tasks";
constexpr const char* kDatabase = "workbench.view.database";
constexpr const char* kPresentation = "workbench.view.presentation";
constexpr const char* kAccount = "workbench.view.account";
} // namespace ActivityBarItemId

// Type alias to replace the old Enum
using ActivityBarItem = std::string;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityBarSelectionEvent)
ActivityBarItem item{ActivityBarItemId::kFileExplorer};

ActivityBarSelectionEvent() = default;
explicit ActivityBarSelectionEvent(ActivityBarItem item_id)
    : item(std::move(item_id))
{
}
MARKAMP_DECLARE_EVENT_END;

// Phase 09 Task 2: SecondarySidebarSelectionEvent — broadcast when a tab is clicked in the
// secondary sidebar
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecondarySidebarSelectionEvent)
ActivityBarItem item;

SecondarySidebarSelectionEvent() = default;
explicit SecondarySidebarSelectionEvent(ActivityBarItem item_id)
    : item(std::move(item_id))
{
}
MARKAMP_DECLARE_EVENT_END;

// Phase 06 Task 8: SidebarModeChangedEvent — broadcast when sidebar mode switches
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarModeChangedEvent)
std::string previous_mode; // Migrated from int to string
std::string new_mode;
MARKAMP_DECLARE_EVENT_END;

// Phase 08 Task 19: Sidebar Header Breadcrumb Mode
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarBreadcrumbUpdateEvent)
std::string panel_id;
std::vector<std::string> breadcrumb_path;
MARKAMP_DECLARE_EVENT_END;

// Phase 06 Task 7: Badge notification events for ActivityBar
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchResultCountEvent)
int count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchProgressEvent)
int files_scanned{0};
int total_files{0};
int matches_found{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticsCountChangedEvent)
int error_count{0};
int warning_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionUpdatesAvailableEvent)
int update_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Workspace management events
// ============================================================================

MARKAMP_DECLARE_EVENT(WorkspaceRefreshRequestEvent);
MARKAMP_DECLARE_EVENT(ShowStartupRequestEvent);

// ============================================================================
// R6 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(FindRequestEvent);
MARKAMP_DECLARE_EVENT(ReplaceRequestEvent);
MARKAMP_DECLARE_EVENT(SearchNextMatchRequestEvent);
MARKAMP_DECLARE_EVENT(SearchPrevMatchRequestEvent);
MARKAMP_DECLARE_EVENT(DuplicateLineRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleCommentRequestEvent);
MARKAMP_DECLARE_EVENT(DeleteLineRequestEvent);
MARKAMP_DECLARE_EVENT(WrapToggleRequestEvent);

// ============================================================================
// R7 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(MoveLineUpRequestEvent);
MARKAMP_DECLARE_EVENT(MoveLineDownRequestEvent);
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenGitLogRequestEvent)
std::string workspace_root;
MARKAMP_DECLARE_EVENT_END;
MARKAMP_DECLARE_EVENT(NavigateToNextGitChangeRequestEvent);
MARKAMP_DECLARE_EVENT(NavigateToPreviousGitChangeRequestEvent);
MARKAMP_DECLARE_EVENT(JoinLinesRequestEvent);
MARKAMP_DECLARE_EVENT(SortLinesAscRequestEvent);
MARKAMP_DECLARE_EVENT(SortLinesDescRequestEvent);
MARKAMP_DECLARE_EVENT(TransformUpperRequestEvent);
MARKAMP_DECLARE_EVENT(TransformLowerRequestEvent);
MARKAMP_DECLARE_EVENT(TransformTitleRequestEvent);
MARKAMP_DECLARE_EVENT(SelectAllOccurrencesRequestEvent);
MARKAMP_DECLARE_EVENT(ExpandLineSelectionRequestEvent);
MARKAMP_DECLARE_EVENT(InsertLineAboveRequestEvent);
MARKAMP_DECLARE_EVENT(InsertLineBelowRequestEvent);
MARKAMP_DECLARE_EVENT(FoldAllRequestEvent);
MARKAMP_DECLARE_EVENT(UnfoldAllRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleLineNumbersRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleWhitespaceRequestEvent);

// ============================================================================
// R8 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(CopyLineUpRequestEvent);
MARKAMP_DECLARE_EVENT(CopyLineDownRequestEvent);
MARKAMP_DECLARE_EVENT(DeleteAllLeftRequestEvent);
MARKAMP_DECLARE_EVENT(DeleteAllRightRequestEvent);
MARKAMP_DECLARE_EVENT(ReverseLinesRequestEvent);
MARKAMP_DECLARE_EVENT(DeleteDuplicateLinesRequestEvent);
MARKAMP_DECLARE_EVENT(TransposeCharsRequestEvent);
MARKAMP_DECLARE_EVENT(IndentSelectionRequestEvent);
MARKAMP_DECLARE_EVENT(OutdentSelectionRequestEvent);
MARKAMP_DECLARE_EVENT(SelectWordRequestEvent);
MARKAMP_DECLARE_EVENT(SelectParagraphRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleReadOnlyRequestEvent);
MARKAMP_DECLARE_EVENT(ConvertIndentSpacesRequestEvent);
MARKAMP_DECLARE_EVENT(ConvertIndentTabsRequestEvent);
MARKAMP_DECLARE_EVENT(JumpToBracketRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleMinimapRequestEvent);
MARKAMP_DECLARE_EVENT(FoldCurrentRequestEvent);
MARKAMP_DECLARE_EVENT(UnfoldCurrentRequestEvent);
MARKAMP_DECLARE_EVENT(AddLineCommentRequestEvent);
MARKAMP_DECLARE_EVENT(RemoveLineCommentRequestEvent);

// ============================================================================
// R9 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(TrimTrailingWSRequestEvent);
MARKAMP_DECLARE_EVENT(ExpandSelectionRequestEvent);
MARKAMP_DECLARE_EVENT(ShrinkSelectionRequestEvent);
MARKAMP_DECLARE_EVENT(CursorUndoRequestEvent);
MARKAMP_DECLARE_EVENT(CursorRedoRequestEvent);
MARKAMP_DECLARE_EVENT(MoveTextLeftRequestEvent);
MARKAMP_DECLARE_EVENT(MoveTextRightRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleAutoIndentRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleBracketMatchingRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleCodeFoldingRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleIndentGuidesRequestEvent);
MARKAMP_DECLARE_EVENT(SelectToBracketRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleBlockCommentRequestEvent);
MARKAMP_DECLARE_EVENT(InsertDateTimeRequestEvent);
MARKAMP_DECLARE_EVENT(BoldRequestEvent);
MARKAMP_DECLARE_EVENT(ItalicRequestEvent);
MARKAMP_DECLARE_EVENT(InlineCodeRequestEvent);
MARKAMP_DECLARE_EVENT(BlockquoteRequestEvent);
MARKAMP_DECLARE_EVENT(CycleHeadingRequestEvent);
MARKAMP_DECLARE_EVENT(InsertTableRequestEvent);

// ============================================================================
// R10 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(ToggleSmartListContinuationRequestEvent);
MARKAMP_DECLARE_EVENT(CloseOtherTabsRequestEvent);
MARKAMP_DECLARE_EVENT(CloseSavedTabsRequestEvent);
MARKAMP_DECLARE_EVENT(InsertLinkRequestEvent);
MARKAMP_DECLARE_EVENT(AddCursorBelowRequestEvent);
MARKAMP_DECLARE_EVENT(AddCursorAboveRequestEvent);
MARKAMP_DECLARE_EVENT(AddCursorNextOccurrenceRequestEvent);
MARKAMP_DECLARE_EVENT(RemoveSurroundingBracketsRequestEvent);
MARKAMP_DECLARE_EVENT(DuplicateSelectionOrLineRequestEvent);
MARKAMP_DECLARE_EVENT(ShowTableEditorRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleScrollBeyondLastLineRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleHighlightCurrentLineRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleAutoClosingBracketsRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleStickyScrollRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleFontLigaturesRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleSmoothCaretRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleInlineColorPreviewRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleEdgeColumnRulerRequestEvent);
MARKAMP_DECLARE_EVENT(EnsureFinalNewlineRequestEvent);
MARKAMP_DECLARE_EVENT(InsertSnippetRequestEvent);

// ============================================================================
// R11 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(ToggleSmoothScrollingRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleTrailingWSHighlightRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleAutoTrimWSRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleGutterSeparatorRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleInsertFinalNewlineRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleWhitespaceBoundaryRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleLinkAutoCompleteRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleDragDropRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleAutoSaveRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleEmptySelClipboardRequestEvent);
MARKAMP_DECLARE_EVENT(CycleRenderWhitespaceRequestEvent);
// NOTE: DeleteCurrentLineRequestEvent removed — use DeleteLineRequestEvent (R6)
MARKAMP_DECLARE_EVENT(CopyLineNoSelRequestEvent);
MARKAMP_DECLARE_EVENT(AddSelNextMatchRequestEvent);
MARKAMP_DECLARE_EVENT(SmartBackspaceRequestEvent);
MARKAMP_DECLARE_EVENT(HideTableEditorRequestEvent);
MARKAMP_DECLARE_EVENT(AutoPairBoldRequestEvent);
MARKAMP_DECLARE_EVENT(AutoPairItalicRequestEvent);
MARKAMP_DECLARE_EVENT(AutoPairCodeRequestEvent);
// NOTE: ToggleMinimapR11RequestEvent removed — use ToggleMinimapRequestEvent (R8)

// ============================================================================
// R12 Editor action events (deduplicated — only truly new ones)
// ============================================================================

// NOTE: The following R12 events were removed as duplicates of R8/R9 canonical events:
//   ReverseSelectedLinesRequestEvent   → use ReverseLinesRequestEvent
//   TransposeCharactersRequestEvent    → use TransposeCharsRequestEvent
//   FoldCurrentRegionRequestEvent      → use FoldCurrentRequestEvent
//   UnfoldCurrentRegionRequestEvent    → use UnfoldCurrentRequestEvent
//   JumpToMatchingBracketRequestEvent  → use JumpToBracketRequestEvent

// NOTE: SelectToMatchingBracketRequestEvent removed — use SelectToBracketRequestEvent (R9)
MARKAMP_DECLARE_EVENT(CloseTabsToLeftRequestEvent);
MARKAMP_DECLARE_EVENT(CloseTabsToRightRequestEvent);
MARKAMP_DECLARE_EVENT(PinTabRequestEvent);
MARKAMP_DECLARE_EVENT(UnpinTabRequestEvent);

// ============================================================================
// R13 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(PrintDocumentRequestEvent);
MARKAMP_DECLARE_EVENT(CopyFilePathRequestEvent);
MARKAMP_DECLARE_EVENT(RevealInFinderRequestEvent);
MARKAMP_DECLARE_EVENT(ZoomInRequestEvent);
MARKAMP_DECLARE_EVENT(ZoomOutRequestEvent);
MARKAMP_DECLARE_EVENT(ZoomResetRequestEvent);
MARKAMP_DECLARE_EVENT(ConvertEolLfRequestEvent);
MARKAMP_DECLARE_EVENT(ConvertEolCrlfRequestEvent);

// ============================================================================
// R15 events
// ============================================================================

MARKAMP_DECLARE_EVENT(NewFileRequestEvent);

// ============================================================================
// R19 events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabDuplicateRequestEvent)
std::string file_path;

TabDuplicateRequestEvent() = default;
explicit TabDuplicateRequestEvent(std::string path)
    : file_path(std::move(path))
{
}
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 07 events: InputBox & QuickPick UI
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShowInputBoxRequestEvent)
std::string title;
std::string prompt;
std::string value;
std::string placeholder;
bool password{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShowQuickPickRequestEvent)
std::string title;
std::string placeholder;
bool can_pick_many{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Phase 01: Block lifecycle events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockCreatedEvent)
std::string block_id;
std::string block_type; // type abbreviation
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockUpdatedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockDeletedEvent)
std::string block_id;
std::string parent_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockMovedEvent)
std::string block_id;
std::string old_parent_id;
std::string new_parent_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockFoldedEvent)
std::string block_id;
bool folded{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Phase 03: Notebook lifecycle events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookCreatedEvent)
std::string notebook_id;
std::string name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookRenamedEvent)
std::string notebook_id;
std::string new_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookRemovedEvent)
std::string notebook_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookOpenedEvent)
std::string notebook_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookClosedEvent)
std::string notebook_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(NotebooksSortedEvent);

// ============================================================================
// V3 Document events (Phase 5: DocumentFileSystem)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentLoadedEvent)
std::string box_id;
std::string path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentSavedEvent)
std::string box_id;
std::string path;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentCreatedEvent)
std::string box_id;
std::string block_id;
std::string title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentRenamedEvent)
std::string box_id;
std::string block_id;
std::string title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentRemovedEvent)
std::string box_id;
std::string block_id;
std::string path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentMovedEvent)
std::string box_id;
std::string from_path;
std::string to_path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Block CRUD events (Phase 7: BlockService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransactionPerformedEvent)
std::string txn_id;
int operation_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransactionUndoneEvent)
std::string txn_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransactionRedoneEvent)
std::string txn_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockInsertedEvent)
std::string block_id;
std::string parent_id;
std::string doc_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockContentUpdatedEvent)
std::string block_id;
std::string doc_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(HeadingFoldedEvent)
std::string heading_id;
bool folded{false};
int affected_block_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Attribute events (Phase 8: AttributeService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockAttrsChangedEvent)
std::string block_id;
std::vector<std::string> changed_keys;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockBookmarkChangedEvent)
std::string block_id;
std::string bookmark_label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockMemoChangedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockNameChangedEvent)
std::string block_id;
std::string new_name;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Block reference events (Phase 9: RefParser/RefResolver)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefCreatedEvent)
std::string source_block_id;
std::string def_block_id;
std::string ref_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefDeletedEvent)
std::string source_block_id;
std::string def_block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefUpdatedEvent)
std::string source_block_id;
std::vector<std::string> old_def_ids;
std::vector<std::string> new_def_ids;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefResolvedEvent)
std::string source_block_id;
std::string def_block_id;
bool target_exists{false};
std::string display_text;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockRefNavigateRequestEvent)
std::string target_block_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Backlink events (Phase 10: BacklinkService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksChangedEvent)
std::string def_block_id;
int backlink_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MentionsChangedEvent)
std::string block_id;
int mention_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkPanelOpenRequestEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphViewOpenRequestEvent)
std::string focal_block_id;
int depth{2};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Outline events (Phase 11: OutlineService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineChangedEvent)
std::string root_id;
int heading_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineScrollToEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineActiveHeadingChangedEvent)
std::string root_id;
std::string heading_text;
int heading_level{0};
int source_line{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlinePanelOpenRequestEvent)
std::string root_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BreadcrumbNavigateEvent)
std::string root_id;
std::string heading_text;
int source_line{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Tag events (Phase 12: TagService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagRenamedEvent)
std::string old_path;
std::string new_path;
int affected_block_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagRemovedEvent)
std::string tag_path;
int affected_block_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagsChangedEvent)
std::string block_id;
std::vector<std::string> old_tags;
std::vector<std::string> new_tags;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagPanelOpenRequestEvent)
std::string initial_tag;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagSelectedEvent)
std::string tag_path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Bookmark events (Phase 13: BookmarkService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkAddedEvent)
std::string block_id;
std::string label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkRemovedEvent)
std::string block_id;
std::string previous_label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkRenamedEvent)
std::string old_label;
std::string new_label;
int affected_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkPanelOpenRequestEvent)
std::string initial_label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkNavigateEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Search events (Phase 14: SearchService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchCompletedEvent)
std::string query_string;
int result_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexUpdatedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexRebuiltEvent)
int total_blocks{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GlobalSearchRequestEvent)
std::string initial_query;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 SQL Query events (Phase 15: QueryService)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QueryExecutedEvent)
std::string sql;
double elapsed_ms{0.0};
int row_count{0};
bool success{true};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QueryErrorEvent)
std::string sql;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QueryPanelOpenRequestEvent)
std::string initial_sql;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V3 Search UI events (Phase 16: SearchPanel)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchPanelOpenedEvent)
std::string initial_query;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchPanelClosedEvent)
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchResultNavigatedEvent)
std::string block_id;
int result_index{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchReplaceCompletedEvent)
std::string search_query;
std::string replace_text;
int replaced_count{0};
int blocks_modified{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 10: Search System Completion events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchHistoryUpdatedEvent)
int entry_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SavedSearchCreatedEvent)
std::string name;
std::string query_string;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SavedSearchDeletedEvent)
std::string name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchExportCompletedEvent)
std::string format; ///< "markdown", "csv", or "json"
std::string path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchIndexHealthEvent)
int total_docs{0};
int stale_docs{0};
bool is_rebuilding{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionSearchEvent)
std::string selected_text;
int match_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View data model events (Phase 17)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVCreatedEvent)
std::string av_id;
std::string name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVDeletedEvent)
std::string av_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKeyAddedEvent)
std::string av_id;
std::string key_id;
std::string key_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKeyRemovedEvent)
std::string av_id;
std::string key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowAddedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowRemovedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVValueChangedEvent)
std::string av_id;
std::string key_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVSavedEvent)
std::string av_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVLoadedEvent)
std::string av_id;
std::string name;
int key_count{0};
int row_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View table interaction events (Phase 19)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVCellEditedEvent)
std::string av_id;
std::string key_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnAddedEvent)
std::string av_id;
std::string key_id;
std::string key_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnRemovedEvent)
std::string av_id;
std::string key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnReorderedEvent)
std::string av_id;
std::string key_id;
int old_index;
int new_index;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnResizedEvent)
std::string av_id;
std::string key_id;
int new_width;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVViewChangedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVPageChangedEvent)
std::string av_id;
int page;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View filter/sort events (Phase 20)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVFilterChangedEvent)
std::string av_id;
std::string view_id;
int active_filter_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVSortChangedEvent)
std::string av_id;
std::string view_id;
int active_sort_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View gallery events (Phase 21)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVGalleryCardClickedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVGalleryViewConfigChangedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View kanban events (Phase 22)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanCardMovedEvent)
std::string av_id;
std::string block_id;
std::string old_value;
std::string new_value;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanLaneCollapsedEvent)
std::string av_id;
std::string option_name;
bool collapsed;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVKanbanCardClickedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Attribute View relation/rollup events (Phase 23)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRelationChangedEvent)
std::string source_av_id;
std::string source_block_id;
std::string relation_key_id;
int target_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVTwoWayRelationCreatedEvent)
std::string source_av_id;
std::string source_key_id;
std::string target_av_id;
std::string target_key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRollupRecalculatedEvent)
std::string av_id;
int values_updated;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// FSRS Scheduling Events (Phase 24)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FSRSCardScheduledEvent)
std::string card_id;
std::string deck_id;
std::string rating;
double new_stability;
double new_difficulty;
double interval_days;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FSRSParametersChangedEvent)
std::string deck_id;
double request_retention;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FSRSCardStateChangedEvent)
std::string card_id;
std::string old_state;
std::string new_state;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardStoreLoadedEvent)
int deck_count;
int card_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardStoreSavedEvent)
int decks_saved;
int cards_saved;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Deck Management Events (Phase 25)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckCreatedEvent)
std::string deck_id;
std::string deck_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckDeletedEvent)
std::string deck_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeckRenamedEvent)
std::string deck_id;
std::string old_name;
std::string new_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardsAddedToDeckEvent)
std::string deck_id;
int card_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardsRemovedFromDeckEvent)
std::string deck_id;
int card_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardResetEvent)
int card_count;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Review Session Events (Phase 26)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReviewSessionStartedEvent)
std::string deck_id;
int total_cards;
int due_cards;
int new_cards;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReviewSessionCompletedEvent)
std::string deck_id;
int cards_reviewed;
int again_count;
int good_count;
int total_seconds;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CardReviewedEvent)
std::string card_id;
std::string deck_id;
std::string rating;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReviewUndoEvent)
std::string card_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Knowledge Graph Data Events (Phase 27)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphDataComputedEvent)
std::string scope; // "global", "local", "notebook"
int node_count;
int link_count;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphFilterChangedEvent)
int min_refs;
bool show_daily_notes;
bool show_orphans;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphCacheInvalidatedEvent);

// ============================================================================
// Knowledge Graph Visualization Events (Phase 28)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeClickedEvent)
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeHoveredEvent)
std::string block_id;
std::string label;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphLayoutCompleteEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShowLocalGraphRequestEvent)
std::string center_block_id;
int depth;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(ShowGlobalGraphRequestEvent);

// ============================================================================
// V4 Phase 01: Document Model events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentParsedEvent)
std::string document_id;
int link_count{0};
int tag_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrontmatterChangedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WikiLinkResolvedEvent)
std::string source_doc_id;
std::string target_path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 02: Vault Management events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultOpenedEvent)
std::string vault_path;
int document_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultClosedEvent)
std::string vault_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultDocumentCreatedEvent)
std::string document_id;
std::string title;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultDocumentRenamedEvent)
std::string document_id;
std::string old_title;
std::string new_title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultDocumentDeletedEvent)
std::string document_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultFileChangedEvent)
std::string file_path;
std::string change_type; // "created", "modified", "deleted"
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultReindexedEvent)
int document_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 03: Backlink Index events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkIndexRebuiltEvent)
int total_links{0};
int document_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinkIndexUpdatedEvent)
std::string document_id;
int backlink_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksQueryEvent)
std::string document_id;
int result_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 04: Tag System events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagIndexRebuiltEvent)
int unique_tags{0};
int total_tag_assignments{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultTagRenamedEvent)
std::string old_tag;
std::string new_tag;
int documents_modified{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultTagDeletedEvent)
std::string tag;
int documents_modified{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 05: Search Engine events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultSearchIndexRebuiltEvent)
int document_count{0};
int term_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 06: Backlinks Panel events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileOpenRequestEvent)
std::string file_path;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksPanelRefreshedEvent)
std::string document_id;
int linked_count{0};
int unlinked_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 07: Graph Engine events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphComputedEvent)
int node_count{0};
int edge_count{0};
int cluster_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphLayoutIterationEvent)
int iteration{0};
double kinetic_energy{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphInvalidatedEvent);

// ============================================================================
// V4 Phase 08: Graph View Interactive UI events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeSelectedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphNodeDoubleClickedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphViewZoomChangedEvent)
double zoom_level{1.0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 09: Daily Notes events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DailyNoteOpenedEvent)
std::string document_id;
std::string date_string;
bool newly_created{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(OpenTodayNoteRequestEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DailyNoteNavigateEvent)
std::string direction;
std::string from_date;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 10: Embed / Transclusion events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EmbedResolvedEvent)
std::string source_doc_id;
std::string target_path;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransclusionUpdatedEvent)
std::string source_doc_id;
std::string embedded_doc_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 11: Link Suggestion events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkAutocompleteTriggerEvent)
std::string prefix;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkSuggestionsReadyEvent)
std::string document_id;
int suggestion_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 12: Outline Panel events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ScrollToLineRequestEvent)
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineUpdatedEvent)
int heading_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 13: Live Preview events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LivePreviewCursorLineChangedEvent)
int old_line{-1};
int new_line{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 14: Template events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateAppliedEvent)
std::string template_name;
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TemplateInsertRequestEvent)
std::string template_name;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 16: Find/Replace events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(FindCompletedEvent)
int match_count{0};
int files_with_matches{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReplaceCompletedEvent)
int replacements{0};
int files_modified{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 17: Clipboard events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PasteProcessedEvent)
std::string content_type;
bool converted{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImagePastedEvent)
std::string saved_path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 18: LaTeX events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LatexRenderErrorEvent)
std::string latex_source;
std::string error_message;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 12: Editor Group System (Group Action Bar)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorGroupSplitRequestEvent)
int source_tabbar_id{0};
bool is_horizontal_split{true};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorGroupMoreActionsEvent)
int source_tabbar_id{0};
int screen_x{0};
int screen_y{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorGroupFocusRequestEvent)
int group_index{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(EditorGroupToggleMaximizeEvent);

// ============================================================================
// V4 Phase 19: Pane events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneSplitEvent)
int new_pane_id{0};
std::string direction;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneClosedEvent)
int pane_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneFocusChangedEvent)
int pane_id{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 20: Navigation events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WikiLinkClickedEvent)
std::string link_target;
bool ctrl_held{false};
bool alt_held{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigationEvent)
std::string document_id;
std::string direction;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 21: Layout events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutChangedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelVisibilityChangedEvent)
std::string panel_id;
bool visible{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelMovedEvent)
std::string panel_id;
int new_position{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 22: Vault style events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultStyleLoadedEvent)
std::string vault_path;
bool has_custom_css{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultStyleChangedEvent)
std::string css_content;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 23: Theme marketplace events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeInstalledEvent)
std::string theme_id;
std::string theme_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeUninstalledEvent)
std::string theme_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeMarketplaceRefreshedEvent)
int theme_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 24: Task management events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskIndexRebuiltEvent)
int total_tasks{0};
int completed{0};
int overdue{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskToggledEvent)
std::string task_id;
int new_status{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 25: Kanban events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KanbanCardMovedEvent)
std::string task_id;
std::string from_column;
std::string to_column;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KanbanBoardLoadedEvent)
std::string document_id;
int column_count{0};
int card_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 26: Calendar events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CalendarDateSelectedEvent)
std::string date_string; // YYYY-MM-DD
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CalendarMonthChangedEvent)
int year{0};
unsigned month{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 27: Presentation events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationStartedEvent)
int slide_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(PresentationEndedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideChangedEvent)
int slide_index{0};
int total_slides{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 28: Encryption events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentEncryptedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentDecryptedEvent)
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PasswordRequiredEvent)
std::string document_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 29: Kernel events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStartedEvent)
std::string kernel_id;
std::string language;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStoppedEvent)
std::string kernel_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelStateChangedEvent)
std::string kernel_id;
int state{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellExecutionCompletedEvent)
std::string cell_id;
std::string kernel_id;
bool success{false};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputEvent)
std::string cell_id;
std::string kernel_id;
std::string mime_type;
std::string content;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 30: Kernel adapter events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(KernelLanguageDetectedEvent)
std::string language;
int environment_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MagicCommandExecutedEvent)
std::string command;
std::string result;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 31: Cell output events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputRenderedEvent)
std::string cell_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputClearedEvent)
std::string cell_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputCollapsedEvent)
std::string cell_id;
bool collapsed{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 32: DataFrame/Chart events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DataFrameRenderedEvent)
std::string cell_id;
int rows{0};
int columns{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChartRenderedEvent)
std::string cell_id;
std::string format;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 33: Widget events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetCreatedEvent)
std::string widget_id;
std::string widget_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetUpdatedEvent)
std::string widget_id;
std::string property;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetDestroyedEvent)
std::string widget_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 34: Notebook cell events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellAddedEvent)
std::string cell_id;
int position{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellRemovedEvent)
std::string cell_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellMovedEvent)
std::string cell_id;
int old_position{0};
int new_position{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellExecutionRecordedEvent)
std::string cell_id;
int execution_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VariablesRefreshedEvent)
int variable_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 35: Git events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitRepoOpenedEvent)
std::string path;
std::string branch;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitStatusChangedEvent)
int modified{0};
int staged{0};
int untracked{0};
std::string branch_name;
int ahead{0};
int behind{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitCommitCreatedEvent)
std::string hash;
std::string message;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 25: Version Control Integration events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitStashSavedEvent)
std::string stash_id;
std::string message;
int file_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitStashAppliedEvent)
std::string stash_id;
std::string message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitRemoteFetchedEvent)
std::string remote_name;
int new_commits{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitRemotePushedEvent)
std::string remote_name;
int pushed_refs{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MergeConflictsDetectedEvent)
std::string file_path;
int conflict_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MergeConflictsResolvedEvent)
std::string file_path;
std::string strategy;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 26: AI Integration events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIProviderConfiguredEvent)
int provider{0};
std::string model;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIRequestCompletedEvent)
int action{0};
int tokens_used{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIRateLimitWarningEvent)
double usage_percentage{0.0};
double limit{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIConversationSavedEvent)
std::string session_id;
int message_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AITagsSuggestedEvent)
std::string document_id;
int suggestion_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIDocumentGeneratedEvent)
std::string topic;
int word_count{0};
MARKAMP_DECLARE_EVENT_END;

// Improvement 22: AI chat request from sidebar
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIChatRequestEvent)
std::string message;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 27: Cloud Sync & Collaboration events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncCompletedEvent)
int files_uploaded{0};
int files_downloaded{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncConflictDetectedEvent)
std::string file_path;
int conflict_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncConflictResolvedEvent)
std::string file_path;
int resolution_strategy{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncScheduleChangedEvent)
int interval_minutes{0};
bool is_paused{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceShareCreatedEvent)
std::string share_id;
int permission{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SyncOfflineQueuedEvent)
int queued_count{0};
int64_t total_bytes{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 36: Local Graph events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LocalGraphBuiltEvent)
std::string focus_id;
int node_count{0};
int edge_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LocalGraphFilteredEvent)
std::string tag;
int remaining_nodes{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 16: Knowledge Graph Analytics events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphAnalyticsComputedEvent)
int node_count{0};
int edge_count{0};
double density{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OrphanDetectedEvent)
int orphan_count{0};
int suggested_links_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphAnnotationAddedEvent)
std::string node_id;
std::string annotation_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphExportedEvent)
std::string format;
int node_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphSnapshotTakenEvent)
int snapshot_index{0};
int node_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphSearchCompletedEvent)
std::string query;
int result_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 37: Notebook Export events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookExportStartedEvent)
int cell_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookExportCompletedEvent)
std::string format;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 38: Magic Command events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(MagicExecutedEvent)
std::string command;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EnvironmentChangedEvent)
std::string environment;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EnvironmentDetectedEvent)
int count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 39: Snapshot events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotCreatedEvent)
std::string snapshot_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotRestoredEvent)
std::string snapshot_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotsPrunedEvent)
std::string file_path;
int pruned_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V4 Phase 40: Notebook Diff events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookDiffComputedEvent)
int cells_changed{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookMergeCompletedEvent)
int conflicts{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V5 Canvas events (Phases 1-5)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasViewportChangedEvent)
double zoom{1.0};
double pan_x{0.0};
double pan_y{0.0};
double visible_min_x{0.0};
double visible_min_y{0.0};
double visible_max_x{0.0};
double visible_max_y{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectAddedEvent)
uint64_t object_id{0};
uint8_t object_type{0}; // CanvasObjectType as uint8_t
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectRemovedEvent)
uint64_t object_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectModifiedEvent)
uint64_t object_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasToolChangedEvent)
uint8_t tool_mode{0}; // ToolMode as uint8_t
std::string tool_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasSelectionChangedEvent)
std::vector<uint64_t> selected_ids;
int count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectHoveredEvent)
uint64_t object_id{0}; // 0 = no hover
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectMovedEvent)
std::vector<uint64_t> object_ids;
double delta_x{0.0};
double delta_y{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectResizedEvent)
std::vector<uint64_t> object_ids;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectRotatedEvent)
std::vector<uint64_t> object_ids;
double angle_radians{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasUndoRedoChangedEvent)
bool can_undo{false};
bool can_redo{false};
std::string undo_description;
std::string redo_description;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardSavedEvent)
std::string board_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardLoadedEvent)
std::string board_id;
std::string board_name;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 6: Canvas Workbench Shell events
// ============================================================================

/// Requests that a board be opened (or a new one created).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardOpenRequestEvent)
std::string board_id;   ///< Board ID to open; empty = create new board
std::string board_name; ///< Optional display name for new boards
MARKAMP_DECLARE_EVENT_END;

/// Fired when the layout switches into canvas mode.
MARKAMP_DECLARE_EVENT(CanvasModeActivatedEvent);

/// Fired when the layout leaves canvas mode and returns to editor.
MARKAMP_DECLARE_EVENT(CanvasModeDeactivatedEvent);

/// V17 Phase 01 W01: Board opened and ready for interaction.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardOpenedEvent)
std::string board_id;
std::string board_name;
size_t object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W01: Board closed and removed from workspace.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardClosedEvent)
std::string board_id;
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W01: Workspace shell initialized and ready.
MARKAMP_DECLARE_EVENT(WorkspaceShellReadyEvent);

/// V17 Phase 01 W01: Inspector panel toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasInspectorToggledEvent)
bool visible{false};
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W01: Minimap panel toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasMinimapToggledEvent)
bool visible{false};
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W02: A new board was created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardCreatedEvent)
std::string board_id;
std::string board_name;
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W02: Board was renamed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardRenamedEvent)
std::string board_id;
std::string old_name;
std::string new_name;
MARKAMP_DECLARE_EVENT_END;

/// V17 Phase 01 W02: Board was duplicated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardDuplicatedEvent)
std::string source_board_id;
std::string new_board_id;
std::string new_board_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W03: Viewport Camera events ─────────────────────

/// W03: Request to fit viewport to board/selection/object.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewportFitRequestEvent)
std::string fit_mode; ///< "board", "selection", "object"
MARKAMP_DECLARE_EVENT_END;

/// W03: Viewport zoom level changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewportZoomChangedEvent)
double old_zoom{1.0};
double new_zoom{1.0};
double anchor_x{0.0};
double anchor_y{0.0};
MARKAMP_DECLARE_EVENT_END;

/// W03: Request to reset viewport to default (zoom 100%, pan origin).
MARKAMP_DECLARE_EVENT(ViewportResetRequestEvent);

// ── V17 Phase 01 W04: Tool Rail events ────────────────────────────

/// W04: Tool rail selection changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolRailSelectionChangedEvent)
std::string tool_id;
std::string tool_name;
std::string group;
MARKAMP_DECLARE_EVENT_END;

/// W04: Tool group expanded/collapsed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolGroupExpandedEvent)
std::string group_name;
bool expanded{false};
MARKAMP_DECLARE_EVENT_END;

/// W04: Quick-switch between previous and current tool.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolQuickSwitchEvent)
std::string from_tool_id;
std::string to_tool_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W05: Pointer Routing events ─────────────────────

/// W05: Pointer capture state changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PointerCaptureChangedEvent)
bool captured{false};
uint8_t tool_mode{0}; ///< ToolMode as uint8_t
MARKAMP_DECLARE_EVENT_END;

/// W05: Input device type changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PointerDeviceChangedEvent)
std::string device_type; ///< "mouse", "trackpad", "stylus", "touch"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W06: Input State Machine events ────────────────

/// W06: Tool state machine transitioned.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolStateTransitionEvent)
std::string from_state;
std::string to_state;
uint8_t tool_mode{0};
MARKAMP_DECLARE_EVENT_END;

/// W06: Active gesture was force-cancelled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolGestureCancelledEvent)
uint8_t tool_mode{0};
std::string reason;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W07: Keyboard Commanding events ────────────────

/// W07: A keyboard command was executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyCommandExecutedEvent)
std::string command_id;
std::string shortcut;
std::string category;
MARKAMP_DECLARE_EVENT_END;

/// W07: Keyboard shortcut conflict detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyConflictDetectedEvent)
std::string shortcut;
std::string conflicting_commands;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W08: Cursor & Mode Feedback events ─────────────

/// W08: Canvas cursor style changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CursorStyleChangedEvent)
std::string cursor_style;
uint8_t tool_mode{0};
MARKAMP_DECLARE_EVENT_END;

/// W08: Mode feedback label/icon changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ModeFeedbackChangedEvent)
std::string label;
std::string icon_hint;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W09: Grid & Rulers events ──────────────────────

/// W09: Grid visibility toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GridVisibilityChangedEvent)
bool visible{false};
double spacing{20.0};
MARKAMP_DECLARE_EVENT_END;

/// W09: Ruler visibility toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RulerVisibilityChangedEvent)
bool visible{false};
std::string units;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W10: Snap Preferences events ───────────────────

/// W10: Snap mode configuration changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapModeChangedEvent)
bool grid_snap{false};
bool object_snap{false};
bool angle_snap{false};
MARKAMP_DECLARE_EVENT_END;

/// W10: Snap threshold changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapThresholdChangedEvent)
double old_threshold{8.0};
double new_threshold{8.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W11: Context Menus events ──────────────────────

/// W11: Context menu opened for a given scope.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ContextMenuOpenedEvent)
std::string scope;       ///< "empty_space", "single_object", "multi_select"
std::string object_type; ///< Object type if single-object scope
int action_count{0};     ///< Number of available actions
MARKAMP_DECLARE_EVENT_END;

/// W11: A context menu action was executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ContextMenuActionExecutedEvent)
std::string action_id; ///< e.g. "canvas.cut", "canvas.paste"
std::string scope;     ///< Scope in which the action was invoked
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W12: Undo & Redo Plumbing events ──────────────

/// W12: An undo operation was executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(UndoExecutedEvent)
std::string description;  ///< Human-readable description of undone action
int remaining_count{0};   ///< Remaining undo entries
MARKAMP_DECLARE_EVENT_END;

/// W12: A redo operation was executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RedoExecutedEvent)
std::string description;  ///< Human-readable description of redone action
int remaining_count{0};   ///< Remaining redo entries
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W13: Autosave & Recovery events ────────────────

/// W13: Autosave was triggered for a board.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutosaveTriggeredEvent)
std::string board_id;   ///< Board being saved
std::string save_path;  ///< File path for the save
MARKAMP_DECLARE_EVENT_END;

/// W13: Recovery data detected for a board on startup.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutosaveRecoveryDetectedEvent)
std::string board_id;       ///< Board with recovery data
std::string recovery_path;  ///< Path to the recovery file
std::string timestamp;      ///< When the recovery data was written
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W14: Inspector Shell events ────────────────────

/// W14: Inspector panel opened/refreshed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(InspectorOpenedEvent)
int section_count{0};    ///< Number of visible sections
int selected_count{0};   ///< Number of selected objects
bool is_multi_select{false};
MARKAMP_DECLARE_EVENT_END;

/// W14: A property was changed via the inspector.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(InspectorPropertyChangedEvent)
std::string property_key; ///< Property that changed
std::string old_value;    ///< Previous value
std::string new_value;    ///< New value
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W15: Minimap Shell events ──────────────────────

/// W15: Minimap visibility toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MinimapVisibilityChangedEvent)
bool visible{false};
std::string mode; ///< "simplified" or "detailed"
MARKAMP_DECLARE_EVENT_END;

/// W15: Navigation triggered from minimap click/drag.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MinimapNavigationEvent)
double target_x{0.0};
double target_y{0.0};
bool from_minimap{true}; ///< true if triggered via minimap click
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W16: Onboarding & Empty States events ─────────

/// W16: An onboarding walkthrough step was completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OnboardingStepCompletedEvent)
std::string step_id;
int progress_percent{0};
MARKAMP_DECLARE_EVENT_END;

/// W16: An onboarding hint was dismissed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OnboardingDismissedEvent)
std::string hint_id;
bool dismissed_by_user{true};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W17: Board Settings events ─────────────────────

/// W17: Board setting changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardSettingsChangedEvent)
std::string board_id;
std::string setting_key;
std::string new_value;
MARKAMP_DECLARE_EVENT_END;

/// W17: Board background preset changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardBackgroundChangedEvent)
std::string board_id;
std::string background_preset;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W18: Theme Tokens events ───────────────────────

/// W18: Canvas theme token resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasThemeTokenResolvedEvent)
std::string token_name;
std::string resolved_value;
MARKAMP_DECLARE_EVENT_END;

/// W18: Canvas theme contrast guardrail warning.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasThemeContrastWarningEvent)
std::string token_name;
double contrast_ratio{0.0};
double min_required{3.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W19: Command Palette Integration events ────────

/// W19: Command palette opened.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandPaletteOpenedEvent)
int command_count{0};
std::string context; ///< "canvas", "editor", "global"
MARKAMP_DECLARE_EVENT_END;

/// W19: Command executed from palette.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandPaletteExecutedEvent)
std::string command_id;
std::string source; ///< "palette", "shortcut", "menu"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 01 W20: Event & Analytics Contracts events ────────

/// W20: Analytics event recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AnalyticsEventRecordedEvent)
std::string event_name;
std::string category;
MARKAMP_DECLARE_EVENT_END;

/// W20: Analytics flush requested.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AnalyticsFlushRequestedEvent)
int pending_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W01: Single Selection events ──────────────────

/// P02-W01: Single object selected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SingleSelectionEvent)
std::string object_id;
std::string object_type;
std::string hit_target; ///< "handle", "object", "container"
MARKAMP_DECLARE_EVENT_END;

/// P02-W01: Selection cleared.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionClearedEvent)
int previous_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W02: Multi Selection events ───────────────────

/// P02-W02: Multi-selection performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MultiSelectionEvent)
int selected_count{0};
std::string method; ///< "box", "additive", "subtractive"
MARKAMP_DECLARE_EVENT_END;

/// P02-W02: Select all objects.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectAllEvent)
int total_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W03: Move Operations events ───────────────────

/// P02-W03: Move operation started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MoveStartedEvent)
int object_count{0};
double start_x{0.0};
double start_y{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W03: Move operation completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MoveCompletedEvent)
int object_count{0};
double delta_x{0.0};
double delta_y{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W04: Resize Operations events ─────────────────

/// P02-W04: Resize operation started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ResizeStartedEvent)
int object_count{0};
std::string handle; ///< e.g. "TopLeft", "BottomRight"
MARKAMP_DECLARE_EVENT_END;

/// P02-W04: Resize operation completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ResizeCompletedEvent)
int object_count{0};
double scale_x{1.0};
double scale_y{1.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W05: Rotation Operations events ───────────────

/// P02-W05: Rotate operation started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RotateStartedEvent)
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W05: Rotate operation completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RotateCompletedEvent)
int object_count{0};
double angle_degrees{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W06: Alignment Guides events ──────────────────

/// P02-W06: Alignment guides shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AlignmentGuideShownEvent)
int guide_count{0};
std::string axis; ///< "horizontal", "vertical", "both"
MARKAMP_DECLARE_EVENT_END;

/// P02-W06: Alignment guides hidden.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AlignmentGuideHiddenEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W07: Object Snapping events ───────────────────

/// P02-W07: Snap engaged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapEngagedEvent)
std::string snap_type; ///< "edge", "center", "grid"
std::string target_id;
MARKAMP_DECLARE_EVENT_END;

/// P02-W07: Snap disengaged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapDisengagedEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W08: Distribute & Align events ────────────────

/// P02-W08: Alignment action performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AlignActionEvent)
std::string action; ///< "left", "center_h", "right", "top", "center_v", "bottom"
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W08: Distribution action performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DistributeActionEvent)
std::string axis; ///< "horizontal", "vertical"
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W09: Smart Spacing events ──────────────────────

/// P02-W09: Smart spacing activated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SmartSpacingActivatedEvent)
double spacing_px{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W09: Smart spacing deactivated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SmartSpacingDeactivatedEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W10: Grouping events ───────────────────────────

/// P02-W10: Objects grouped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectsGroupedEvent)
std::string group_id;
int member_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W10: Objects ungrouped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectsUngroupedEvent)
std::string group_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W11: Frame Membership events ──────────────────

/// P02-W11: Object added to frame.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrameMemberAddedEvent)
std::string frame_id;
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

/// P02-W11: Object removed from frame.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrameMemberRemovedEvent)
std::string frame_id;
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W12: Layers & Z Order events ──────────────────

/// P02-W12: Layer order changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayerOrderChangedEvent)
std::string object_id;
std::string action; ///< "bring_to_front", "send_to_back"
MARKAMP_DECLARE_EVENT_END;

/// P02-W12: Layer order reset.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayerOrderResetEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W13: Locking & Pinning events ─────────────────

/// P02-W13: Object locked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectLockedEvent)
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

/// P02-W13: Object unlocked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectUnlockedEvent)
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W14: Precision Nudge events ───────────────────

/// P02-W14: Precision nudge performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PrecisionNudgeEvent)
std::string direction; ///< "up", "down", "left", "right"
double step_px{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W14: Nudge step size changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NudgeStepChangedEvent)
double step_px{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W15: Duplicate & Paste In Place events ────────

/// P02-W15: Duplicate in place.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DuplicateInPlaceEvent)
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W15: Paste in place.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PasteInPlaceEvent)
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W16: Drag Autoscroll events ───────────────────

/// P02-W16: Autoscroll started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoscrollStartedEvent)
std::string direction; ///< "up", "down", "left", "right", etc.
MARKAMP_DECLARE_EVENT_END;

/// P02-W16: Autoscroll stopped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoscrollStoppedEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W17: Viewport-Aware Transforms events ─────────

/// P02-W17: Viewport transform synced.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewportTransformSyncEvent)
double zoom{1.0};
double pan_x{0.0};
double pan_y{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W17: Viewport clamped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewportClampedEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W18: Multi User Selection events ──────────────

/// P02-W18: Remote selection received.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RemoteSelectionReceivedEvent)
std::string user_id;
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P02-W18: Remote lock conflict.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RemoteLockConflictEvent)
std::string user_id;
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W19: Transform History Quality events ─────────

/// P02-W19: Transform undo.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransformUndoEvent)
std::string action;
MARKAMP_DECLARE_EVENT_END;

/// P02-W19: Transform redo.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransformRedoEvent)
std::string action;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 02 W20: Selection Action Surfaces events ─────────

/// P02-W20: Action surface shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActionSurfaceShownEvent)
std::string surface_type; ///< "toolbar", "contextmenu", "quickaction"
MARKAMP_DECLARE_EVENT_END;

/// P02-W20: Action surface hidden.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActionSurfaceHiddenEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W01: Freehand Pen Engine events ───────────────

/// P03-W01: Stroke captured.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StrokeCapturedEvent)
std::string stroke_id;
int point_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P03-W01: Stroke smoothed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StrokeSmoothedEvent)
std::string stroke_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W02: Pen Presets events ────────────────────────

/// P03-W02: Pen preset applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PenPresetAppliedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

/// P03-W02: Pen preset saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PenPresetSavedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W03: Eraser & Lasso events ────────────────────

/// P03-W03: Eraser stroke performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EraserStrokeEvent)
int erased_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P03-W03: Lasso selection performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LassoSelectionEvent)
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W04: Shape Tool events ─────────────────────────

/// P03-W04: Shape created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeCreatedEvent)
std::string shape_type;
MARKAMP_DECLARE_EVENT_END;

/// P03-W04: Shape resized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeResizedEvent)
std::string shape_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W05: Text Box Authoring events ────────────────

/// P03-W05: Text box created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TextBoxCreatedEvent)
std::string text_box_id;
MARKAMP_DECLARE_EVENT_END;

/// P03-W05: Text box edited.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TextBoxEditedEvent)
std::string text_box_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W06: Sticky Notes events ──────────────────────

/// P03-W06: Sticky note created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StickyNoteCreatedEvent)
std::string note_id;
std::string color;
MARKAMP_DECLARE_EVENT_END;

/// P03-W06: Sticky note color changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StickyNoteColorChangedEvent)
std::string note_id;
std::string color;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W07: Color Authoring events ───────────────────

/// P03-W07: Palette color selected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteColorSelectedEvent)
std::string color;
MARKAMP_DECLARE_EVENT_END;

/// P03-W07: Palette color saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteColorSavedEvent)
std::string color;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W08: Typography Controls events ───────────────

/// P03-W08: Typography changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TypographyChangedEvent)
std::string property; ///< "font_size", "line_height", etc.
std::string value;
MARKAMP_DECLARE_EVENT_END;

/// P03-W08: Font applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FontAppliedEvent)
std::string font_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W09: Fill, Stroke & Effects events ────────────

/// P03-W09: Fill style changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FillStyleChangedEvent)
std::string object_id;
std::string fill;
MARKAMP_DECLARE_EVENT_END;

/// P03-W09: Stroke style changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StrokeStyleChangedEvent)
std::string object_id;
std::string stroke;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W10: Style Presets events ──────────────────────

/// P03-W10: Style preset applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StylePresetAppliedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

/// P03-W10: Style preset saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StylePresetSavedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W11: Inline Rich Text events ──────────────────

/// P03-W11: Inline rich text enabled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(InlineRichTextEnabledEvent)
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

/// P03-W11: Inline rich text formatted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(InlineRichTextFormattedEvent)
std::string object_id;
std::string format; ///< "bold", "italic", "link", etc.
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W12: Shape Library Quick Insert events ─────────

/// P03-W12: Shape inserted from library.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeLibraryInsertEvent)
std::string shape_name;
MARKAMP_DECLARE_EVENT_END;

/// P03-W12: Shape library opened.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeLibraryOpenedEvent)
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W13: Shape Recognition events ─────────────────

/// P03-W13: Shape recognized from freehand.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeRecognizedEvent)
std::string recognized_type;
double confidence{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P03-W13: Shape recognition rejected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ShapeRecognitionRejectedEvent)
std::string stroke_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W14: Image Annotation events ──────────────────

/// P03-W14: Image annotation added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImageAnnotationAddedEvent)
std::string image_id;
std::string annotation_type; ///< "arrow", "highlight", "text"
MARKAMP_DECLARE_EVENT_END;

/// P03-W14: Image annotation removed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImageAnnotationRemovedEvent)
std::string image_id;
std::string annotation_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W15: Comments & Callouts events ───────────────

/// P03-W15: Comment added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommentAddedEvent)
std::string object_id;
std::string comment_text;
MARKAMP_DECLARE_EVENT_END;

/// P03-W15: Callout created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CalloutCreatedEvent)
std::string callout_type; ///< "info", "warning", "critical"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W16: Tables For Authoring events ──────────────

/// P03-W16: Table created on canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasTableCreatedEvent)
int rows{0};
int columns{0};
MARKAMP_DECLARE_EVENT_END;

/// P03-W16: Table cell edited.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasTableCellEditedEvent)
std::string table_id;
int row{0};
int column{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W17: Icons, Stickers & Emoji events ───────────

/// P03-W17: Icon inserted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(IconInsertedEvent)
std::string icon_name;
MARKAMP_DECLARE_EVENT_END;

/// P03-W17: Sticker inserted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StickerInsertedEvent)
std::string sticker_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W18: Creator Shortcuts events ─────────────────

/// P03-W18: Creator shortcut invoked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CreatorShortcutInvokedEvent)
std::string shortcut_id;
MARKAMP_DECLARE_EVENT_END;

/// P03-W18: Creator shortcut registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CreatorShortcutRegisteredEvent)
std::string shortcut_id;
std::string action;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W19: Bulk Style Editing events ────────────────

/// P03-W19: Bulk style applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BulkStyleAppliedEvent)
int object_count{0};
std::string style_property;
MARKAMP_DECLARE_EVENT_END;

/// P03-W19: Bulk style reset.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BulkStyleResetEvent)
int object_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 03 W20: Authoring Motion Feedback events ─────────

/// P03-W20: Motion preview started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MotionPreviewStartedEvent)
std::string object_id;
std::string motion_type;
MARKAMP_DECLARE_EVENT_END;

/// P03-W20: Motion preview ended.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MotionPreviewEndedEvent)
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W01: Connector Creation events ────────────────

/// P04-W01: Connector created between objects.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorCreatedEvent)
std::string from_id;
std::string to_id;
std::string connector_type; ///< "straight", "curved", "orthogonal"
MARKAMP_DECLARE_EVENT_END;

/// P04-W01: Connector deleted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorDeletedEvent)
std::string connector_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W02: Connector Routing events ─────────────────

/// P04-W02: Connector route changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorRouteChangedEvent)
std::string connector_id;
std::string route_type; ///< "straight", "bezier", "orthogonal"
MARKAMP_DECLARE_EVENT_END;

/// P04-W02: Connector waypoint added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorWaypointAddedEvent)
std::string connector_id;
double waypoint_x{0.0};
double waypoint_y{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W03: Connector Labels & Ports events ──────────

/// P04-W03: Connector label set.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorLabelSetEvent)
std::string connector_id;
std::string label;
MARKAMP_DECLARE_EVENT_END;

/// P04-W03: Port assigned on object.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PortAssignedEvent)
std::string object_id;
std::string port_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W04: Diagram Library events ───────────────────

/// P04-W04: Diagram shape inserted from library.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramShapeInsertedEvent)
std::string shape_type;
std::string category;
MARKAMP_DECLARE_EVENT_END;

/// P04-W04: Diagram library browsed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramLibraryBrowsedEvent)
std::string category;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W05: Flowchart Grammar events ─────────────────

/// P04-W05: Flowchart defaults applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlowchartDefaultsAppliedEvent)
std::string grammar_name;
MARKAMP_DECLARE_EVENT_END;

/// P04-W05: Flowchart validated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlowchartValidatedEvent)
int node_count{0};
int connector_count{0};
bool is_valid{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W06: Mind Maps events ─────────────────────────

/// P04-W06: Mind map node added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MindMapNodeAddedEvent)
std::string parent_id;
std::string node_label;
MARKAMP_DECLARE_EVENT_END;

/// P04-W06: Mind map branch collapsed/expanded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MindMapBranchToggledEvent)
std::string node_id;
bool collapsed{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W07: Kanban Boards events ─────────────────────

/// P04-W07: Kanban card moved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasKanbanCardMovedEvent)
std::string card_id;
std::string from_lane;
std::string to_lane;
MARKAMP_DECLARE_EVENT_END;

/// P04-W07: Kanban lane added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KanbanLaneAddedEvent)
std::string lane_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W08: Frames & Sections events ─────────────────

/// P04-W08: Frame created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrameCreatedEvent)
std::string frame_name;
double width{0.0};
double height{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P04-W08: Section added to frame.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SectionAddedEvent)
std::string frame_id;
std::string section_name;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W09: Swimlanes & Grids events ────────────────

/// P04-W09: Swimlane created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SwimlaneCreatedEvent)
std::string orientation; ///< "horizontal", "vertical"
int lane_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P04-W09: Grid layout applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GridLayoutAppliedEvent)
int grid_rows{0};
int grid_columns{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W10: Container Auto Layout events ─────────────

/// P04-W10: Auto layout triggered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoLayoutTriggeredEvent)
std::string container_id;
std::string layout_type; ///< "horizontal", "vertical", "grid", "wrap"
MARKAMP_DECLARE_EVENT_END;

/// P04-W10: Auto layout spacing changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutoLayoutSpacingChangedEvent)
std::string container_id;
double spacing{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W11: Advanced Tables events ───────────────────

/// P04-W11: Table column added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AdvancedTableColumnAddedEvent)
std::string table_id;
std::string column_name;
MARKAMP_DECLARE_EVENT_END;

/// P04-W11: Table row sorted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AdvancedTableSortedEvent)
std::string table_id;
std::string sort_column;
bool ascending{true};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W12: Cross Board Links events ─────────────────

/// P04-W12: Cross board link created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrossBoardLinkCreatedEvent)
std::string source_board_id;
std::string target_board_id;
MARKAMP_DECLARE_EVENT_END;

/// P04-W12: Cross board link navigated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrossBoardLinkNavigatedEvent)
std::string link_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W13: Diagram From Selection events ────────────

/// P04-W13: Diagram generated from selection.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramFromSelectionEvent)
int selected_count{0};
std::string diagram_type;
MARKAMP_DECLARE_EVENT_END;

/// P04-W13: Selection structured into diagram.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionStructuredEvent)
std::string structure_type; ///< "flowchart", "hierarchy", "sequence"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W14: Connected Layout Cleanup events ──────────

/// P04-W14: Layout cleanup triggered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutCleanupTriggeredEvent)
int affected_connectors{0};
MARKAMP_DECLARE_EVENT_END;

/// P04-W14: Connector reflow after move.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConnectorReflowEvent)
std::string connector_id;
std::string trigger_action; ///< "align", "distribute", "move"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W15: Relationship Metadata events ─────────────

/// P04-W15: Relationship metadata set.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RelationshipMetadataSetEvent)
std::string connector_id;
std::string metadata_key;
std::string metadata_value;
MARKAMP_DECLARE_EVENT_END;

/// P04-W15: Relationship type assigned.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RelationshipTypeAssignedEvent)
std::string connector_id;
std::string relationship_type; ///< "dependency", "association", "aggregation"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W16: Diagram Templates events ────────────────

/// P04-W16: Diagram template applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramTemplateAppliedEvent)
std::string template_name;
std::string template_category;
MARKAMP_DECLARE_EVENT_END;

/// P04-W16: Diagram template saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramTemplateSavedEvent)
std::string template_name;
int node_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W17: Dependency Validation events ─────────────

/// P04-W17: Dependency check run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DependencyCheckRunEvent)
int total_links{0};
int broken_links{0};
MARKAMP_DECLARE_EVENT_END;

/// P04-W17: Broken dependency fixed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BrokenDependencyFixedEvent)
std::string connector_id;
std::string fix_action; ///< "reconnect", "remove", "reroute"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W18: Diagram Export Readiness events ──────────

/// P04-W18: Diagram export preview generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramExportPreviewEvent)
std::string export_format; ///< "svg", "png", "pdf"
MARKAMP_DECLARE_EVENT_END;

/// P04-W18: Diagram export completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramExportCompletedEvent)
std::string export_format;
std::string output_path;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W19: Collaborative Diagram Editing events ─────

/// P04-W19: Collaborative diagram lock acquired.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramLockAcquiredEvent)
std::string diagram_id;
std::string user_id;
MARKAMP_DECLARE_EVENT_END;

/// P04-W19: Collaborative diagram lock released.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramLockReleasedEvent)
std::string diagram_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 04 W20: Diagram Test Matrix events ──────────────

/// P04-W20: Diagram test suite run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramTestSuiteRunEvent)
int tests_passed{0};
int tests_failed{0};
MARKAMP_DECLARE_EVENT_END;

/// P04-W20: Diagram regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagramRegressionDetectedEvent)
std::string test_name;
std::string description;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W01: Image Placement events ──────────────────

/// P05-W01: Image placed on canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImagePlacedEvent)
std::string image_path;
double scale{1.0};
MARKAMP_DECLARE_EVENT_END;

/// P05-W01: Image replaced on canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImageReplacedEvent)
std::string object_id;
std::string new_image_path;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W02: Video & Web Embeds events ───────────────

/// P05-W02: Video embed added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VideoEmbedAddedEvent)
std::string embed_url;
std::string embed_type; ///< "youtube", "vimeo", "iframe"
MARKAMP_DECLARE_EVENT_END;

/// P05-W02: Web embed resized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WebEmbedResizedEvent)
std::string object_id;
double width{0.0};
double height{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W03: PDF Pages events ────────────────────────

/// P05-W03: PDF page added to canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PdfPageAddedEvent)
std::string pdf_path;
int page_number{1};
MARKAMP_DECLARE_EVENT_END;

/// P05-W03: PDF page navigated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasPdfPageNavigatedEvent)
std::string object_id;
int target_page{1};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W04: Bookmark Cards events ──────────────────

/// P05-W04: Bookmark card created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkCardCreatedEvent)
std::string url;
std::string title;
MARKAMP_DECLARE_EVENT_END;

/// P05-W04: Bookmark card refreshed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkCardRefreshedEvent)
std::string object_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W05: App Widgets events ──────────────────────

/// P05-W05: App widget added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AppWidgetAddedEvent)
std::string widget_type;
std::string widget_config;
MARKAMP_DECLARE_EVENT_END;

/// P05-W05: App widget configured.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AppWidgetConfiguredEvent)
std::string object_id;
std::string config_key;
std::string config_value;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W06: Icon Insertion events ───────────────────

/// P05-W06: Icon inserted on canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasIconInsertedEvent)
std::string icon_name;
std::string icon_set; ///< "lucide", "material", "custom"
MARKAMP_DECLARE_EVENT_END;

/// P05-W06: Icon color changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(IconColorChangedEvent)
std::string object_id;
std::string new_color;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W07: Drag & Drop Intake events ──────────────

/// P05-W07: External content dropped on canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ContentDroppedEvent)
std::string content_type; ///< "image", "file", "text", "url"
std::string source_path;
MARKAMP_DECLARE_EVENT_END;

/// P05-W07: Drop intake completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DropIntakeCompletedEvent)
int items_ingested{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W08: Clipboard Ingest events ────────────────

/// P05-W08: Clipboard content pasted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ClipboardPastedEvent)
std::string paste_format; ///< "html", "image", "text", "rtf"
MARKAMP_DECLARE_EVENT_END;

/// P05-W08: Rich paste converted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RichPasteConvertedEvent)
std::string source_format;
std::string target_format;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W09: Media Optimization events ──────────────

/// P05-W09: Media optimization run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaOptimizationRunEvent)
int assets_optimized{0};
double size_reduction_pct{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P05-W09: Media quality level set.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaQualitySetEvent)
std::string quality_level; ///< "low", "medium", "high", "original"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W10: Cropping & Masking events ──────────────

/// P05-W10: Image cropped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImageCroppedEvent)
std::string object_id;
double crop_x{0.0};
double crop_y{0.0};
double crop_width{0.0};
double crop_height{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P05-W10: Image mask applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImageMaskAppliedEvent)
std::string object_id;
std::string mask_type; ///< "circle", "rounded_rect", "custom"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W11: Captions & Metadata events ─────────────

/// P05-W11: Caption set on asset.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetCaptionSetEvent)
std::string object_id;
std::string caption_text;
MARKAMP_DECLARE_EVENT_END;

/// P05-W11: Alt text set on asset.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetAltTextSetEvent)
std::string object_id;
std::string alt_text;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W12: Asset Library Panel events ─────────────

/// P05-W12: Asset added to library.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetLibraryAddedEvent)
std::string asset_name;
std::string asset_category;
MARKAMP_DECLARE_EVENT_END;

/// P05-W12: Asset removed from library.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetLibraryRemovedEvent)
std::string asset_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W13: Asset Styling events ───────────────────

/// P05-W13: Asset frame applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetFrameAppliedEvent)
std::string object_id;
std::string frame_style; ///< "shadow", "border", "rounded", "none"
MARKAMP_DECLARE_EVENT_END;

/// P05-W13: Asset shadow changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetShadowChangedEvent)
std::string object_id;
double shadow_offset{0.0};
double shadow_blur{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W14: Mixed Media Boards events ─────────────

/// P05-W14: Media composition created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaCompositionCreatedEvent)
int object_count{0};
std::string layout_mode; ///< "freeform", "grid", "stack"
MARKAMP_DECLARE_EVENT_END;

/// P05-W14: Media layer reordered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaLayerReorderedEvent)
std::string object_id;
int new_z_index{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W15: Import & Convert events ────────────────

/// P05-W15: External file imported to canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExternalFileImportedEvent)
std::string source_path;
std::string converted_format;
MARKAMP_DECLARE_EVENT_END;

/// P05-W15: Import conversion completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportConversionCompletedEvent)
int files_converted{0};
bool all_succeeded{true};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W16: Media Review Flows events ──────────────

/// P05-W16: Media review started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaReviewStartedEvent)
std::string object_id;
std::string reviewer;
MARKAMP_DECLARE_EVENT_END;

/// P05-W16: Media review completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MediaReviewCompletedEvent)
std::string object_id;
bool approved{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W17: Licensing & Security events ────────────

/// P05-W17: Content license set.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ContentLicenseSetEvent)
std::string object_id;
std::string license_type; ///< "cc-by", "cc-by-sa", "proprietary", "public-domain"
MARKAMP_DECLARE_EVENT_END;

/// P05-W17: Security scan completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityScanCompletedEvent)
int assets_scanned{0};
int threats_found{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W18: Asset Serialization & Sync events ──────

/// P05-W18: Asset metadata serialized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetMetadataSerializedEvent)
int assets_serialized{0};
std::string format; ///< "json", "yaml", "binary"
MARKAMP_DECLARE_EVENT_END;

/// P05-W18: Asset sync completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetSyncCompletedEvent)
int assets_synced{0};
int conflicts_detected{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W19: Rich Content Export events ─────────────

/// P05-W19: Rich content export started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RichContentExportStartedEvent)
std::string export_format; ///< "png", "pdf", "html", "svg"
int objects_to_export{0};
MARKAMP_DECLARE_EVENT_END;

/// P05-W19: Rich content export completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RichContentExportCompletedEvent)
std::string output_path;
bool success{true};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 05 W20: Asset Test Coverage events ─────────────

/// P05-W20: Asset test suite run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetTestSuiteRunEvent)
int tests_run{0};
int tests_passed{0};
MARKAMP_DECLARE_EVENT_END;

/// P05-W20: Asset regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetRegressionDetectedEvent)
std::string test_name;
std::string description;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W01: Live Presence events ───────────────────

/// P06-W01: Collaborator joined board.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollaboratorJoinedEvent)
std::string user_id;
std::string display_name;
MARKAMP_DECLARE_EVENT_END;

/// P06-W01: Collaborator left board.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollaboratorLeftEvent)
std::string user_id;
std::string reason; ///< "disconnect", "navigated_away", "timeout"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W02: Remote Selections & Follow events ─────

/// P06-W02: Remote selection changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RemoteSelectionChangedEvent)
std::string user_id;
int selected_object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W02: Follow mode toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FollowModeToggledEvent)
std::string target_user_id;
bool following{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W03: Conflict Handling events ──────────────

/// P06-W03: Edit conflict detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditConflictDetectedEvent)
std::string object_id;
std::string conflicting_user;
MARKAMP_DECLARE_EVENT_END;

/// P06-W03: Conflict resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConflictResolvedEvent)
std::string object_id;
std::string resolution_strategy; ///< "last_write_wins", "merge", "manual"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W04: Comment Threads events ────────────────

/// P06-W04: Comment thread created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommentThreadCreatedEvent)
std::string thread_id;
std::string anchor_object_id;
MARKAMP_DECLARE_EVENT_END;

/// P06-W04: Comment reply added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommentReplyAddedEvent)
std::string thread_id;
std::string author;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W05: Voting & Reactions events ─────────────

/// P06-W05: Vote cast on object.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VoteCastEvent)
std::string object_id;
std::string voter;
MARKAMP_DECLARE_EVENT_END;

/// P06-W05: Reaction added to object.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReactionAddedEvent)
std::string object_id;
std::string reaction_type; ///< "thumbs_up", "heart", "star", "fire"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W06: Timer & Facilitation events ───────────

/// P06-W06: Facilitation timer started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FacilitationTimerStartedEvent)
int duration_seconds{0};
std::string facilitator;
MARKAMP_DECLARE_EVENT_END;

/// P06-W06: Facilitation timer expired.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FacilitationTimerExpiredEvent)
std::string session_id;
bool auto_extended{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W07: Private Reveal & Presentation events ──

/// P06-W07: Private reveal initiated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PrivateRevealInitiatedEvent)
std::string presenter;
int hidden_object_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W07: Content revealed to audience.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ContentRevealedEvent)
int revealed_count{0};
std::string reveal_mode; ///< "all_at_once", "sequential", "by_group"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W08: Permissions & Shared Locks events ─────

/// P06-W08: Object permission changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectPermissionChangedEvent)
std::string object_id;
std::string permission_level; ///< "edit", "view", "locked"
MARKAMP_DECLARE_EVENT_END;

/// P06-W08: Shared lock acquired.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SharedLockAcquiredEvent)
std::string object_id;
std::string locked_by;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W09: Offline Queue events ──────────────────

/// P06-W09: Offline operation queued.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OfflineOperationQueuedEvent)
int queue_depth{0};
std::string operation_type; ///< "create", "update", "delete"
MARKAMP_DECLARE_EVENT_END;

/// P06-W09: Offline queue flushed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OfflineQueueFlushedEvent)
int operations_applied{0};
int operations_conflicted{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W10: Activity Feed events ──────────────────

/// P06-W10: Activity feed entry added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityFeedEntryAddedEvent)
std::string actor;
std::string action_description;
MARKAMP_DECLARE_EVENT_END;

/// P06-W10: Activity feed cleared.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityFeedClearedEvent)
int entries_cleared{0};
std::string cleared_by;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W11: Change Highlights events ─────────────

/// P06-W11: Change highlight detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChangeHighlightDetectedEvent)
int changes_since_last_visit{0};
std::string last_visitor;
MARKAMP_DECLARE_EVENT_END;

/// P06-W11: Change highlight dismissed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChangeHighlightDismissedEvent)
std::string user_id;
int dismissed_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W12: Invites & Session Entry events ────────

/// P06-W12: Session invite sent.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SessionInviteSentEvent)
std::string invitee_email;
std::string session_id;
MARKAMP_DECLARE_EVENT_END;

/// P06-W12: Session joined via invite.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SessionJoinedViaInviteEvent)
std::string user_id;
std::string invite_code;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W13: Collaborator Panels events ───────────

/// P06-W13: Collaborator panel opened.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollaboratorPanelOpenedEvent)
std::string panel_type; ///< "awareness", "moderation", "facilitation"
int active_users{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W13: Collaborator panel action taken.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollaboratorPanelActionEvent)
std::string action_type; ///< "mute", "kick", "promote"
std::string target_user;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W14: Co-Editing Text events ───────────────

/// P06-W14: Co-editing session started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CoEditingStartedEvent)
std::string object_id;
int editors_count{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W14: Co-editing cursor moved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CoEditingCursorMovedEvent)
std::string user_id;
int cursor_position{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W15: Workshop Templates events ────────────

/// P06-W15: Workshop template applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkshopTemplateAppliedEvent)
std::string template_name;
int objects_created{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W15: Workshop template saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkshopTemplateSavedEvent)
std::string template_name;
std::string author;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W16: Async Review events ──────────────────

/// P06-W16: Async review requested.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AsyncReviewRequestedEvent)
std::string reviewer_id;
std::string board_id;
MARKAMP_DECLARE_EVENT_END;

/// P06-W16: Async review completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AsyncReviewCompletedEvent)
std::string reviewer_id;
bool approved{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W17: Moderation & Recovery events ─────────

/// P06-W17: User moderated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(UserModeratedEvent)
std::string target_user;
std::string moderation_action; ///< "mute", "remove", "restrict"
MARKAMP_DECLARE_EVENT_END;

/// P06-W17: Board state recovered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardStateRecoveredEvent)
std::string recovery_point;
int objects_restored{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W18: Multiplayer Performance events ───────

/// P06-W18: Presence throttled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresenceThrottledEvent)
int active_connections{0};
int throttle_interval_ms{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W18: Batch update sent.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BatchUpdateSentEvent)
int operations_batched{0};
int batch_size_bytes{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W19: Collaboration Analytics events ───────

/// P06-W19: Collaboration metrics snapshot.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollabMetricsSnapshotEvent)
int total_edits{0};
int unique_contributors{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W19: Engagement score calculated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EngagementScoreCalculatedEvent)
double engagement_score{0.0};
std::string session_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 06 W20: Collaboration Coverage events ────────

/// P06-W20: Collab test suite run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollabTestSuiteRunEvent)
int tests_run{0};
int tests_passed{0};
MARKAMP_DECLARE_EVENT_END;

/// P06-W20: Collab regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollabRegressionDetectedEvent)
std::string test_name;
std::string failure_detail;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W01: Minimap Navigation events ────────────

/// P07-W01: Minimap viewport changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MinimapViewportChangedEvent)
double viewport_x{0.0};
double viewport_y{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W01: Minimap visibility toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MinimapVisibilityToggledEvent)
bool visible{false};
std::string toggle_source; ///< "menu", "shortcut", "auto"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W02: Outline Navigation events ────────────

/// P07-W02: Outline node selected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineNodeSelectedEvent)
std::string node_id;
std::string node_type; ///< "section", "frame", "group"
MARKAMP_DECLARE_EVENT_END;

/// P07-W02: Outline tree refreshed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineTreeRefreshedEvent)
int total_nodes{0};
int depth_levels{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W03: Search & Find events ─────────────────

/// P07-W03: Board search executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardSearchExecutedEvent)
std::string query;
int results_found{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W03: Board search result navigated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardSearchResultNavigatedEvent)
std::string object_id;
int result_index{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W04: Tagging & Filters events ─────────────

/// P07-W04: Tag filter applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagFilterAppliedEvent)
std::string tag_name;
int matching_objects{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W04: Tag filter cleared.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TagFilterClearedEvent)
int filters_removed{0};
int total_objects_visible{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W05: Sections & Landmarks events ──────────

/// P07-W05: Section landmark created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SectionLandmarkCreatedEvent)
std::string section_name;
std::string section_id;
MARKAMP_DECLARE_EVENT_END;

/// P07-W05: Navigate to landmark.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LandmarkNavigatedEvent)
std::string landmark_id;
double target_zoom{1.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W06: Large Board Streaming events ─────────

/// P07-W06: Tile loaded for streaming.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TileLoadedEvent)
int tile_x{0};
int tile_y{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W06: Streaming LOD changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StreamingLodChangedEvent)
int lod_level{0};
int visible_objects{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W07: Breadcrumbs & History events ─────────

/// P07-W07: Breadcrumb pushed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BreadcrumbPushedEvent)
std::string location_label;
int stack_depth{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W07: Breadcrumb navigated back.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BreadcrumbNavigatedBackEvent)
int steps_back{0};
std::string destination_label;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W08: Zoom Presets events ───────────────────

/// P07-W08: Zoom preset applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ZoomPresetAppliedEvent)
std::string preset_name; ///< "fit_all", "fit_selection", "100%", "50%"
double zoom_level{1.0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W08: Zoom preset saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ZoomPresetSavedEvent)
std::string preset_name;
double zoom_level{1.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W09: Board Bookmarks events ───────────────

/// P07-W09: Board bookmark created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardBookmarkCreatedEvent)
std::string bookmark_name;
std::string bookmark_id;
MARKAMP_DECLARE_EVENT_END;

/// P07-W09: Board bookmark navigated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardBookmarkNavigatedEvent)
std::string bookmark_id;
double viewport_zoom{1.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W10: Metadata Driven Navigation events ────

/// P07-W10: Metadata filter applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MetadataFilterAppliedEvent)
std::string filter_key; ///< "owner", "status", "type"
std::string filter_value;
MARKAMP_DECLARE_EVENT_END;

/// P07-W10: Metadata navigation jump.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MetadataNavJumpEvent)
std::string target_object_id;
std::string metadata_key;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W11: Cross Board Traversal events ─────────

/// P07-W11: Cross board link followed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrossBoardLinkFollowedEvent)
std::string source_board_id;
std::string target_board_id;
MARKAMP_DECLARE_EVENT_END;

/// P07-W11: Cross board back navigation.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrossBoardBackNavigatedEvent)
std::string returning_to_board_id;
int boards_traversed{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W12: Selection Sync events ────────────────

/// P07-W12: Selection synced to panel.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionSyncedToPanelEvent)
std::string panel_id;
int synced_objects{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W12: Selection sync toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SelectionSyncToggledEvent)
bool sync_enabled{false};
std::string panel_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W13: Saved Views events ───────────────────

/// P07-W13: Named view saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NamedViewSavedEvent)
std::string view_name;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;

/// P07-W13: Named view restored.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NamedViewRestoredEvent)
std::string view_id;
double restored_zoom{1.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W14: Presentation Navigation events ───────

/// P07-W14: Presentation slide advanced.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationSlideAdvancedEvent)
int slide_index{0};
int total_slides{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W14: Presentation mode toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationModeToggledEvent)
bool presentation_active{false};
std::string presenter_id;
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W15: Semantic Navigation events ───────────

/// P07-W15: Semantic cluster navigated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SemanticClusterNavigatedEvent)
std::string cluster_label;
int objects_in_cluster{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W15: Semantic grouping recalculated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SemanticGroupingRecalculatedEvent)
int clusters_found{0};
int ungrouped_objects{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W16: Quick Action Navigation events ───────

/// P07-W16: Quick action invoked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickActionInvokedEvent)
std::string action_name;
std::string action_source; ///< "palette", "shortcut", "contextmenu"
MARKAMP_DECLARE_EVENT_END;

/// P07-W16: Quick action result selected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickActionResultSelectedEvent)
std::string result_id;
int result_rank{0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W17: Discoverability events ───────────────

/// P07-W17: Feature hint shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureHintShownEvent)
std::string hint_id;
std::string feature_area; ///< "navigation", "editing", "collaboration"
MARKAMP_DECLARE_EVENT_END;

/// P07-W17: Feature hint dismissed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureHintDismissedEvent)
std::string hint_id;
bool dont_show_again{false};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W18: Responsive Layouts events ────────────

/// P07-W18: Layout breakpoint changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutBreakpointChangedEvent)
std::string breakpoint_name; ///< "compact", "medium", "wide"
int window_width{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W18: Navigation panel repositioned.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavPanelRepositionedEvent)
std::string panel_position; ///< "left", "bottom", "floating"
std::string trigger; ///< "auto", "manual"
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W19: Wayfinding Telemetry events ──────────

/// P07-W19: Navigation confusion detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavConfusionDetectedEvent)
int rapid_pans{0};
int zoom_reversals{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W19: Wayfinding metric recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WayfindingMetricRecordedEvent)
std::string metric_name;
double metric_value{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V17 Phase 07 W20: Navigation Coverage events ───────────

/// P07-W20: Navigation test suite run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavTestSuiteRunEvent)
int tests_run{0};
int tests_passed{0};
MARKAMP_DECLARE_EVENT_END;

/// P07-W20: Navigation regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavRegressionDetectedEvent)
std::string test_name;
std::string failure_detail;
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W01: Board Templates events ───────────────

/// P08-W01: Board template applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardTemplateAppliedEvent)
std::string template_name;
std::string template_category; ///< "sprint", "retro", "brainstorm"
MARKAMP_DECLARE_EVENT_END;

/// P08-W01: Board template saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardTemplateSavedEvent)
std::string template_name;
int objects_in_template{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W02: Object Templates events ──────────────

/// P08-W02: Object template inserted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectTemplateInsertedEvent)
std::string template_id;
int objects_inserted{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W02: Object template registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ObjectTemplateRegisteredEvent)
std::string template_name;
std::string template_type; ///< "cluster", "module", "pattern"
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W03: Advanced Style Presets events ─────────

/// P08-W03: Board style preset applied to selection.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardStylePresetAppliedEvent)
std::string preset_name;
int objects_affected{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W03: Style preset created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StylePresetCreatedEvent)
std::string preset_name;
std::string preset_scope; ///< "board", "team", "global"
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W04: Automation Rules events ──────────────

/// P08-W04: Automation rule triggered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutomationRuleTriggeredEvent)
std::string rule_name;
int actions_executed{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W04: Automation rule created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutomationRuleCreatedEvent)
std::string rule_name;
std::string trigger_type; ///< "on_create", "on_move", "on_tag"
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W05: AI Board Generation events ───────────

/// P08-W05: AI board generation requested.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIBoardGenerationRequestedEvent)
std::string prompt_text;
std::string board_type; ///< "flowchart", "mind_map", "kanban"
MARKAMP_DECLARE_EVENT_END;

/// P08-W05: AI board generation completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AIBoardGenerationCompletedEvent)
int objects_generated{0};
double generation_time_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W06: AI Cleanup & Refinement events ───────

/// P08-W06: AI cleanup requested.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AICleanupRequestedEvent)
std::string cleanup_type; ///< "normalize", "restructure", "align"
int objects_in_scope{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W06: AI cleanup completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AICleanupCompletedEvent)
int objects_modified{0};
int objects_removed{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W07: AI Summaries & Explanations events ────

/// P08-W07: AI summary requested.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AISummaryRequestedEvent)
std::string scope; ///< "board", "selection", "section"
int objects_summarized{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W07: AI summary generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AISummaryGeneratedEvent)
int word_count{0};
std::string output_format; ///< "text", "bullets", "outline"
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W08: AI Tags & Links events ───────────────

/// P08-W08: AI tag suggestions generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AITagSuggestionsGeneratedEvent)
int tags_suggested{0};
int objects_analyzed{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W08: AI link suggestions generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AILinkSuggestionsGeneratedEvent)
int links_suggested{0};
int cross_board_links{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W09: Batch Operations events ──────────────

/// P08-W09: Batch operation executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BatchOperationExecutedEvent)
std::string operation_type; ///< "move", "style", "tag", "delete"
int objects_affected{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W09: Batch operation undone.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BatchOperationUndoneEvent)
std::string operation_type;
int objects_restored{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W10: Markdown To Canvas events ────────────

/// P08-W10: Markdown import to canvas started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MarkdownToCanvasStartedEvent)
std::string source_file;
int lines_to_parse{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W10: Markdown import to canvas completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MarkdownToCanvasCompletedEvent)
int objects_created{0};
int connectors_created{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W11: CSV & Database Imports events ────────

/// P08-W11: CSV import to canvas started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CsvImportStartedEvent)
std::string source_file;
int rows_to_import{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W11: CSV import to canvas completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CsvImportCompletedEvent)
int objects_created{0};
int columns_mapped{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W12: Export Workflows events ──────────────

/// P08-W12: Board export started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardExportStartedEvent)
std::string export_format; ///< "png", "pdf", "svg", "json"
std::string export_scope; ///< "full", "selection", "viewport"
MARKAMP_DECLARE_EVENT_END;

/// P08-W12: Board export completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardExportCompletedEvent)
std::string output_path;
int objects_exported{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W13: Reusable Components events ───────────

/// P08-W13: Reusable component saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReusableComponentSavedEvent)
std::string component_name;
int child_objects{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W13: Reusable component instantiated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReusableComponentInstantiatedEvent)
std::string component_id;
std::string instance_id;
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W14: Plugin Hooks events ──────────────────

/// P08-W14: Plugin hook registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginHookRegisteredEvent)
std::string hook_name;
std::string plugin_id;
MARKAMP_DECLARE_EVENT_END;

/// P08-W14: Plugin hook invoked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginHookInvokedEvent)
std::string hook_name;
int listeners_notified{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W15: Command Macros events ────────────────

/// P08-W15: Command macro recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandMacroRecordedEvent)
std::string macro_name;
int steps_recorded{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W15: Command macro replayed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandMacroReplayedEvent)
std::string macro_name;
int objects_affected{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W16: Quick Insert events ──────────────────

/// P08-W16: Quick insert menu opened.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickInsertMenuOpenedEvent)
std::string trigger_source; ///< "slash", "toolbar", "shortcut"
int items_available{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W16: Quick insert item selected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickInsertItemSelectedEvent)
std::string item_type;
int search_rank{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W17: Smart Defaults events ────────────────

/// P08-W17: Smart default applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SmartDefaultAppliedEvent)
std::string default_type; ///< "color", "size", "font", "layout"
std::string context;
MARKAMP_DECLARE_EVENT_END;

/// P08-W17: Smart default suggestion shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SmartDefaultSuggestionShownEvent)
int suggestions_count{0};
bool suggestion_accepted{false};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W18: Profile & Preference Sync events ─────

/// P08-W18: Profile synced.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ProfileSyncedEvent)
std::string profile_id;
int preferences_synced{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W18: Preference conflict resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PreferenceConflictResolvedEvent)
std::string preference_key;
std::string resolution; ///< "local", "remote", "merged"
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W19: Onboarding Programs events ───────────

/// P08-W19: Canvas onboarding step completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasOnboardingStepCompletedEvent)
std::string step_id;
int steps_remaining{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W19: Onboarding program finished.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OnboardingProgramFinishedEvent)
std::string program_id;
int total_steps_completed{0};
MARKAMP_DECLARE_EVENT_END;

// ── V18 Phase 08 W20: Agent Ready Scaffolds events ─────────

/// P08-W20: Creation test suite run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CreationTestSuiteRunEvent)
int tests_run{0};
int tests_passed{0};
MARKAMP_DECLARE_EVENT_END;

/// P08-W20: Creation regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CreationRegressionDetectedEvent)
std::string test_name;
std::string failure_detail;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W01: Keyboard Only Canvas events ──────────

/// P09-W01: Keyboard navigation action performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyboardNavActionEvent)
std::string action; ///< "move_focus", "select", "pan"
std::string direction; ///< "up", "down", "left", "right"
MARKAMP_DECLARE_EVENT_END;

/// P09-W01: Keyboard shortcut conflict detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyboardShortcutConflictEvent)
std::string shortcut_key;
std::string conflicting_action;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W02: Screen Reader Semantics events ───────

/// P09-W02: Screen reader announcement queued.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ScreenReaderAnnouncementEvent)
std::string announcement_text;
std::string priority; ///< "polite", "assertive"
MARKAMP_DECLARE_EVENT_END;

/// P09-W02: Accessible label updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AccessibleLabelUpdatedEvent)
std::string object_id;
std::string new_label;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W03: Contrast & Color Blindness events ────

/// P09-W03: High contrast mode toggled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HighContrastModeToggledEvent)
bool high_contrast_enabled{false};
std::string contrast_profile; ///< "standard", "deuteranopia", "protanopia"
MARKAMP_DECLARE_EVENT_END;

/// P09-W03: Color accessibility check run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ColorAccessibilityCheckEvent)
int elements_checked{0};
int issues_found{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W04: Focus & Announcements events ─────────

/// P09-W04: Focus transferred between objects.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FocusTransferredEvent)
std::string from_object_id;
std::string to_object_id;
MARKAMP_DECLARE_EVENT_END;

/// P09-W04: Live region update announced.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LiveRegionUpdateEvent)
std::string region_id;
std::string update_text;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W05: Touch & Pen Parity events ────────────

/// P09-W05: Touch gesture recognized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TouchGestureRecognizedEvent)
std::string gesture_type; ///< "pinch", "rotate", "two_finger_pan"
int touch_points{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W05: Pen pressure sensitivity applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PenPressureAppliedEvent)
double pressure_level{0.0};
std::string pen_tool; ///< "draw", "erase", "highlight"
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W06: Performance Budgets events ───────────

/// P09-W06: Performance budget exceeded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PerfBudgetExceededEvent)
std::string operation;
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W06: Frame rate metric recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FrameRateMetricEvent)
double fps{0.0};
int objects_rendered{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W07: Virtualization events ────────────────

/// P09-W07: Viewport culling updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ViewportCullingUpdatedEvent)
int visible_objects{0};
int total_objects{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W07: Tile cache hit/miss recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TileCacheMetricEvent)
int cache_hits{0};
int cache_misses{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W08: Crash Recovery events ────────────────

/// P09-W08: Crash recovery checkpoint saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrashRecoveryCheckpointEvent)
std::string checkpoint_id;
int objects_saved{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W08: Crash board state recovered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CrashBoardStateRecoveredEvent)
std::string recovery_source; ///< "autosave", "checkpoint", "journal"
int objects_recovered{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W09: Data Integrity events ────────────────

/// P09-W09: Board integrity check run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardIntegrityCheckEvent)
int objects_validated{0};
int corruption_found{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W09: Data repair applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DataRepairAppliedEvent)
std::string repair_type; ///< "orphan_cleanup", "ref_fix", "schema_migrate"
int items_repaired{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W10: Accessibility Tooling events ─────────

/// P09-W10: Accessibility audit run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AccessibilityAuditRunEvent)
int elements_audited{0};
int violations_found{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W10: Accessibility regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AccessibilityRegressionEvent)
std::string test_name;
std::string violation_type;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W11: Safe Degradation events ──────────────

/// P09-W11: Safe degradation fallback activated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SafeDegradationActivatedEvent)
std::string feature_name;
std::string fallback_reason; ///< "slow_gpu", "failed_embed", "plugin_crash"
MARKAMP_DECLARE_EVENT_END;

/// P09-W11: Degraded mode exited.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DegradedModeExitedEvent)
std::string feature_name;
double degraded_duration_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W12: Observability events ─────────────────

/// P09-W12: Telemetry span recorded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TelemetrySpanRecordedEvent)
std::string span_name;
double duration_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W12: Error telemetry emitted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ErrorTelemetryEmittedEvent)
std::string error_category;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W13: Feature Flags events ─────────────────

/// P09-W13: Feature flag evaluated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureFlagEvaluatedEvent)
std::string flag_name;
bool flag_value{false};
MARKAMP_DECLARE_EVENT_END;

/// P09-W13: Feature flag override applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureFlagOverrideEvent)
std::string flag_name;
std::string override_source; ///< "user", "admin", "experiment"
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W14: Snapshot & Harnesses events ──────────

/// P09-W14: Visual snapshot captured.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VisualSnapshotCapturedEvent)
std::string snapshot_id;
int pixels_diffed{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W14: Snapshot comparison result.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SnapshotComparisonResultEvent)
std::string baseline_id;
double diff_percentage{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W15: Unit & Integration Gaps events ───────

/// P09-W15: Test coverage gap identified.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TestCoverageGapEvent)
std::string module_name;
int uncovered_lines{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W15: Integration test added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(IntegrationTestAddedEvent)
std::string test_name;
std::string covered_module;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W16: End To End Determinism events ────────

/// P09-W16: E2E test flakiness detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(E2EFlakinessDetectedEvent)
std::string test_name;
int flaky_runs{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W16: E2E timing stabilized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(E2ETimingStabilizedEvent)
std::string test_name;
double variance_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W17: Security & Privacy events ────────────

/// P09-W17: Security audit finding.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityAuditFindingEvent)
std::string finding_type; ///< "xss", "injection", "leak"
std::string severity; ///< "low", "medium", "high", "critical"
MARKAMP_DECLARE_EVENT_END;

/// P09-W17: Privacy data scrubbed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PrivacyDataScrubbedEvent)
int fields_scrubbed{0};
std::string scrub_scope; ///< "export", "telemetry", "collab"
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W18: Localization & IME events ────────────

/// P09-W18: Locale switched.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LocaleSwitchedEvent)
std::string from_locale;
std::string to_locale;
MARKAMP_DECLARE_EVENT_END;

/// P09-W18: IME composition event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(IMECompositionEvent)
std::string ime_state; ///< "start", "update", "end"
int composition_length{0};
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W19: Compliance Checklists events ─────────

/// P09-W19: Compliance check run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ComplianceCheckRunEvent)
int checks_passed{0};
int checks_failed{0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W19: Compliance violation flagged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ComplianceViolationFlaggedEvent)
std::string rule_id;
std::string violation_detail;
MARKAMP_DECLARE_EVENT_END;

// ── V19 Phase 09 W20: Operational Dashboards events ────────

/// P09-W20: Dashboard metric published.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DashboardMetricPublishedEvent)
std::string metric_name;
double metric_value{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P09-W20: Canvas health check completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasHealthCheckCompletedEvent)
int healthy_systems{0};
int degraded_systems{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W01: Parity Audit events ──────────────────

/// P10-W01: Parity audit item checked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ParityAuditItemCheckedEvent)
std::string competitor_feature;
std::string parity_status; ///< "matched", "partial", "missing"
MARKAMP_DECLARE_EVENT_END;

/// P10-W01: Parity gap identified.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ParityGapIdentifiedEvent)
std::string feature_name;
std::string gap_severity; ///< "blocker", "important", "nice_to_have"
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W02: Control Polish events ────────────────

/// P10-W02: Control density adjusted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ControlDensityAdjustedEvent)
std::string control_group;
std::string density_level; ///< "compact", "default", "spacious"
MARKAMP_DECLARE_EVENT_END;

/// P10-W02: Affordance refinement applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AffordanceRefinementAppliedEvent)
std::string element_id;
std::string refinement_type; ///< "label", "tooltip", "icon"
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W03: Motion & Microinteraction events ─────

/// P10-W03: Microinteraction triggered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MicrointeractionTriggeredEvent)
std::string interaction_name;
double duration_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W03: Motion preference applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MotionPreferenceAppliedEvent)
std::string preference; ///< "full", "reduced", "none"
bool system_prefers_reduced{false};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W04: Platform Conventions events ──────────

/// P10-W04: Platform convention applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PlatformConventionAppliedEvent)
std::string platform; ///< "macos", "windows", "linux"
std::string convention_type; ///< "shortcut", "menu", "scroll"
MARKAMP_DECLARE_EVENT_END;

/// P10-W04: Platform parity check run.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PlatformParityCheckEvent)
std::string platform;
int conventions_matched{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W05: Settings Migration events ────────────

/// P10-W05: Settings migration started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsMigrationStartedEvent)
std::string from_version;
std::string to_version;
MARKAMP_DECLARE_EVENT_END;

/// P10-W05: Settings migration completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SettingsMigrationCompletedEvent)
int settings_migrated{0};
int settings_defaulted{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W06: Documentation events ─────────────────

/// P10-W06: Documentation page generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocPageGeneratedEvent)
std::string page_id;
int sections_written{0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W06: Documentation coverage checked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocCoverageCheckedEvent)
int features_documented{0};
int features_undocumented{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W07: Benchmarks & Baselines events ────────

/// P10-W07: Benchmark run completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BenchmarkRunCompletedEvent)
std::string benchmark_name;
double result_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W07: Benchmark regression detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BenchmarkRegressionDetectedEvent)
std::string benchmark_name;
double regression_pct{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W08: Release Gates events ─────────────────

/// P10-W08: Release gate evaluated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReleaseGateEvaluatedEvent)
std::string gate_name;
bool gate_passed{false};
MARKAMP_DECLARE_EVENT_END;

/// P10-W08: Release readiness summary.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReleaseReadinessSummaryEvent)
int gates_passed{0};
int gates_failed{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W09: Beta Feedback Loops events ───────────

/// P10-W09: Beta feedback submitted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BetaFeedbackSubmittedEvent)
std::string feedback_category;
std::string board_state_id;
MARKAMP_DECLARE_EVENT_END;

/// P10-W09: Beta feedback triaged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BetaFeedbackTriagedEvent)
std::string feedback_id;
std::string triage_priority; ///< "p0", "p1", "p2", "p3"
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W10: Extension Ecosystem events ───────────

/// P10-W10: Extension compatibility checked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionCompatibilityCheckedEvent)
std::string extension_id;
bool is_compatible{false};
MARKAMP_DECLARE_EVENT_END;

/// P10-W10: Extension ecosystem report generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionEcosystemReportEvent)
int total_extensions{0};
int compatible_extensions{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W11: Advanced Drawing Polish events ───────

/// P10-W11: Drawing stroke refined.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DrawingStrokeRefinedEvent)
std::string tool_name; ///< "pen", "shape", "text"
double pressure_sensitivity{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W11: Drawing tool calibrated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DrawingToolCalibratedEvent)
std::string tool_name;
int calibration_points{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W12: Advanced Layout Polish events ────────

/// P10-W12: Layout transform refined.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutTransformRefinedEvent)
std::string transform_type; ///< "rotate", "scale", "skew"
double precision_delta{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W12: Alignment edge case resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AlignmentEdgeCaseResolvedEvent)
std::string alignment_type;
int objects_affected{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W13: Advanced Collaboration Polish events ─

/// P10-W13: Collaboration friction resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CollabFrictionResolvedEvent)
std::string friction_type; ///< "cursor_jitter", "selection_conflict", "sync_delay"
double latency_improvement_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W13: Multi-user session stabilized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(MultiUserSessionStabilizedEvent)
int concurrent_users{0};
double session_uptime_hours{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W14: Advanced Navigation Polish events ────

/// P10-W14: Board travel optimized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardTravelOptimizedEvent)
std::string navigation_mode; ///< "minimap", "keyboard", "search"
double travel_time_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W14: Navigation waypoint set.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigationWaypointSetEvent)
std::string waypoint_id;
double x{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W15: Advanced Export Polish events ────────

/// P10-W15: Export artifact finalized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportArtifactFinalizedEvent)
std::string format; ///< "pdf", "png", "svg"
int pages_exported{0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W15: Export fidelity validated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportFidelityValidatedEvent)
std::string format;
double fidelity_score{0.0}; ///< 0.0–1.0
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W16: Enterprise & Admin events ────────────

/// P10-W16: Admin policy applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AdminPolicyAppliedEvent)
std::string policy_name;
int users_affected{0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W16: Governance audit logged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GovernanceAuditLoggedEvent)
std::string action_type;
std::string actor_id;
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W17: Quality Backlog Triage events ────────

/// P10-W17: Defect triaged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DefectTriagedEvent)
std::string defect_id;
std::string severity; ///< "p0", "p1", "p2", "p3"
MARKAMP_DECLARE_EVENT_END;

/// P10-W17: Quality backlog reduced.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QualityBacklogReducedEvent)
int defects_resolved{0};
int defects_remaining{0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W18: Adoption Metrics events ──────────────

/// P10-W18: Adoption metric tracked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AdoptionMetricTrackedEvent)
std::string metric_name;
double metric_value{0.0};
MARKAMP_DECLARE_EVENT_END;

/// P10-W18: Usage trend analyzed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(UsageTrendAnalyzedEvent)
std::string feature_name;
double trend_direction{0.0}; ///< positive = growth
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W19: Support Playbooks events ─────────────

/// P10-W19: Support playbook activated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SupportPlaybookActivatedEvent)
std::string playbook_id;
std::string issue_category;
MARKAMP_DECLARE_EVENT_END;

/// P10-W19: Support resolution logged.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SupportResolutionLoggedEvent)
std::string ticket_id;
double resolution_time_hours{0.0};
MARKAMP_DECLARE_EVENT_END;

// ── V20 Phase 10 W20: Architecture Follow Ups events ───────

/// P10-W20: Tech debt item addressed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TechDebtAddressedEvent)
std::string debt_item;
std::string resolution_type; ///< "refactor", "remove", "document"
MARKAMP_DECLARE_EVENT_END;

/// P10-W20: Platform investment completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PlatformInvestmentCompletedEvent)
std::string investment_area;
int files_affected{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 7: Realtime Collaboration events
// ============================================================================

/// A participant has joined the current board session.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasSessionJoinedEvent)
std::string session_id;       ///< Session identifier
std::string participant_id;   ///< Unique user/participant ID
std::string participant_name; ///< Display name
std::string avatar_url;       ///< Optional avatar URL
MARKAMP_DECLARE_EVENT_END;

/// A participant has left the current board session.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasSessionLeftEvent)
std::string session_id;
std::string participant_id;
MARKAMP_DECLARE_EVENT_END;

/// A remote participant's presence state was updated (active, idle, away).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasPresenceUpdatedEvent)
std::string participant_id;
std::string status; ///< "active", "idle", or "away"
MARKAMP_DECLARE_EVENT_END;

/// A remote participant moved their cursor on the canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasRemoteCursorMovedEvent)
std::string participant_id;
double cursor_x{0.0};
double cursor_y{0.0};
MARKAMP_DECLARE_EVENT_END;

/// A remote participant changed their selection on the canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasRemoteSelectionChangedEvent)
std::string participant_id;
std::vector<std::string> selected_object_ids; ///< Object IDs selected by remote user
MARKAMP_DECLARE_EVENT_END;

/// A remote participant applied a patch to a canvas object.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasRemoteObjectPatchedEvent)
std::string participant_id;
std::string object_id;  ///< Target object identifier
std::string patch_type; ///< "move", "resize", "style", "content", etc.
std::string patch_json; ///< Serialized patch data
MARKAMP_DECLARE_EVENT_END;

/// A collaborative vote was started on the board.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasVoteStartedEvent)
std::string vote_id;
std::string topic;
std::string initiator_id;
std::vector<std::string> options; ///< Voting options
MARKAMP_DECLARE_EVENT_END;

/// A vote received an update (new vote cast or vote closed).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasVoteUpdatedEvent)
std::string vote_id;
std::string voter_id;
std::string chosen_option;
bool is_closed{false}; ///< True when the vote is finalized
MARKAMP_DECLARE_EVENT_END;

/// A facilitation timer was updated (started, ticked, or stopped).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasTimerUpdatedEvent)
std::string timer_id;
int remaining_seconds{0};
bool is_running{false};
bool is_expired{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 13: Canvas Collaboration & Facilitation events
// ============================================================================

/// Board or region lock state changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasLockChangedEvent)
std::string event_name; ///< e.g. "board_locked", "board_unlocked", "region_locked"
std::string participant_id;
MARKAMP_DECLARE_EVENT_END;

/// Follow mode / presenter mode status changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasFollowModeChangedEvent)
std::string status; ///< "following", "presenting", "none"
std::string participant_id;
MARKAMP_DECLARE_EVENT_END;

/// A private-reveal round started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasPrivateRoundStartedEvent)
std::string round_id;
std::string host_id;
std::string topic;
MARKAMP_DECLARE_EVENT_END;

/// Private objects were revealed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasObjectsRevealedEvent)
std::string round_id;
size_t revealed_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Collaborative undo or redo was performed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasCollabUndoRedoEvent)
std::string operation_id;
std::string participant_id;
bool is_undo{true}; ///< true for undo, false for redo
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 8: Canvas Apps Marketplace events
// ============================================================================

/// A canvas-capable extension has been registered with the marketplace.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasAppRegisteredEvent)
std::string app_id;
std::string app_name;
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

/// An app widget has been inserted onto the canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetInsertedEvent)
std::string widget_id;
std::string app_id;
std::string object_id; ///< Canvas object hosting the widget
MARKAMP_DECLARE_EVENT_END;

/// A widget sync job has started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetSyncStartedEvent)
std::string widget_id;
std::string provider_id;
MARKAMP_DECLARE_EVENT_END;

/// A widget sync job completed successfully.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetSyncCompletedEvent)
std::string widget_id;
std::string provider_id;
int items_synced{0};
MARKAMP_DECLARE_EVENT_END;

/// A widget sync job failed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetSyncFailedEvent)
std::string widget_id;
std::string provider_id;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

/// A canvas app attempted an operation without required permissions.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetPermissionDeniedEvent)
std::string app_id;
std::string requested_scope;
std::string denied_reason;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 14: Canvas Extensibility & App Widgets events
// ============================================================================

/// A custom canvas object type has been registered by an extension.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasCustomObjectRegisteredEvent)
std::string type_id;
std::string display_name;
std::string extension_id;
MARKAMP_DECLARE_EVENT_END;

/// A canvas tool has been activated (built-in or extension).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasToolActivatedEvent)
std::string tool_id;
std::string tool_label;
std::string previous_tool_id;
MARKAMP_DECLARE_EVENT_END;

/// A widget lifecycle state has changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetLifecycleEvent)
std::string widget_id;
std::string old_state;
std::string new_state;
MARKAMP_DECLARE_EVENT_END;

/// A board template has been applied to the canvas.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasTemplateAppliedEvent)
std::string template_id;
std::string template_name;
int objects_created{0};
MARKAMP_DECLARE_EVENT_END;

/// A widget message has been delivered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasWidgetMessageEvent)
std::string source_widget_id;
std::string target_widget_id;
std::string message_type;
MARKAMP_DECLARE_EVENT_END;
// ============================================================================

// V8 Phase 15: Notebook System Completion events

/// A notebook has been parsed from .ipynb format.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookParsedEvent)
std::string notebook_id;
int cell_count{0};
int nbformat_version{4};
MARKAMP_DECLARE_EVENT_END;

/// A notebook cell has been executed through the pipeline.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookCellExecutedEvent)
std::string notebook_id;
std::string cell_id;
int execution_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

/// A notebook checkpoint has been created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookCheckpointCreatedEvent)
std::string notebook_id;
std::string checkpoint_id;
MARKAMP_DECLARE_EVENT_END;

/// A notebook search has completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotebookSearchCompletedEvent)
std::string query;
int result_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Cell dependency analysis has changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellDependencyChangedEvent)
std::string cell_id;
int stale_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── Phase 20: File Management & Workspace ──

/// A file system change was detected by the FileWatcher.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileChangedEvent)
std::string path;
int change_type{0};
MARKAMP_DECLARE_EVENT_END;

/// The file tree model has been refreshed from disk.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileTreeRefreshedEvent)
std::string root_path;
int node_count{0};
MARKAMP_DECLARE_EVENT_END;

/// A workspace setting has changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceSettingsChangedEvent)
std::string key;
int scope{0};
MARKAMP_DECLARE_EVENT_END;

/// A file was created from a template.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileCreatedFromTemplateEvent)
std::string template_id;
std::string path;
MARKAMP_DECLARE_EVENT_END;

/// Files were moved to trash.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FilesTrashedEvent)
std::vector<std::string> paths;
MARKAMP_DECLARE_EVENT_END;

/// Files were restored from trash.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FilesRestoredEvent)
std::vector<std::string> paths;
MARKAMP_DECLARE_EVENT_END;

/// A workspace root folder was added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceRootAddedEvent)
std::string root_path;
std::string display_name;
MARKAMP_DECLARE_EVENT_END;

// ── Phase 21: Attribute View Database ──

/// A formula was evaluated in an AV column.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVFormulaEvaluatedEvent)
std::string av_id;
std::string key_id;
int rows_evaluated{0};
MARKAMP_DECLARE_EVENT_END;

/// Data was imported into an AV from CSV/JSON.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVDataImportedEvent)
std::string av_id;
int rows_imported{0};
int columns_created{0};
MARKAMP_DECLARE_EVENT_END;

/// An undo or redo operation was performed on an AV.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVUndoRedoEvent)
std::string av_id;
bool is_undo{true};
std::string description;
MARKAMP_DECLARE_EVENT_END;

/// An aggregate row was computed for an AV.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVAggregateComputedEvent)
std::string av_id;
int columns_aggregated{0};
MARKAMP_DECLARE_EVENT_END;

/// A linked database view was refreshed from its source.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVLinkedDatabaseRefreshedEvent)
std::string link_id;
std::string source_av_id;
int row_count{0};
MARKAMP_DECLARE_EVENT_END;

// V9 Phase 22: Flashcard & Spaced Repetition events

/// A flashcard deck was loaded from persistent storage.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardDeckLoadedEvent)
std::string deck_id;
std::string deck_name;
int card_count{0};
int due_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Flashcards were extracted from a Markdown document.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardExtractedEvent)
std::string source_file;
int cards_added{0};
int cards_removed{0};
MARKAMP_DECLARE_EVENT_END;

/// A card was detected as a leech (too many lapses).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardLeechDetectedEvent)
std::string card_id;
std::string deck_id;
int lapse_count{0};
MARKAMP_DECLARE_EVENT_END;

/// A flashcard review session summary with retention metrics.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardReviewSummaryEvent)
std::string deck_id;
int cards_reviewed{0};
int again_count{0};
int good_count{0};
double retention_rate{0.0};
MARKAMP_DECLARE_EVENT_END;

/// The study streak counter was updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(StudyStreakUpdatedEvent)
int current_streak{0};
int longest_streak{0};
bool milestone_achieved{false};
MARKAMP_DECLARE_EVENT_END;

/// Flashcards were imported from an external file.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FlashcardImportedEvent)
std::string deck_id;
std::string format;
int cards_imported{0};
int cards_failed{0};
MARKAMP_DECLARE_EVENT_END;

/// Published after first frame rendered — triggers deferred theme/extension loading.
MARKAMP_DECLARE_EVENT(StartupDeferralEvent);

/// Published when background user theme scanning completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(UserThemesLoadedEvent)
int theme_count{0}; ///< Number of user themes discovered
MARKAMP_DECLARE_EVENT_END;

/// Published when background extension scanning finishes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionsScanCompleteEvent)
int extension_count{0}; ///< Number of extensions discovered
int activated_count{0}; ///< Number of extensions successfully activated
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 10: Code Intelligence UX events
// ============================================================================

/// User clicked a severity icon in the editor gutter.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticIndicatorClickedEvent)
int line{0};              ///< Source line number (0-based)
std::uint8_t severity{0}; ///< DiagnosticSeverity as uint8_t
std::string message;      ///< Diagnostic message text
MARKAMP_DECLARE_EVENT_END;

/// Quick-fix lightbulb or Ctrl+. invoked at cursor.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(QuickFixRequestedEvent)
int line{0};
int character{0};
std::string file_uri;
MARKAMP_DECLARE_EVENT_END;

/// Inline "peek problem" overlay requested at a diagnostic range.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PeekProblemRequestedEvent)
int start_line{0};
int start_char{0};
int end_line{0};
int end_char{0};
std::string file_uri;
MARKAMP_DECLARE_EVENT_END;

/// Preview render error mapped back to a source line.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PreviewRenderErrorEvent)
int source_line{0};        ///< Line in the markdown source
std::string error_message; ///< Render error description
std::string block_type;    ///< e.g. "mermaid", "math", "code"
MARKAMP_DECLARE_EVENT_END;

/// User clicked "jump to source" from a preview error context.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(JumpToSourceFromPreviewEvent)
int target_line{0};
int target_character{0};
std::string file_uri;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 11: Unified Workbench Navigation + Tool Window events
// ============================================================================

/// Workbench surface mode for unified navigation.
enum class WorkbenchMode : std::uint8_t
{
    kEditor,
    kCanvas,
    kNotebook,
    kGraph,
    kSettings
};

/// Dock position for tool window panels.
enum class DockPosition : std::uint8_t
{
    kLeft,
    kRight,
    kBottom
};

/// Fired when the workbench mode has successfully changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkbenchModeChangedEvent)
WorkbenchMode previous_mode{WorkbenchMode::kEditor};
WorkbenchMode new_mode{WorkbenchMode::kEditor};
MARKAMP_DECLARE_EVENT_END;

/// Request to switch to a specific workbench mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkbenchModeSwitchRequestEvent)
WorkbenchMode target_mode{WorkbenchMode::kEditor};
MARKAMP_DECLARE_EVENT_END;

/// Request to toggle a tool window panel's visibility.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolWindowToggleRequestEvent)
std::string panel_id;
bool visible{true};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a tool window's dock position changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ToolWindowDockPositionChangedEvent)
std::string panel_id;
DockPosition dock_position{DockPosition::kBottom};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 18: Panel System events
// ============================================================================

/// Task 6: Panel command dispatch event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelCommandEvent)
std::string panel_id;
std::string command_id;
MARKAMP_DECLARE_EVENT_END;

/// Task 7: Panel focus change event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelFocusChangedEvent)
std::string panel_id;
bool focused{false};
MARKAMP_DECLARE_EVENT_END;

/// Task 8: Badge data for a panel.
struct PanelBadge
{
    int count{0};
    std::string text;
    std::uint32_t color{0xFF4444FFu}; // RGBA red default
};

/// Task 8: Panel badge changed event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelBadgeChangedEvent)
std::string panel_id;
PanelBadge badge;
MARKAMP_DECLARE_EVENT_END;

/// Task 9: Menu item for panel context menus.
struct PanelMenuItem
{
    std::string label;
    std::string command_id;
    bool enabled{true};
    bool separator{false};
};

/// Task 9: Panel context menu request event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelContextMenuRequestEvent)
std::string panel_id;
std::vector<PanelMenuItem> items;
MARKAMP_DECLARE_EVENT_END;

/// Task 13: Panel search text changed event.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelSearchChangedEvent)
std::string panel_id;
std::string search_text;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 11 (Phase 29): Surface Link events
// ============================================================================

/// Request to open a surface link (cross-surface navigation).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OpenSurfaceLinkRequestEvent)
SurfaceLink link;
MARKAMP_DECLARE_EVENT_END;

/// Request to reveal a specific anchor in any surface.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RevealInSurfaceRequestEvent)
LinkAnchor target;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a surface link has been successfully resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SurfaceLinkResolvedEvent)
SurfaceLink link;
bool success{true};
MARKAMP_DECLARE_EVENT_END;

/// Fired when surface traversal fails.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SurfaceTraversalFailedEvent)
LinkAnchor target;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 12 (Phase 36): Surface Transition events
// ============================================================================

/// Fired when a surface transition begins.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SurfaceTransitionStartEvent)
SurfaceKind from_surface{SurfaceKind::kEditor};
SurfaceKind to_surface{SurfaceKind::kEditor};
std::string reason;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a surface transition completes successfully.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SurfaceTransitionCompleteEvent)
SurfaceKind from_surface{SurfaceKind::kEditor};
SurfaceKind to_surface{SurfaceKind::kEditor};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a surface transition is cancelled (e.g. rapid repeated jumps).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SurfaceTransitionCancelEvent)
std::string reason;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 12 (Phase 37): Paired Traverse Mode events
// ============================================================================

/// Request to pair two surfaces together.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PairSurfacesRequestEvent)
SurfaceKind primary{SurfaceKind::kEditor};
SurfaceKind secondary{SurfaceKind::kPreview};
MARKAMP_DECLARE_EVENT_END;

/// Fired when surface pairing changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PairSurfacesChangedEvent)
SurfaceKind primary{SurfaceKind::kEditor};
SurfaceKind secondary{SurfaceKind::kPreview};
bool paired{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 12 (Phases 40, 43): VSCode Theme events
// ============================================================================

/// Request to import a VSCode theme file.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportVsCodeThemeRequestEvent)
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

/// Theme compatibility report after import analysis.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeCompatibilityReportEvent)
std::string theme_name;
std::string compatibility; ///< "full", "mapped", "partial"
int supported_tokens{0};
int total_tokens{0};
MARKAMP_DECLARE_EVENT_END;

/// Request to auto-repair a theme (fix contrast, fill missing tokens).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeRepairRequestEvent)
std::string theme_name;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V8 Phase 13 (Phase 46): FX Engine events
// ============================================================================

/// Fired when the FX preset is changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxPresetChangedEvent)
std::string preset_name;
std::string previous_preset;
MARKAMP_DECLARE_EVENT_END;

/// Fired when the FX master toggle changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxMasterToggleEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the FX quality tier changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxQualityTierChangedEvent)
std::string tier_name; ///< "Cinematic", "Balanced", etc.
std::string previous_tier;
MARKAMP_DECLARE_EVENT_END;

/// Fired when FX safety mode changes (reduced motion, low power, etc.).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxSafetyModeChangedEvent)
std::string mode_name; ///< "reduced_motion", "low_power", etc.
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the FX engine auto-degrades quality due to frame budget violations.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxAutoDegrade)
std::string from_tier;
std::string to_tier;
uint32_t violation_count{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 28: FX Visual Effects System events
// ============================================================================

/// Fired when an FX transition animation starts.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxTransitionStartedEvent)
std::string preset_name;
int32_t duration_ms{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when an FX transition animation completes or is cancelled.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxTransitionCompletedEvent)
std::string preset_name;
bool was_cancelled{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when CSS is generated from FX settings.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxCssGeneratedEvent)
int32_t css_length{0};
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when an FX profile is saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxProfileSavedEvent)
std::string profile_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a diagnostics snapshot is taken.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxDiagnosticsSnapshotEvent)
float avg_frame_ms{0.0F};
int32_t pass_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when an FX accessibility announcement is made.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FxAccessibilityAnnouncedEvent)
std::string message;
MARKAMP_DECLARE_EVENT_END;

// ========================= Phase 29 Security & Input Validation Events =========================

/// Fired when a URL is blocked by the sanitizer.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityUrlBlockedEvent)
std::string url;
std::string reason;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a CSP violation is detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityCspViolationEvent)
std::string directive;
std::string blocked_uri;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a user action is rate-limited.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityRateLimitedEvent)
std::string action_name;
int32_t remaining_tokens{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when clipboard content is sanitized (modified during paste).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityClipboardSanitizedEvent)
std::string content_type;
int32_t bytes_removed{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired for generic security audit log entries.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SecurityAuditEvent)
std::string severity;
std::string category;
std::string message;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a config value changes. Carries the key and old/new values.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConfigChangedEvent)
std::string key;
std::string old_value;
std::string new_value;
std::string scope; // "application", "workspace", "project"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a setting requiring restart is changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RestartRequiredEvent)
std::string setting_id;
std::string reason;
MARKAMP_DECLARE_EVENT_END;

/// Fired when the config file is modified externally (by another process).
MARKAMP_DECLARE_EVENT(ConfigFileModifiedEvent);

/// Fired when a config value is read (for telemetry/observability).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConfigReadEvent)
std::string key;
bool was_cached{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a config value is written.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ConfigWriteEvent)
std::string key;
std::string value;
MARKAMP_DECLARE_EVENT_END;

// ================================ Phase 07 Editor Core Improvement Events
// ================================

/// Fired when an editor performance budget threshold is violated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorBudgetViolationEvent)
std::string metric_name; ///< e.g. "keystroke_latency_ms"
double measured_value{0.0};
double budget_value{0.0};
bool is_critical{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a peek-problem inline dialog is shown for a diagnostic.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorDiagnosticPeekEvent)
std::string panel_id;
int line{0};
std::string message;
std::string severity; ///< "error", "warning", "info", "hint"
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user clicks a quick-fix lightbulb affordance.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorQuickFixRequestEvent)
int line{0};
std::string diagnostic_id;
MARKAMP_DECLARE_EVENT_END;

// ================================ Phase 08 Markdown Rendering Completeness Events
// ================================

/// Fired when the user clicks on a rendered element to navigate to its source line.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PreviewSourceLineClickEvent)
int source_line{0};      ///< 0-indexed source markdown line
std::string element_tag; ///< HTML tag that was clicked (e.g. "p", "h2")
MARKAMP_DECLARE_EVENT_END;

/// Fired when a heading is selected in the heading navigation overlay.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PreviewHeadingNavEvent)
std::string heading_slug; ///< Slug/anchor ID to scroll to
int heading_level{0};
std::string heading_text;
MARKAMP_DECLARE_EVENT_END;

/// Fired when an incremental render completes for a dirty region.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PreviewIncrementalRenderEvent)
int start_line{0};
int end_line{0};
bool was_full_rerender{false};
MARKAMP_DECLARE_EVENT_END;

// ================================ Phase 19 Code Intelligence Events
// ================================

/// Fired when the editor requests completion at the cursor position.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CompletionRequestEvent)
std::string panel_id;
int line{0};         ///< 0-based line
int character{0};    ///< 0-based column
std::string trigger; ///< Trigger character (e.g. "[", "/", ":")
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user selects a completion item.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CompletionSelectedEvent)
std::string panel_id;
std::string label;
std::string insert_text;
int kind{0}; ///< CompletionKind as int
MARKAMP_DECLARE_EVENT_END;

/// Fired when the editor requests hover information.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HoverRequestEvent)
std::string panel_id;
int line{0};
int character{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the editor requests go-to-definition.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GoToDefinitionRequestEvent)
std::string panel_id;
int line{0};
int character{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the editor requests code actions for a range.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CodeActionRequestEvent)
std::string panel_id;
int start_line{0};
int start_char{0};
int end_line{0};
int end_char{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when inlay hints are toggled on/off.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(InlayHintToggleEvent)
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the editor requests document highlights for the word under cursor.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentHighlightEvent)
std::string panel_id;
int line{0};
int character{0};
std::string word; ///< The word to highlight occurrences of
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user searches for workspace symbols (Ctrl+T / Cmd+T).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceSymbolSearchEvent)
std::string query;
int max_results{50};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 45: Live Preview & WYSIWYG Events
// ============================================================================

/// Fired when the WYSIWYG editing mode changes (Source / LivePreview / WYSIWYG).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygModeChangedEvent)
int old_mode{0}; ///< Previous WysiwygMode (cast from enum)
int new_mode{0}; ///< New WysiwygMode (cast from enum)
MARKAMP_DECLARE_EVENT_END;

/// Fired when cursor enters a new block in WYSIWYG mode, activating it for editing.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygBlockActivatedEvent)
int block_index{0};
int block_type{0}; ///< WysiwygBlockType (cast from enum)
int line{0};       ///< Line number of the activated block
MARKAMP_DECLARE_EVENT_END;

/// Fired when a checkbox is toggled in a task list in WYSIWYG mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygCheckboxToggledEvent)
int line{0};
bool new_state{false}; ///< true = checked, false = unchecked
MARKAMP_DECLARE_EVENT_END;

/// Fired when a link is clicked in WYSIWYG mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygLinkClickedEvent)
std::string url;
int action{0}; ///< LinkClickAction (cast from enum)
MARKAMP_DECLARE_EVENT_END;

/// Fired when a block edit is committed in WYSIWYG mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygBlockEditCommittedEvent)
int block_index{0};
std::string old_content;
std::string new_content;
MARKAMP_DECLARE_EVENT_END;

/// Fired when an image is inserted in WYSIWYG mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WysiwygImageInsertedEvent)
std::string path;
std::string alt_text;
int line{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 17: Cross-Surface Navigation & Linking events
// ============================================================================

/// Fired when a block reference (^block-id) is resolved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BlockReferenceResolvedEvent)
std::string block_id;
std::string document_id;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a heading jump navigates to/from a heading.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HeadingJumpEvent)
std::string source_document;
std::string target_document;
std::string heading_text;
MARKAMP_DECLARE_EVENT_END;

/// Fired when broken links are detected in a document.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BrokenLinkDetectedEvent)
std::string document_id;
int broken_count{0};
std::string scan_type;
MARKAMP_DECLARE_EVENT_END;

/// Fired when links are refactored (rename, format conversion, etc.).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkRefactoredEvent)
int edit_count{0};
std::string refactor_type;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a deep link URI is generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeepLinkGeneratedEvent)
std::string uri;
int surface_kind{0}; ///< SurfaceKind cast to int
MARKAMP_DECLARE_EVENT_END;

/// Fired when a peek definition popup is shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PeekDefinitionEvent)
std::string target_uri;
int content_type{0}; ///< PeekContentType cast to int
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 23: Task Management & Calendar events
// ============================================================================

/// Tasks were aggregated/re-indexed across all documents.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskAggregatedEvent)
int total_tasks{0};
int documents_scanned{0};
MARKAMP_DECLARE_EVENT_END;

/// A recurring task pattern was detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskRecurrenceCreatedEvent)
std::string task_id;
std::string pattern; ///< e.g. "daily", "every 2 weeks"
MARKAMP_DECLARE_EVENT_END;

/// A task reminder has triggered (is now due).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskReminderDueEvent)
std::string reminder_id;
std::string task_id;
std::string task_text;
MARKAMP_DECLARE_EVENT_END;

/// The task board layout or column assignments changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskBoardUpdatedEvent)
std::string task_id;
std::string from_column;
std::string to_column;
MARKAMP_DECLARE_EVENT_END;

/// Gantt timeline data was rebuilt.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskGanttRefreshedEvent)
int bar_count{0};
int milestone_count{0};
MARKAMP_DECLARE_EVENT_END;

/// A task has become overdue.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskOverdueEvent)
std::string task_id;
std::string task_text;
int days_overdue{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 24: Export & Publishing events
// ============================================================================

/// An export template was applied to content.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportTemplateAppliedEvent)
std::string template_id;
std::string format;
MARKAMP_DECLARE_EVENT_END;

/// A batch export job started.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BatchExportStartedEvent)
int document_count{0};
std::string format;
MARKAMP_DECLARE_EVENT_END;

/// A batch export job completed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BatchExportCompletedEvent)
int succeeded{0};
int failed{0};
int64_t elapsed_ms{0};
MARKAMP_DECLARE_EVENT_END;

/// A print preview was generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PrintPreviewGeneratedEvent)
int page_count{0};
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

/// A publishing profile was executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PublishingProfileExecutedEvent)
std::string profile_id;
std::string profile_name;
bool success{false};
MARKAMP_DECLARE_EVENT_END;

/// Export validation completed on a document.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExportValidationCompletedEvent)
int errors{0};
int warnings{0};
std::string document_id;
MARKAMP_DECLARE_EVENT_END;

// ========================= Phase 30 Performance Optimization Events =========================

/// Fired when application startup completes with timing data.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PerfStartupCompletedEvent)
double startup_ms{0.0};
int32_t phase_count{0};
std::string slowest_phase;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a frame exceeds the frame budget.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PerfFrameOverbudgetEvent)
double frame_time_us{0.0};
double budget_us{0.0};
uint64_t frame_number{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a subsystem exceeds its memory budget.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PerfMemoryBudgetExceededEvent)
std::string subsystem;
size_t usage_bytes{0};
size_t budget_bytes{0};
double usage_percent{0.0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user requests the performance metrics view.
MARKAMP_DECLARE_EVENT(PerfShowMetricsEvent);

/// Fired when a frame profile capture completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PerfProfileCapturedEvent)
uint64_t frame_count{0};
double avg_frame_us{0.0};
double p95_frame_us{0.0};
double min_frame_us{0.0};
double max_frame_us{0.0};
MARKAMP_DECLARE_EVENT_END;

// ========================= Phase 35 Documentation & Help System Events =========================

/// Fired when the user requests help for a specific topic or article.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HelpRequestEvent)
std::string article_id; ///< Target article ID (e.g. "syntax.headings")
std::string context_id; ///< Current context (e.g. "editor", "canvas")
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user searches help content.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HelpSearchEvent)
std::string query;   ///< Search query string
int result_count{0}; ///< Number of results found
MARKAMP_DECLARE_EVENT_END;

/// Fired when a feature discovery hint should be displayed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FeatureHintEvent)
std::string hint_id;       ///< Hint identifier
std::string message;       ///< User-facing hint message
std::string learn_more_id; ///< Help article for "Learn More"
MARKAMP_DECLARE_EVENT_END;

/// Fired during onboarding flow step transitions.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OnboardingStepEvent)
int step_index{0};     ///< Current step index (0-based)
std::string step_name; ///< Step name (e.g. "Welcome", "ChooseTheme")
bool skipped{false};   ///< Whether the step was skipped
MARKAMP_DECLARE_EVENT_END;

// ── V9 Phase 36: Command System Completion ──

/// Fired when a new command is registered in the CommandRegistry.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandRegisteredEvent)
std::string command_id; ///< Registered command ID
std::string category;   ///< Command category
MARKAMP_DECLARE_EVENT_END;

/// Fired when a command is executed via any source.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandExecutedEvent)
std::string command_id; ///< Executed command ID
std::string source;     ///< Source: "palette", "shortcut", "menu", "extension", "internal"
int64_t duration_ms{0}; ///< Execution duration in milliseconds
bool success{true};     ///< Whether execution succeeded
MARKAMP_DECLARE_EVENT_END;

/// Fired when the command palette is opened or changes mode.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CommandPaletteEvent)
std::string mode;  ///< Mode: "commands", "quick_open", "go_to_symbol", "go_to_line"
bool opened{true}; ///< True if opened, false if closed
MARKAMP_DECLARE_EVENT_END;

/// Fired when a chord shortcut enters waiting state.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChordWaitingEvent)
std::string description; ///< E.g. "Cmd+K was pressed, awaiting second key..."
bool waiting{true};      ///< True if entering waiting state, false if resolved/canceled
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V9 Phase 37 — Bookmark & Asset Management Events
// ============================================================================

/// Fired when bookmarks are exported to a file format.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkExportedEvent)
std::string format;    ///< Export format: "markdown", "json", "csv", "opml"
int bookmark_count{0}; ///< Number of bookmarks exported
MARKAMP_DECLARE_EVENT_END;

/// Fired when sequential bookmark navigation moves to a new entry.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BookmarkNavigatedEvent)
std::string block_id;  ///< Target block ID
std::string label;     ///< Bookmark label
std::string direction; ///< "next", "previous", or "go_to"
MARKAMP_DECLARE_EVENT_END;

/// Fired when an image asset has been optimized.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetOptimizedEvent)
std::string asset_id;      ///< Optimized asset ID
int64_t original_size{0};  ///< Original size in bytes
int64_t optimized_size{0}; ///< Optimized size in bytes
double savings_percent{0.0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a broken asset link is detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetLinkBrokenEvent)
std::string document_id;   ///< Document containing the broken link
std::string expected_path; ///< The broken path
std::string suggestion;    ///< Suggested repair (may be empty)
MARKAMP_DECLARE_EVENT_END;

/// Fired when a broken asset link is auto-repaired.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetLinkRepairedEvent)
std::string document_id; ///< Document containing the repaired link
std::string old_path;    ///< Original broken path
std::string new_path;    ///< Repaired path
MARKAMP_DECLARE_EVENT_END;

/// Fired when an asset storage report is generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AssetStorageReportEvent)
int64_t used_bytes{0};        ///< Total storage used
int32_t total_assets{0};      ///< Total asset count
int32_t orphans{0};           ///< Orphaned asset count
double savings_estimate{0.0}; ///< Potential savings percentage
MARKAMP_DECLARE_EVENT_END;

// ── Phase 38: Vault Style & Workspace Customization ─────────────────────────

/// Fired when a CSS snippet is applied to vault styling.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CssSnippetAppliedEvent)
std::string snippet_name; ///< Name of the applied snippet
std::string category;     ///< Snippet category
MARKAMP_DECLARE_EVENT_END;

/// Fired when a workspace profile is saved.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceProfileSavedEvent)
std::string profile_name; ///< Name of the saved profile
bool is_new{false};       ///< True if this is a new profile
MARKAMP_DECLARE_EVENT_END;

/// Fired when a workspace profile is loaded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceProfileLoadedEvent)
std::string profile_name; ///< Name of the loaded profile
MARKAMP_DECLARE_EVENT_END;

/// Fired when a custom font is registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CustomFontRegisteredEvent)
std::string font_name;   ///< Display name of the font
std::string font_family; ///< CSS font-family
std::string file_path;   ///< Path to font file
MARKAMP_DECLARE_EVENT_END;

/// Fired when a theme override is changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ThemeOverrideChangedEvent)
std::string token_name; ///< Token that was overridden
std::string new_value;  ///< New override value
std::string scope;      ///< Scope: "vault", "workspace", "global"
MARKAMP_DECLARE_EVENT_END;

/// Fired when workspace customization is reset to defaults.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceCustomizationResetEvent)
std::string scope;       ///< Scope that was reset
int settings_cleared{0}; ///< Number of settings cleared
MARKAMP_DECLARE_EVENT_END;

// ── Phase 39: Notification & Activity System ────────────────────────────────

/// Fired when a notification is added to the center.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotificationAddedEvent)
std::string notification_id; ///< Unique notification ID
std::string title;           ///< Notification title
std::string source;          ///< Source module
MARKAMP_DECLARE_EVENT_END;

/// Fired when notification status changes (read, dismissed, archived).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotificationStatusChangedEvent)
std::string notification_id; ///< Notification ID
std::string new_status;      ///< "read", "dismissed", "archived"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a filter rule is added or modified.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NotificationFilterChangedEvent)
std::string rule_id; ///< Filter rule ID
std::string action;  ///< "added", "removed", "enabled", "disabled"
MARKAMP_DECLARE_EVENT_END;

/// Fired when an activity is recorded in the feed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityRecordedEvent)
std::string activity_id; ///< Activity entry ID
std::string description; ///< Activity description
std::string category;    ///< Activity category name
MARKAMP_DECLARE_EVENT_END;

/// Fired when the activity timeline is rebuilt.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityTimelineBuiltEvent)
int active_days{0};  ///< Number of active days
int total_events{0}; ///< Total events in timeline
MARKAMP_DECLARE_EVENT_END;

/// Fired when an activity entry is pinned or unpinned.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityPinChangedEvent)
std::string activity_id; ///< Activity entry ID
bool pinned{false};      ///< New pin state
MARKAMP_DECLARE_EVENT_END;

// ── Phase 40: Workspace Automation & Hooks ──────────────────────────────────

/// Fired when a workspace lifecycle hook is triggered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WorkspaceHookFiredEvent)
std::string hook_id;        ///< Hook that was fired
std::string hook_type;      ///< Hook type name
std::string workspace_path; ///< Workspace context
MARKAMP_DECLARE_EVENT_END;

/// Fired when an automation rule executes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutomationRuleExecutedEvent)
std::string rule_id;   ///< Rule that executed
std::string rule_name; ///< Rule display name
std::string trigger;   ///< Trigger type
MARKAMP_DECLARE_EVENT_END;

/// Fired when a scheduled task completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ScheduledTaskCompletedEvent)
std::string task_id;   ///< Task that completed
std::string task_name; ///< Task display name
bool success{false};   ///< Whether task succeeded
MARKAMP_DECLARE_EVENT_END;

/// Fired when a file change is detected by the file watcher service.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileWatcherNotificationEvent)
std::string watch_id;    ///< Watch that detected change
std::string file_path;   ///< Changed file
std::string change_type; ///< Type of change
MARKAMP_DECLARE_EVENT_END;

/// Fired when a new hook is registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HookRegisteredEvent)
std::string hook_id;     ///< Newly registered hook
std::string hook_type;   ///< Hook type
std::string description; ///< Hook description
MARKAMP_DECLARE_EVENT_END;

/// Fired when an automation rule is added or modified.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(AutomationRuleChangedEvent)
std::string rule_id; ///< Affected rule
std::string action;  ///< "added", "removed", "enabled", "disabled"
MARKAMP_DECLARE_EVENT_END;

// ── Phase 41: Widget & Sidebar Extensions ───────────────────────────────────

/// Fired when a sidebar widget is registered or unregistered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarWidgetChangedEvent)
std::string widget_id; ///< Affected widget
std::string action;    ///< "registered", "unregistered", "moved"
std::string slot;      ///< Current slot name
MARKAMP_DECLARE_EVENT_END;

/// Fired when a widget's display mode changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetDisplayModeChangedEvent)
std::string widget_id; ///< Affected widget
std::string new_mode;  ///< New display mode
MARKAMP_DECLARE_EVENT_END;

/// Fired when widget data is refreshed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetDataRefreshedEvent)
std::string provider_id; ///< Data provider
std::string widget_id;   ///< Target widget
int item_count{0};       ///< Number of items returned
MARKAMP_DECLARE_EVENT_END;

/// Fired when a custom panel is registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CustomPanelRegisteredEvent)
std::string panel_id;     ///< Panel identifier
std::string extension_id; ///< Contributing extension
std::string location;     ///< Panel location
MARKAMP_DECLARE_EVENT_END;

/// Fired when a custom panel is unregistered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CustomPanelUnregisteredEvent)
std::string panel_id; ///< Panel identifier
MARKAMP_DECLARE_EVENT_END;

/// Fired when a widget container layout changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetLayoutChangedEvent)
std::string container_id; ///< Container that changed
int widget_count{0};      ///< Number of widgets
MARKAMP_DECLARE_EVENT_END;

/// Fired when a sidebar panel visibility changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarPanelVisibilityEvent)
std::string panel_id; ///< Panel that changed
bool visible{false};  ///< New visibility state
MARKAMP_DECLARE_EVENT_END;

/// Fired when a panel is dragged between sidebars.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarPanelMovedEvent)
std::string panel_id;       ///< Panel identifier
std::string target_sidebar; ///< "primary" or "secondary"
MARKAMP_DECLARE_EVENT_END;

// ── Phase 42: PDF Viewer & Document Import ──────────────────────────────────

/// Fired when a PDF document is loaded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PdfDocumentLoadedEvent)
std::string document_id; ///< Loaded document
std::string file_path;   ///< Source file path
int total_pages{0};      ///< Number of pages
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user navigates to a different PDF page.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PdfPageNavigatedEvent)
std::string document_id; ///< Document
int page_number{0};      ///< New page number
MARKAMP_DECLARE_EVENT_END;

/// Fired when a PDF annotation is added or removed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PdfAnnotationChangedEvent)
std::string annotation_id; ///< Annotation
std::string document_id;   ///< Document
std::string action;        ///< "added", "removed"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a document import completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentImportCompletedEvent)
std::string import_id;   ///< Import operation
std::string source_path; ///< Source file
std::string format;      ///< Import format
bool success{false};     ///< Whether import succeeded
MARKAMP_DECLARE_EVENT_END;

/// Fired when an import preset is created or modified.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ImportPresetChangedEvent)
std::string preset_id; ///< Preset
std::string action;    ///< "created", "removed", "updated"
MARKAMP_DECLARE_EVENT_END;

/// Fired when PDF text is extracted.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PdfTextExtractedEvent)
std::string document_id; ///< Source document
int page_count{0};       ///< Pages extracted
int text_length{0};      ///< Total chars extracted
MARKAMP_DECLARE_EVENT_END;

// ═══════════════════════════════════════════════════════════════════
// Phase 43 — Presentation & Slide System Events
// ═══════════════════════════════════════════════════════════════════

/// Fired when a presentation is created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationCreatedEvent)
std::string presentation_id;
std::string title;
int slide_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a slideshow starts or stops.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideshowStateChangedEvent)
std::string presentation_id;
std::string new_state; ///< "playing", "paused", "idle"
int current_slide{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a presentation slide is added, removed, or reordered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PresentationSlideChangedEvent)
std::string slide_id;
std::string action; ///< "added", "removed", "moved", "modified"
int slide_number{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a slide theme is applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideThemeAppliedEvent)
std::string presentation_id;
std::string theme_id;
std::string theme_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a presentation export completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideExportCompletedEvent)
std::string export_id;
std::string presentation_id;
std::string format; ///< "pdf", "html", "png", etc.
bool success{false};
int slides_exported{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when slide content blocks change.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SlideBlockChangedEvent)
std::string slide_id;
std::string block_id;
std::string action; ///< "added", "removed", "modified"
MARKAMP_DECLARE_EVENT_END;

// ── Phase 44: Encryption & Privacy ──────────────────────────────────────────

/// Fired when vault encryption state changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultEncryptionChangedEvent)
std::string vault_id;
std::string new_state; ///< "encrypted", "decrypted"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a vault is locked or unlocked.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VaultLockedEvent)
std::string vault_id;
bool is_locked{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when an encryption key is rotated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(KeyRotatedEvent)
std::string old_key_id;
std::string new_key_id;
std::string algorithm;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a document privacy classification changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DocumentClassifiedEvent)
std::string document_id;
std::string level; ///< "public", "internal", "confidential", "secret"
MARKAMP_DECLARE_EVENT_END;

/// Fired when PII redaction is applied to content.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DataRedactedEvent)
std::string document_id;
int redactions_applied{0};
int rules_matched{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the privacy policy is updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PrivacyPolicyChangedEvent)
std::string setting; ///< which policy field changed
std::string new_value;
MARKAMP_DECLARE_EVENT_END;

// ── Phase 46: Multi-Window & Workspace Management ───────────────────────────

/// Fired when a new window is created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WindowCreatedEvent)
std::string window_id;
std::string title;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a window is closed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WindowClosedEvent)
std::string window_id;
MARKAMP_DECLARE_EVENT_END;

/// Fired when an editor group is created, closed, or focused.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EditorGroupChangedEvent)
std::string group_id;
std::string action; ///< "created", "closed", "focused", "split"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a file tab is opened in an editor group.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TabOpenedEvent)
std::string group_id;
std::string tab_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a layout preset is applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutRestoredEvent)
std::string preset_id;
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a workspace session is restored.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SessionRestoredEvent)
std::string snapshot_id;
std::string workspace_name;
int files_restored{0};
MARKAMP_DECLARE_EVENT_END;

// ── Phase 47: Localization & Internationalization ────────────────────────────

/// Fired when the active locale is changed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LocaleChangedEvent)
std::string old_locale;
std::string new_locale;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a translation entry is added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TranslationAddedEvent)
std::string key;
std::string locale_id;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a translation key is not found (fallback used).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TranslationMissingEvent)
std::string key;
std::string locale_id;
MARKAMP_DECLARE_EVENT_END;

/// Fired when text direction changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TextDirectionChangedEvent)
std::string direction; ///< "ltr", "rtl", "auto"
MARKAMP_DECLARE_EVENT_END;

/// Fired when a translation catalog is loaded.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CatalogLoadedEvent)
int entries_loaded{0};
int locales_loaded{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the formatter locale is updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FormatterLocaleChangedEvent)
std::string old_locale;
std::string new_locale;
MARKAMP_DECLARE_EVENT_END;

// ── Phase 48: Data Processing & Computation ─────────────────────────────────

/// Fired when a data table is created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TableCreatedEvent)
std::string table_id;
std::string table_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a row is added to a data table.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RowAddedEvent)
std::string table_id;
int row_index{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a formula is evaluated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FormulaEvaluatedEvent)
std::string expression;
double result{0.0};
bool is_error{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a transform pipeline is executed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TransformExecutedEvent)
int steps_applied{0};
int rows_input{0};
int rows_output{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a chart dataset is generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChartGeneratedEvent)
std::string dataset_id;
std::string chart_type;
int series_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when data is imported.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DataImportedEvent)
std::string source;
int rows_imported{0};
int columns_imported{0};
MARKAMP_DECLARE_EVENT_END;

// ── Phase 49: End-to-End Integration Testing ────────────────────────────────

/// Fired when a test suite completes execution.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TestSuiteCompletedEvent)
std::string suite_id;
int passed{0};
int failed{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a test fails.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TestFailedEvent)
std::string test_id;
std::string test_name;
std::string error_msg;
MARKAMP_DECLARE_EVENT_END;

/// Fired when all health checks complete.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(HealthCheckCompletedEvent)
int checks_passed{0};
int checks_failed{0};
bool is_healthy{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a regression is detected.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RegressionDetectedEvent)
std::string test_name;
double expected_val{0.0};
double actual_val{0.0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a dependency check completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DependencyCheckEvent)
int modules_checked{0};
int deps_satisfied{0};
int deps_missing{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when an integration report is generated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(IntegrationReportEvent)
int total_tests{0};
int total_passed{0};
int total_failed{0};
MARKAMP_DECLARE_EVENT_END;

// ── Phase 50: Release Preparation & Polish ──────────────────────────────────

/// Fired when a new release is created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReleaseCreatedEvent)
std::string release_id;
std::string version;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a version is bumped.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(VersionBumpedEvent)
std::string old_version;
std::string new_version;
MARKAMP_DECLARE_EVENT_END;

/// Fired when changelog is updated.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChangelogUpdatedEvent)
std::string version;
int entries_added{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a deprecation is added.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DeprecationAddedEvent)
std::string feature_name;
std::string replacement;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a release is published.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ReleasePublishedEvent)
std::string version;
int entry_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when polish checks complete.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PolishCompleteEvent)
int checks_passed{0};
int issues_found{0};
bool release_ready{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// V11 Node Editor events
// ============================================================================

// -- Graph lifecycle --
MARKAMP_DECLARE_EVENT(NodeEditorOpenedEvent);
MARKAMP_DECLARE_EVENT(NodeEditorClosedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphLoadedEvent)
std::string graph_name;
uint64_t graph_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GraphSavedEvent)
std::string graph_name;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(GraphClearedEvent);

// -- Node CRUD --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeAddedEvent)
uint64_t node_id{0};
std::string node_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeRemovedEvent)
uint64_t node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeMovedEvent)
uint64_t node_id{0};
float new_x{0};
float new_y{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeSelectedEvent)
uint64_t node_id{0};
bool multi_select{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeDeselectedEvent)
uint64_t node_id{0};
MARKAMP_DECLARE_EVENT_END;

// -- Link CRUD --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkCreatedEvent)
uint64_t link_id{0};
uint64_t source_socket{0};
uint64_t target_socket{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkRemovedEvent)
uint64_t link_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(LinkValidationChangedEvent)
uint64_t link_id{0};
bool valid{true};
MARKAMP_DECLARE_EVENT_END;

// -- Node definition --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeDefinitionRegisteredEvent)
std::string definition_type;
std::string category;
MARKAMP_DECLARE_EVENT_END;

// -- Evaluation --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(EvaluationStartedEvent)
uint64_t graph_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EvaluationCompletedEvent)
uint64_t graph_id{0};
int nodes_evaluated{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EvaluationErrorEvent)
uint64_t graph_id{0};
uint64_t node_id{0};
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

// -- Mode --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorModeChangedEvent)
int mode{0}; // GraphMode as int (avoids circular include with node_editor/)
MARKAMP_DECLARE_EVENT_END;

// -- V11 Tier 2: Domain & Execution events --
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DomainRegisteredEvent)
std::string domain_id;
int graph_mode{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(DomainRemovedEvent)
std::string domain_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(EvaluationStepEvent)
uint64_t graph_id{0};
uint64_t node_id{0};
int step_index{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeDirtyEvent)
uint64_t node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeCleanEvent)
uint64_t node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SandboxViolationEvent)
uint64_t graph_id{0};
int violation_type{0};
std::string violation_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorStateChangedEvent)
bool editor_active{false};
bool has_selection{false};
int selection_count{0};
MARKAMP_DECLARE_EVENT_END;

// ── V11 Node Editor Tier 3 Events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorViewportChangedEvent)
float zoom_level{1.0F};
float pan_x{0.0F};
float pan_y{0.0F};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorSelectionChangedEvent)
int selected_count{0};
uint64_t focused_node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorNodeDraggedEvent)
uint64_t node_id{0};
float delta_x{0.0F};
float delta_y{0.0F};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorLinkCreatedEvent)
uint64_t link_id{0};
uint64_t source_socket_id{0};
uint64_t target_socket_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorLinkRemovedEvent)
uint64_t link_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(NodeEditorSearchOpenedEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorClipboardPasteEvent)
int pasted_node_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorAnnotationAddedEvent)
uint64_t frame_id{0};
int annotation_type{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorGroupEnteredEvent)
uint64_t group_node_id{0};
int navigation_depth{0};
MARKAMP_DECLARE_EVENT_END;

// ── V11 Node Editor Tier 4 Events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorInspectorPropertyChangedEvent)
uint64_t node_id{0};
int property_index{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorPreviewRequestedEvent)
uint64_t node_id{0};
bool pinned{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorBreakpointToggledEvent)
uint64_t node_id{0};
bool enabled{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorDirtyPropagatedEvent)
int dirty_count{0};
uint64_t source_node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorEvaluationJobCompletedEvent)
uint64_t job_id{0};
int status{0};
float duration_ms{0.0F};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorDiagnosticAddedEvent)
uint64_t node_id{0};
int severity{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorShortcutTriggeredEvent)
int key_code{0};
int modifiers{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorWorkspaceSnapshotEvent)
uint64_t version{0};
bool is_recovery{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(NodeEditorAutosaveTriggeredEvent);

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorBenchmarkCompletedEvent)
float duration_ms{0.0F};
bool passed{true};
MARKAMP_DECLARE_EVENT_END;

// ── V11 Node Editor Tier 5 Events ──

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorPackRegisteredEvent)
uint64_t pack_id{0};
std::string pack_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorWidgetEmbedChangedEvent)
uint64_t embed_id{0};
uint64_t node_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorDecorationUpdatedEvent)
uint64_t decoration_id{0};
uint64_t target_node{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorNodeStatusChangedEvent)
uint64_t node_id{0};
int status_level{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorTemplateLoadedEvent)
uint64_t template_id{0};
std::string template_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorFormatExportedEvent)
uint64_t adapter_id{0};
std::string format_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorTrustPolicyViolationEvent)
std::string pack_id;
int permission{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorGraphLintCompletedEvent)
int issue_count{0};
int error_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorMigrationAppliedEvent)
int from_version{0};
int to_version{0};
int steps_applied{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NodeEditorRolloutStageChangedEvent)
uint64_t stage_id{0};
std::string stage_name;
int phase{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 22: Output Panel V2 events
// ============================================================================

/// Fired when content is appended to an output channel.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelContentEvent)
std::string channel_name;
std::string text;
int log_level{2}; ///< LogLevel as int (0=Trace..5=Fatal)
MARKAMP_DECLARE_EVENT_END;

/// Fired when a new output channel is created.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelCreatedEvent)
std::string channel_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when the active output channel changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelActiveChangedEvent)
std::string old_channel;
std::string new_channel;
MARKAMP_DECLARE_EVENT_END;

/// Fired to request auto-reveal of the output panel.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputAutoRevealEvent)
std::string channel_name;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 23: Problems Panel V2 events
// ============================================================================

/// Fired when diagnostics change for a URI.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticsChangedEvent)
std::string uri;
int error_count{0};
int warning_count{0};
int info_count{0};
int hint_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired to navigate the editor to a specific problem location.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigateToProblemEvent)
std::string file_uri;
int line{0};
int character{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a quick fix is requested for a diagnostic in the problems panel.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DiagnosticQuickFixRequestEvent)
std::string file_uri;
int line{0};
int character{0};
std::string diagnostic_code;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 24: Debug Console Panel events
// ============================================================================

/// Fired when text is written to the debug console output.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleOutputEvent)
std::string text;
int entry_type{0}; ///< 0=Output, 1=Error, 2=System, 3=Debug
std::string source;
MARKAMP_DECLARE_EVENT_END;

/// Fired when the user submits input in the debug console.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleInputEvent)
std::string expression;
MARKAMP_DECLARE_EVENT_END;

/// Fired when an expression evaluation completes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DebugConsoleEvalResultEvent)
std::string expression;
std::string result;
bool is_error{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the debug console is cleared.
MARKAMP_DECLARE_EVENT(DebugConsoleClearEvent);

// ============================================================================
// Phase 25: Build & Task Runner Panel events
// ============================================================================

/// Fired when a build process starts.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildStartedEvent)
std::string build_system; ///< "cmake", "make", "custom"
std::string target;
std::string configuration; ///< "debug", "release", etc.
MARKAMP_DECLARE_EVENT_END;

/// Fired to report build progress.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildProgressEvent)
int completed{0};
int total{0};
std::string current_file;
float percentage{0.0F};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a build process finishes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildFinishedEvent)
bool success{false};
int error_count{0};
int warning_count{0};
float duration_seconds{0.0F};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the build produces output text.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(BuildOutputEvent)
std::string text;
bool is_stderr{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a task starts execution.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskExecutionStartedEvent)
std::string task_name;
std::string task_type; ///< "build", "test", "deploy", etc.
std::string command;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a task finishes execution.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TaskExecutionFinishedEvent)
std::string task_name;
bool success{false};
int exit_code{0};
float duration_seconds{0.0F};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 26: Run configuration events
// ============================================================================

/// Fired when a run configuration starts executing.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigStartedEvent)
std::string config_name;
bool is_debug{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a run configuration finishes (process exits).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigFinishedEvent)
std::string config_name;
int exit_code{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a run configuration is manually stopped by the user.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigStoppedEvent)
std::string config_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when the active run configuration changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigChangedEvent)
std::string config_name;
MARKAMP_DECLARE_EVENT_END;

/// Fired when layout mode changes (Default/Zen/Presentation).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutModeChangedEvent)
int mode{0}; ///< 0=Default, 1=Zen, 2=Presentation
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 29: Code Actions V2 events
// ============================================================================

/// Fired when code actions are computed for a cursor/selection range.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CodeActionsComputedEvent)
std::string document_uri;
int action_count{0};
bool has_preferred{false};
MARKAMP_DECLARE_EVENT_END;

/// Fired when a code action is applied.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CodeActionAppliedEvent)
std::string document_uri;
std::string action_title;
int action_kind{0}; ///< CodeActionKind as int
int edits_applied{0};
MARKAMP_DECLARE_EVENT_END;

/// Fired when the lightbulb visibility changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LightbulbVisibilityChangedEvent)
int line{-1};
bool visible{false};
int primary_kind{0}; ///< CodeActionKind as int
MARKAMP_DECLARE_EVENT_END;

/// Fired when an extension code action provider is registered.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ExtensionActionProviderRegisteredEvent)
std::string provider_id;
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 30: Floating Toolbar events
// ============================================================================

/// Fired when a floating toolbar is shown.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarShownEvent)
std::string toolbar_id;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a floating toolbar is hidden.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarHiddenEvent)
std::string toolbar_id;
MARKAMP_DECLARE_EVENT_END;

/// Fired when a floating toolbar is moved via drag.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FloatingToolbarMovedEvent)
std::string toolbar_id;
int pos_x{0};
int pos_y{0};
MARKAMP_DECLARE_EVENT_END;

// ════════════════════════════════════════════════════════════════════
// Phase 31 — Command Palette V2 Events
// ════════════════════════════════════════════════════════════════════

/// Emitted when the command palette is opened.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteOpenedEvent)
int mode{0}; ///< PaletteMode as integer (0=Commands, 1=QuickOpen, 2=GoToSymbol, 3=GoToLine)
MARKAMP_DECLARE_EVENT_END;

/// Emitted when the command palette is closed.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteClosedEvent)
std::string selected_command; ///< Command ID that was selected (empty if cancelled)
bool cancelled{false};        ///< True if the user cancelled without selection
MARKAMP_DECLARE_EVENT_END;

// ════════════════════════════════════════════════════════════════════
// Phase 32 — Go-To System Events
// ════════════════════════════════════════════════════════════════════

/// Emitted when the navigation location changes (back/forward/go_to).
MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigationChangedEvent)
std::string document_id;
int line{0};
int column{0};
MARKAMP_DECLARE_EVENT_END;

/// Emitted to navigate the editor to a specific line.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GoToLineEvent)
int line{0};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 33: Outline Panel Events
// ============================================================================

/// Emitted after the outline for a document has been rebuilt.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineRebuiltEvent)
std::string root_id;
int heading_count{0};
MARKAMP_DECLARE_EVENT_END;

/// Emitted when the outline panel visibility changes.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineVisibilityChangedEvent)
bool visible{false};
MARKAMP_DECLARE_EVENT_END;

// ============================================================================
// Phase 35: Quick Open Events
// ============================================================================

/// Emitted to preview a file without fully opening it.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FilePreviewRequestEvent)
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

/// Emitted to open a file in a split editor pane.
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileSplitOpenRequestEvent)
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

} // namespace markamp::core::events
