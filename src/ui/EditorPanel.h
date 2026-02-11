#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/stc/stc.h>
#include <wx/timer.h>

#include <functional>
#include <string>

class wxTextCtrl;
class wxStaticText;

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

    // ── Editor operations ──
    void Undo();
    void Redo();
    [[nodiscard]] auto CanUndo() const -> bool;
    [[nodiscard]] auto CanRedo() const -> bool;

    // ── Find and replace ──
    void ShowFindBar();
    void HideFindBar();
    [[nodiscard]] auto IsFindBarVisible() const -> bool;

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
    static constexpr int kCaretBlinkMs = 530;
    static constexpr int kDebounceMs = 300;
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

    // ── Placeholder ──
    wxStaticText* placeholder_label_{nullptr};
    bool showing_placeholder_{false};

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
    void OnDebounceTimer(wxTimerEvent& event);

    // ── Bracket matching helpers ──
    void CheckBracketMatch();

    // ── Auto-indent helpers ──
    void HandleMarkdownAutoIndent(int char_added);

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
    void DuplicateLine();
    void MoveLineUp();
    void MoveLineDown();
    void DeleteLine();
    void InsertLineBelow();
    void GoToLineDialog();

    // ── Markdown formatting ──
    void WrapSelectionWith(const std::string& prefix, const std::string& suffix);
    void ToggleBold();
    void ToggleItalic();
    void InsertLink();
    void ToggleInlineCode();

    // ── Phase 2: Syntax overlays state ──
    bool syntax_overlays_enabled_{true};

    // ── Phase 3: Behavior state ──
    bool trailing_ws_visible_{true};
    bool auto_trim_trailing_ws_{false};
    std::string last_highlighted_word_;
};

} // namespace markamp::ui
