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

} // namespace markamp::core::events
