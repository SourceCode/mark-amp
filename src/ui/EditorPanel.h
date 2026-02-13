#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/stc/stc.h>
#include <wx/timer.h>

#include <filesystem>
#include <functional>
#include <optional>
#include <string>
#include <utility>

class wxTextCtrl;
class wxStaticText;

namespace markamp::ui
{
class FloatingFormatBar;
class LinkPreviewPopover;
class ImagePreviewPopover;
class TableEditorOverlay;
} // namespace markamp::ui

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Text editor panel wrapping wxStyledTextCtrl (Scintilla) for markdown editing.
/// Features: markdown syntax highlighting, undo/redo, find/replace,
/// debounced content change events, cursor tracking, placeholder text,
/// bracket matching, markdown auto-indent, configurable word wrap,
/// dynamic line number gutter, and large-file optimizations.
class EditorPanel : public ThemeAwareWindow
{
public:
    EditorPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // ── Content management ──
    void SetContent(const std::string& content);
    [[nodiscard]] auto GetContent() const -> std::string;
    [[nodiscard]] auto IsModified() const -> bool;
    void ClearModified();

    // ── Cursor ──
    [[nodiscard]] auto GetCursorLine() const -> int;
    [[nodiscard]] auto GetCursorColumn() const -> int;
    void SetCursorPosition(int line, int column);
    void SetSelection(int start, int end);

    // ── Editor operations ──
    void Undo();
    void Redo();
    [[nodiscard]] auto CanUndo() const -> bool;
    [[nodiscard]] auto CanRedo() const -> bool;

    // ── Find and replace ──
    void ShowFindBar();
    void HideFindBar();
    [[nodiscard]] auto IsFindBarVisible() const -> bool;

    // ── Raw Scintilla access (for Duplicate Line, Delete Line, etc.) ──
    [[nodiscard]] auto GetStyledTextCtrl() -> wxStyledTextCtrl*
    {
        return editor_;
    }

    // ── Configuration ──
    void SetWordWrap(bool enabled);
    void SetWordWrapMode(core::events::WrapMode mode);
    [[nodiscard]] auto GetWordWrapMode() const -> core::events::WrapMode;
    void SetShowLineNumbers(bool enabled);
    [[nodiscard]] auto GetShowLineNumbers() const -> bool;
    void SetFontSize(int size);
    void SetTabSize(int size);
    void SetBracketMatching(bool enabled);
    [[nodiscard]] auto GetBracketMatching() const -> bool;
    void SetAutoIndent(bool enabled);
    [[nodiscard]] auto GetAutoIndent() const -> bool;
    void SetSmartListContinuation(bool enabled);
    [[nodiscard]] auto GetSmartListContinuation() const -> bool;

    // ── QoL: Editor Actions ──
    void ToggleLineComment();
    void InsertDateTime();

    // ── R13: Zoom + EOL ──
    void ZoomIn();
    void ZoomOut();
    void ZoomReset();
    [[nodiscard]] auto GetZoomLevel() const -> int;
    void ConvertEolToLf();
    void ConvertEolToCrlf();
    void SortSelectedLines();
    void ConvertSelectionUpperCase();
    void ConvertSelectionLowerCase();

    void DuplicateLine();
    void MoveLineUp();
    void MoveLineDown();
    void DeleteLine();
    void InsertLineBelow();
    void GoToLineDialog();

    // ── Phase 1: Editor Core Improvements ──
    void SetIndentationGuides(bool enabled);
    [[nodiscard]] auto GetIndentationGuides() const -> bool;
    void SetCodeFolding(bool enabled);
    [[nodiscard]] auto GetCodeFolding() const -> bool;
    void SetShowWhitespace(bool enabled);
    [[nodiscard]] auto GetShowWhitespace() const -> bool;
    void SetEdgeColumn(int column);
    [[nodiscard]] auto GetEdgeColumn() const -> int;
    void SetGutterSeparator(bool enabled);
    [[nodiscard]] auto GetGutterSeparator() const -> bool;

    // ── Phase 3: Editor Behavior & UX ──
    void SetTrailingWhitespace(bool enabled);
    [[nodiscard]] auto GetTrailingWhitespace() const -> bool;
    void SetAutoTrimTrailingWhitespace(bool enabled);
    [[nodiscard]] auto GetAutoTrimTrailingWhitespace() const -> bool;
    void TrimTrailingWhitespace();

    // ── Phase 5: Snippets & Session ──
    /// A reusable snippet. The body may contain a cursor placeholder `$0`.
    struct Snippet
    {
        std::string name;    // e.g. "Bold"
        std::string trigger; // e.g. "**"
        std::string body;    // e.g. "**$0**"
    };
    /// Insert a snippet at the current cursor position
    void InsertSnippet(const Snippet& snippet);

    // ── R15: Additional editing helpers ──
    void SortSelectedLinesDesc();
    [[nodiscard]] auto GetWordAtCaret() const -> std::string;

    // ── Phase 5: Contextual Inline Tools ──
    void InsertBlockquote();
    void CycleHeading();
    void InsertTable();
    void ShowTableEditor();
    void HideTableEditor();
    void SetDocumentBasePath(const std::filesystem::path& base_path);

    // ── Phase 6D: Minimap ──
    void ToggleMinimap();

    // ── VS Code-Inspired Editor Improvements (20 items) ──

    // #1 Auto-closing brackets/quotes
    void SetAutoClosingBrackets(bool enabled);
    [[nodiscard]] auto GetAutoClosingBrackets() const -> bool;

    // #2 Multi-cursor editing
    void AddCursorAbove();
    void AddCursorBelow();
    void AddCursorAtNextOccurrence();

    // #3 Sticky scroll heading (enhanced)
    void SetStickyScrollEnabled(bool enabled);
    [[nodiscard]] auto GetStickyScrollEnabled() const -> bool;

    // #4 Inline color preview decorations
    void SetInlineColorPreview(bool enabled);
    [[nodiscard]] auto GetInlineColorPreview() const -> bool;

    // #5 Font ligature support
    void SetFontLigatures(bool enabled);
    [[nodiscard]] auto GetFontLigatures() const -> bool;

    // #6 Smooth caret animation
    void SetSmoothCaret(bool enabled);
    [[nodiscard]] auto GetSmoothCaret() const -> bool;

    // #7 Current line highlight
    void SetHighlightCurrentLine(bool enabled);
    [[nodiscard]] auto GetHighlightCurrentLine() const -> bool;

    // #8 Editor font family configuration
    void SetFontFamily(const std::string& family);
    [[nodiscard]] auto GetFontFamily() const -> std::string;

    // #9 Auto-save with configurable delay
    void SetAutoSave(bool enabled, int delay_seconds = 30);
    [[nodiscard]] auto GetAutoSave() const -> bool;
    [[nodiscard]] auto GetAutoSaveDelay() const -> int;

    // #10 Insert final newline on save
    void SetInsertFinalNewline(bool enabled);
    [[nodiscard]] auto GetInsertFinalNewline() const -> bool;
    void EnsureFinalNewline();

    // #11 Whitespace boundary rendering
    void SetWhitespaceBoundary(bool enabled);
    [[nodiscard]] auto GetWhitespaceBoundary() const -> bool;

    // #12 Markdown link auto-complete
    void SetLinkAutoComplete(bool enabled);
    [[nodiscard]] auto GetLinkAutoComplete() const -> bool;
    void SetWorkspaceFiles(const std::vector<std::string>& files);

    // #13 Drag-and-drop file insertion
    void SetDragDropEnabled(bool enabled);
    [[nodiscard]] auto GetDragDropEnabled() const -> bool;

    // #14 Word wrap column indicator (ruler)
    void SetShowEdgeColumnRuler(bool enabled);
    [[nodiscard]] auto GetShowEdgeColumnRuler() const -> bool;

    // #15 Selection highlight occurrences count → published via EventBus

    // #16 Go-to-symbol (heading navigation)
    struct HeadingSymbol
    {
        std::string text;
        int level{0};
        int line{0};
    };
    [[nodiscard]] auto GetHeadingSymbols() const -> std::vector<HeadingSymbol>;
    void GoToHeading(int line);

    // #17 Toggle block comment (HTML)
    void ToggleBlockComment();

    // #18 Smart select (expand / shrink)
    void ExpandSelection();
    void ShrinkSelection();

    // ── Phase 7: UX / Quality-of-Life Improvements (20 items) ──

    // #1 Cursor surrounding lines — keep N context lines visible around cursor
    void SetCursorSurroundingLines(int lines);
    [[nodiscard]] auto GetCursorSurroundingLines() const -> int;

    // #2 Scroll beyond last line — allow scrolling past EOF
    void SetScrollBeyondLastLine(bool enabled);
    [[nodiscard]] auto GetScrollBeyondLastLine() const -> bool;

    // #3 Smooth scrolling — animated scroll transitions
    void SetSmoothScrolling(bool enabled);
    [[nodiscard]] auto GetSmoothScrolling() const -> bool;

    // #4 Copy line (empty selection) — Ctrl+C with no selection copies whole line
    void CopyLineIfNoSelection();
    void SetEmptySelectionClipboard(bool enabled);
    [[nodiscard]] auto GetEmptySelectionClipboard() const -> bool;

    // #5 Join lines — merge selected lines into one
    void JoinLines();

    // #6 Reverse selected lines — reverse line order in selection
    void ReverseSelectedLines();

    // #7 Delete duplicate lines — remove duplicates from selection
    void DeleteDuplicateLines();

    // #8 Transpose characters — swap two characters around cursor
    void TransposeCharacters();

    // #9 Move selected text left/right — shift selection by one char
    void MoveSelectedTextLeft();
    void MoveSelectedTextRight();

    // #10 Block indent/outdent — Tab/Shift+Tab for block indentation
    void IndentSelection();
    void OutdentSelection();

    // #11 Cursor undo/redo — undo/redo cursor positions independently
    void CursorUndo();
    void CursorRedo();

    // #12 Select all occurrences of current word/selection
    void SelectAllOccurrences();

    // #13 Add selection to next find match (incremental multi-cursor)
    void AddSelectionToNextFindMatch();

    // #14 Toggle word wrap via keyboard (Alt+Z)
    void ToggleWordWrap();

    // #19 Auto-pair markdown emphasis — wrap selection in *, **, or `
    void AutoPairEmphasis(char emphasis_char);

    // #20 Smart backspace in pairs — delete matching pair
    void SmartBackspace();

    // ── Phase 8: 20 More VS Code-Inspired Improvements ──

    // #1–4 Folding actions (from folding/folding.ts)
    void FoldCurrentRegion();
    void UnfoldCurrentRegion();
    void FoldAllRegions();
    void UnfoldAllRegions();

    // #5 Expand line selection — select entire current line (Ctrl+L)
    void ExpandLineSelection();

    // #6 Delete current line — remove the line the cursor is on
    void DeleteCurrentLine();

    // #7 Toggle render whitespace — cycle whitespace display mode
    void ToggleRenderWhitespace();

    // #8 Toggle line numbers — show/hide the line number gutter
    void ToggleLineNumbers();

    // #9–11 Bracket operations (from bracketMatching.ts)
    void JumpToMatchingBracket();
    void SelectToMatchingBracket();
    void RemoveSurroundingBrackets();

    // #12 Duplicate selection or current line
    void DuplicateSelectionOrLine();

    // #13–15 Case transforms (from linesOperations.ts)
    void TransformToUppercase();
    void TransformToLowercase();
    void TransformToTitleCase();

    // #16–17 Sort lines (from linesOperations.ts)
    void SortLinesAscending();
    void SortLinesDescending();

    // #18–19 Insert blank line above cursor / Trim trailing whitespace
    void InsertLineAbove();
    void TrimTrailingWhitespaceNow();

    // #20 Toggle minimap panel visibility
    void ToggleMinimapVisibility();

    // ── Phase 9: 20 More VS Code-Inspired Improvements ──

    // #1–2 Copy line up/down (from linesOperations.ts)
    void CopyLineUp();
    void CopyLineDown();

    // #3–4 Delete all left/right of cursor (from linesOperations.ts)
    void DeleteAllLeft();
    void DeleteAllRight();

    // #5–6 Add/Remove line comment (from comment.ts)
    void AddLineComment();
    void RemoveLineComment();

    // #7–10 Toggle editor features via settings
    void ToggleAutoIndent();
    void ToggleBracketMatching();
    void ToggleCodeFolding();
    void ToggleIndentationGuides();

    // #11–12 Selection helpers
    void SelectWordAtCursor();
    void SelectCurrentParagraph();

    // #13 Toggle read-only mode
    void ToggleReadOnly();

    // #14, #20 Indentation conversion
    void ConvertIndentationToSpaces();
    void ConvertIndentationToTabs();

    /// Session state for save/restore
    struct SessionState
    {
        int cursor_position{0};
        int first_visible_line{0};
        int wrap_mode{0};
    };
    [[nodiscard]] auto GetSessionState() const -> SessionState;
    void RestoreSessionState(const SessionState& state);

    // ── Preferences persistence ──
    void LoadPreferences(core::Config& config);
    void SavePreferences(core::Config& config) const;

    // ── Constants ──
    static constexpr int kDefaultFontSize = 13;
    static constexpr int kDefaultTabSize = 4;
    static constexpr int kCaretWidth = 2;
    static constexpr int kCaretBlinkMs = 400; // R16 Fix 22: faster blink for smoother feel
    static constexpr int kDebounceMs = 50;    // Responsive! (was 300)
    static constexpr int kDebounceMaxMs = 500;
    static constexpr int kFindBarHeight = 36;
    static constexpr int kLargeFileThreshold = 50000;
    static constexpr int kMinGutterDigits = 3;
    static constexpr int kDefaultEdgeColumn = 80;
    static constexpr int kFoldMarginWidth = 14;
    static constexpr int kFoldMarginIndex = 2;

    // Phase 2: Indicator indices for overlay syntax highlighting
    static constexpr int kIndicatorFind = 0;            // find/replace highlights
    static constexpr int kIndicatorYamlFrontmatter = 1; // YAML frontmatter block
    static constexpr int kIndicatorTaskCheckbox = 2;    // task list checkboxes
    static constexpr int kIndicatorFootnote = 3;        // footnote references
    static constexpr int kIndicatorHtmlTag = 4;         // inline HTML tags
    static constexpr int kIndicatorBlockquoteNest = 5;  // nested blockquote depth
    static constexpr int kIndicatorWordHighlight = 6;   // word under cursor occurrences
    static constexpr int kIndicatorTrailingWS = 7;      // trailing whitespace

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    wxStyledTextCtrl* editor_{nullptr};
    core::EventBus& event_bus_;

    // ── Find bar widgets ──
    wxPanel* find_bar_{nullptr};
    wxTextCtrl* find_input_{nullptr};
    wxTextCtrl* replace_input_{nullptr};
    wxStaticText* match_count_label_{nullptr};
    bool find_bar_visible_{false};
    bool match_case_{false};
    bool replace_visible_{false};
    bool smart_list_continuation_{true};

    // ── Debounce timer ──
    wxTimer debounce_timer_;

    // ── Configuration state ──
    core::events::WrapMode wrap_mode_{core::events::WrapMode::Word};
    bool show_line_numbers_{true};
    bool bracket_matching_{true};
    bool auto_indent_{true};
    bool indentation_guides_{true};
    bool code_folding_{true};
    bool show_whitespace_{false};
    bool gutter_separator_{true};
    int font_size_{kDefaultFontSize};
    int tab_size_{kDefaultTabSize};
    int edge_column_{kDefaultEdgeColumn};
    int large_file_threshold_{kLargeFileThreshold};

    // ── Setup ──
    void CreateEditor();
    void CreateFindBar();
    void CreatePlaceholder();
    void ConfigureEditorDefaults();
    void SetupMarkdownLexer();
    void ApplyThemeToEditor();
    void UpdateLineNumberMargin();
    void ConfigureBracketMatching();
    void ConfigureFoldMargin();
    void ConfigureEdgeColumn();
    void ConfigureWhitespace();
    void ConfigureIndentGuides();
    void ApplyLargeFileOptimizations(int line_count);

    // ── Phase 2: Syntax overlay painting ──
    void SetupSyntaxIndicators();
    void ApplySyntaxOverlays();
    void ClearSyntaxOverlays();
    void HighlightYamlFrontmatter();
    void HighlightTaskCheckboxes();
    void HighlightFootnoteReferences();
    void HighlightInlineHtmlTags();
    void HighlightBlockquoteNesting();

    // ── Phase 3: Behavior methods ──
    void HandleSmartPairCompletion(int char_added);
    void SelectNextOccurrence();
    void HighlightWordUnderCursor();
    void ClearWordHighlights();
    void HandleSmartHome();
    void HighlightTrailingWhitespace();
    void UpdateStickyScrollHeading();

    // ── Phase 5: Snippet helpers ──
    void RegisterDefaultSnippets();

    // ── Event handlers ──
    void OnEditorChange(wxStyledTextEvent& event);
    void OnEditorUpdateUI(wxStyledTextEvent& event);
    void OnCharAdded(wxStyledTextEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event); // R4 Fix 1
    void ShowEditorContextMenu();          // R4 Fix 1
    void OnDebounceTimer(wxTimerEvent& event);

    // ── Bracket matching helpers ──
    void CheckBracketMatch();

    // ── Auto-indent helpers ──
    void HandleMarkdownAutoIndent(int char_added);
    void HandleSmartListContinuation();
    void CalculateAndPublishStats();

    // ── Find helpers ──
    void FindNext();
    void FindPrevious();
    void ReplaceOne();
    void ReplaceAll();
    void UpdateMatchCount();
    void HighlightAllMatches();
    void ClearFindHighlights();

    // ── Placeholder helpers ──
    void UpdatePlaceholderVisibility();

    // ── Line manipulation ──

    // ── Markdown formatting (public for menu/event wiring) ──
public:
    void ToggleBold();
    void ToggleItalic();
    void InsertLink();
    void ToggleInlineCode();

private:
    // ── Markdown formatting helpers ──
    void WrapSelectionWith(const std::string& prefix, const std::string& suffix);

    // ── Phase 2: Syntax overlays state ──
    bool syntax_overlays_enabled_{true};

    // ── Phase 3: Behavior state ──
    bool trailing_ws_visible_{true};
    bool auto_trim_trailing_ws_{false};
    std::string last_highlighted_word_;

    // ── Phase 5: Contextual Inline Tools state ──
    FloatingFormatBar* format_bar_{nullptr};
    LinkPreviewPopover* link_popover_{nullptr};
    ImagePreviewPopover* image_popover_{nullptr};
    TableEditorOverlay* table_overlay_{nullptr};
    wxTimer format_bar_timer_;
    std::filesystem::path document_base_path_;

    void ShowFormatBar();
    void HideFormatBar();
    void UpdateFormatBarPosition();
    void HandleFormatBarAction(int action);

    // Link/image detection at a character position
    struct LinkInfo
    {
        std::string text;
        std::string url;
    };
    auto DetectLinkAtPosition(int pos) -> std::optional<LinkInfo>;
    auto DetectImageAtPosition(int pos) -> std::optional<LinkInfo>;
    auto DetectTableAtCursor() -> std::optional<std::pair<int, int>>;

    void OnDwellStart(wxStyledTextEvent& event);
    void OnDwellEnd(wxStyledTextEvent& event);
    void OnFormatBarTimer(wxTimerEvent& event);

    // ── Phase 6D: Minimap ──
    wxStyledTextCtrl* minimap_{nullptr};
    bool minimap_visible_{false};
    void CreateMinimap();
    void UpdateMinimapContent();
    void OnMinimapClick(wxMouseEvent& event);

    // ── VS Code Improvements state ──
    bool auto_closing_brackets_{true};
    bool sticky_scroll_enabled_{false};
    bool inline_color_preview_{false};
    bool font_ligatures_{false};
    bool smooth_caret_{false};
    bool highlight_current_line_{true};
    std::string font_family_{"Menlo"};
    bool auto_save_{false};
    int auto_save_delay_seconds_{30};
    wxTimer auto_save_timer_;
    bool insert_final_newline_{true};
    bool whitespace_boundary_{false};
    bool link_auto_complete_{false};
    std::vector<std::string> workspace_files_;
    bool drag_drop_enabled_{true};
    bool show_edge_ruler_{false};
    std::vector<std::pair<int, int>> selection_stack_; // For smart select expand/shrink

    // ── Phase 7 UX/QoL state ──
    int cursor_surrounding_lines_{5};
    bool scroll_beyond_last_line_{true};
    bool smooth_scrolling_{false};
    bool empty_selection_clipboard_{true};
    std::vector<int> cursor_position_history_; // For cursor undo/redo
    int cursor_history_index_{-1};
    int last_recorded_cursor_pos_{-1};

    void OnAutoSaveTimer(wxTimerEvent& event);
    void OnFileDrop(wxDropFilesEvent& event);
    void UpdateSelectionCount();
    void HandleLinkAutoComplete();

    // ── R15 state ──
    std::string sticky_heading_;
    std::vector<Snippet> default_snippets_;
};

} // namespace markamp::ui
