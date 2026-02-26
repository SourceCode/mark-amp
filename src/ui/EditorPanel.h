#pragma once

#include "GutterDecorationProvider.h"
#include "OverviewRulerPanel.h"
#include "ThemeAwareWindow.h"
#include "core/DiagnosticsService.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"
#include "ui/animation/TransitionManager.h" // Phase 12: Animation

#include <wx/stc/stc.h>
#include <wx/timer.h>

#include <array>
#include <filesystem>
#include <functional>
#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

class wxTextCtrl;
class wxStaticText;

namespace markamp::ui
{
class LinkPreviewPopover;
class ImagePreviewPopover;
class TableEditorOverlay;
class MinimapPanel;
class FloatingFormatBar;
class FloatingFormatBar;
} // namespace markamp::ui

namespace markamp::core
{
class Config;
class FeatureRegistry;
} // namespace markamp::core

namespace markamp::ui
{

/// V8 Phase 9: Logical state layers that compose the editor surface.
/// Priority order (lower = higher visual priority when layers overlap):
///   Cursor → Selection → Diagnostics → ActiveLine → Search → CodeActions → WriteMode
enum class EditorStateLayer
{
    kCursor,      // Caret position and blink
    kSelection,   // Text selection(s) and multi-cursors
    kDiagnostics, // Error/warn/info squiggles from linting
    kActiveLine,  // Active line highlight band
    kSearch,      // Find/replace match highlights
    kCodeActions, // Quick-fix lightbulb margin
    kWriteMode    // Typewriter / Zen mode dimming
};

/// V8 Phase 10: Per-line diagnostic state for gutter indicators.
struct DiagnosticIndicator
{
    int line{0};         ///< Source line (0-based)
    int column_start{0}; ///< Start column for squiggle
    int length{0};       ///< Length of squiggle (0 = whole line)
    core::DiagnosticSeverity severity{core::DiagnosticSeverity::kError};
    std::string message;             ///< Primary diagnostic message
    bool quick_fix_available{false}; ///< Show lightbulb affordance
};

/// V8 Phase 10: Editor productivity modes with per-mode configurations.
enum class ProductivityMode
{
    kWriting, // Generous line spacing, minimal diagnostics, live preview sync
    kReview,  // Compact lines, full diagnostics, side-by-side preview
    kRefactor // Minimap on, aggressive diagnostics, no preview sync
};

/// V8 Phase 10: Actions available on preview content blocks.
enum class PreviewBlockAction
{
    kCopyCode,       // Copy code block contents
    kCollapseToggle, // Toggle section collapse/expand
    kOpenSource,     // Navigate to source line in editor
    kEditSection     // Open "edit this section" flow
};

/// V8 Phase 10: Reading profiles for preview typography and spacing.
enum class ReadingProfile
{
    kDocumentation, // Default spacing, readable widths
    kPresentation,  // Larger type, increased spacing
    kPrintReady     // Print-optimized margins and typography
};

/// Text editor panel wrapping wxStyledTextCtrl (Scintilla) for markdown editing.
/// Features: markdown syntax highlighting, undo/redo, find/replace,
/// debounced content change events, cursor tracking, placeholder text,
/// bracket matching, markdown auto-indent, configurable word wrap,
/// dynamic line number gutter, and large-file optimizations.
class EditorPanel : public ThemeAwareWindow
{
public:
    EditorPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);
    ~EditorPanel() override;

    // Non-copyable, non-movable (wxWidgets panel)
    EditorPanel(const EditorPanel&) = delete;
    EditorPanel& operator=(const EditorPanel&) = delete;
    EditorPanel(EditorPanel&&) = delete;
    EditorPanel& operator=(EditorPanel&&) = delete;

    // ── Content management ──
    void SetContent(const std::string& content);
    [[nodiscard]] auto GetContent() const -> std::string;
    [[nodiscard]] auto IsModified() const -> bool;
    void ClearModified();

    // ── Focus ──
    void SetFocus() override;

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

    /// Inject FeatureRegistry for feature-guard checks.
    void set_feature_registry(core::FeatureRegistry* registry)
    {
        feature_registry_ = registry;
    }

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

    // ── V8 Phase 10: Productivity modes ──
    void SetProductivityMode(ProductivityMode mode);
    [[nodiscard]] auto GetProductivityMode() const -> ProductivityMode;
    [[nodiscard]] auto diagnostic_indicators() const -> const std::vector<DiagnosticIndicator>&;
    void set_diagnostic_indicators(std::vector<DiagnosticIndicator> indicators);

    // ── Phase 7: Editor Core Improvements ──

    // Task 1: Relative line numbers
    void SetRelativeLineNumbers(bool enabled);
    [[nodiscard]] auto GetRelativeLineNumbers() const -> bool;

    // Task 6: Inline diagnostic annotations
    void ShowInlineDiagnostics(bool enabled);
    [[nodiscard]] auto GetInlineDiagnostics() const -> bool;
    void RefreshInlineDiagnostics();

    // Task 7: Quick-fix lightbulb
    void ShowQuickFixLightbulb(int line);
    void HideQuickFixLightbulb();
    [[nodiscard]] auto GetLightbulbLine() const -> int;

    // Task 8: Peek problem
    void PeekProblem(int line);
    void ClosePeekProblem();
    [[nodiscard]] auto IsPeekProblemVisible() const -> bool;
    [[nodiscard]] auto GetPeekProblemLine() const -> int;

    // Task 9–10: Bracket pair colorization
    void SetBracketPairColorization(bool enabled);
    [[nodiscard]] auto GetBracketPairColorization() const -> bool;

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

    // ── Phase 14: Editor Gutter Architecture ──
    static constexpr int kMarginLineNumbers = 0; // Absolute/Relative Line numbers
    static constexpr int kMarginFolding = 1;     // Fold markers (+/- or triangles)
    static constexpr int kMarginBreakpoints = 2; // Breakpoints (red dots) and Diagnostics
    static constexpr int kMarginGitChanges = 3;  // Git change indicators
    static constexpr int kMarginBookmarks = 4;   // Bookmark flags

    struct MarginConfig
    {
        bool visible{false};
        int width{0};
    };

    // Scintilla Marker Constants
    static constexpr int kMarkerBreakpoint = 1;
    static constexpr int kMarkerBookmark = 2;
    static constexpr int kMarkerError = 3;
    static constexpr int kMarkerWarning = 4;
    static constexpr int kMarkerInfo = 5;
    static constexpr int kMarkerQuickFix = 6;

    static constexpr int kMarkerGitAdded = 14;
    static constexpr int kMarkerGitModified = 15;
    static constexpr int kMarkerGitDeleted = 16;

    // Indicators (Indices > 7)
    static constexpr int kIndicatorSelectionLine = 8;
    static constexpr int kIndicatorError = 9;
    static constexpr int kIndicatorWarning = 10;
    static constexpr int kIndicatorInfo = 11;
    static constexpr int kIndicatorHint = 12;
    static constexpr int kIndicatorFoldRegion = 13;

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
    std::reference_wrapper<core::EventBus> event_bus_;
    core::FeatureRegistry* feature_registry_{nullptr};

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

    // ── Phase 14: Gutter State ──
    std::array<MarginConfig, 5> margin_configs_{};
    std::set<int> bookmarks_;
    std::unordered_map<std::string, std::vector<int>> breakpoints_;

    std::vector<std::unique_ptr<IGutterDecorationProvider>> gutter_providers_;
    void RenderGutterDecorations();

    struct MergeConflict
    {
        int start_line;
        int separator_line;
        int end_line;
    };
    std::vector<MergeConflict> conflicts_;

    // ── Setup ──
    void CreateEditor();
    void CreateFindBar();
    void CreatePlaceholder();
    void ConfigureEditorDefaults();
    void SetupMarkdownLexer();
    void ApplyThemeToEditor();
    void ApplyVSCodeSettings();
    void UpdateMarginWidths();
    void ConfigureBracketMatching();
    void SetupFoldMarkers();
    void ConfigureEdgeColumn();
    void ConfigureWhitespace();
    void ConfigureIndentGuides();
    void ApplyLargeFileOptimizations(int line_count);
    void UpdateLineNumberMargin();
    void ConfigureFoldMargin();

    // ── Phase 2: Syntax overlay painting ──
    void SetupSyntaxIndicators();
    void ApplySyntaxOverlays();
    void ClearSyntaxOverlays();
    void OnMarginClick(wxStyledTextEvent& event);
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
    MinimapPanel* minimap_{nullptr};
    OverviewRulerPanel* overview_ruler_{nullptr};
    std::vector<OverviewMarker> search_markers_;
    std::vector<OverviewMarker> diagnostic_markers_;
    std::vector<OverviewMarker> git_markers_;
    void UpdateOverviewRulerMarkers();

    bool minimap_visible_{false};
    void CreateMinimap();
    void UpdateMinimapContent();
    void OnMinimapClick(wxMouseEvent& event, int target_line);

    // ── Phase 12: Animation ──
    std::unique_ptr<animation::TransitionManager> transition_manager_;

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

    // ── R22: VS Code-Equivalent Settings state ──

    // Editor Behavior
    std::string cursor_blinking_{"blink"};
    int cursor_width_{2};
    bool mouse_wheel_zoom_{false};
    bool render_control_characters_{true};
    bool rounded_selection_{true};
    bool select_on_line_numbers_{true};
    std::string match_brackets_mode_{"always"};
    std::string render_line_highlight_{"all"};
    bool render_line_highlight_only_focus_{false};
    int word_wrap_column_{80};
    std::string wrapping_indent_{"same"};
    int line_height_override_{0};
    int letter_spacing_{0};
    int scroll_beyond_last_column_{5};
    bool auto_closing_quotes_{true};

    // Editor Appearance
    std::string show_folding_controls_{"mouseover"};
    bool folding_highlight_{true};
    bool glyph_margin_{true};
    bool overview_ruler_border_{true};
    int line_numbers_min_chars_{5};
    int padding_top_{0};
    int padding_bottom_{0};
    int minimap_max_column_{120};
    int minimap_scale_{1};
    std::string minimap_side_{"right"};
    bool bracket_pair_guides_{false};
    bool highlight_active_indentation_{true};
    bool bracket_pair_colorization_{false};
    bool color_decorators_{true};

    // Syntax Highlighting Token Toggles
    bool syntax_bold_keywords_{true};
    bool syntax_italic_comments_{true};
    bool syntax_highlight_strings_{true};
    bool syntax_highlight_numbers_{true};
    bool syntax_highlight_operators_{true};
    bool syntax_highlight_types_{true};
    bool syntax_highlight_functions_{true};
    bool syntax_highlight_constants_{true};
    bool syntax_highlight_preprocessor_{true};
    bool syntax_dim_whitespace_{false};

    // ── V8 Phase 10: Code Intelligence state ──
    ProductivityMode current_productivity_mode_{ProductivityMode::kWriting};
    std::vector<DiagnosticIndicator> diagnostic_indicators_;

    // ── Phase 7: Editor Core Improvements state ──
    bool relative_line_numbers_{false};
    bool inline_diagnostics_{true};
    int lightbulb_line_{-1};
    bool peek_problem_visible_{false};
    int peek_problem_line_{-1};

    // Phase 7 private helpers
    void RefreshBracketColors();
    void ApplyProductivityModeConfig();
    void ApplyDiagnosticMarkers();
};

} // namespace markamp::ui
