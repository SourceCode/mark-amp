#pragma once

#include "EventBus.h"

#include <string>

namespace markamp::core::events
{

// --- Theme events ---
struct ThemeChangedEvent : Event
{
    std::string theme_id;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ThemeChangedEvent";
    }
};

// --- File events ---
struct FileOpenedEvent : Event
{
    std::string file_path;
    std::string content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileOpenedEvent";
    }
};

struct FileContentChangedEvent : Event
{
    std::string file_id;
    std::string new_content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileContentChangedEvent";
    }
};

struct FileSavedEvent : Event
{
    std::string file_path;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileSavedEvent";
    }
};

struct ActiveFileChangedEvent : Event
{
    std::string file_id;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ActiveFileChangedEvent";
    }
};

// --- View events ---
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

struct ViewModeChangedEvent : Event
{
    ViewMode mode{ViewMode::Split};

    ViewModeChangedEvent() = default;
    explicit ViewModeChangedEvent(ViewMode m)
        : mode(m)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ViewModeChangedEvent";
    }
};

struct SidebarToggleEvent : Event
{
    bool visible;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SidebarToggleEvent";
    }
};

// --- Editor events ---
struct CursorPositionChangedEvent : Event
{
    int line;
    int column;
    int selection_length{0};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CursorPositionChangedEvent";
    }
};

struct EditorContentChangedEvent : Event
{
    std::string content;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EditorContentChangedEvent";
    }
};

struct EditorStatsChangedEvent : Event
{
    int word_count{0};
    int char_count{0};
    int line_count{0};
    int selection_length{0};

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EditorStatsChangedEvent";
    }
};

// --- Application events ---
struct AppReadyEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AppReadyEvent";
    }
};

struct AppShutdownEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AppShutdownEvent";
    }
};

// --- Scroll sync events ---
struct EditorScrollChangedEvent : Event
{
    double scroll_fraction{0.0}; // 0.0 = top, 1.0 = bottom
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EditorScrollChangedEvent";
    }
};

// --- Status bar events ---
struct FileEncodingDetectedEvent : Event
{
    std::string encoding_name; // e.g. "UTF-8", "UTF-8 BOM", "ASCII"
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileEncodingDetectedEvent";
    }
};

struct MermaidRenderStatusEvent : Event
{
    std::string status; // "ACTIVE", "INACTIVE", "RENDERING", "ERROR"
    bool active{false};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MermaidRenderStatusEvent";
    }
};

// --- Accessibility events ---
struct UIScaleChangedEvent : Event
{
    float scale_factor{1.0F};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UIScaleChangedEvent";
    }
};

struct InputModeChangedEvent : Event
{
    bool using_keyboard{false};
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InputModeChangedEvent";
    }
};

// --- Startup / Workspace Events ---

struct OpenFolderRequestEvent : Event
{
    // If empty, prompt user. If set, open directly.
    std::string path;

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "OpenFolderRequestEvent";
    }
};

struct WorkspaceOpenRequestEvent : Event
{
    std::string path;
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "WorkspaceOpenRequestEvent";
    }
};

// --- Focus mode events ---
struct FocusModeChangedEvent : Event
{
    bool active{false};

    FocusModeChangedEvent() = default;
    explicit FocusModeChangedEvent(bool is_active)
        : active(is_active)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FocusModeChangedEvent";
    }
};

// --- Scroll sync mode ---
enum class ScrollSyncMode
{
    Proportional,
    HeadingAnchor
};

struct ScrollSyncModeChangedEvent : Event
{
    ScrollSyncMode mode{ScrollSyncMode::Proportional};

    ScrollSyncModeChangedEvent() = default;
    explicit ScrollSyncModeChangedEvent(ScrollSyncMode sync_mode)
        : mode(sync_mode)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ScrollSyncModeChangedEvent";
    }
};

/// Request to open the theme gallery.
struct ThemeGalleryRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ThemeGalleryRequestEvent";
    }
};

// --- Tab events ---
struct TabSwitchedEvent : Event
{
    std::string file_path;

    TabSwitchedEvent() = default;
    explicit TabSwitchedEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TabSwitchedEvent";
    }
};

struct TabCloseRequestEvent : Event
{
    std::string file_path;

    TabCloseRequestEvent() = default;
    explicit TabCloseRequestEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TabCloseRequestEvent";
    }
};

struct TabSaveRequestEvent : Event
{
    std::string file_path;

    TabSaveRequestEvent() = default;
    explicit TabSaveRequestEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TabSaveRequestEvent";
    }
};

struct TabSaveAsRequestEvent : Event
{
    std::string file_path;

    TabSaveAsRequestEvent() = default;
    explicit TabSaveAsRequestEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TabSaveAsRequestEvent";
    }
};

struct FileReloadRequestEvent : Event
{
    std::string file_path;

    FileReloadRequestEvent() = default;
    explicit FileReloadRequestEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FileReloadRequestEvent";
    }
};

struct GoToLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "GoToLineRequestEvent";
    }
};

// --- Settings events ---
struct SettingsOpenRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SettingsOpenRequestEvent";
    }
};

struct SettingChangedEvent : Event
{
    std::string key;
    std::string value;

    SettingChangedEvent() = default;
    SettingChangedEvent(std::string key_name, std::string new_value)
        : key(std::move(key_name))
        , value(std::move(new_value))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SettingChangedEvent";
    }
};

// --- Plugin events ---
struct PluginActivatedEvent : Event
{
    std::string plugin_id;

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "PluginActivatedEvent";
    }
};

struct PluginDeactivatedEvent : Event
{
    std::string plugin_id;

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "PluginDeactivatedEvent";
    }
};

// --- Feature toggle events ---
struct FeatureToggledEvent : Event
{
    std::string feature_id;
    bool enabled{false};

    FeatureToggledEvent() = default;
    FeatureToggledEvent(std::string id, bool state)
        : feature_id(std::move(id))
        , enabled(state)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FeatureToggledEvent";
    }
};

// --- Extension management events ---
struct ExtensionInstalledEvent : Event
{
    std::string extension_id;
    std::string version;

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ExtensionInstalledEvent";
    }
};

struct ExtensionUninstalledEvent : Event
{
    std::string extension_id;

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ExtensionUninstalledEvent";
    }
};

struct ExtensionEnablementChangedEvent : Event
{
    std::string extension_id;
    bool enabled{false};

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ExtensionEnablementChangedEvent";
    }
};

/// Request to show the Extensions Browser panel in the sidebar.
struct ShowExtensionsBrowserRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ShowExtensionsBrowserRequestEvent";
    }
};

/// Request to show the Explorer panel in the sidebar.
struct ShowExplorerRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ShowExplorerRequestEvent";
    }
};

// --- Notification events ---
enum class NotificationLevel
{
    Info,
    Warning,
    Error,
    Success
};

struct NotificationEvent : Event
{
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

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "NotificationEvent";
    }
};

// --- Activity bar events ---
enum class ActivityBarItem
{
    FileExplorer,
    Search,
    Settings,
    Themes,
    Extensions
};

struct ActivityBarSelectionEvent : Event
{
    ActivityBarItem item{ActivityBarItem::FileExplorer};

    ActivityBarSelectionEvent() = default;
    explicit ActivityBarSelectionEvent(ActivityBarItem item_id)
        : item(item_id)
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ActivityBarSelectionEvent";
    }
};

// --- Workspace management events ---
struct WorkspaceRefreshRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "WorkspaceRefreshRequestEvent";
    }
};

struct ShowStartupRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ShowStartupRequestEvent";
    }
};

// --- R6 Editor action events ---
struct FindRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FindRequestEvent";
    }
};

struct ReplaceRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ReplaceRequestEvent";
    }
};

struct DuplicateLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DuplicateLineRequestEvent";
    }
};

struct ToggleCommentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleCommentRequestEvent";
    }
};

struct DeleteLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DeleteLineRequestEvent";
    }
};

struct WrapToggleRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "WrapToggleRequestEvent";
    }
};

// --- R7 Editor action events ---
struct MoveLineUpRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MoveLineUpRequestEvent";
    }
};

struct MoveLineDownRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MoveLineDownRequestEvent";
    }
};

struct JoinLinesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "JoinLinesRequestEvent";
    }
};

struct SortLinesAscRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SortLinesAscRequestEvent";
    }
};

struct SortLinesDescRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SortLinesDescRequestEvent";
    }
};

struct TransformUpperRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TransformUpperRequestEvent";
    }
};

struct TransformLowerRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TransformLowerRequestEvent";
    }
};

struct TransformTitleRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TransformTitleRequestEvent";
    }
};

struct SelectAllOccurrencesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SelectAllOccurrencesRequestEvent";
    }
};

struct ExpandLineSelectionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ExpandLineSelectionRequestEvent";
    }
};

struct InsertLineAboveRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertLineAboveRequestEvent";
    }
};

struct InsertLineBelowRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertLineBelowRequestEvent";
    }
};

struct FoldAllRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FoldAllRequestEvent";
    }
};

struct UnfoldAllRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UnfoldAllRequestEvent";
    }
};

struct ToggleLineNumbersRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleLineNumbersRequestEvent";
    }
};

struct ToggleWhitespaceRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleWhitespaceRequestEvent";
    }
};

// ── R8: 20 more editor action request events ──

struct CopyLineUpRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CopyLineUpRequestEvent";
    }
};

struct CopyLineDownRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CopyLineDownRequestEvent";
    }
};

struct DeleteAllLeftRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DeleteAllLeftRequestEvent";
    }
};

struct DeleteAllRightRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DeleteAllRightRequestEvent";
    }
};

struct ReverseLinesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ReverseLinesRequestEvent";
    }
};

struct DeleteDuplicateLinesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DeleteDuplicateLinesRequestEvent";
    }
};

struct TransposeCharsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TransposeCharsRequestEvent";
    }
};

struct IndentSelectionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "IndentSelectionRequestEvent";
    }
};

struct OutdentSelectionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "OutdentSelectionRequestEvent";
    }
};

struct SelectWordRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SelectWordRequestEvent";
    }
};

struct SelectParagraphRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SelectParagraphRequestEvent";
    }
};

struct ToggleReadOnlyRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleReadOnlyRequestEvent";
    }
};

struct ConvertIndentSpacesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ConvertIndentSpacesRequestEvent";
    }
};

struct ConvertIndentTabsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ConvertIndentTabsRequestEvent";
    }
};

struct JumpToBracketRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "JumpToBracketRequestEvent";
    }
};

struct ToggleMinimapRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleMinimapRequestEvent";
    }
};

struct FoldCurrentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FoldCurrentRequestEvent";
    }
};

struct UnfoldCurrentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UnfoldCurrentRequestEvent";
    }
};

struct AddLineCommentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AddLineCommentRequestEvent";
    }
};

struct RemoveLineCommentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "RemoveLineCommentRequestEvent";
    }
};

// ── R9: 20 more editor action request events ──

struct TrimTrailingWSRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TrimTrailingWSRequestEvent";
    }
};

struct ExpandSelectionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ExpandSelectionRequestEvent";
    }
};

struct ShrinkSelectionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ShrinkSelectionRequestEvent";
    }
};

struct CursorUndoRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CursorUndoRequestEvent";
    }
};

struct CursorRedoRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CursorRedoRequestEvent";
    }
};

struct MoveTextLeftRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MoveTextLeftRequestEvent";
    }
};

struct MoveTextRightRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "MoveTextRightRequestEvent";
    }
};

struct ToggleAutoIndentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleAutoIndentRequestEvent";
    }
};

struct ToggleBracketMatchingRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleBracketMatchingRequestEvent";
    }
};

struct ToggleCodeFoldingRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleCodeFoldingRequestEvent";
    }
};

struct ToggleIndentGuidesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleIndentGuidesRequestEvent";
    }
};

struct SelectToBracketRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SelectToBracketRequestEvent";
    }
};

struct ToggleBlockCommentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleBlockCommentRequestEvent";
    }
};

struct InsertDateTimeRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertDateTimeRequestEvent";
    }
};

struct BoldRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "BoldRequestEvent";
    }
};

struct ItalicRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ItalicRequestEvent";
    }
};

struct InlineCodeRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InlineCodeRequestEvent";
    }
};

struct BlockquoteRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "BlockquoteRequestEvent";
    }
};

struct CycleHeadingRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CycleHeadingRequestEvent";
    }
};

struct InsertTableRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertTableRequestEvent";
    }
};

// ── R10 events ──
struct ToggleSmartListContinuationRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleSmartListContinuationRequestEvent";
    }
};

struct CloseOtherTabsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CloseOtherTabsRequestEvent";
    }
};

struct CloseSavedTabsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CloseSavedTabsRequestEvent";
    }
};

struct InsertLinkRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertLinkRequestEvent";
    }
};

struct AddCursorBelowRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AddCursorBelowRequestEvent";
    }
};

struct AddCursorAboveRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AddCursorAboveRequestEvent";
    }
};

struct AddCursorNextOccurrenceRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AddCursorNextOccurrenceRequestEvent";
    }
};

struct RemoveSurroundingBracketsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "RemoveSurroundingBracketsRequestEvent";
    }
};

struct DuplicateSelectionOrLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DuplicateSelectionOrLineRequestEvent";
    }
};

struct ShowTableEditorRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ShowTableEditorRequestEvent";
    }
};

struct ToggleScrollBeyondLastLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleScrollBeyondLastLineRequestEvent";
    }
};

struct ToggleHighlightCurrentLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleHighlightCurrentLineRequestEvent";
    }
};

struct ToggleAutoClosingBracketsRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleAutoClosingBracketsRequestEvent";
    }
};

struct ToggleStickyScrollRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleStickyScrollRequestEvent";
    }
};

struct ToggleFontLigaturesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleFontLigaturesRequestEvent";
    }
};

struct ToggleSmoothCaretRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleSmoothCaretRequestEvent";
    }
};

struct ToggleInlineColorPreviewRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleInlineColorPreviewRequestEvent";
    }
};

struct ToggleEdgeColumnRulerRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleEdgeColumnRulerRequestEvent";
    }
};

struct EnsureFinalNewlineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "EnsureFinalNewlineRequestEvent";
    }
};

struct InsertSnippetRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "InsertSnippetRequestEvent";
    }
};

// ── R11 events ──
struct ToggleSmoothScrollingRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleSmoothScrollingRequestEvent";
    }
};
struct ToggleTrailingWSHighlightRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleTrailingWSHighlightRequestEvent";
    }
};
struct ToggleAutoTrimWSRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleAutoTrimWSRequestEvent";
    }
};
struct ToggleGutterSeparatorRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleGutterSeparatorRequestEvent";
    }
};
struct ToggleInsertFinalNewlineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleInsertFinalNewlineRequestEvent";
    }
};
struct ToggleWhitespaceBoundaryRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleWhitespaceBoundaryRequestEvent";
    }
};
struct ToggleLinkAutoCompleteRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleLinkAutoCompleteRequestEvent";
    }
};
struct ToggleDragDropRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleDragDropRequestEvent";
    }
};
struct ToggleAutoSaveRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleAutoSaveRequestEvent";
    }
};
struct ToggleEmptySelClipboardRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleEmptySelClipboardRequestEvent";
    }
};
struct CycleRenderWhitespaceRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CycleRenderWhitespaceRequestEvent";
    }
};
struct DeleteCurrentLineRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "DeleteCurrentLineRequestEvent";
    }
};
struct CopyLineNoSelRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CopyLineNoSelRequestEvent";
    }
};
struct AddSelNextMatchRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AddSelNextMatchRequestEvent";
    }
};
struct SmartBackspaceRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SmartBackspaceRequestEvent";
    }
};
struct HideTableEditorRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "HideTableEditorRequestEvent";
    }
};
struct AutoPairBoldRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AutoPairBoldRequestEvent";
    }
};
struct AutoPairItalicRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AutoPairItalicRequestEvent";
    }
};
struct AutoPairCodeRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "AutoPairCodeRequestEvent";
    }
};
struct ToggleMinimapR11RequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ToggleMinimapR11RequestEvent";
    }
};

// ── R12 events (only truly new ones) ──
struct ReverseSelectedLinesRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ReverseSelectedLinesRequestEvent";
    }
};
struct TransposeCharactersRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TransposeCharactersRequestEvent";
    }
};
struct FoldCurrentRegionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "FoldCurrentRegionRequestEvent";
    }
};
struct UnfoldCurrentRegionRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UnfoldCurrentRegionRequestEvent";
    }
};
struct JumpToMatchingBracketRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "JumpToMatchingBracketRequestEvent";
    }
};
struct SelectToMatchingBracketRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "SelectToMatchingBracketRequestEvent";
    }
};
struct CloseTabsToLeftRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CloseTabsToLeftRequestEvent";
    }
};
struct CloseTabsToRightRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CloseTabsToRightRequestEvent";
    }
};

struct PinTabRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "PinTabRequestEvent";
    }
};
struct UnpinTabRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "UnpinTabRequestEvent";
    }
};

// ── R13 RequestEvents ──
struct PrintDocumentRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "PrintDocumentRequestEvent";
    }
};
struct CopyFilePathRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "CopyFilePathRequestEvent";
    }
};
struct RevealInFinderRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "RevealInFinderRequestEvent";
    }
};
struct ZoomInRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ZoomInRequestEvent";
    }
};
struct ZoomOutRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ZoomOutRequestEvent";
    }
};
struct ZoomResetRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ZoomResetRequestEvent";
    }
};
struct ConvertEolLfRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ConvertEolLfRequestEvent";
    }
};
struct ConvertEolCrlfRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "ConvertEolCrlfRequestEvent";
    }
};

// ── R15 events ──
struct NewFileRequestEvent : Event
{
    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "NewFileRequestEvent";
    }
};

// ── R19 events ──
struct TabDuplicateRequestEvent : Event
{
    std::string file_path;

    TabDuplicateRequestEvent() = default;
    explicit TabDuplicateRequestEvent(std::string path)
        : file_path(std::move(path))
    {
    }

    [[nodiscard]] auto type_name() const -> std::string_view override
    {
        return "TabDuplicateRequestEvent";
    }
};

} // namespace markamp::core::events
