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

// Phase 06 Task 8: SidebarModeChangedEvent — broadcast when sidebar mode switches
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarModeChangedEvent)
std::string previous_mode; // Migrated from int to string
std::string new_mode;
MARKAMP_DECLARE_EVENT_END;

// Phase 06 Task 7: Badge notification events for ActivityBar
MARKAMP_DECLARE_EVENT_WITH_FIELDS(SearchResultCountEvent)
int count{0};
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
MARKAMP_DECLARE_EVENT(DuplicateLineRequestEvent);
MARKAMP_DECLARE_EVENT(ToggleCommentRequestEvent);
MARKAMP_DECLARE_EVENT(DeleteLineRequestEvent);
MARKAMP_DECLARE_EVENT(WrapToggleRequestEvent);

// ============================================================================
// R7 Editor action events
// ============================================================================

MARKAMP_DECLARE_EVENT(MoveLineUpRequestEvent);
MARKAMP_DECLARE_EVENT(MoveLineDownRequestEvent);
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

} // namespace markamp::core::events
