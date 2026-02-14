#pragma once

#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core::events
{

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
    Split
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

MARKAMP_DECLARE_EVENT_WITH_FIELDS(SidebarToggleEvent)
bool visible;
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
    HeadingAnchor
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

MARKAMP_DECLARE_EVENT(SettingsOpenRequestEvent);

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

// ============================================================================
// Plugin events
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginActivatedEvent)
std::string plugin_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PluginDeactivatedEvent)
std::string plugin_id;
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

enum class ActivityBarItem
{
    FileExplorer,
    Search,
    Settings,
    Themes,
    Extensions
};

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ActivityBarSelectionEvent)
ActivityBarItem item{ActivityBarItem::FileExplorer};

ActivityBarSelectionEvent() = default;
explicit ActivityBarSelectionEvent(ActivityBarItem item_id)
    : item(item_id)
{
}
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

} // namespace markamp::core::events
