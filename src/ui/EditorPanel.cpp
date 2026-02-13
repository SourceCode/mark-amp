#include "EditorPanel.h"

#include "FloatingFormatBar.h"
#include "ImagePreviewPopover.h"
#include "LinkPreviewPopover.h"
#include "TableEditorOverlay.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/Logger.h"

#include <wx/button.h>
#include <wx/datetime.h>
#include <wx/numdlg.h>
#include <wx/sizer.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/tglbtn.h>

#include <array>
#include <cmath>
#include <numeric>
#include <regex>
#include <set>

namespace markamp::ui
{

// ═══════════════════════════════════════════════════════
// Construction
// ═══════════════════════════════════════════════════════

EditorPanel::EditorPanel(wxWindow* parent,
                         core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus)
    : ThemeAwareWindow(parent, theme_engine)
    , event_bus_(event_bus)
    , debounce_timer_(this)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    CreateFindBar();
    sizer->Add(find_bar_, 0, wxEXPAND);
    find_bar_->Hide();

    CreateEditor();
    sizer->Add(editor_, 1, wxEXPAND);

    SetSizer(sizer);

    // Bind debounce timer
    Bind(wxEVT_TIMER, &EditorPanel::OnDebounceTimer, this, debounce_timer_.GetId());

    // Phase 5: Format bar show timer
    Bind(wxEVT_TIMER, &EditorPanel::OnFormatBarTimer, this, format_bar_timer_.GetId());

    ApplyThemeToEditor();
}

// ═══════════════════════════════════════════════════════
// Content management
// ═══════════════════════════════════════════════════════

void EditorPanel::SetContent(const std::string& content)
{
    editor_->SetText(wxString::FromUTF8(content));
    editor_->EmptyUndoBuffer();
    editor_->SetSavePoint();
    editor_->GotoPos(0);
    editor_->EnsureCaretVisible();
    UpdateLineNumberMargin();

    // Apply large file optimizations based on content size
    int line_count = editor_->GetLineCount();
    ApplyLargeFileOptimizations(line_count);
}

auto EditorPanel::GetContent() const -> std::string
{
    return editor_->GetText().ToStdString();
}

auto EditorPanel::IsModified() const -> bool
{
    return editor_->GetModify();
}

void EditorPanel::ClearModified()
{
    editor_->SetSavePoint();
}

// ═══════════════════════════════════════════════════════
// Cursor
// ═══════════════════════════════════════════════════════

auto EditorPanel::GetCursorLine() const -> int
{
    return editor_->GetCurrentLine() + 1; // 1-based
}

auto EditorPanel::GetCursorColumn() const -> int
{
    auto pos = editor_->GetCurrentPos();
    return editor_->GetColumn(pos) + 1; // 1-based
}

void EditorPanel::SetCursorPosition(int line, int column)
{
    auto pos = editor_->FindColumn(line - 1, column - 1);
    editor_->GotoPos(pos);
}

void EditorPanel::SetSelection(int start, int end)
{
    editor_->SetSelection(start, end);
}

// ═══════════════════════════════════════════════════════
// Editor operations
// ═══════════════════════════════════════════════════════

void EditorPanel::Undo()
{
    editor_->Undo();
}

void EditorPanel::Redo()
{
    editor_->Redo();
}

auto EditorPanel::CanUndo() const -> bool
{
    return editor_->CanUndo();
}

auto EditorPanel::CanRedo() const -> bool
{
    return editor_->CanRedo();
}

// ═══════════════════════════════════════════════════════
// Find and replace
// ═══════════════════════════════════════════════════════

void EditorPanel::ShowFindBar()
{
    find_bar_visible_ = true;
    find_bar_->Show();
    find_input_->SetFocus();
    find_input_->SelectAll();
    GetSizer()->Layout();
}

void EditorPanel::HideFindBar()
{
    find_bar_visible_ = false;
    find_bar_->Hide();
    ClearFindHighlights();
    editor_->SetFocus();
    GetSizer()->Layout();
}

auto EditorPanel::IsFindBarVisible() const -> bool
{
    return find_bar_visible_;
}

// ═══════════════════════════════════════════════════════
// Configuration
// ═══════════════════════════════════════════════════════

void EditorPanel::SetWordWrap(bool enabled)
{
    SetWordWrapMode(enabled ? core::events::WrapMode::Word : core::events::WrapMode::None);
}

void EditorPanel::SetWordWrapMode(core::events::WrapMode mode)
{
    wrap_mode_ = mode;
    switch (mode)
    {
        case core::events::WrapMode::None:
            editor_->SetWrapMode(wxSTC_WRAP_NONE);
            editor_->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_NONE);
            break;
        case core::events::WrapMode::Word:
            editor_->SetWrapMode(wxSTC_WRAP_WORD);
            editor_->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
            break;
        case core::events::WrapMode::Character:
            editor_->SetWrapMode(wxSTC_WRAP_CHAR);
            editor_->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
            break;
    }
}

auto EditorPanel::GetWordWrapMode() const -> core::events::WrapMode
{
    return wrap_mode_;
}

void EditorPanel::SetShowLineNumbers(bool enabled)
{
    show_line_numbers_ = enabled;
    UpdateLineNumberMargin();
}

auto EditorPanel::GetShowLineNumbers() const -> bool
{
    return show_line_numbers_;
}

void EditorPanel::SetFontSize(int size)
{
    font_size_ = size;
    ApplyThemeToEditor();
}

void EditorPanel::SetTabSize(int size)
{
    tab_size_ = size;
    editor_->SetTabWidth(size);
    editor_->SetIndent(size);
}

void EditorPanel::SetBracketMatching(bool enabled)
{
    bracket_matching_ = enabled;
    if (!enabled)
    {
        // Clear any existing brace highlights
        editor_->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
    }
}

auto EditorPanel::GetBracketMatching() const -> bool
{
    return bracket_matching_;
}

void EditorPanel::SetAutoIndent(bool enabled)
{
    auto_indent_ = enabled;
}

auto EditorPanel::GetAutoIndent() const -> bool
{
    return auto_indent_;
}

void EditorPanel::SetSmartListContinuation(bool enabled)
{
    smart_list_continuation_ = enabled;
}

auto EditorPanel::GetSmartListContinuation() const -> bool
{
    return smart_list_continuation_;
}

// ═══════════════════════════════════════════════════════
// Preferences persistence
// ═══════════════════════════════════════════════════════

// ═══════════════════════════════════════════════════════
// Phase 1: New configuration methods
// ═══════════════════════════════════════════════════════

void EditorPanel::SetIndentationGuides(bool enabled)
{
    indentation_guides_ = enabled;
    ConfigureIndentGuides();
}

auto EditorPanel::GetIndentationGuides() const -> bool
{
    return indentation_guides_;
}

void EditorPanel::SetCodeFolding(bool enabled)
{
    code_folding_ = enabled;
    ConfigureFoldMargin();
}

auto EditorPanel::GetCodeFolding() const -> bool
{
    return code_folding_;
}

void EditorPanel::SetShowWhitespace(bool enabled)
{
    show_whitespace_ = enabled;
    ConfigureWhitespace();
}

auto EditorPanel::GetShowWhitespace() const -> bool
{
    return show_whitespace_;
}

void EditorPanel::SetEdgeColumn(int column)
{
    edge_column_ = column;
    ConfigureEdgeColumn();
}

auto EditorPanel::GetEdgeColumn() const -> int
{
    return edge_column_;
}

void EditorPanel::SetGutterSeparator(bool enabled)
{
    gutter_separator_ = enabled;
    // Separator is applied as part of fold margin + theme
    ConfigureFoldMargin();
}

auto EditorPanel::GetGutterSeparator() const -> bool
{
    return gutter_separator_;
}

// ═══════════════════════════════════════════════════════
// Preferences persistence
// ═══════════════════════════════════════════════════════

void EditorPanel::LoadPreferences(core::Config& config)
{
    font_size_ = config.get_int("editor.font_size", kDefaultFontSize);
    tab_size_ = config.get_int("editor.tab_size", kDefaultTabSize);
    show_line_numbers_ = config.get_bool("editor.show_line_numbers", true);
    bracket_matching_ = config.get_bool("editor.bracket_matching", true);
    auto_indent_ = config.get_bool("editor.auto_indent", true);
    large_file_threshold_ = config.get_int("editor.large_file_threshold", kLargeFileThreshold);
    indentation_guides_ = config.get_bool("editor.indentation_guides", true);
    code_folding_ = config.get_bool("editor.code_folding", true);
    show_whitespace_ = config.get_bool("editor.show_whitespace", false);
    edge_column_ = config.get_int("editor.edge_column", kDefaultEdgeColumn);
    gutter_separator_ = config.get_bool("editor.gutter_separator", true);

    int wrap_mode_int = config.get_int("editor.word_wrap_mode", 1); // default: Word
    switch (wrap_mode_int)
    {
        case 0:
            wrap_mode_ = core::events::WrapMode::None;
            break;
        case 2:
            wrap_mode_ = core::events::WrapMode::Character;
            break;
        default:
            wrap_mode_ = core::events::WrapMode::Word;
            break;
    }

    // Apply all loaded settings
    SetWordWrapMode(wrap_mode_);
    SetShowLineNumbers(show_line_numbers_);
    SetFontSize(font_size_);
    SetTabSize(tab_size_);
    SetIndentationGuides(indentation_guides_);
    SetCodeFolding(code_folding_);
    SetShowWhitespace(show_whitespace_);
    SetEdgeColumn(edge_column_);
    SetGutterSeparator(gutter_separator_);

    // Phase 3 preferences
    trailing_ws_visible_ = config.get_bool("editor.trailing_whitespace", true);
    auto_trim_trailing_ws_ = config.get_bool("editor.auto_trim_trailing_ws", false);

    // QoL
    smart_list_continuation_ = config.get_bool("editor.smart_list_continuation", true);
}

void EditorPanel::SavePreferences(core::Config& config) const
{
    config.set("editor.font_size", font_size_);
    config.set("editor.tab_size", tab_size_);
    config.set("editor.show_line_numbers", show_line_numbers_);
    config.set("editor.bracket_matching", bracket_matching_);
    config.set("editor.auto_indent", auto_indent_);
    config.set("editor.large_file_threshold", large_file_threshold_);
    config.set("editor.indentation_guides", indentation_guides_);
    config.set("editor.code_folding", code_folding_);
    config.set("editor.show_whitespace", show_whitespace_);
    config.set("editor.edge_column", edge_column_);
    config.set("editor.gutter_separator", gutter_separator_);

    int wrap_mode_int = 1; // Word default
    switch (wrap_mode_)
    {
        case core::events::WrapMode::None:
            wrap_mode_int = 0;
            break;
        case core::events::WrapMode::Word:
            wrap_mode_int = 1;
            break;
        case core::events::WrapMode::Character:
            wrap_mode_int = 2;
            break;
    }
    config.set("editor.word_wrap_mode", wrap_mode_int);

    // Phase 3 preferences
    config.set("editor.trailing_whitespace", trailing_ws_visible_);
    config.set("editor.auto_trim_trailing_ws", auto_trim_trailing_ws_);

    // QoL
    config.set("editor.smart_list_continuation", smart_list_continuation_);
}

// ═══════════════════════════════════════════════════════
// Theme
// ═══════════════════════════════════════════════════════

void EditorPanel::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);
    ApplyThemeToEditor();
}

// ═══════════════════════════════════════════════════════
// Private: Setup
// ═══════════════════════════════════════════════════════

void EditorPanel::CreateEditor()
{
    editor_ = new wxStyledTextCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
    ConfigureEditorDefaults();
    SetupMarkdownLexer();
    ConfigureBracketMatching();

    // Bind events
    editor_->Bind(wxEVT_STC_CHANGE, &EditorPanel::OnEditorChange, this);
    editor_->Bind(wxEVT_STC_UPDATEUI, &EditorPanel::OnEditorUpdateUI, this);
    editor_->Bind(wxEVT_STC_CHARADDED, &EditorPanel::OnCharAdded, this);
    editor_->Bind(wxEVT_KEY_DOWN, &EditorPanel::OnKeyDown, this);
    editor_->Bind(wxEVT_MOUSEWHEEL, &EditorPanel::OnMouseWheel, this);

    // R4 Fix 1: Editor right-click context menu
    editor_->Bind(wxEVT_RIGHT_DOWN, &EditorPanel::OnRightDown, this);

    // Phase 5: Dwell events for link/image preview
    editor_->SetMouseDwellTime(500);
    editor_->Bind(wxEVT_STC_DWELLSTART, &EditorPanel::OnDwellStart, this);
    editor_->Bind(wxEVT_STC_DWELLEND, &EditorPanel::OnDwellEnd, this);
}

void EditorPanel::CreateFindBar()
{
    find_bar_ = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, kFindBarHeight));
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    find_input_ = new wxTextCtrl(
        find_bar_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);
    replace_input_ = new wxTextCtrl(
        find_bar_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(200, -1), wxTE_PROCESS_ENTER);

    auto* prev_btn = new wxButton(
        find_bar_, wxID_ANY, wxString::FromUTF8("\u25B2"), wxDefaultPosition, wxSize(28, -1));
    auto* next_btn = new wxButton(
        find_bar_, wxID_ANY, wxString::FromUTF8("\u25BC"), wxDefaultPosition, wxSize(28, -1));
    auto* close_btn = new wxButton(
        find_bar_, wxID_ANY, wxString::FromUTF8("\u2715"), wxDefaultPosition, wxSize(28, -1));
    auto* case_btn =
        new wxToggleButton(find_bar_, wxID_ANY, "Aa", wxDefaultPosition, wxSize(28, -1));

    match_count_label_ = new wxStaticText(find_bar_, wxID_ANY, "");

    auto* replace_btn =
        new wxButton(find_bar_, wxID_ANY, "Replace", wxDefaultPosition, wxDefaultSize);
    auto* replace_all_btn =
        new wxButton(find_bar_, wxID_ANY, "All", wxDefaultPosition, wxDefaultSize);

    sizer->AddSpacer(8);
    sizer->Add(find_input_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    sizer->Add(prev_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    sizer->Add(next_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    sizer->Add(case_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    sizer->Add(match_count_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
    sizer->Add(replace_input_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    sizer->Add(replace_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
    sizer->Add(replace_all_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    sizer->AddStretchSpacer();
    sizer->Add(close_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    find_bar_->SetSizer(sizer);

    // Bind find bar events
    find_input_->Bind(wxEVT_TEXT,
                      [this](wxCommandEvent&)
                      {
                          HighlightAllMatches();
                          UpdateMatchCount();
                      });

    find_input_->Bind(wxEVT_TEXT_ENTER, [this](wxCommandEvent&) { FindNext(); });
    prev_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { FindPrevious(); });
    next_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { FindNext(); });
    close_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { HideFindBar(); });

    case_btn->Bind(wxEVT_TOGGLEBUTTON,
                   [this, case_btn](wxCommandEvent&)
                   {
                       match_case_ = case_btn->GetValue();
                       HighlightAllMatches();
                       UpdateMatchCount();
                   });

    replace_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { ReplaceOne(); });
    replace_all_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) { ReplaceAll(); });
}

void EditorPanel::ConfigureEditorDefaults()
{
    editor_->SetUseTabs(false);
    editor_->SetTabWidth(tab_size_);
    editor_->SetIndent(tab_size_);
    editor_->SetBackSpaceUnIndents(true);
    editor_->SetTabIndents(true);
    editor_->SetViewEOL(false);

    // Scrolling
    editor_->SetEndAtLastLine(false);
    editor_->SetScrollWidthTracking(true);

    // Caret — accent-colored, smooth blink (Item 4)
    editor_->SetCaretPeriod(kCaretBlinkMs);
    editor_->SetCaretWidth(kCaretWidth);
    editor_->SetCaretStyle(wxSTC_CARETSTYLE_LINE);

    // Word wrap (default: Word mode with end indicator)
    editor_->SetWrapMode(wxSTC_WRAP_WORD);
    editor_->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);

    // Line number margin — ON by default (Items 1, 5)
    editor_->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    editor_->SetMarginWidth(1, 0); // Symbols — off

    // Disable auto-complete
    editor_->AutoCompCancel();

    // Padding inside the editor (QoL Item 4)
    editor_->SetMarginLeft(16);
    editor_->SetMarginRight(16); // Balanced padding

    // Current line highlight — always visible
    editor_->SetCaretLineVisibleAlways(true);

    // Multi-selection prep (Phase 3 Item 22)
    editor_->SetMultipleSelection(true);
    editor_->SetAdditionalSelectionTyping(true);
    editor_->SetVirtualSpaceOptions(wxSTC_VS_RECTANGULARSELECTION);

    // Phase 3 Item 27: Column selection mode (Alt+drag)
    editor_->SetRectangularSelectionModifier(wxSTC_KEYMOD_ALT);

    // Performance: layout cache for smoother scrolling
    editor_->SetLayoutCache(wxSTC_CACHE_PAGE);
    editor_->SetBufferedDraw(true);

    // Phase 1 configurations
    ConfigureIndentGuides();
    ConfigureFoldMargin();
    ConfigureEdgeColumn();
    ConfigureWhitespace();
    UpdateLineNumberMargin();

    // Phase 2: syntax indicator overlays
    SetupSyntaxIndicators();
}

void EditorPanel::SetupMarkdownLexer()
{
    editor_->SetLexer(wxSTC_LEX_MARKDOWN);
}

void EditorPanel::ConfigureBracketMatching()
{
    // Bracket matching styles are applied in ApplyThemeToEditor()
    // This method ensures the highlight indicators are initialized
    editor_->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
}

void EditorPanel::ConfigureFoldMargin()
{
    if (code_folding_)
    {
        // Enable fold margin (margin index 2)
        editor_->SetMarginType(kFoldMarginIndex, wxSTC_MARGIN_SYMBOL);
        editor_->SetMarginMask(kFoldMarginIndex, static_cast<int>(wxSTC_MASK_FOLDERS));
        editor_->SetMarginWidth(kFoldMarginIndex, kFoldMarginWidth);
        editor_->SetMarginSensitive(kFoldMarginIndex, true);

        // Fold markers — modern arrow style
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDER, wxSTC_MARK_ARROWDOWN);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN, wxSTC_MARK_ARROWDOWN);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDEREND, wxSTC_MARK_ARROWDOWN);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDERSUB, wxSTC_MARK_VLINE);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL, wxSTC_MARK_LCORNER);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_ARROWDOWN);
        editor_->MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER);

        // Folding properties
        editor_->SetProperty("fold", "1");
        editor_->SetProperty("fold.compact", "0");
        editor_->SetProperty("fold.html", "1");

        // Automatic fold level painting
        editor_->SetAutomaticFold(wxSTC_AUTOMATICFOLD_SHOW | wxSTC_AUTOMATICFOLD_CLICK |
                                  wxSTC_AUTOMATICFOLD_CHANGE);
    }
    else
    {
        editor_->SetMarginWidth(kFoldMarginIndex, 0);
    }
}

void EditorPanel::ConfigureEdgeColumn()
{
    if (edge_column_ > 0)
    {
        editor_->SetEdgeMode(wxSTC_EDGE_LINE);
        editor_->SetEdgeColumn(edge_column_);
    }
    else
    {
        editor_->SetEdgeMode(wxSTC_EDGE_NONE);
    }
}

void EditorPanel::ConfigureWhitespace()
{
    editor_->SetViewWhiteSpace(show_whitespace_ ? wxSTC_WS_VISIBLEALWAYS : wxSTC_WS_INVISIBLE);
}

void EditorPanel::ConfigureIndentGuides()
{
    editor_->SetIndentationGuides(indentation_guides_ ? wxSTC_IV_LOOKBOTH : wxSTC_IV_NONE);
}

void EditorPanel::ApplyLargeFileOptimizations(int line_count)
{
    if (line_count > large_file_threshold_)
    {
        // Disable syntax highlighting for very large files
        editor_->SetLexer(wxSTC_LEX_NULL);
        // Maximize rendering cache
        editor_->SetLayoutCache(wxSTC_CACHE_DOCUMENT);
        // Enable idle styling — style incrementally without blocking UI
        editor_->SetIdleStyling(wxSTC_IDLESTYLING_ALL);
        // Disable bracket matching for performance
        bracket_matching_ = false;
        // Disable code folding for large files
        editor_->SetMarginWidth(kFoldMarginIndex, 0);

        MARKAMP_LOG_INFO(
            "Large file mode: {} lines (threshold: {})", line_count, large_file_threshold_);
    }
    else
    {
        // Ensure normal mode
        editor_->SetLayoutCache(wxSTC_CACHE_PAGE);
        editor_->SetIdleStyling(wxSTC_IDLESTYLING_NONE);
    }
}

// ═══════════════════════════════════════════════════════
// Phase 2: Syntax overlay highlighting
// ═══════════════════════════════════════════════════════

void EditorPanel::SetupSyntaxIndicators()
{
    // Indicator 1: YAML frontmatter — subtle dotbox background
    editor_->IndicatorSetStyle(kIndicatorYamlFrontmatter, wxSTC_INDIC_DOTBOX);
    editor_->IndicatorSetAlpha(kIndicatorYamlFrontmatter, 40);
    editor_->IndicatorSetOutlineAlpha(kIndicatorYamlFrontmatter, 80);

    // Indicator 2: Task checkboxes — roundbox
    editor_->IndicatorSetStyle(kIndicatorTaskCheckbox, wxSTC_INDIC_ROUNDBOX);
    editor_->IndicatorSetAlpha(kIndicatorTaskCheckbox, 50);
    editor_->IndicatorSetOutlineAlpha(kIndicatorTaskCheckbox, 100);

    // Indicator 3: Footnote references — underline
    editor_->IndicatorSetStyle(kIndicatorFootnote, wxSTC_INDIC_COMPOSITIONTHIN);

    // Indicator 4: Inline HTML tags — box
    editor_->IndicatorSetStyle(kIndicatorHtmlTag, wxSTC_INDIC_BOX);
    editor_->IndicatorSetAlpha(kIndicatorHtmlTag, 60);

    // Indicator 5: Nested blockquote depth — textfore (color change)
    editor_->IndicatorSetStyle(kIndicatorBlockquoteNest, wxSTC_INDIC_TEXTFORE);

    // Phase 3 — Indicator 6: Word highlight occurrences — subtle roundbox
    editor_->IndicatorSetStyle(kIndicatorWordHighlight, wxSTC_INDIC_ROUNDBOX);
    editor_->IndicatorSetAlpha(kIndicatorWordHighlight, 35);
    editor_->IndicatorSetOutlineAlpha(kIndicatorWordHighlight, 90);

    // Phase 3 — Indicator 7: Trailing whitespace — squiggle underline
    editor_->IndicatorSetStyle(kIndicatorTrailingWS, wxSTC_INDIC_SQUIGGLE);
}

void EditorPanel::ClearSyntaxOverlays()
{
    auto doc_len = editor_->GetLength();
    for (int ind = kIndicatorYamlFrontmatter; ind <= kIndicatorBlockquoteNest; ++ind)
    {
        editor_->SetIndicatorCurrent(ind);
        editor_->IndicatorClearRange(0, doc_len);
    }
}

void EditorPanel::ApplySyntaxOverlays()
{
    if (!syntax_overlays_enabled_)
    {
        return;
    }

    // Skip for large files
    if (editor_->GetLineCount() > large_file_threshold_)
    {
        return;
    }

    // Apply themed indicator colors
    auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
    auto accent2 = theme_engine().color(core::ThemeColorToken::AccentSecondary);
    auto muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    auto border = theme_engine().color(core::ThemeColorToken::BorderLight);

    editor_->IndicatorSetForeground(kIndicatorYamlFrontmatter, muted);
    editor_->IndicatorSetForeground(kIndicatorTaskCheckbox, accent);
    editor_->IndicatorSetForeground(kIndicatorFootnote, accent2);
    editor_->IndicatorSetForeground(kIndicatorHtmlTag, border);
    editor_->IndicatorSetForeground(kIndicatorBlockquoteNest, accent2);

    ClearSyntaxOverlays();

    HighlightYamlFrontmatter();
    HighlightTaskCheckboxes();
    HighlightFootnoteReferences();
    HighlightInlineHtmlTags();
    HighlightBlockquoteNesting();
}

void EditorPanel::HighlightYamlFrontmatter()
{
    // YAML frontmatter: starts at line 0 with "---" and ends at next "---"
    if (editor_->GetLineCount() < 3)
    {
        return;
    }

    auto first_line = editor_->GetLine(0).ToStdString();
    // Trim trailing newline/whitespace
    while (!first_line.empty() &&
           (first_line.back() == '\n' || first_line.back() == '\r' || first_line.back() == ' '))
    {
        first_line.pop_back();
    }

    if (first_line != "---")
    {
        return;
    }

    // Find closing ---
    int end_line = -1;
    int line_count = editor_->GetLineCount();
    for (int line_idx = 1; line_idx < std::min(line_count, 100); ++line_idx) // scan max 100 lines
    {
        auto line_text = editor_->GetLine(line_idx).ToStdString();
        while (!line_text.empty() &&
               (line_text.back() == '\n' || line_text.back() == '\r' || line_text.back() == ' '))
        {
            line_text.pop_back();
        }
        if (line_text == "---")
        {
            end_line = line_idx;
            break;
        }
    }

    if (end_line < 0)
    {
        return;
    }

    int start_pos = 0;
    int end_pos = editor_->GetLineEndPosition(end_line) + 1; // include the newline
    if (end_pos > editor_->GetLength())
    {
        end_pos = editor_->GetLength();
    }

    editor_->SetIndicatorCurrent(kIndicatorYamlFrontmatter);
    editor_->IndicatorFillRange(start_pos, end_pos - start_pos);
}

void EditorPanel::HighlightTaskCheckboxes()
{
    // Match patterns: "- [ ]" and "- [x]" (case-insensitive x)
    editor_->SetIndicatorCurrent(kIndicatorTaskCheckbox);

    int doc_len = editor_->GetLength();
    int line_count = editor_->GetLineCount();

    for (int line_idx = 0; line_idx < line_count; ++line_idx)
    {
        auto line_text = editor_->GetLine(line_idx).ToStdString();
        // Look for "- [ ]" or "- [x]" or "- [X]"
        size_t search_pos = 0;
        while (search_pos < line_text.size())
        {
            // Skip leading whitespace
            size_t check_start = line_text.find("- [", search_pos);
            if (check_start == std::string::npos)
            {
                break;
            }
            // Verify the pattern is complete: "- [ ]" or "- [x]" or "- [X]"
            if (check_start + 5 <= line_text.size())
            {
                char checkbox_char = line_text[check_start + 3];
                if ((checkbox_char == ' ' || checkbox_char == 'x' || checkbox_char == 'X') &&
                    line_text[check_start + 4] == ']')
                {
                    int abs_pos =
                        editor_->PositionFromLine(line_idx) + static_cast<int>(check_start);
                    if (abs_pos + 5 <= doc_len)
                    {
                        editor_->IndicatorFillRange(abs_pos, 5);
                    }
                }
            }
            search_pos = check_start + 3;
        }
    }
}

void EditorPanel::HighlightFootnoteReferences()
{
    // Match pattern: "[^identifier]" — not at start of line (inline reference)
    editor_->SetIndicatorCurrent(kIndicatorFootnote);

    int doc_len = editor_->GetLength();
    int line_count = editor_->GetLineCount();

    for (int line_idx = 0; line_idx < line_count; ++line_idx)
    {
        auto line_text = editor_->GetLine(line_idx).ToStdString();
        size_t search_pos = 0;
        while (search_pos < line_text.size())
        {
            size_t fn_start = line_text.find("[^", search_pos);
            if (fn_start == std::string::npos)
            {
                break;
            }
            size_t fn_end = line_text.find(']', fn_start + 2);
            if (fn_end == std::string::npos)
            {
                break;
            }
            int length = static_cast<int>(fn_end - fn_start + 1);
            int abs_pos = editor_->PositionFromLine(line_idx) + static_cast<int>(fn_start);
            if (abs_pos + length <= doc_len)
            {
                editor_->IndicatorFillRange(abs_pos, length);
            }
            search_pos = fn_end + 1;
        }
    }
}

void EditorPanel::HighlightInlineHtmlTags()
{
    // Match pattern: "<tagname>" or "</tagname>" or "<tagname attr>" — simple HTML tags
    editor_->SetIndicatorCurrent(kIndicatorHtmlTag);

    int doc_len = editor_->GetLength();
    int line_count = editor_->GetLineCount();

    for (int line_idx = 0; line_idx < line_count; ++line_idx)
    {
        auto line_text = editor_->GetLine(line_idx).ToStdString();
        size_t search_pos = 0;
        while (search_pos < line_text.size())
        {
            size_t tag_start = line_text.find('<', search_pos);
            if (tag_start == std::string::npos)
            {
                break;
            }
            size_t tag_end = line_text.find('>', tag_start + 1);
            if (tag_end == std::string::npos)
            {
                break;
            }
            // Validate it looks like a tag (starts with letter or /)
            if (tag_start + 1 < line_text.size())
            {
                char first = line_text[tag_start + 1];
                if (std::isalpha(static_cast<unsigned char>(first)) || first == '/')
                {
                    int length = static_cast<int>(tag_end - tag_start + 1);
                    int abs_pos = editor_->PositionFromLine(line_idx) + static_cast<int>(tag_start);
                    if (abs_pos + length <= doc_len)
                    {
                        editor_->IndicatorFillRange(abs_pos, length);
                    }
                }
            }
            search_pos = tag_end + 1;
        }
    }
}

void EditorPanel::HighlightBlockquoteNesting()
{
    // Highlight lines starting with "> > " etc. with increasing visual distinction
    editor_->SetIndicatorCurrent(kIndicatorBlockquoteNest);

    int line_count = editor_->GetLineCount();
    int doc_len = editor_->GetLength();

    for (int line_idx = 0; line_idx < line_count; ++line_idx)
    {
        auto line_text = editor_->GetLine(line_idx).ToStdString();

        // Count nesting depth (consecutive '>' characters)
        int depth = 0;
        size_t char_pos = 0;
        while (char_pos < line_text.size())
        {
            if (line_text[char_pos] == '>')
            {
                ++depth;
                ++char_pos;
                // Skip optional space after >
                if (char_pos < line_text.size() && line_text[char_pos] == ' ')
                {
                    ++char_pos;
                }
            }
            else if (line_text[char_pos] == ' ')
            {
                ++char_pos; // skip leading whitespace
            }
            else
            {
                break;
            }
        }

        // Only highlight when nesting depth >= 2
        if (depth >= 2)
        {
            int line_start = editor_->PositionFromLine(line_idx);
            // Highlight the '>' markers portion
            int marker_len = static_cast<int>(char_pos);
            if (line_start + marker_len <= doc_len)
            {
                editor_->IndicatorFillRange(line_start, marker_len);
            }
        }
    }
}

// ═══════════════════════════════════════════════════════
// Phase 3: Editor Behavior & UX
// ═══════════════════════════════════════════════════════

void EditorPanel::HandleSmartPairCompletion(int char_added)
{
    int pos = editor_->GetCurrentPos();

    // Map of opening → closing characters
    struct PairEntry
    {
        char opening;
        char closing;
    };

    static constexpr std::array<PairEntry, 6> kPairs = {{
        {'(', ')'},
        {'[', ']'},
        {'{', '}'},
        {'"', '"'},
        {'\'', '\''},
        {'`', '`'},
    }};

    // Check if the character at cursor position matches the closing character
    // If so, skip over it instead of inserting a duplicate
    for (const auto& pair : kPairs)
    {
        if (char_added == pair.closing && pos < editor_->GetLength())
        {
            char next_char = static_cast<char>(editor_->GetCharAt(pos));
            if (next_char == pair.closing && pair.opening != pair.closing)
            {
                // Skip over the existing closing character
                editor_->SetCurrentPos(pos + 1);
                editor_->SetAnchor(pos + 1);
                // Remove the just-inserted character
                editor_->DeleteRange(pos - 1, 1);
                return;
            }
        }
    }

    // Auto-close pairs
    for (const auto& pair : kPairs)
    {
        if (char_added == pair.opening)
        {
            // Don't auto-close if next character is alphanumeric
            if (pos < editor_->GetLength())
            {
                char next_char = static_cast<char>(editor_->GetCharAt(pos));
                if (std::isalnum(static_cast<unsigned char>(next_char)))
                {
                    return;
                }
            }
            // Insert closing character
            editor_->InsertText(pos, wxString(pair.closing));
            return;
        }
    }

    // Markdown-specific pairs: ** and __
    if (char_added == '*' && pos >= 2)
    {
        char prev_char = static_cast<char>(editor_->GetCharAt(pos - 2));
        if (prev_char == '*')
        {
            // We just typed the second *, insert closing **
            editor_->InsertText(pos, wxString("**"));
        }
    }
    else if (char_added == '_' && pos >= 2)
    {
        char prev_char = static_cast<char>(editor_->GetCharAt(pos - 2));
        if (prev_char == '_')
        {
            editor_->InsertText(pos, wxString("__"));
        }
    }
}

void EditorPanel::SelectNextOccurrence()
{
    // If no selection, select the word under cursor
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
    {
        // Select the word under cursor
        int word_start = editor_->WordStartPosition(sel_start, true);
        int word_end = editor_->WordEndPosition(sel_start, true);
        if (word_start == word_end)
        {
            return; // no word under cursor
        }
        editor_->SetSelection(word_start, word_end);
        return;
    }

    // Get the selected text
    auto selected = editor_->GetSelectedText().ToStdString();
    if (selected.empty())
    {
        return;
    }

    // Search for the next occurrence after the end of the last selection
    int main_sel = editor_->GetMainSelection();
    int search_from = editor_->GetSelectionNEnd(main_sel);

    editor_->SetTargetStart(search_from);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(wxSTC_FIND_MATCHCASE | wxSTC_FIND_WHOLEWORD);

    int found = editor_->SearchInTarget(wxString::FromUTF8(selected));
    if (found == wxSTC_INVALID_POSITION)
    {
        // Wrap around to beginning
        editor_->SetTargetStart(0);
        editor_->SetTargetEnd(search_from);
        found = editor_->SearchInTarget(wxString::FromUTF8(selected));
    }

    if (found != wxSTC_INVALID_POSITION)
    {
        int match_end = editor_->GetTargetEnd();
        editor_->AddSelection(found, match_end);
        editor_->ScrollRange(found, match_end);
    }
}

void EditorPanel::HighlightWordUnderCursor()
{
    int pos = editor_->GetCurrentPos();
    int word_start = editor_->WordStartPosition(pos, true);
    int word_end = editor_->WordEndPosition(pos, true);

    if (word_start == word_end)
    {
        ClearWordHighlights();
        last_highlighted_word_.clear();
        return;
    }

    auto word = editor_->GetTextRange(word_start, word_end).ToStdString();

    // Avoid re-highlighting the same word
    if (word == last_highlighted_word_)
    {
        return;
    }

    ClearWordHighlights();
    last_highlighted_word_ = word;

    // Skip very short words (single chars flood the view)
    if (word.size() < 2)
    {
        return;
    }

    // Themed indicator color
    auto accent2 = theme_engine().color(core::ThemeColorToken::AccentSecondary);
    editor_->IndicatorSetForeground(kIndicatorWordHighlight, accent2);

    editor_->SetIndicatorCurrent(kIndicatorWordHighlight);

    // Search for all occurrences
    editor_->SetTargetStart(0);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(wxSTC_FIND_MATCHCASE | wxSTC_FIND_WHOLEWORD);

    int safety = 0;
    while (editor_->SearchInTarget(wxString::FromUTF8(word)) != wxSTC_INVALID_POSITION &&
           safety < 10000)
    {
        int start = editor_->GetTargetStart();
        int end = editor_->GetTargetEnd();
        editor_->IndicatorFillRange(start, end - start);

        editor_->SetTargetStart(end);
        editor_->SetTargetEnd(editor_->GetLength());
        ++safety;
    }
}

void EditorPanel::ClearWordHighlights()
{
    editor_->SetIndicatorCurrent(kIndicatorWordHighlight);
    editor_->IndicatorClearRange(0, editor_->GetLength());
}

void EditorPanel::HandleSmartHome()
{
    int pos = editor_->GetCurrentPos();
    int cur_line = editor_->LineFromPosition(pos);
    int line_start = editor_->PositionFromLine(cur_line);
    int line_indent = editor_->GetLineIndentPosition(cur_line);

    if (pos == line_indent)
    {
        // Already at indent position → go to column 0
        editor_->GotoPos(line_start);
    }
    else
    {
        // Go to first non-whitespace character
        editor_->GotoPos(line_indent);
    }
}

void EditorPanel::HighlightTrailingWhitespace()
{
    editor_->SetIndicatorCurrent(kIndicatorTrailingWS);
    editor_->IndicatorClearRange(0, editor_->GetLength());

    auto error_color = theme_engine().color(core::ThemeColorToken::AccentPrimary);
    editor_->IndicatorSetForeground(kIndicatorTrailingWS, error_color);

    int line_count = editor_->GetLineCount();
    int doc_len = editor_->GetLength();
    int current_line = editor_->LineFromPosition(editor_->GetCurrentPos());

    for (int line_idx = 0; line_idx < line_count; ++line_idx)
    {
        // Skip the line the cursor is on (don't distract while typing)
        if (line_idx == current_line)
        {
            continue;
        }

        int line_end_pos = editor_->GetLineEndPosition(line_idx);
        int line_start_pos = editor_->PositionFromLine(line_idx);

        // Scan backwards from line end to find trailing whitespace
        int ws_start = line_end_pos;
        while (ws_start > line_start_pos)
        {
            char prev = static_cast<char>(editor_->GetCharAt(ws_start - 1));
            if (prev == ' ' || prev == '\t')
            {
                --ws_start;
            }
            else
            {
                break;
            }
        }

        if (ws_start < line_end_pos && ws_start + (line_end_pos - ws_start) <= doc_len)
        {
            editor_->IndicatorFillRange(ws_start, line_end_pos - ws_start);
        }
    }
}

void EditorPanel::SetTrailingWhitespace(bool enabled)
{
    trailing_ws_visible_ = enabled;
    if (!enabled)
    {
        editor_->SetIndicatorCurrent(kIndicatorTrailingWS);
        editor_->IndicatorClearRange(0, editor_->GetLength());
    }
}

auto EditorPanel::GetTrailingWhitespace() const -> bool
{
    return trailing_ws_visible_;
}

void EditorPanel::SetAutoTrimTrailingWhitespace(bool enabled)
{
    auto_trim_trailing_ws_ = enabled;
}

auto EditorPanel::GetAutoTrimTrailingWhitespace() const -> bool
{
    return auto_trim_trailing_ws_;
}

void EditorPanel::TrimTrailingWhitespace()
{
    editor_->BeginUndoAction();

    int line_count = editor_->GetLineCount();
    for (int line_idx = line_count - 1; line_idx >= 0; --line_idx)
    {
        int line_end_pos = editor_->GetLineEndPosition(line_idx);
        int line_start_pos = editor_->PositionFromLine(line_idx);

        int ws_start = line_end_pos;
        while (ws_start > line_start_pos)
        {
            char prev = static_cast<char>(editor_->GetCharAt(ws_start - 1));
            if (prev == ' ' || prev == '\t')
            {
                --ws_start;
            }
            else
            {
                break;
            }
        }

        if (ws_start < line_end_pos)
        {
            editor_->DeleteRange(ws_start, line_end_pos - ws_start);
        }
    }

    editor_->EndUndoAction();
}

void EditorPanel::UpdateStickyScrollHeading()
{
    // R15 Fix 14: Scan upward from the first visible line to find the nearest
    // Markdown heading. Store it in sticky_heading_ for potential overlay display.
    int first_visible = editor_->GetFirstVisibleLine();
    sticky_heading_.clear();
    for (int line = first_visible; line >= 0; --line)
    {
        wxString text = editor_->GetLine(line).Trim();
        if (text.StartsWith("# ") || text.StartsWith("## ") || text.StartsWith("### "))
        {
            sticky_heading_ = text.ToStdString();
            break;
        }
    }
}

// ═══════════════════════════════════════════════════════
// Phase 5: Snippets & Session
// ═══════════════════════════════════════════════════════

void EditorPanel::InsertSnippet(const Snippet& snippet)
{
    // Replace $0 cursor placeholder with insertion point
    auto body = snippet.body;
    auto cursor_marker = body.find("$0");

    editor_->BeginUndoAction();

    if (cursor_marker != std::string::npos)
    {
        // Insert text before $0
        auto before = body.substr(0, cursor_marker);
        auto after = body.substr(cursor_marker + 2);

        int insert_pos = editor_->GetCurrentPos();
        editor_->InsertText(insert_pos, wxString::FromUTF8(before + after));

        // Place cursor at $0 position
        int new_pos = insert_pos + static_cast<int>(before.size());
        editor_->SetCurrentPos(new_pos);
        editor_->SetSelection(new_pos, new_pos);
    }
    else
    {
        // No cursor marker, just insert at current position
        int insert_pos = editor_->GetCurrentPos();
        editor_->InsertText(insert_pos, wxString::FromUTF8(body));
        editor_->SetCurrentPos(insert_pos + static_cast<int>(body.size()));
        editor_->SetSelection(insert_pos + static_cast<int>(body.size()),
                              insert_pos + static_cast<int>(body.size()));
    }

    editor_->EndUndoAction();
}

auto EditorPanel::GetSessionState() const -> SessionState
{
    SessionState state;
    state.cursor_position = editor_->GetCurrentPos();
    state.first_visible_line = editor_->GetFirstVisibleLine();
    state.wrap_mode = static_cast<int>(wrap_mode_);
    return state;
}

void EditorPanel::RestoreSessionState(const SessionState& state)
{
    editor_->SetCurrentPos(state.cursor_position);
    editor_->SetSelection(state.cursor_position, state.cursor_position);
    editor_->SetFirstVisibleLine(state.first_visible_line);
    editor_->EnsureCaretVisible();
}

void EditorPanel::RegisterDefaultSnippets()
{
    // R15 Fix 13: Register 8 basic Markdown snippets.
    default_snippets_ = {{"Bold", "**", "**$0**"},
                         {"Italic", "*", "*$0*"},
                         {"Link", "[]", "[$0](url)"},
                         {"Image", "![]", "![$0](url)"},
                         {"Code Block", "```", "```\n$0\n```"},
                         {"Heading 1", "#", "# $0"},
                         {"Bullet List", "-", "- $0"},
                         {"Table", "||", "| Column 1 | Column 2 |\n| --- | --- |\n| $0 | |"}};
}

void EditorPanel::ApplyThemeToEditor()
{
    auto bg = theme_engine().color(core::ThemeColorToken::BgInput);
    auto fg = theme_engine().color(core::ThemeColorToken::TextMain);
    auto muted = theme_engine().color(core::ThemeColorToken::TextMuted);
    auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
    auto accent2 = theme_engine().color(core::ThemeColorToken::AccentSecondary);
    auto panel_bg = theme_engine().color(core::ThemeColorToken::BgPanel);
    auto border = theme_engine().color(core::ThemeColorToken::BorderLight);
    auto border_dark = theme_engine().color(core::ThemeColorToken::BorderDark);
    auto error_color = theme_engine().color(core::ThemeColorToken::ErrorColor);
    auto app_bg = theme_engine().color(core::ThemeColorToken::BgApp);

    // Selection: accent at 25% opacity blended with bg (Item 9: improved alpha)
    auto sel_r = static_cast<unsigned char>((accent.Red() * 64 + bg.Red() * 191) / 255);
    auto sel_g = static_cast<unsigned char>((accent.Green() * 64 + bg.Green() * 191) / 255);
    auto sel_b = static_cast<unsigned char>((accent.Blue() * 64 + bg.Blue() * 191) / 255);
    wxColour sel_bg(sel_r, sel_g, sel_b);

    // Active line: accent at 6% opacity
    auto al_r = static_cast<unsigned char>((accent.Red() * 15 + bg.Red() * 240) / 255);
    auto al_g = static_cast<unsigned char>((accent.Green() * 15 + bg.Green() * 240) / 255);
    auto al_b = static_cast<unsigned char>((accent.Blue() * 15 + bg.Blue() * 240) / 255);
    wxColour active_line_bg(al_r, al_g, al_b);

    // Bracket match: accent_secondary at 30% opacity
    auto bm_r = static_cast<unsigned char>((accent2.Red() * 77 + bg.Red() * 178) / 255);
    auto bm_g = static_cast<unsigned char>((accent2.Green() * 77 + bg.Green() * 178) / 255);
    auto bm_b = static_cast<unsigned char>((accent2.Blue() * 77 + bg.Blue() * 178) / 255);
    wxColour brace_match_bg(bm_r, bm_g, bm_b);

    // Indent guide color: border at 50% opacity
    auto ig_r = static_cast<unsigned char>((border.Red() * 128 + bg.Red() * 127) / 255);
    auto ig_g = static_cast<unsigned char>((border.Green() * 128 + bg.Green() * 127) / 255);
    auto ig_b = static_cast<unsigned char>((border.Blue() * 128 + bg.Blue() * 127) / 255);
    wxColour indent_guide_color(ig_r, ig_g, ig_b);

    // Mono font
    wxFont mono_font(wxFontInfo(font_size_).FaceName("JetBrains Mono").AntiAliased(true));
    if (!mono_font.IsOk())
    {
        mono_font = wxFont(wxFontInfo(font_size_).Family(wxFONTFAMILY_TELETYPE));
    }
    wxFont mono_bold = mono_font.Bold();
    wxFont mono_italic = mono_font.Italic();

    // --- Default style ---
    editor_->StyleSetFont(wxSTC_STYLE_DEFAULT, mono_font);
    editor_->StyleSetBackground(wxSTC_STYLE_DEFAULT, bg);
    editor_->StyleSetForeground(wxSTC_STYLE_DEFAULT, fg);
    editor_->StyleClearAll(); // Propagate default to all styles

    // --- Markdown styles ---
    // Headings — H1–H6 with gradient from accent to accent2 (Phase 2 Item 12 prep)
    auto blend_accent = [&](int ratio) -> wxColour
    {
        auto r = static_cast<unsigned char>((accent.Red() * ratio + accent2.Red() * (100 - ratio)) /
                                            100);
        auto g = static_cast<unsigned char>(
            (accent.Green() * ratio + accent2.Green() * (100 - ratio)) / 100);
        auto b = static_cast<unsigned char>(
            (accent.Blue() * ratio + accent2.Blue() * (100 - ratio)) / 100);
        return wxColour(r, g, b);
    };

    auto set_heading = [&](int style_id, int size_bump, bool bold, int accent_ratio)
    {
        wxFont hfont = bold ? mono_bold : mono_font;
        hfont.SetPointSize(font_size_ + size_bump);
        editor_->StyleSetFont(style_id, hfont);
        editor_->StyleSetForeground(style_id, blend_accent(accent_ratio));
        editor_->StyleSetBackground(style_id, bg);
    };

    set_heading(wxSTC_MARKDOWN_HEADER1, 4, true, 100); // H1: full accent
    set_heading(wxSTC_MARKDOWN_HEADER2, 2, true, 85);  // H2: 85% accent
    set_heading(wxSTC_MARKDOWN_HEADER3, 1, true, 70);  // H3: 70% accent
    set_heading(wxSTC_MARKDOWN_HEADER4, 0, true, 55);  // H4: 55% accent
    set_heading(wxSTC_MARKDOWN_HEADER5, 0, true, 40);  // H5: 40% accent
    set_heading(wxSTC_MARKDOWN_HEADER6, 0, false, 30); // H6: 30% accent, normal weight

    // Bold
    editor_->StyleSetFont(wxSTC_MARKDOWN_STRONG1, mono_bold);
    editor_->StyleSetForeground(wxSTC_MARKDOWN_STRONG1, accent);
    editor_->StyleSetFont(wxSTC_MARKDOWN_STRONG2, mono_bold);
    editor_->StyleSetForeground(wxSTC_MARKDOWN_STRONG2, accent);

    // Italic
    editor_->StyleSetFont(wxSTC_MARKDOWN_EM1, mono_italic);
    editor_->StyleSetForeground(wxSTC_MARKDOWN_EM1, accent2);
    editor_->StyleSetFont(wxSTC_MARKDOWN_EM2, mono_italic);
    editor_->StyleSetForeground(wxSTC_MARKDOWN_EM2, accent2);

    // Code (inline)
    editor_->StyleSetForeground(wxSTC_MARKDOWN_CODE, accent);
    editor_->StyleSetBackground(wxSTC_MARKDOWN_CODE, panel_bg);
    editor_->StyleSetFont(wxSTC_MARKDOWN_CODE, mono_font);

    editor_->StyleSetForeground(wxSTC_MARKDOWN_CODE2, accent);
    editor_->StyleSetBackground(wxSTC_MARKDOWN_CODE2, panel_bg);
    editor_->StyleSetFont(wxSTC_MARKDOWN_CODE2, mono_font);

    // Code block (Item 15: subtle distinct background)
    editor_->StyleSetForeground(wxSTC_MARKDOWN_CODEBK, fg);
    editor_->StyleSetBackground(wxSTC_MARKDOWN_CODEBK, panel_bg);
    editor_->StyleSetFont(wxSTC_MARKDOWN_CODEBK, mono_font);

    // Links — clickable hotspot (Item 14)
    editor_->StyleSetForeground(wxSTC_MARKDOWN_LINK, accent2);
    editor_->StyleSetUnderline(wxSTC_MARKDOWN_LINK, true);
    editor_->StyleSetHotSpot(wxSTC_MARKDOWN_LINK, true);

    // Blockquote
    editor_->StyleSetForeground(wxSTC_MARKDOWN_BLOCKQUOTE, muted);
    editor_->StyleSetFont(wxSTC_MARKDOWN_BLOCKQUOTE, mono_italic);

    // Strikethrough
    editor_->StyleSetForeground(wxSTC_MARKDOWN_STRIKEOUT, muted);

    // Horizontal rule
    editor_->StyleSetForeground(wxSTC_MARKDOWN_HRULE, border);
    editor_->StyleSetBold(wxSTC_MARKDOWN_HRULE, true);

    // List items — accent for markers
    editor_->StyleSetForeground(wxSTC_MARKDOWN_ULIST_ITEM, fg);
    editor_->StyleSetForeground(wxSTC_MARKDOWN_OLIST_ITEM, fg);

    // --- Caret — accent-colored (Item 4) ---
    editor_->SetCaretForeground(accent);
    editor_->SetSelBackground(true, sel_bg);
    editor_->SetSelAlpha(80); // R16 Fix 18: more opaque selection for readability

    // Additional selections (multi-cursor) match primary
    editor_->SetAdditionalSelBackground(sel_bg);
    editor_->SetAdditionalSelAlpha(40);
    editor_->SetAdditionalCaretForeground(accent);

    editor_->SetCaretLineVisible(true);
    editor_->SetCaretLineBackground(active_line_bg);
    editor_->SetCaretLineBackAlpha(40); // R16 Fix 15: subtle current line highlight

    // --- Bracket matching styles ---
    editor_->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, accent2);
    editor_->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, brace_match_bg);
    editor_->StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);

    editor_->StyleSetForeground(wxSTC_STYLE_BRACEBAD, error_color);
    editor_->StyleSetBackground(wxSTC_STYLE_BRACEBAD, bg);
    editor_->StyleSetBold(wxSTC_STYLE_BRACEBAD, true);

    // R16 Fix 19: Bracket match box indicator for visual emphasis
    editor_->IndicatorSetStyle(1, wxSTC_INDIC_BOX);
    editor_->IndicatorSetForeground(1, accent2);

    // --- Line number margin — themed (Items 1, 5, 6) ---
    editor_->StyleSetForeground(wxSTC_STYLE_LINENUMBER, muted);
    editor_->StyleSetBackground(wxSTC_STYLE_LINENUMBER, app_bg);
    editor_->StyleSetFont(wxSTC_STYLE_LINENUMBER, mono_font);

    // --- Indentation guide colors (Item 2) ---
    editor_->StyleSetForeground(wxSTC_STYLE_INDENTGUIDE, indent_guide_color);
    editor_->StyleSetBackground(wxSTC_STYLE_INDENTGUIDE, bg);

    // --- Fold margin colors (Items 3, 6: gutter separator) ---
    if (code_folding_ || gutter_separator_)
    {
        editor_->SetFoldMarginColour(true, app_bg);
        editor_->SetFoldMarginHiColour(true, app_bg);

        // Themed fold markers
        for (int i = wxSTC_MARKNUM_FOLDEREND; i <= wxSTC_MARKNUM_FOLDEROPEN; ++i)
        {
            editor_->MarkerSetForeground(i, app_bg);
            editor_->MarkerSetBackground(i, muted);
        }
    }

    // R16 Fix 21: Edge column uses subtle BorderLight color
    editor_->SetEdgeColour(theme_engine().color(core::ThemeColorToken::BorderLight));

    // --- Whitespace color (Item 8) ---
    editor_->SetWhitespaceForeground(true, indent_guide_color);
    editor_->SetWhitespaceSize(2); // Slightly larger dots

    // Overall background
    SetBackgroundColour(bg);

    // Find bar theme
    if (find_bar_ != nullptr)
    {
        find_bar_->SetBackgroundColour(panel_bg);
        if (find_input_ != nullptr)
        {
            find_input_->SetBackgroundColour(bg);
            find_input_->SetForegroundColour(fg);
        }
        if (replace_input_ != nullptr)
        {
            replace_input_->SetBackgroundColour(bg);
            replace_input_->SetForegroundColour(fg);
        }
        if (match_count_label_ != nullptr)
        {
            match_count_label_->SetForegroundColour(muted);
        }
    }

    editor_->Refresh();
}

void EditorPanel::UpdateLineNumberMargin()
{
    if (show_line_numbers_)
    {
        int line_count = editor_->GetLineCount();
        int digits =
            std::max(kMinGutterDigits, static_cast<int>(std::log10(std::max(1, line_count))) + 1);
        int width = editor_->TextWidth(wxSTC_STYLE_LINENUMBER,
                                       std::string(static_cast<size_t>(digits + 1), '9'));
        editor_->SetMarginWidth(0, width + 8); // R16 Fix 17: extra right padding
        editor_->SetMarginType(0, wxSTC_MARGIN_NUMBER);
    }
    else
    {
        editor_->SetMarginWidth(0, 0);
    }
}

// ═══════════════════════════════════════════════════════
// Event handlers
// ═══════════════════════════════════════════════════════

void EditorPanel::OnEditorChange(wxStyledTextEvent& /*event*/)
{
    // Adaptive debounce: increase delay for large files to reduce CPU pressure
    const int line_count = editor_->GetLineCount();
    const int debounce_ms = (line_count > large_file_threshold_) ? kDebounceMaxMs : kDebounceMs;

    // Restart debounce timer with adaptive delay
    debounce_timer_.Stop();
    debounce_timer_.StartOnce(debounce_ms);

    // Update line number margin width if digits changed
    if (show_line_numbers_)
    {
        UpdateLineNumberMargin();
    }
}

// QoL Item 10: Status Bar Stats -> Moved to DebounceTimer to avoid lag

void EditorPanel::OnEditorUpdateUI(wxStyledTextEvent& /*event*/)
{
    // Publish cursor position
    core::events::CursorPositionChangedEvent evt;
    evt.line = GetCursorLine();
    evt.column = GetCursorColumn();
    evt.selection_length = std::abs(editor_->GetSelectionEnd() - editor_->GetSelectionStart());
    event_bus_.publish(evt);

    // Check bracket matching
    if (bracket_matching_)
    {
        CheckBracketMatch();
    }

    // Phase 2: refresh syntax overlay indicators
    ApplySyntaxOverlays();

    // Phase 3 Item 26: highlight all occurrences of word under cursor
    HighlightWordUnderCursor();

    // QoL Item 10: Status Bar Stats -> Moved to DebounceTimer to avoid lag
    // CalculateAndPublishStats();

    // Phase 3 Item 29: trailing whitespace visualization
    if (trailing_ws_visible_)
    {
        HighlightTrailingWhitespace();
    }

    // Phase 5: Show/hide floating format bar based on selection
    {
        int sel_len = std::abs(editor_->GetSelectionEnd() - editor_->GetSelectionStart());
        if (sel_len > 0)
        {
            // Start debounce timer — 200ms delay to avoid flicker during click-drags
            if (!format_bar_timer_.IsRunning())
            {
                format_bar_timer_.StartOnce(200);
            }
        }
        else
        {
            format_bar_timer_.Stop();
            HideFormatBar();
        }
    }
}

void EditorPanel::OnCharAdded(wxStyledTextEvent& event)
{
    // Phase 3 Item 21: Smart pair completion
    HandleSmartPairCompletion(event.GetKey());

    if (auto_indent_)
    {
        HandleMarkdownAutoIndent(event.GetKey());
    }

    // QoL Item 1: Smart List Continuation
    if (smart_list_continuation_ && (event.GetKey() == '\n' || event.GetKey() == '\r'))
    {
        HandleSmartListContinuation();
    }
}

void EditorPanel::OnKeyDown(wxKeyEvent& event)
{
    auto key = event.GetKeyCode();
    bool cmd = event.CmdDown(); // Cmd on macOS, Ctrl on others

    if (cmd && key == 'F')
    {
        ShowFindBar();
        return;
    }

    if (key == WXK_ESCAPE && find_bar_visible_)
    {
        HideFindBar();
        return;
    }

    if (cmd && key == 'H')
    {
        replace_visible_ = true;
        ShowFindBar();
        if (replace_input_ != nullptr)
        {
            replace_input_->Show(replace_visible_);
            find_bar_->GetSizer()->Layout();
        }
        return;
    }

    if (cmd && key == 'Z')
    {
        if (event.ShiftDown())
        {
            Redo();
        }
        else
        {
            Undo();
        }
        return;
    }

    // Cmd+G: Go to Line (QoL Item 2)
    if (cmd && key == 'G')
    {
        GoToLineDialog();
        return;
    }

    // Cmd+D: Duplicate Line (QoL Item 2)
    if (cmd && key == 'D')
    {
        DuplicateLine();
        return;
    }

    // Alt+Up/Down: Move Line (QoL Item 2)
    if (event.AltDown() && !cmd)
    {
        if (key == WXK_UP)
        {
            MoveLineUp();
            return;
        }
        if (key == WXK_DOWN)
        {
            MoveLineDown();
            return;
        }
    }

    // Cmd+/: Toggle Comment (QoL Item 3)
    if (cmd && (key == '/' || key == '?')) // ? is usually / with shift, but check /
    {
        ToggleLineComment();
        return;
    }

    // Cmd+Shift+I: Insert Date/Time (QoL Item 6)
    if (cmd && event.ShiftDown() && key == 'I')
    {
        InsertDateTime();
        return;
    }

    // Cmd+Shift+K: Delete Line (QoL Item 2)
    if (cmd && event.ShiftDown() && key == 'K')
    {
        DeleteLine();
        return;
    }

    // Cmd+L: toggle line numbers
    if (cmd && key == 'L')
    {
        SetShowLineNumbers(!show_line_numbers_);
        return;
    }

    // Alt+Z: cycle word wrap mode (None → Word → Character → None)
    if (event.AltDown() && key == 'Z' && !cmd)
    {
        switch (wrap_mode_)
        {
            case core::events::WrapMode::None:
                SetWordWrapMode(core::events::WrapMode::Word);
                break;
            case core::events::WrapMode::Word:
                SetWordWrapMode(core::events::WrapMode::Character);
                break;
            case core::events::WrapMode::Character:
                SetWordWrapMode(core::events::WrapMode::None);
                break;
        }
        return;
    }

    // Cmd+Shift+W: toggle whitespace visualization (Item 8)
    if (cmd && event.ShiftDown() && key == 'W')
    {
        SetShowWhitespace(!show_whitespace_);
        return;
    }

    // Cmd+= / Cmd+-: editor zoom (Phase 3 Item 25 prep)
    if (cmd && (key == '=' || key == WXK_NUMPAD_ADD) && !event.ShiftDown())
    {
        SetFontSize(font_size_ + 1);
        return;
    }
    if (cmd && (key == '-' || key == WXK_NUMPAD_SUBTRACT) && !event.ShiftDown())
    {
        if (font_size_ > 8)
        {
            SetFontSize(font_size_ - 1);
        }
        return;
    }
    if (cmd && key == '0' && !event.ShiftDown())
    {
        SetFontSize(kDefaultFontSize);
        return;
    }

    // Cmd+D: select next occurrence (VSCode behavior, Phase 3 Item 22)
    if (cmd && key == 'D' && !event.ShiftDown())
    {
        SelectNextOccurrence();
        return;
    }

    // Cmd+Shift+D: duplicate current line
    if (cmd && event.ShiftDown() && key == 'D')
    {
        DuplicateLine();
        return;
    }

    // Shift+Alt+Down: duplicate current line (VS Code style)
    if (event.ShiftDown() && event.AltDown() && key == WXK_DOWN)
    {
        DuplicateLine();
        return;
    }

    // Zoom controls
    if (cmd && (key == '=' || key == WXK_NUMPAD_ADD || key == '+'))
    {
        editor_->ZoomIn();
        return;
    }
    if (cmd && (key == '-' || key == WXK_NUMPAD_SUBTRACT))
    {
        editor_->ZoomOut();
        return;
    }
    if (cmd && (key == '0' || key == WXK_NUMPAD0))
    {
        editor_->SetZoom(0);
        return;
    }

    // Alt+Up: move line up
    if (event.AltDown() && key == WXK_UP && !cmd)
    {
        MoveLineUp();
        return;
    }

    // Alt+Down: move line down
    if (event.AltDown() && key == WXK_DOWN && !cmd)
    {
        MoveLineDown();
        return;
    }

    // Cmd+Shift+K: delete current line
    if (cmd && event.ShiftDown() && key == 'K')
    {
        DeleteLine();
        return;
    }

    // Cmd+Enter: insert line below
    if (cmd && key == WXK_RETURN && !event.ShiftDown())
    {
        InsertLineBelow();
        return;
    }

    // Cmd+G: go to line dialog
    if (cmd && key == 'G' && !event.ShiftDown())
    {
        GoToLineDialog();
        return;
    }

    // --- Markdown formatting ---

    // Cmd+B: toggle bold (wrap selection in **)
    if (cmd && key == 'B' && !event.ShiftDown())
    {
        ToggleBold();
        return;
    }

    // Cmd+I: toggle italic (wrap selection in *)
    if (cmd && key == 'I' && !event.ShiftDown())
    {
        ToggleItalic();
        return;
    }

    // Cmd+K: insert link template
    if (cmd && key == 'K' && !event.ShiftDown())
    {
        InsertLink();
        return;
    }

    // Cmd+Shift+C: toggle inline code (wrap in backticks)
    if (cmd && event.ShiftDown() && key == 'C')
    {
        ToggleInlineCode();
        return;
    }

    // Phase 3 Item 28: Smart Home key
    if (key == WXK_HOME && !cmd && !event.AltDown())
    {
        HandleSmartHome();
        return;
    }

    event.Skip();
}

void EditorPanel::OnMouseWheel(wxMouseEvent& event)
{
    if (event.CmdDown())
    {
        // Ctrl/Cmd + Wheel for Zoom
        int rotation = event.GetWheelRotation();
        if (rotation > 0)
        {
            editor_->ZoomIn();
        }
        else if (rotation < 0)
        {
            editor_->ZoomOut();
        }
    }
    else
    {
        event.Skip();
    }
}

void EditorPanel::OnDebounceTimer(wxTimerEvent& /*event*/)
{
    core::events::EditorContentChangedEvent evt;
    evt.content = GetContent();
    event_bus_.publish(evt);

    // QoL Item 10: Status Bar Stats
    CalculateAndPublishStats();
}

// ═══════════════════════════════════════════════════════
// Bracket matching
// ═══════════════════════════════════════════════════════

void EditorPanel::CheckBracketMatch()
{
    int pos = editor_->GetCurrentPos();

    // Check character at current position and the one before
    auto is_brace = [](int ch) -> bool
    { return ch == '(' || ch == ')' || ch == '[' || ch == ']' || ch == '{' || ch == '}'; };

    int brace_pos = wxSTC_INVALID_POSITION;
    if (pos > 0 && is_brace(editor_->GetCharAt(pos - 1)))
    {
        brace_pos = pos - 1;
    }
    else if (is_brace(editor_->GetCharAt(pos)))
    {
        brace_pos = pos;
    }

    if (brace_pos != wxSTC_INVALID_POSITION)
    {
        int match_pos = editor_->BraceMatch(brace_pos);
        if (match_pos != wxSTC_INVALID_POSITION)
        {
            editor_->BraceHighlight(brace_pos, match_pos);
        }
        else
        {
            editor_->BraceBadLight(brace_pos);
        }
    }
    else
    {
        // No brace at cursor — clear highlight
        editor_->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
    }
}

// ═══════════════════════════════════════════════════════
// Markdown auto-indent
// ═══════════════════════════════════════════════════════

void EditorPanel::HandleMarkdownAutoIndent(int char_added)
{
    if (char_added != '\n' && char_added != '\r')
    {
        return;
    }

    int cur_line = editor_->GetCurrentLine();
    if (cur_line < 1)
    {
        return;
    }

    // Get the previous line's content
    int prev_line = cur_line - 1;
    wxString prev_text = editor_->GetLine(prev_line);
    std::string prev = prev_text.ToStdString();

    // Remove trailing newline characters
    while (!prev.empty() && (prev.back() == '\n' || prev.back() == '\r'))
    {
        prev.pop_back();
    }

    // Patterns for markdown list continuation
    // Capture leading whitespace + prefix

    // Task list: "  - [ ] " or "  - [x] "
    static const std::regex task_re(R"(^(\s*- \[[ xX]\] )(.*)$)");
    // Unordered list: "  - ", "  * ", "  + "
    static const std::regex ulist_re(R"(^(\s*[-*+] )(.*)$)");
    // Ordered list: "  1. ", "  12. "
    static const std::regex olist_re(R"(^(\s*)(\d+)(\. )(.*)$)");
    // Blockquote: "> " or "> > "
    static const std::regex bquote_re(R"(^(\s*(?:>\s*)+)(.*)$)");

    std::smatch match;
    std::string prefix;
    std::string content;

    if (std::regex_match(prev, match, task_re))
    {
        prefix = match[1].str();
        content = match[2].str();

        if (content.empty())
        {
            // Empty task item — remove the prefix (terminate list)
            int line_start = editor_->PositionFromLine(prev_line);
            int line_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(line_start);
            editor_->SetTargetEnd(line_end);
            editor_->ReplaceTarget("");
            return;
        }

        // Continue with unchecked task
        std::string indent(prefix.size() - 6, ' '); // extract the leading whitespace
        editor_->InsertText(editor_->GetCurrentPos(), wxString::FromUTF8(indent + "- [ ] "));
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(indent.size()) + 6);
    }
    else if (std::regex_match(prev, match, olist_re))
    {
        std::string indent = match[1].str();
        int number = std::stoi(match[2].str());
        std::string dot_space = match[3].str();
        content = match[4].str();

        if (content.empty())
        {
            // Empty ordered item — terminate
            int line_start = editor_->PositionFromLine(prev_line);
            int line_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(line_start);
            editor_->SetTargetEnd(line_end);
            editor_->ReplaceTarget("");
            return;
        }

        // Continue with incremented number
        std::string new_prefix = indent + std::to_string(number + 1) + dot_space;
        editor_->InsertText(editor_->GetCurrentPos(), wxString::FromUTF8(new_prefix));
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(new_prefix.size()));
    }
    else if (std::regex_match(prev, match, ulist_re))
    {
        prefix = match[1].str();
        content = match[2].str();

        if (content.empty())
        {
            // Empty list item — terminate
            int line_start = editor_->PositionFromLine(prev_line);
            int line_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(line_start);
            editor_->SetTargetEnd(line_end);
            editor_->ReplaceTarget("");
            return;
        }

        editor_->InsertText(editor_->GetCurrentPos(), wxString::FromUTF8(prefix));
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(prefix.size()));
    }
    else if (std::regex_match(prev, match, bquote_re))
    {
        prefix = match[1].str();
        content = match[2].str();

        if (content.empty())
        {
            // Empty blockquote line — terminate
            int line_start = editor_->PositionFromLine(prev_line);
            int line_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(line_start);
            editor_->SetTargetEnd(line_end);
            editor_->ReplaceTarget("");
            return;
        }

        editor_->InsertText(editor_->GetCurrentPos(), wxString::FromUTF8(prefix));
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(prefix.size()));
    }
}

// ═══════════════════════════════════════════════════════
// Find helpers
// ═══════════════════════════════════════════════════════

void EditorPanel::FindNext()
{
    auto search = find_input_->GetValue().ToStdString();
    if (search.empty())
    {
        return;
    }

    int flags = match_case_ ? wxSTC_FIND_MATCHCASE : 0;

    auto pos = editor_->GetCurrentPos();
    editor_->SetTargetStart(pos);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(flags);

    int found = editor_->SearchInTarget(wxString::FromUTF8(search));
    if (found == wxSTC_INVALID_POSITION)
    {
        // Wrap around
        editor_->SetTargetStart(0);
        editor_->SetTargetEnd(pos);
        found = editor_->SearchInTarget(wxString::FromUTF8(search));
    }

    if (found != wxSTC_INVALID_POSITION)
    {
        editor_->SetSelection(editor_->GetTargetStart(), editor_->GetTargetEnd());
        editor_->EnsureCaretVisible();
    }
}

void EditorPanel::FindPrevious()
{
    auto search = find_input_->GetValue().ToStdString();
    if (search.empty())
    {
        return;
    }

    int flags = match_case_ ? wxSTC_FIND_MATCHCASE : 0;

    auto pos = editor_->GetCurrentPos();
    editor_->SetTargetStart(pos - 1);
    editor_->SetTargetEnd(0);
    editor_->SetSearchFlags(flags);

    int found = editor_->SearchInTarget(wxString::FromUTF8(search));
    if (found == wxSTC_INVALID_POSITION)
    {
        // Wrap around
        editor_->SetTargetStart(editor_->GetLength());
        editor_->SetTargetEnd(pos);
        found = editor_->SearchInTarget(wxString::FromUTF8(search));
    }

    if (found != wxSTC_INVALID_POSITION)
    {
        editor_->SetSelection(editor_->GetTargetStart(), editor_->GetTargetEnd());
        editor_->EnsureCaretVisible();
    }
}

void EditorPanel::ReplaceOne()
{
    auto search = find_input_->GetValue().ToStdString();
    auto replace = replace_input_->GetValue().ToStdString();
    if (search.empty())
    {
        return;
    }

    // If current selection matches search, replace it
    auto sel = editor_->GetSelectedText().ToStdString();
    bool matches =
        match_case_ ? (sel == search) : (wxString(sel).Lower() == wxString(search).Lower());
    if (matches)
    {
        editor_->ReplaceSelection(wxString::FromUTF8(replace));
    }
    FindNext();
}

void EditorPanel::ReplaceAll()
{
    auto search = find_input_->GetValue().ToStdString();
    auto replace = replace_input_->GetValue().ToStdString();
    if (search.empty())
    {
        return;
    }

    int flags = match_case_ ? wxSTC_FIND_MATCHCASE : 0;

    editor_->BeginUndoAction();
    editor_->SetTargetStart(0);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(flags);

    int count = 0;
    while (editor_->SearchInTarget(wxString::FromUTF8(search)) != wxSTC_INVALID_POSITION)
    {
        editor_->ReplaceTarget(wxString::FromUTF8(replace));
        editor_->SetTargetStart(editor_->GetTargetEnd());
        editor_->SetTargetEnd(editor_->GetLength());
        ++count;

        // Safety: prevent infinite loops
        if (count > 100000)
        {
            break;
        }
    }
    editor_->EndUndoAction();

    UpdateMatchCount();
}

void EditorPanel::UpdateMatchCount()
{
    auto search = find_input_->GetValue().ToStdString();
    if (search.empty())
    {
        match_count_label_->SetLabel("");
        return;
    }

    int flags = match_case_ ? wxSTC_FIND_MATCHCASE : 0;
    int count = 0;

    editor_->SetTargetStart(0);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(flags);

    while (editor_->SearchInTarget(wxString::FromUTF8(search)) != wxSTC_INVALID_POSITION)
    {
        ++count;
        editor_->SetTargetStart(editor_->GetTargetEnd());
        editor_->SetTargetEnd(editor_->GetLength());

        if (count > 99999)
        {
            break;
        }
    }

    match_count_label_->SetLabel(wxString::Format("%d found", count));
    find_bar_->GetSizer()->Layout();
}

void EditorPanel::HighlightAllMatches()
{
    ClearFindHighlights();

    auto search = find_input_->GetValue().ToStdString();
    if (search.empty())
    {
        return;
    }

    // Use indicator 0 for find highlights
    editor_->SetIndicatorCurrent(kIndicatorFind);
    editor_->IndicatorSetStyle(kIndicatorFind, wxSTC_INDIC_ROUNDBOX);

    auto accent = theme_engine().color(core::ThemeColorToken::AccentPrimary);
    editor_->IndicatorSetForeground(kIndicatorFind, accent);
    editor_->IndicatorSetAlpha(kIndicatorFind, 60);
    editor_->IndicatorSetOutlineAlpha(kIndicatorFind, 120);

    int flags = match_case_ ? wxSTC_FIND_MATCHCASE : 0;

    editor_->SetTargetStart(0);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(flags);

    int safety = 0;
    while (editor_->SearchInTarget(wxString::FromUTF8(search)) != wxSTC_INVALID_POSITION &&
           safety < 100000)
    {
        auto start = editor_->GetTargetStart();
        auto end = editor_->GetTargetEnd();
        editor_->IndicatorFillRange(start, end - start);

        editor_->SetTargetStart(end);
        editor_->SetTargetEnd(editor_->GetLength());
        ++safety;
    }
}

void EditorPanel::ClearFindHighlights()
{
    editor_->SetIndicatorCurrent(kIndicatorFind);
    editor_->IndicatorClearRange(0, editor_->GetLength());
}

} // namespace markamp::ui

// ═══════════════════════════════════════════════════════
// Line manipulation
// ═══════════════════════════════════════════════════════

namespace markamp::ui
{

void EditorPanel::DuplicateLine()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int cur_line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(cur_line);
    int line_end = editor_->GetLineEndPosition(cur_line);
    auto line_text = editor_->GetTextRange(line_start, line_end);

    editor_->SetTargetStart(line_end);
    editor_->SetTargetEnd(line_end);
    editor_->ReplaceTarget("\n" + line_text);
}

void EditorPanel::MoveLineUp()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int cur_line = editor_->GetCurrentLine();
    if (cur_line <= 0)
    {
        return;
    }

    editor_->BeginUndoAction();

    int line_start = editor_->PositionFromLine(cur_line);
    int line_end = editor_->GetLineEndPosition(cur_line);
    auto line_text = editor_->GetTextRange(line_start, line_end);

    // Delete current line (including preceding newline)
    int delete_start = editor_->GetLineEndPosition(cur_line - 1);
    editor_->SetTargetStart(delete_start);
    editor_->SetTargetEnd(line_end);
    editor_->ReplaceTarget("");

    // Insert before previous line
    int prev_line_start = editor_->PositionFromLine(cur_line - 1);
    editor_->SetTargetStart(prev_line_start);
    editor_->SetTargetEnd(prev_line_start);
    editor_->ReplaceTarget(line_text + "\n");

    // Place cursor on moved line
    editor_->GotoLine(cur_line - 1);

    editor_->EndUndoAction();
}

void EditorPanel::MoveLineDown()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int cur_line = editor_->GetCurrentLine();
    int line_count = editor_->GetLineCount();
    if (cur_line >= line_count - 1)
    {
        return;
    }

    editor_->BeginUndoAction();

    int line_start = editor_->PositionFromLine(cur_line);
    int line_end = editor_->GetLineEndPosition(cur_line);
    auto line_text = editor_->GetTextRange(line_start, line_end);

    // Get next line text
    int next_line_end = editor_->GetLineEndPosition(cur_line + 1);
    auto next_line_text =
        editor_->GetTextRange(editor_->PositionFromLine(cur_line + 1), next_line_end);

    // Replace both lines: swap them
    editor_->SetTargetStart(line_start);
    editor_->SetTargetEnd(next_line_end);
    editor_->ReplaceTarget(next_line_text + "\n" + line_text);

    // Place cursor on moved line
    editor_->GotoLine(cur_line + 1);

    editor_->EndUndoAction();
}

void EditorPanel::DeleteLine()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int cur_line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(cur_line);
    int next_line_start = editor_->PositionFromLine(cur_line + 1);

    // If this is the last line, delete from end of previous line
    if (next_line_start <= line_start && cur_line > 0)
    {
        int prev_line_end = editor_->GetLineEndPosition(cur_line - 1);
        editor_->SetTargetStart(prev_line_end);
        editor_->SetTargetEnd(editor_->GetLineEndPosition(cur_line));
        editor_->ReplaceTarget("");
    }
    else
    {
        editor_->SetTargetStart(line_start);
        editor_->SetTargetEnd(next_line_start);
        editor_->ReplaceTarget("");
    }
}

void EditorPanel::InsertLineBelow()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int line_end = editor_->GetLineEndPosition(editor_->GetCurrentLine());
    editor_->SetTargetStart(line_end);
    editor_->SetTargetEnd(line_end);
    editor_->ReplaceTarget("\n");
    editor_->GotoPos(line_end + 1);
}

void EditorPanel::GoToLineDialog()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int line_count = editor_->GetLineCount();
    long line_num = wxGetNumberFromUser(wxString::Format("Enter line number (1-%d):", line_count),
                                        "Line:",
                                        "Go to Line",
                                        editor_->GetCurrentLine() + 1, // default
                                        1,                             // min
                                        line_count                     // max
    );
    if (line_num > 0)
    {
        editor_->GotoLine(static_cast<int>(line_num - 1));
        editor_->EnsureCaretVisible();
    }
}

// ═══════════════════════════════════════════════════════
// Markdown formatting
// ═══════════════════════════════════════════════════════

void EditorPanel::WrapSelectionWith(const std::string& prefix, const std::string& suffix)
{
    if (editor_ == nullptr)
    {
        return;
    }
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
    {
        // No selection: insert markers at cursor
        editor_->InsertText(sel_start, prefix + suffix);
        editor_->GotoPos(sel_start + static_cast<int>(prefix.length()));
        return;
    }

    auto selected = editor_->GetTextRange(sel_start, sel_end);
    auto selected_str = selected.ToStdString();

    // Check if already wrapped: toggle off
    auto prefix_len = static_cast<int>(prefix.length());
    auto suffix_len = static_cast<int>(suffix.length());

    if (static_cast<int>(selected_str.length()) >= prefix_len + suffix_len &&
        selected_str.substr(0, prefix.length()) == prefix &&
        selected_str.substr(selected_str.length() - suffix.length()) == suffix)
    {
        // Unwrap: remove prefix and suffix
        auto unwrapped = selected_str.substr(
            prefix.length(), selected_str.length() - prefix.length() - suffix.length());
        editor_->SetTargetStart(sel_start);
        editor_->SetTargetEnd(sel_end);
        editor_->ReplaceTarget(unwrapped);
        editor_->SetSelection(sel_start, sel_start + static_cast<int>(unwrapped.length()));
    }
    else
    {
        // Wrap: add prefix and suffix
        editor_->SetTargetStart(sel_start);
        editor_->SetTargetEnd(sel_end);
        editor_->ReplaceTarget(prefix + selected_str + suffix);
        editor_->SetSelection(sel_start + prefix_len,
                              sel_start + prefix_len + static_cast<int>(selected_str.length()));
    }
}

void EditorPanel::ToggleBold()
{
    WrapSelectionWith("**", "**");
}

void EditorPanel::ToggleItalic()
{
    WrapSelectionWith("*", "*");
}

void EditorPanel::InsertLink()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
    {
        // No selection: insert link template
        editor_->InsertText(sel_start, "[text](url)");
        editor_->SetSelection(sel_start + 1, sel_start + 5); // Select "text"
    }
    else
    {
        // Use selection as link text
        auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();
        editor_->SetTargetStart(sel_start);
        editor_->SetTargetEnd(sel_end);
        auto replacement = "[" + selected + "](url)";
        editor_->ReplaceTarget(replacement);
        // Select "url" so user can type the URL
        int url_start = sel_start + static_cast<int>(selected.length()) + 3; // after "]("
        editor_->SetSelection(url_start, url_start + 3);
    }
}

void EditorPanel::ToggleInlineCode()
{
    WrapSelectionWith("`", "`");
}

// ═══════════════════════════════════════════════════════
// QoL: Editor Actions
// ═══════════════════════════════════════════════════════

void EditorPanel::ToggleLineComment()
{
    if (editor_ == nullptr)
        return;

    editor_->BeginUndoAction();

    int start_line = editor_->LineFromPosition(editor_->GetSelectionStart());
    int end_line = editor_->LineFromPosition(editor_->GetSelectionEnd());

    // If selection ends at the start of a line (and spans multiple lines), exclude that last line
    if (start_line < end_line && editor_->PositionFromLine(end_line) == editor_->GetSelectionEnd())
    {
        end_line--;
    }

    for (int line = start_line; line <= end_line; ++line)
    {
        auto line_text = editor_->GetLine(line).ToStdString();
        // Trim newline
        if (!line_text.empty() && line_text.back() == '\n')
            line_text.pop_back();
        if (!line_text.empty() && line_text.back() == '\r')
            line_text.pop_back();

        // Check for existing comment
        // Simple heuristic: starts with <!-- and ends with -->
        // We need to handle leading whitespace
        size_t first_char = line_text.find_first_not_of(" \t");
        if (first_char == std::string::npos)
            continue; // Empty line

        std::string content = line_text.substr(first_char);
        std::string prefix = line_text.substr(0, first_char);

        if (content.rfind("<!-- ", 0) == 0 && content.length() >= 7 &&
            content.substr(content.length() - 4) == " -->")
        {
            // Uncomment
            // <!-- content --> -> content
            std::string uncommented = content.substr(5, content.length() - 9);

            editor_->SetTargetStart(editor_->PositionFromLine(line));
            editor_->SetTargetEnd(editor_->GetLineEndPosition(line));
            editor_->ReplaceTarget(prefix + uncommented);
        }
        else
        {
            // Comment
            // content -> <!-- content -->
            editor_->SetTargetStart(editor_->PositionFromLine(line));
            editor_->SetTargetEnd(editor_->GetLineEndPosition(line));
            editor_->ReplaceTarget(prefix + "<!-- " + content + " -->");
        }
    }

    editor_->EndUndoAction();
}

void EditorPanel::InsertDateTime()
{
    if (editor_ == nullptr)
        return;
    auto now = wxDateTime::Now();
    editor_->ReplaceSelection(now.FormatISOCombined(' '));
}

// ── R13: Zoom + EOL ──
void EditorPanel::ZoomIn()
{
    if (editor_ != nullptr)
    {
        editor_->ZoomIn();
    }
}

void EditorPanel::ZoomOut()
{
    if (editor_ != nullptr)
    {
        editor_->ZoomOut();
    }
}

void EditorPanel::ZoomReset()
{
    if (editor_ != nullptr)
    {
        editor_->SetZoom(0);
    }
}

auto EditorPanel::GetZoomLevel() const -> int
{
    if (editor_ != nullptr)
    {
        return editor_->GetZoom();
    }
    return 0;
}

void EditorPanel::ConvertEolToLf()
{
    if (editor_ != nullptr)
    {
        editor_->ConvertEOLs(wxSTC_EOL_LF);
        editor_->SetEOLMode(wxSTC_EOL_LF);
    }
}

void EditorPanel::ConvertEolToCrlf()
{
    if (editor_ != nullptr)
    {
        editor_->ConvertEOLs(wxSTC_EOL_CRLF);
        editor_->SetEOLMode(wxSTC_EOL_CRLF);
    }
}

void EditorPanel::SortSelectedLines()
{
    if (editor_ == nullptr)
        return;
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int start_line = editor_->LineFromPosition(sel_start);
    int end_line = editor_->LineFromPosition(sel_end);

    if (start_line >= end_line)
        return;

    // If selection end is at start of a line, don't include that line
    if (editor_->PositionFromLine(end_line) == sel_end && end_line > start_line)
    {
        --end_line;
    }

    editor_->BeginUndoAction();

    std::vector<std::string> lines;
    for (int line = start_line; line <= end_line; ++line)
    {
        int line_start = editor_->PositionFromLine(line);
        int line_end = editor_->GetLineEndPosition(line);
        lines.push_back(editor_->GetTextRange(line_start, line_end).ToStdString());
    }
    std::sort(lines.begin(), lines.end());

    // Replace the range
    int range_start = editor_->PositionFromLine(start_line);
    int range_end = editor_->GetLineEndPosition(end_line);
    std::string joined;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
            joined += '\n';
        joined += lines[idx];
    }
    editor_->SetTargetStart(range_start);
    editor_->SetTargetEnd(range_end);
    editor_->ReplaceTarget(joined);
    editor_->SetSelection(range_start, range_start + static_cast<int>(joined.size()));

    editor_->EndUndoAction();
}

void EditorPanel::SortSelectedLinesDesc()
{
    if (editor_ == nullptr)
        return;
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int start_line = editor_->LineFromPosition(sel_start);
    int end_line = editor_->LineFromPosition(sel_end);

    if (start_line >= end_line)
        return;

    if (editor_->PositionFromLine(end_line) == sel_end && end_line > start_line)
    {
        --end_line;
    }

    editor_->BeginUndoAction();

    std::vector<std::string> lines;
    for (int line = start_line; line <= end_line; ++line)
    {
        int line_start = editor_->PositionFromLine(line);
        int line_end = editor_->GetLineEndPosition(line);
        lines.push_back(editor_->GetTextRange(line_start, line_end).ToStdString());
    }
    std::sort(lines.begin(), lines.end(), std::greater<std::string>());

    int range_start = editor_->PositionFromLine(start_line);
    int range_end = editor_->GetLineEndPosition(end_line);
    std::string joined;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
            joined += '\n';
        joined += lines[idx];
    }
    editor_->SetTargetStart(range_start);
    editor_->SetTargetEnd(range_end);
    editor_->ReplaceTarget(joined);
    editor_->SetSelection(range_start, range_start + static_cast<int>(joined.size()));

    editor_->EndUndoAction();
}

auto EditorPanel::GetWordAtCaret() const -> std::string
{
    if (editor_ == nullptr)
        return "";
    int pos = editor_->GetCurrentPos();
    int word_start = editor_->WordStartPosition(pos, true);
    int word_end = editor_->WordEndPosition(pos, true);
    if (word_start >= word_end)
        return "";
    return editor_->GetTextRange(word_start, word_end).ToStdString();
}

void EditorPanel::ConvertSelectionUpperCase()
{
    editor_->UpperCase();
}

void EditorPanel::ConvertSelectionLowerCase()
{
    editor_->LowerCase();
}

// ═══════════════════════════════════════════════════════
// Smart List Continuation & Stats
// ═══════════════════════════════════════════════════════

void EditorPanel::HandleSmartListContinuation()
{
    if (editor_ == nullptr)
        return;

    int cur_line = editor_->GetCurrentLine();
    if (cur_line == 0)
        return;

    // Look at previous line (since we just hit enter, we are on a new empty line)
    // Actually, we are on the new line. The *previous* line contains the list item.
    int prev_line = cur_line - 1;
    auto prev_text = editor_->GetLine(prev_line).ToStdString();

    // Trim newline
    while (!prev_text.empty() && (prev_text.back() == '\n' || prev_text.back() == '\r'))
    {
        prev_text.pop_back();
    }

    std::smatch match;

    // Regex for unordered list: ^(\s*)([-*+])\s+(.*)$
    static const std::regex re_ul(R"(^(\s*)([-*+])\s+(.*)$)");
    // Regex for ordered list: ^(\s*)(\d+)\.(\s+)(.*)$
    static const std::regex re_ol(R"(^(\s*)(\d+)\.(\s+)(.*)$)");
    // Regex for task list: ^(\s*)- \[([ xX])\]\s+(.*)$
    static const std::regex re_task(R"(^(\s*)-\s\[([ xX])\]\s+(.*)$)");

    std::string insertion;

    // Check task list first (subset of unordered)
    if (std::regex_match(prev_text, match, re_task))
    {
        // match[1] = indent, match[2] = x/space, match[3] = content
        if (match[3].length() == 0)
        {
            // Empty task item: user pressed enter twice. Remove the bullet from prev line.
            editor_->BeginUndoAction();
            int prev_start = editor_->PositionFromLine(prev_line);
            int prev_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(prev_start);
            editor_->SetTargetEnd(prev_end);
            editor_->ReplaceTarget("");   // Clear line
            editor_->GotoPos(prev_start); // Go back
            editor_->EndUndoAction();
            return;
        }
        else
        {
            // Continue task list with empty box
            insertion = match[1].str() + "- [ ] ";
        }
    }
    else if (std::regex_match(prev_text, match, re_ul))
    {
        // match[1] = indent, match[2] = bullet, match[3] = content
        if (match[3].length() == 0)
        {
            // Empty item: terminate list
            editor_->BeginUndoAction();
            int prev_start = editor_->PositionFromLine(prev_line);
            int prev_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(prev_start);
            editor_->SetTargetEnd(prev_end);
            editor_->ReplaceTarget("");
            editor_->GotoPos(prev_start);
            editor_->EndUndoAction();
            return;
        }
        else
        {
            insertion = match[1].str() + match[2].str() + " ";
        }
    }
    else if (std::regex_match(prev_text, match, re_ol))
    {
        // match[1] = indent, match[2] = number, match[3] = space, match[4] = content
        if (match[4].length() == 0)
        {
            // Empty item: terminate
            editor_->BeginUndoAction();
            int prev_start = editor_->PositionFromLine(prev_line);
            int prev_end = editor_->GetLineEndPosition(prev_line);
            editor_->SetTargetStart(prev_start);
            editor_->SetTargetEnd(prev_end);
            editor_->ReplaceTarget("");
            editor_->GotoPos(prev_start);
            editor_->EndUndoAction();
            return;
        }
        else
        {
            int num = std::stoi(match[2].str());
            insertion = match[1].str() + std::to_string(num + 1) + "." + match[3].str();
        }
    }

    if (!insertion.empty())
    {
        editor_->InsertText(editor_->GetCurrentPos(), insertion);
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(insertion.length()));
    }
}

void EditorPanel::CalculateAndPublishStats()
{
    if (editor_ == nullptr)
        return;

    core::events::EditorStatsChangedEvent evt;

    evt.char_count = editor_->GetLength();
    evt.line_count = editor_->GetLineCount();

    // Word count calculation
    // Simple iteration or regex. For speed, simpler is better.
    // Scintilla doesn't give word count directly.
    std::string text = editor_->GetText().ToStdString();

    // Simple word count: counting transitions from space to non-space
    int words = 0;
    bool in_word = false;
    for (char c : text)
    {
        bool is_space = std::isspace(static_cast<unsigned char>(c));
        if (!is_space && !in_word)
        {
            in_word = true;
            words++;
        }
        else if (is_space)
        {
            in_word = false;
        }
    }
    evt.word_count = words;

    evt.selection_length = std::abs(editor_->GetSelectionEnd() - editor_->GetSelectionStart());

    event_bus_.publish(evt);
}

// ═══════════════════════════════════════════════════════
// Phase 5: Contextual Inline Markdown Tools
// ═══════════════════════════════════════════════════════

void EditorPanel::InsertBlockquote()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
    {
        // No selection: insert "> " at the start of the current line
        int line = editor_->GetCurrentLine();
        int line_start = editor_->PositionFromLine(line);
        editor_->InsertText(line_start, "> ");
    }
    else
    {
        // Prefix each selected line with "> "
        int start_line = editor_->LineFromPosition(sel_start);
        int end_line = editor_->LineFromPosition(sel_end);
        editor_->BeginUndoAction();
        for (int ln = end_line; ln >= start_line; --ln)
        {
            int pos = editor_->PositionFromLine(ln);
            editor_->InsertText(pos, "> ");
        }
        editor_->EndUndoAction();
    }
}

void EditorPanel::CycleHeading()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int line = editor_->GetCurrentLine();
    auto line_text = editor_->GetLine(line).ToStdString();

    // Count existing # prefix
    int hash_count = 0;
    while (hash_count < static_cast<int>(line_text.size()) &&
           line_text[static_cast<size_t>(hash_count)] == '#')
    {
        ++hash_count;
    }

    int line_start = editor_->PositionFromLine(line);

    editor_->BeginUndoAction();

    // Remove existing heading prefix
    if (hash_count > 0)
    {
        // Remove "# " or "## " etc.
        int remove_len = hash_count;
        if (hash_count < static_cast<int>(line_text.size()) &&
            line_text[static_cast<size_t>(hash_count)] == ' ')
        {
            remove_len++;
        }
        editor_->SetTargetStart(line_start);
        editor_->SetTargetEnd(line_start + remove_len);
        editor_->ReplaceTarget("");
    }

    // Cycle: 0 -> # -> ## -> ### -> (nothing)
    int new_level = (hash_count < 3) ? hash_count + 1 : 0;
    if (new_level > 0)
    {
        std::string prefix(static_cast<size_t>(new_level), '#');
        prefix += ' ';
        editor_->InsertText(line_start, prefix);
    }

    editor_->EndUndoAction();
}

void EditorPanel::InsertTable()
{
    if (editor_ == nullptr)
    {
        return;
    }
    int pos = editor_->GetCurrentPos();
    std::string table_template = "| Column 1 | Column 2 | Column 3 |\n"
                                 "| -------- | -------- | -------- |\n"
                                 "| cell 1   | cell 2   | cell 3   |\n";

    editor_->InsertText(pos, table_template);
    // Position cursor in first cell
    editor_->GotoPos(pos + 2); // after "| "
}

void EditorPanel::SetDocumentBasePath(const std::filesystem::path& base_path)
{
    document_base_path_ = base_path;
}

void EditorPanel::ShowFormatBar()
{
    if (editor_ == nullptr)
    {
        return;
    }

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    if (sel_start == sel_end)
    {
        return; // No selection
    }

    if (format_bar_ == nullptr)
    {
        format_bar_ = new FloatingFormatBar(this,
                                            theme_engine(),
                                            event_bus_,
                                            [this](FloatingFormatBar::Action action)
                                            { HandleFormatBarAction(static_cast<int>(action)); });
    }

    UpdateFormatBarPosition();
    format_bar_->Popup();
}

void EditorPanel::HideFormatBar()
{
    if (format_bar_ != nullptr && format_bar_->IsShown())
    {
        format_bar_->Dismiss();
    }
}

void EditorPanel::UpdateFormatBarPosition()
{
    if (format_bar_ == nullptr || editor_ == nullptr)
    {
        return;
    }

    int sel_start = editor_->GetSelectionStart();
    wxPoint pos = editor_->PointFromPosition(sel_start);

    // Convert to screen coordinates
    wxPoint screen_pos = editor_->ClientToScreen(pos);

    // Position 4px above the selection
    wxSize bar_size = format_bar_->GetSize();
    screen_pos.y -= bar_size.GetHeight() + 4;

    // Clamp to screen bounds
    wxRect screen_rect = wxGetClientDisplayRect();
    if (screen_pos.x + bar_size.GetWidth() > screen_rect.GetRight())
    {
        screen_pos.x = screen_rect.GetRight() - bar_size.GetWidth();
    }
    if (screen_pos.x < screen_rect.GetLeft())
    {
        screen_pos.x = screen_rect.GetLeft();
    }
    if (screen_pos.y < screen_rect.GetTop())
    {
        // If no room above, show below
        screen_pos.y = editor_->ClientToScreen(pos).y + editor_->TextHeight(0) + 4;
    }

    format_bar_->SetPosition(screen_pos);
}

void EditorPanel::HandleFormatBarAction(int action)
{
    auto typed_action = static_cast<FloatingFormatBar::Action>(action);
    switch (typed_action)
    {
        case FloatingFormatBar::Action::Bold:
            ToggleBold();
            break;
        case FloatingFormatBar::Action::Italic:
            ToggleItalic();
            break;
        case FloatingFormatBar::Action::InlineCode:
            ToggleInlineCode();
            break;
        case FloatingFormatBar::Action::Link:
            InsertLink();
            break;
        case FloatingFormatBar::Action::Blockquote:
            InsertBlockquote();
            break;
        case FloatingFormatBar::Action::Heading:
            CycleHeading();
            break;
        case FloatingFormatBar::Action::Table:
            InsertTable();
            break;
    }
}

void EditorPanel::OnFormatBarTimer(wxTimerEvent& /*event*/)
{
    ShowFormatBar();
}

// ── Dwell handlers for link/image preview ──

void EditorPanel::OnDwellStart(wxStyledTextEvent& event)
{
    int pos = event.GetPosition();
    if (pos < 0)
    {
        return;
    }

    // Check for image first (superset pattern: ![alt](path))
    auto image_info = DetectImageAtPosition(pos);
    if (image_info.has_value())
    {
        if (image_popover_ == nullptr)
        {
            image_popover_ = new ImagePreviewPopover(this, theme_engine(), event_bus_);
        }

        // Resolve relative path
        std::filesystem::path img_path(image_info->url);
        if (img_path.is_relative() && !document_base_path_.empty())
        {
            img_path = document_base_path_ / img_path;
        }

        if (image_popover_->SetImage(img_path, image_info->text))
        {
            wxPoint screen_pos = editor_->ClientToScreen(editor_->PointFromPosition(pos));
            screen_pos.y += editor_->TextHeight(0) + 4;
            image_popover_->SetPosition(screen_pos);
            image_popover_->Popup();
        }
        return;
    }

    // Check for link pattern: [text](url)
    auto link_info = DetectLinkAtPosition(pos);
    if (link_info.has_value())
    {
        if (link_popover_ == nullptr)
        {
            link_popover_ = new LinkPreviewPopover(this, theme_engine(), event_bus_);
        }

        link_popover_->SetLink(link_info->text, link_info->url);

        wxPoint screen_pos = editor_->ClientToScreen(editor_->PointFromPosition(pos));
        screen_pos.y += editor_->TextHeight(0) + 4;
        link_popover_->SetPosition(screen_pos);
        link_popover_->Popup();
    }
}

void EditorPanel::OnDwellEnd(wxStyledTextEvent& /*event*/)
{
    if (link_popover_ != nullptr && link_popover_->IsShown())
    {
        link_popover_->Dismiss();
    }
    if (image_popover_ != nullptr && image_popover_->IsShown())
    {
        image_popover_->Dismiss();
    }
}

// ── Link/image/table detection ──

auto EditorPanel::DetectLinkAtPosition(int pos) -> std::optional<LinkInfo>
{
    if (editor_ == nullptr || pos < 0)
    {
        return std::nullopt;
    }

    int line = editor_->LineFromPosition(pos);
    auto line_text = editor_->GetLine(line).ToStdString();
    int col = pos - editor_->PositionFromLine(line);

    // Search for [text](url) pattern containing the cursor position
    std::regex link_re(R"(\[([^\]]*?)\]\(([^\)]+?)\))");
    std::smatch match;
    std::string search_str = line_text;
    int search_offset = 0;

    while (std::regex_search(search_str, match, link_re))
    {
        int match_start = search_offset + static_cast<int>(match.position());
        int match_end = match_start + static_cast<int>(match.length());

        if (col >= match_start && col <= match_end)
        {
            // Don't match image links (they start with !)
            if (match_start > 0 && line_text[static_cast<size_t>(match_start - 1)] == '!')
            {
                search_str = match.suffix().str();
                search_offset = match_end;
                continue;
            }
            return LinkInfo{match[1].str(), match[2].str()};
        }

        search_str = match.suffix().str();
        search_offset = match_end;
    }

    return std::nullopt;
}

auto EditorPanel::DetectImageAtPosition(int pos) -> std::optional<LinkInfo>
{
    if (editor_ == nullptr || pos < 0)
    {
        return std::nullopt;
    }

    int line = editor_->LineFromPosition(pos);
    auto line_text = editor_->GetLine(line).ToStdString();
    int col = pos - editor_->PositionFromLine(line);

    // Search for ![alt](path) pattern
    std::regex image_re(R"(!\[([^\]]*?)\]\(([^\)]+?)\))");
    std::smatch match;
    std::string search_str = line_text;
    int search_offset = 0;

    while (std::regex_search(search_str, match, image_re))
    {
        int match_start = search_offset + static_cast<int>(match.position());
        int match_end = match_start + static_cast<int>(match.length());

        if (col >= match_start && col <= match_end)
        {
            return LinkInfo{match[1].str(), match[2].str()};
        }

        search_str = match.suffix().str();
        search_offset = match_end;
    }

    return std::nullopt;
}

auto EditorPanel::DetectTableAtCursor() -> std::optional<std::pair<int, int>>
{
    if (editor_ == nullptr)
    {
        return std::nullopt;
    }

    int current_line = editor_->GetCurrentLine();
    auto line_text = editor_->GetLine(current_line).ToStdString();

    // Check if current line looks like a table row (starts with |)
    auto trimmed = line_text;
    while (!trimmed.empty() && trimmed.front() == ' ')
    {
        trimmed.erase(trimmed.begin());
    }
    if (trimmed.empty() || trimmed.front() != '|')
    {
        return std::nullopt;
    }

    // Scan upward to find the start of the table
    int start_line = current_line;
    while (start_line > 0)
    {
        auto prev_text = editor_->GetLine(start_line - 1).ToStdString();
        while (!prev_text.empty() && prev_text.front() == ' ')
        {
            prev_text.erase(prev_text.begin());
        }
        if (prev_text.empty() || prev_text.front() != '|')
        {
            break;
        }
        start_line--;
    }

    // Scan downward to find the end of the table
    int end_line = current_line;
    int total_lines = editor_->GetLineCount();
    while (end_line < total_lines - 1)
    {
        auto next_text = editor_->GetLine(end_line + 1).ToStdString();
        while (!next_text.empty() && next_text.front() == ' ')
        {
            next_text.erase(next_text.begin());
        }
        if (next_text.empty() || next_text.front() != '|')
        {
            break;
        }
        end_line++;
    }

    if (start_line == end_line)
    {
        return std::nullopt; // Single line isn't a valid table
    }

    return std::make_pair(start_line, end_line);
}

void EditorPanel::ShowTableEditor()
{
    auto table_range = DetectTableAtCursor();
    if (!table_range.has_value())
    {
        return;
    }

    auto [start_line, end_line] = table_range.value();

    // Collect table lines
    std::vector<std::string> lines;
    for (int ln = start_line; ln <= end_line; ++ln)
    {
        lines.push_back(editor_->GetLine(ln).ToStdString());
    }

    if (table_overlay_ == nullptr)
    {
        table_overlay_ =
            new TableEditorOverlay(this,
                                   theme_engine(),
                                   event_bus_,
                                   [this](const std::string& markdown, int start_ln, int end_ln)
                                   {
                                       // Replace the table lines in the editor
                                       int start_pos = editor_->PositionFromLine(start_ln);
                                       int end_pos = (end_ln + 1 < editor_->GetLineCount())
                                                         ? editor_->PositionFromLine(end_ln + 1)
                                                         : editor_->GetLength();

                                       editor_->BeginUndoAction();
                                       editor_->SetTargetStart(start_pos);
                                       editor_->SetTargetEnd(end_pos);
                                       editor_->ReplaceTarget(markdown);
                                       editor_->EndUndoAction();
                                   });
    }

    if (!table_overlay_->LoadTable(lines, start_line, end_line))
    {
        return;
    }

    // Position overlay over the editor
    auto sizer = GetSizer();
    if (sizer != nullptr)
    {
        table_overlay_->Show();
        sizer->Layout();
    }
}

void EditorPanel::HideTableEditor()
{
    if (table_overlay_ != nullptr)
    {
        table_overlay_->Hide();
    }
}

// ═══════════════════════════════════════════════════════
// Phase 6D: Document Minimap
// ═══════════════════════════════════════════════════════

void EditorPanel::CreateMinimap()
{
    if (minimap_ != nullptr)
    {
        return;
    }

    minimap_ = new wxStyledTextCtrl(this, wxID_ANY);

    // Read-only, no visible chrome
    minimap_->SetReadOnly(true);
    minimap_->SetUseHorizontalScrollBar(false);
    minimap_->SetUseVerticalScrollBar(false);
    minimap_->SetMarginWidth(0, 0);
    minimap_->SetMarginWidth(1, 0);
    minimap_->SetMarginWidth(2, 0);

    // Very small font for overview
    minimap_->StyleSetSize(wxSTC_STYLE_DEFAULT, 1);
    minimap_->StyleClearAll();

    // Fixed width
    minimap_->SetMinSize(wxSize(120, -1));
    minimap_->SetMaxSize(wxSize(120, -1));

    // Disable caret
    minimap_->SetCaretWidth(0);

    // No cursor in minimap
    minimap_->SetCursor(wxCURSOR_HAND);

    // Click handler
    minimap_->Bind(wxEVT_LEFT_DOWN, &EditorPanel::OnMinimapClick, this);

    // Theme: match editor bg/fg
    const auto& theme_colors = theme().colors;
    minimap_->StyleSetBackground(wxSTC_STYLE_DEFAULT, theme_colors.bg_app.to_wx_colour());
    minimap_->StyleSetForeground(wxSTC_STYLE_DEFAULT, theme_colors.text_muted.to_wx_colour());
    minimap_->StyleClearAll();

    // Add to sizer
    auto sizer = GetSizer();
    if (sizer != nullptr)
    {
        sizer->Add(minimap_, 0, wxEXPAND);
        sizer->Layout();
    }

    minimap_->Hide();
}

void EditorPanel::ToggleMinimap()
{
    if (minimap_ == nullptr)
    {
        CreateMinimap();
    }

    minimap_visible_ = !minimap_visible_;

    if (minimap_visible_)
    {
        UpdateMinimapContent();
        minimap_->Show();
    }
    else
    {
        minimap_->Hide();
    }

    auto sizer = GetSizer();
    if (sizer != nullptr)
    {
        sizer->Layout();
    }
}

void EditorPanel::UpdateMinimapContent()
{
    if (minimap_ == nullptr || editor_ == nullptr || !minimap_visible_)
    {
        return;
    }

    auto content = editor_->GetText().ToStdString();

    minimap_->SetReadOnly(false);
    minimap_->SetText(content);
    minimap_->SetReadOnly(true);

    // Scroll minimap proportionally to the editor's scroll position
    int first_line = editor_->GetFirstVisibleLine();
    int total_lines = editor_->GetLineCount();
    int minimap_total = minimap_->GetLineCount();

    if (total_lines > 0)
    {
        int minimap_line = first_line * minimap_total / total_lines;
        minimap_->SetFirstVisibleLine(minimap_line);
    }
}

void EditorPanel::OnMinimapClick(wxMouseEvent& event)
{
    if (minimap_ == nullptr || editor_ == nullptr)
    {
        event.Skip();
        return;
    }

    // Get click position in minimap coordinates
    int click_y = event.GetPosition().y;
    int minimap_height = minimap_->GetClientSize().GetHeight();

    if (minimap_height <= 0)
    {
        event.Skip();
        return;
    }

    // Calculate proportional position
    double fraction = static_cast<double>(click_y) / static_cast<double>(minimap_height);
    int total_lines = editor_->GetLineCount();
    int target_line = static_cast<int>(fraction * total_lines);

    // Scroll editor to the target line, centering it
    int visible_lines = editor_->LinesOnScreen();
    int first_line = std::max(0, target_line - visible_lines / 2);
    editor_->SetFirstVisibleLine(first_line);

    // Move cursor to that line
    int target_pos = editor_->PositionFromLine(target_line);
    editor_->GotoPos(target_pos);
}

// ═══════════════════════════════════════════════════════
// VS Code-Inspired Editor Improvements (20 items)
// ═══════════════════════════════════════════════════════

// #1 Auto-closing brackets/quotes
void EditorPanel::SetAutoClosingBrackets(bool enabled)
{
    auto_closing_brackets_ = enabled;
}

auto EditorPanel::GetAutoClosingBrackets() const -> bool
{
    return auto_closing_brackets_;
}

// #2 Multi-cursor editing
void EditorPanel::AddCursorAbove()
{
    if (editor_ == nullptr)
        return;
    editor_->SetAdditionalSelectionTyping(true);
    editor_->SetMultipleSelection(true);
    int cur_line = editor_->GetCurrentLine();
    int cur_col = editor_->GetColumn(editor_->GetCurrentPos());
    if (cur_line > 0)
    {
        int new_pos = editor_->FindColumn(cur_line - 1, cur_col);
        editor_->AddSelection(new_pos, new_pos);
    }
}

void EditorPanel::AddCursorBelow()
{
    if (editor_ == nullptr)
        return;
    editor_->SetAdditionalSelectionTyping(true);
    editor_->SetMultipleSelection(true);
    int cur_line = editor_->GetCurrentLine();
    int cur_col = editor_->GetColumn(editor_->GetCurrentPos());
    if (cur_line < editor_->GetLineCount() - 1)
    {
        int new_pos = editor_->FindColumn(cur_line + 1, cur_col);
        editor_->AddSelection(new_pos, new_pos);
    }
}

void EditorPanel::AddCursorAtNextOccurrence()
{
    if (editor_ == nullptr)
        return;
    editor_->SetAdditionalSelectionTyping(true);
    editor_->SetMultipleSelection(true);

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    if (sel_start == sel_end)
        return;

    auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();

    // Search forward from current selection end
    editor_->SetTargetStart(sel_end);
    editor_->SetTargetEnd(editor_->GetLength());
    editor_->SetSearchFlags(wxSTC_FIND_MATCHCASE);

    int found = editor_->SearchInTarget(selected);
    if (found >= 0)
    {
        editor_->AddSelection(found, found + static_cast<int>(selected.length()));
    }
}

// #3 Sticky scroll heading
void EditorPanel::SetStickyScrollEnabled(bool enabled)
{
    sticky_scroll_enabled_ = enabled;
}

auto EditorPanel::GetStickyScrollEnabled() const -> bool
{
    return sticky_scroll_enabled_;
}

// #4 Inline color preview decorations
void EditorPanel::SetInlineColorPreview(bool enabled)
{
    inline_color_preview_ = enabled;
}

auto EditorPanel::GetInlineColorPreview() const -> bool
{
    return inline_color_preview_;
}

// #5 Font ligature support
void EditorPanel::SetFontLigatures(bool enabled)
{
    font_ligatures_ = enabled;
    if (editor_ == nullptr)
        return;

#ifdef __WXMSW__
    // On Windows, enable DirectWrite technology for ligature support
    if (enabled)
    {
        editor_->SetTechnology(wxSTC_TECHNOLOGY_DIRECTWRITERETAIN);
    }
    else
    {
        editor_->SetTechnology(wxSTC_TECHNOLOGY_DEFAULT);
    }
#elif defined(__WXOSX__)
    // macOS Core Text supports ligatures natively with the right font
    // The font itself (e.g. Fira Code) controls ligature rendering
    (void)enabled; // Font ligatures work automatically on macOS with compatible fonts
#endif
}

auto EditorPanel::GetFontLigatures() const -> bool
{
    return font_ligatures_;
}

// #6 Smooth caret animation
void EditorPanel::SetSmoothCaret(bool enabled)
{
    smooth_caret_ = enabled;
    if (editor_ == nullptr)
        return;

    if (enabled)
    {
        // Phase-based caret blinking for smoother visual appearance
        editor_->SetCaretPeriod(0); // No blinking = steady smooth caret
        editor_->SetCaretWidth(kCaretWidth);
    }
    else
    {
        editor_->SetCaretPeriod(kCaretBlinkMs);
        editor_->SetCaretWidth(kCaretWidth);
    }
}

auto EditorPanel::GetSmoothCaret() const -> bool
{
    return smooth_caret_;
}

// #7 Current line highlight
void EditorPanel::SetHighlightCurrentLine(bool enabled)
{
    highlight_current_line_ = enabled;
    if (editor_ == nullptr)
        return;

    editor_->SetCaretLineVisible(enabled);

    if (enabled)
    {
        const auto& theme_colors = theme().colors;
        // Subtle highlight slightly brighter/darker than background
        auto bg_color = theme_colors.bg_app.to_wx_colour();
        int delta = 12;
        auto new_r = static_cast<unsigned char>(std::min(255, bg_color.Red() + delta));
        auto new_g = static_cast<unsigned char>(std::min(255, bg_color.Green() + delta));
        auto new_b = static_cast<unsigned char>(std::min(255, bg_color.Blue() + delta));
        editor_->SetCaretLineBackground(wxColour(new_r, new_g, new_b));
        editor_->SetCaretLineBackAlpha(40);
    }
}

auto EditorPanel::GetHighlightCurrentLine() const -> bool
{
    return highlight_current_line_;
}

// #8 Editor font family configuration
void EditorPanel::SetFontFamily(const std::string& family)
{
    font_family_ = family;
    if (editor_ == nullptr)
        return;

    for (int style = 0; style < wxSTC_STYLE_LASTPREDEFINED; ++style)
    {
        editor_->StyleSetFaceName(style, wxString(family));
    }
}

auto EditorPanel::GetFontFamily() const -> std::string
{
    return font_family_;
}

// #9 Auto-save with configurable delay
void EditorPanel::SetAutoSave(bool enabled, int delay_seconds)
{
    auto_save_ = enabled;
    auto_save_delay_seconds_ = std::clamp(delay_seconds, 1, 120);

    if (enabled)
    {
        if (!auto_save_timer_.IsRunning())
        {
            auto_save_timer_.Start(auto_save_delay_seconds_ * 1000);
        }
    }
    else
    {
        auto_save_timer_.Stop();
    }
}

auto EditorPanel::GetAutoSave() const -> bool
{
    return auto_save_;
}

auto EditorPanel::GetAutoSaveDelay() const -> int
{
    return auto_save_delay_seconds_;
}

void EditorPanel::OnAutoSaveTimer(wxTimerEvent& /*event*/)
{
    if (editor_ == nullptr || !auto_save_ || !IsModified())
        return;

    core::events::FileSavedEvent save_evt;
    event_bus_.publish(save_evt);
}

// #10 Insert final newline on save
void EditorPanel::SetInsertFinalNewline(bool enabled)
{
    insert_final_newline_ = enabled;
}

auto EditorPanel::GetInsertFinalNewline() const -> bool
{
    return insert_final_newline_;
}

void EditorPanel::EnsureFinalNewline()
{
    if (editor_ == nullptr || !insert_final_newline_)
        return;

    int length = editor_->GetLength();
    if (length == 0)
        return;

    auto last_char = static_cast<char>(editor_->GetCharAt(length - 1));
    if (last_char != '\n')
    {
        editor_->AppendText("\n");
    }
}

// #11 Whitespace boundary rendering
void EditorPanel::SetWhitespaceBoundary(bool enabled)
{
    whitespace_boundary_ = enabled;
    if (editor_ == nullptr)
        return;

    if (enabled)
    {
        // Show only trailing/leading whitespace (boundary mode)
        editor_->SetViewWhiteSpace(wxSTC_WS_VISIBLEAFTERINDENT);
    }
    else if (show_whitespace_)
    {
        editor_->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS);
    }
    else
    {
        editor_->SetViewWhiteSpace(wxSTC_WS_INVISIBLE);
    }
}

auto EditorPanel::GetWhitespaceBoundary() const -> bool
{
    return whitespace_boundary_;
}

// #12 Markdown link auto-complete
void EditorPanel::SetLinkAutoComplete(bool enabled)
{
    link_auto_complete_ = enabled;
}

auto EditorPanel::GetLinkAutoComplete() const -> bool
{
    return link_auto_complete_;
}

void EditorPanel::SetWorkspaceFiles(const std::vector<std::string>& files)
{
    workspace_files_ = files;
}

void EditorPanel::HandleLinkAutoComplete()
{
    if (editor_ == nullptr || !link_auto_complete_ || workspace_files_.empty())
        return;

    int pos = editor_->GetCurrentPos();
    if (pos < 2)
        return;

    // Check if we just typed "](" — the start of a link URL
    auto prev1 = static_cast<char>(editor_->GetCharAt(pos - 1));
    auto prev2 = static_cast<char>(editor_->GetCharAt(pos - 2));
    if (prev2 == ']' && prev1 == '(')
    {
        // Build auto-completion list from workspace files
        std::string completions;
        for (const auto& file : workspace_files_)
        {
            if (!completions.empty())
            {
                completions += " ";
            }
            completions += file;
        }
        editor_->AutoCompShow(0, completions);
    }
}

// #13 Drag-and-drop file insertion
void EditorPanel::SetDragDropEnabled(bool enabled)
{
    drag_drop_enabled_ = enabled;
    if (editor_ != nullptr)
    {
        editor_->DragAcceptFiles(enabled);
    }
}

auto EditorPanel::GetDragDropEnabled() const -> bool
{
    return drag_drop_enabled_;
}

void EditorPanel::OnFileDrop(wxDropFilesEvent& event)
{
    if (editor_ == nullptr || !drag_drop_enabled_)
        return;

    int count = event.GetNumberOfFiles();
    auto* files = event.GetFiles();

    editor_->BeginUndoAction();
    for (int file_idx = 0; file_idx < count; ++file_idx)
    {
        std::string path = files[file_idx].ToStdString();
        std::string ext = std::filesystem::path(path).extension().string();

        std::string insertion;
        if (ext == ".png" || ext == ".jpg" || ext == ".jpeg" || ext == ".gif" || ext == ".svg" ||
            ext == ".webp")
        {
            // Image: insert ![alt](path)
            std::string filename = std::filesystem::path(path).stem().string();
            insertion = "![" + filename + "](" + path + ")";
        }
        else
        {
            // File: insert [filename](path)
            std::string filename = std::filesystem::path(path).filename().string();
            insertion = "[" + filename + "](" + path + ")";
        }

        editor_->InsertText(editor_->GetCurrentPos(), insertion);
        editor_->GotoPos(editor_->GetCurrentPos() + static_cast<int>(insertion.length()));
    }
    editor_->EndUndoAction();
}

// #14 Word wrap column indicator (ruler)
void EditorPanel::SetShowEdgeColumnRuler(bool enabled)
{
    show_edge_ruler_ = enabled;
    if (editor_ == nullptr)
        return;

    if (enabled)
    {
        editor_->SetEdgeMode(wxSTC_EDGE_LINE);
        editor_->SetEdgeColumn(edge_column_);
        const auto& theme_colors = theme().colors;
        editor_->SetEdgeColour(theme_colors.text_muted.to_wx_colour());
    }
    else
    {
        editor_->SetEdgeMode(wxSTC_EDGE_NONE);
    }
}

auto EditorPanel::GetShowEdgeColumnRuler() const -> bool
{
    return show_edge_ruler_;
}

// #15 Selection count is published via UpdateSelectionCount in EditorStatsChangedEvent
void EditorPanel::UpdateSelectionCount()
{
    if (editor_ == nullptr)
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
        return;

    auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();
    if (selected.empty())
        return;

    // Count occurrences of selected text in document
    int count = 0;
    int search_pos = 0;
    auto full_text = editor_->GetText().ToStdString();
    while (true)
    {
        auto found = full_text.find(selected, static_cast<std::size_t>(search_pos));
        if (found == std::string::npos)
            break;
        count++;
        search_pos = static_cast<int>(found) + 1;
    }

    // Publish the count as part of stats
    if (count > 1)
    {
        core::events::EditorStatsChangedEvent stats_evt;
        stats_evt.selection_length = static_cast<int>(selected.length());
        stats_evt.word_count = count; // Reuse field to communicate occurrence count
        event_bus_.publish(stats_evt);
    }
}

// #16 Go-to-symbol (heading navigation)
auto EditorPanel::GetHeadingSymbols() const -> std::vector<HeadingSymbol>
{
    std::vector<HeadingSymbol> symbols;
    if (editor_ == nullptr)
        return symbols;

    int line_count = editor_->GetLineCount();
    for (int line_num = 0; line_num < line_count; ++line_num)
    {
        auto line_text = editor_->GetLine(line_num).ToStdString();

        // Strip trailing newline
        while (!line_text.empty() && (line_text.back() == '\n' || line_text.back() == '\r'))
        {
            line_text.pop_back();
        }

        // Check for ATX heading (# Heading)
        if (line_text.empty() || line_text[0] != '#')
            continue;

        int level = 0;
        while (level < static_cast<int>(line_text.size()) &&
               line_text[static_cast<std::size_t>(level)] == '#')
        {
            level++;
        }
        if (level > 6)
            continue;

        // Get heading text (after "# ")
        std::string text = line_text.substr(static_cast<std::size_t>(level));
        if (!text.empty() && text[0] == ' ')
        {
            text = text.substr(1);
        }

        symbols.push_back(HeadingSymbol{text, level, line_num});
    }

    return symbols;
}

void EditorPanel::GoToHeading(int line)
{
    if (editor_ == nullptr)
        return;

    int pos = editor_->PositionFromLine(line);
    editor_->GotoPos(pos);
    editor_->EnsureVisibleEnforcePolicy(line);

    // Center the line in the visible area
    int visible_lines = editor_->LinesOnScreen();
    int first_visible = std::max(0, line - visible_lines / 2);
    editor_->SetFirstVisibleLine(first_visible);
}

// #17 Toggle block comment (HTML)
void EditorPanel::ToggleBlockComment()
{
    if (editor_ == nullptr)
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    if (sel_start == sel_end)
    {
        // No selection — wrap current line
        int line = editor_->GetCurrentLine();
        sel_start = editor_->PositionFromLine(line);
        sel_end = editor_->GetLineEndPosition(line);
    }

    auto selected_text = editor_->GetTextRange(sel_start, sel_end).ToStdString();

    editor_->BeginUndoAction();

    // Check if already wrapped in <!-- ... -->
    if (selected_text.size() >= 7 && selected_text.substr(0, 4) == "<!--" &&
        selected_text.substr(selected_text.size() - 3) == "-->")
    {
        // Unwrap: remove <!-- and -->
        std::string inner = selected_text.substr(4, selected_text.size() - 7);
        // Trim leading/trailing space from comment markers
        if (!inner.empty() && inner[0] == ' ')
            inner = inner.substr(1);
        if (!inner.empty() && inner.back() == ' ')
            inner.pop_back();

        editor_->SetTargetStart(sel_start);
        editor_->SetTargetEnd(sel_end);
        editor_->ReplaceTarget(inner);
    }
    else
    {
        // Wrap in <!-- ... -->
        editor_->SetTargetStart(sel_start);
        editor_->SetTargetEnd(sel_end);
        editor_->ReplaceTarget("<!-- " + selected_text + " -->");
    }

    editor_->EndUndoAction();
}

// #18 Smart select (expand / shrink selection)
void EditorPanel::ExpandSelection()
{
    if (editor_ == nullptr)
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();

    // Save current selection for shrink
    selection_stack_.push_back({sel_start, sel_end});

    if (sel_start == sel_end)
    {
        // No selection → select word
        int word_start = editor_->WordStartPosition(sel_start, true);
        int word_end = editor_->WordEndPosition(sel_start, true);
        editor_->SetSelection(word_start, word_end);
    }
    else
    {
        // Check if current selection is a word → expand to line
        int line = editor_->LineFromPosition(sel_start);
        int line_start = editor_->PositionFromLine(line);
        int line_end = editor_->GetLineEndPosition(line);

        if (sel_start > line_start || sel_end < line_end)
        {
            // Expand to full line
            editor_->SetSelection(line_start, line_end);
        }
        else
        {
            // Expand to paragraph (blank-line delimited block)
            int para_start = line;
            while (para_start > 0)
            {
                auto prev_text = editor_->GetLine(para_start - 1).ToStdString();
                if (prev_text.find_first_not_of(" \t\n\r") == std::string::npos)
                    break;
                para_start--;
            }
            int para_end = line;
            int total = editor_->GetLineCount();
            while (para_end < total - 1)
            {
                auto next_text = editor_->GetLine(para_end + 1).ToStdString();
                if (next_text.find_first_not_of(" \t\n\r") == std::string::npos)
                    break;
                para_end++;
            }
            editor_->SetSelection(editor_->PositionFromLine(para_start),
                                  editor_->GetLineEndPosition(para_end));
        }
    }
}

void EditorPanel::ShrinkSelection()
{
    if (editor_ == nullptr || selection_stack_.empty())
        return;

    auto [prev_start, prev_end] = selection_stack_.back();
    selection_stack_.pop_back();
    editor_->SetSelection(prev_start, prev_end);
}

} // namespace markamp::ui

// ══════════════════════════════════════════════════════════════
// Phase 7: UX / Quality-of-Life Improvements (20 items)
// ══════════════════════════════════════════════════════════════

namespace markamp::ui
{

// #1 Cursor surrounding lines — keep N context lines around cursor
void EditorPanel::SetCursorSurroundingLines(int lines)
{
    cursor_surrounding_lines_ = std::clamp(lines, 0, 20);
    if (editor_ == nullptr)
        return;
    // VISIBLE_SLOP keeps 'lines' worth of padding around the caret
    editor_->SetVisiblePolicy(wxSTC_VISIBLE_SLOP | wxSTC_VISIBLE_STRICT, cursor_surrounding_lines_);
    editor_->SetYCaretPolicy(wxSTC_CARET_SLOP | wxSTC_CARET_STRICT | wxSTC_CARET_EVEN,
                             cursor_surrounding_lines_);
}

auto EditorPanel::GetCursorSurroundingLines() const -> int
{
    return cursor_surrounding_lines_;
}

// #2 Scroll beyond last line — allow scrolling past EOF
void EditorPanel::SetScrollBeyondLastLine(bool enabled)
{
    scroll_beyond_last_line_ = enabled;
    if (editor_ == nullptr)
        return;
    editor_->SetEndAtLastLine(!enabled);
}

auto EditorPanel::GetScrollBeyondLastLine() const -> bool
{
    return scroll_beyond_last_line_;
}

// #3 Smooth scrolling — animated scroll transitions
void EditorPanel::SetSmoothScrolling(bool enabled)
{
    smooth_scrolling_ = enabled;
    // Scintilla doesn't have native smooth scrolling;
    // we enable scroll-width tracking for best available smooth behavior
    if (editor_ == nullptr)
        return;
    editor_->SetScrollWidthTracking(enabled);
}

auto EditorPanel::GetSmoothScrolling() const -> bool
{
    return smooth_scrolling_;
}

// #4 Copy line (empty selection) — Ctrl+C with no selection copies whole line
void EditorPanel::CopyLineIfNoSelection()
{
    if (editor_ == nullptr)
        return;
    if (editor_->GetSelectionEmpty())
    {
        editor_->LineCopy();
    }
    else
    {
        editor_->Copy();
    }
}

void EditorPanel::SetEmptySelectionClipboard(bool enabled)
{
    empty_selection_clipboard_ = enabled;
}

auto EditorPanel::GetEmptySelectionClipboard() const -> bool
{
    return empty_selection_clipboard_;
}

// #5 Join lines — merge selected lines into one
void EditorPanel::JoinLines()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int start_line = editor_->LineFromPosition(sel_start);
    int end_line = editor_->LineFromPosition(sel_end);

    // If nothing selected, join current line with next
    if (start_line == end_line && end_line < editor_->GetLineCount() - 1)
    {
        end_line = start_line + 1;
    }

    // Work backwards to preserve positions
    for (int line = end_line; line > start_line; --line)
    {
        int line_start = editor_->PositionFromLine(line);
        // Remove leading whitespace on the joined line
        int pos = line_start;
        while (pos < editor_->GetLength())
        {
            auto ch = static_cast<char>(editor_->GetCharAt(pos));
            if (ch != ' ' && ch != '\t')
                break;
            ++pos;
        }
        // Also remove the newline at end of previous line
        int prev_line_end = editor_->GetLineEndPosition(line - 1);
        editor_->SetTargetStart(prev_line_end);
        editor_->SetTargetEnd(pos);
        editor_->ReplaceTarget(" ");
    }

    editor_->EndUndoAction();
}

// #6 Reverse selected lines — reverse line order in selection
void EditorPanel::ReverseSelectedLines()
{
    if (editor_ == nullptr)
        return;
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int start_line = editor_->LineFromPosition(sel_start);
    int end_line = editor_->LineFromPosition(sel_end);

    if (start_line >= end_line)
        return;

    // If selection end is at start of a line, don't include that line
    if (editor_->PositionFromLine(end_line) == sel_end && end_line > start_line)
    {
        --end_line;
    }

    editor_->BeginUndoAction();

    std::vector<std::string> lines;
    for (int line = start_line; line <= end_line; ++line)
    {
        int line_start = editor_->PositionFromLine(line);
        int line_end = editor_->GetLineEndPosition(line);
        lines.push_back(editor_->GetTextRange(line_start, line_end).ToStdString());
    }
    std::reverse(lines.begin(), lines.end());

    // Replace the range
    int range_start = editor_->PositionFromLine(start_line);
    int range_end = editor_->GetLineEndPosition(end_line);
    std::string joined;
    for (size_t idx = 0; idx < lines.size(); ++idx)
    {
        if (idx > 0)
            joined += '\n';
        joined += lines[idx];
    }
    editor_->SetTargetStart(range_start);
    editor_->SetTargetEnd(range_end);
    editor_->ReplaceTarget(joined);

    editor_->EndUndoAction();
}

// #7 Delete duplicate lines — remove duplicates from selection
void EditorPanel::DeleteDuplicateLines()
{
    if (editor_ == nullptr)
        return;
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int start_line = editor_->LineFromPosition(sel_start);
    int end_line = editor_->LineFromPosition(sel_end);

    if (start_line >= end_line)
        return;

    if (editor_->PositionFromLine(end_line) == sel_end && end_line > start_line)
    {
        --end_line;
    }

    editor_->BeginUndoAction();

    std::vector<std::string> lines;
    std::vector<std::string> unique_lines;
    for (int line = start_line; line <= end_line; ++line)
    {
        int line_start = editor_->PositionFromLine(line);
        int line_end = editor_->GetLineEndPosition(line);
        auto text = editor_->GetTextRange(line_start, line_end).ToStdString();
        lines.push_back(text);
    }

    // Remove duplicates preserving order
    std::set<std::string> seen;
    for (const auto& line : lines)
    {
        if (seen.insert(line).second)
        {
            unique_lines.push_back(line);
        }
    }

    if (unique_lines.size() < lines.size())
    {
        int range_start = editor_->PositionFromLine(start_line);
        int range_end = editor_->GetLineEndPosition(end_line);
        std::string joined;
        for (size_t idx = 0; idx < unique_lines.size(); ++idx)
        {
            if (idx > 0)
                joined += '\n';
            joined += unique_lines[idx];
        }
        editor_->SetTargetStart(range_start);
        editor_->SetTargetEnd(range_end);
        editor_->ReplaceTarget(joined);
    }

    editor_->EndUndoAction();
}

// #8 Transpose characters — swap two characters around cursor
void EditorPanel::TransposeCharacters()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();
    if (pos < 1 || pos >= editor_->GetLength())
        return;

    editor_->BeginUndoAction();
    auto ch_before = static_cast<char>(editor_->GetCharAt(pos - 1));
    auto ch_after = static_cast<char>(editor_->GetCharAt(pos));
    editor_->SetTargetStart(pos - 1);
    editor_->SetTargetEnd(pos + 1);
    std::string swapped;
    swapped += ch_after;
    swapped += ch_before;
    editor_->ReplaceTarget(swapped);
    editor_->SetCurrentPos(pos + 1);
    editor_->SetAnchor(pos + 1);
    editor_->EndUndoAction();
}

// #9 Move selected text left/right — shift selection by one character
void EditorPanel::MoveSelectedTextLeft()
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    if (sel_start <= 0)
        return;

    editor_->BeginUndoAction();
    auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();
    auto char_before = static_cast<char>(editor_->GetCharAt(sel_start - 1));

    editor_->SetTargetStart(sel_start - 1);
    editor_->SetTargetEnd(sel_end);
    std::string replacement = selected + char_before;
    editor_->ReplaceTarget(replacement);
    editor_->SetSelection(sel_start - 1, sel_end - 1);
    editor_->EndUndoAction();
}

void EditorPanel::MoveSelectedTextRight()
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    if (sel_end >= editor_->GetLength())
        return;

    editor_->BeginUndoAction();
    auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();
    auto char_after = static_cast<char>(editor_->GetCharAt(sel_end));

    editor_->SetTargetStart(sel_start);
    editor_->SetTargetEnd(sel_end + 1);
    std::string replacement;
    replacement += char_after;
    replacement += selected;
    editor_->ReplaceTarget(replacement);
    editor_->SetSelection(sel_start + 1, sel_end + 1);
    editor_->EndUndoAction();
}

// #10 Block indent/outdent — Tab/Shift+Tab
void EditorPanel::IndentSelection()
{
    if (editor_ == nullptr)
        return;
    if (editor_->GetSelectionEmpty())
    {
        // No selection: just insert tab
        editor_->Tab();
        return;
    }
    editor_->BeginUndoAction();
    int start_line = editor_->LineFromPosition(editor_->GetSelectionStart());
    int end_line = editor_->LineFromPosition(editor_->GetSelectionEnd());
    for (int line = start_line; line <= end_line; ++line)
    {
        int pos = editor_->PositionFromLine(line);
        std::string indent(static_cast<size_t>(tab_size_), ' ');
        editor_->InsertText(pos, indent);
    }
    editor_->EndUndoAction();
}

void EditorPanel::OutdentSelection()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int start_line = editor_->LineFromPosition(editor_->GetSelectionStart());
    int end_line = editor_->LineFromPosition(editor_->GetSelectionEnd());
    for (int line = start_line; line <= end_line; ++line)
    {
        int pos = editor_->PositionFromLine(line);
        int removed = 0;
        while (removed < tab_size_ && pos + removed < editor_->GetLength())
        {
            auto ch = static_cast<char>(editor_->GetCharAt(pos + removed));
            if (ch == ' ')
                ++removed;
            else if (ch == '\t')
            {
                ++removed;
                break;
            }
            else
                break;
        }
        if (removed > 0)
        {
            editor_->SetTargetStart(pos);
            editor_->SetTargetEnd(pos + removed);
            editor_->ReplaceTarget("");
        }
    }
    editor_->EndUndoAction();
}

// #11 Cursor undo/redo — undo/redo cursor positions independently
void EditorPanel::CursorUndo()
{
    if (editor_ == nullptr || cursor_position_history_.empty())
        return;

    if (cursor_history_index_ < 0)
    {
        cursor_history_index_ = static_cast<int>(cursor_position_history_.size()) - 1;
    }

    if (cursor_history_index_ > 0)
    {
        --cursor_history_index_;
        int pos = cursor_position_history_[static_cast<size_t>(cursor_history_index_)];
        last_recorded_cursor_pos_ = pos; // prevent re-recording
        editor_->GotoPos(pos);
        editor_->EnsureCaretVisible();
    }
}

void EditorPanel::CursorRedo()
{
    if (editor_ == nullptr || cursor_position_history_.empty() || cursor_history_index_ < 0)
        return;

    if (cursor_history_index_ < static_cast<int>(cursor_position_history_.size()) - 1)
    {
        ++cursor_history_index_;
        int pos = cursor_position_history_[static_cast<size_t>(cursor_history_index_)];
        last_recorded_cursor_pos_ = pos; // prevent re-recording
        editor_->GotoPos(pos);
        editor_->EnsureCaretVisible();
    }
}

// #12 Select all occurrences of current word/selection
void EditorPanel::SelectAllOccurrences()
{
    if (editor_ == nullptr)
        return;

    std::string target;
    if (editor_->GetSelectionEmpty())
    {
        // Select word under cursor
        int pos = editor_->GetCurrentPos();
        int word_start = editor_->WordStartPosition(pos, true);
        int word_end = editor_->WordEndPosition(pos, true);
        if (word_start == word_end)
            return;
        target = editor_->GetTextRange(word_start, word_end).ToStdString();
    }
    else
    {
        target = editor_->GetTextRange(editor_->GetSelectionStart(), editor_->GetSelectionEnd())
                     .ToStdString();
    }

    if (target.empty())
        return;

    // Find all occurrences and add them as selections
    editor_->SetSearchFlags(wxSTC_FIND_MATCHCASE | wxSTC_FIND_WHOLEWORD);
    editor_->SetTargetStart(0);
    editor_->SetTargetEnd(editor_->GetLength());

    bool first_selection = true;
    while (editor_->SearchInTarget(target) >= 0)
    {
        int match_start = editor_->GetTargetStart();
        int match_end = editor_->GetTargetEnd();

        if (first_selection)
        {
            editor_->SetSelection(match_start, match_end);
            first_selection = false;
        }
        else
        {
            editor_->AddSelection(match_start, match_end);
        }

        // Move target past this match
        editor_->SetTargetStart(match_end);
        editor_->SetTargetEnd(editor_->GetLength());
    }
}

// #13 Add selection to next find match (incremental multi-cursor)
void EditorPanel::AddSelectionToNextFindMatch()
{
    if (editor_ == nullptr)
        return;

    std::string target;
    if (editor_->GetSelectionEmpty())
    {
        int pos = editor_->GetCurrentPos();
        int word_start = editor_->WordStartPosition(pos, true);
        int word_end = editor_->WordEndPosition(pos, true);
        if (word_start == word_end)
            return;
        target = editor_->GetTextRange(word_start, word_end).ToStdString();
        editor_->SetSelection(word_start, word_end);
        return; // First invocation: just select current word
    }

    target = editor_->GetTextRange(editor_->GetSelectionStart(), editor_->GetSelectionEnd())
                 .ToStdString();

    if (target.empty())
        return;

    // Find next occurrence after the main selection
    int main_sel_end = editor_->GetSelectionEnd();
    editor_->SetSearchFlags(wxSTC_FIND_MATCHCASE);
    editor_->SetTargetStart(main_sel_end);
    editor_->SetTargetEnd(editor_->GetLength());

    if (editor_->SearchInTarget(target) >= 0)
    {
        int match_start = editor_->GetTargetStart();
        int match_end = editor_->GetTargetEnd();
        editor_->AddSelection(match_start, match_end);
    }
    else
    {
        // Wrap around to beginning of document
        editor_->SetTargetStart(0);
        editor_->SetTargetEnd(main_sel_end);
        if (editor_->SearchInTarget(target) >= 0)
        {
            int match_start = editor_->GetTargetStart();
            int match_end = editor_->GetTargetEnd();
            editor_->AddSelection(match_start, match_end);
        }
    }
}

// #14 Toggle word wrap via keyboard (Alt+Z)
void EditorPanel::ToggleWordWrap()
{
    if (editor_ == nullptr)
        return;

    if (wrap_mode_ == core::events::WrapMode::None)
    {
        SetWordWrapMode(core::events::WrapMode::Word);
    }
    else
    {
        SetWordWrapMode(core::events::WrapMode::None);
    }

    // Publish status update
    core::events::SettingChangedEvent setting_evt(
        "editor.wordWrap", wrap_mode_ == core::events::WrapMode::Word ? "true" : "false");
    event_bus_.publish(setting_evt);
}

// #19 Auto-pair markdown emphasis — wrap selection in *, **, or `
void EditorPanel::AutoPairEmphasis(char emphasis_char)
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    auto selected = editor_->GetTextRange(sel_start, sel_end).ToStdString();

    std::string prefix;
    std::string suffix;
    prefix += emphasis_char;
    suffix += emphasis_char;

    editor_->BeginUndoAction();
    editor_->SetTargetStart(sel_start);
    editor_->SetTargetEnd(sel_end);
    std::string wrapped = prefix + selected + suffix;
    editor_->ReplaceTarget(wrapped);
    // Keep the text selected (excluding the delimiters)
    editor_->SetSelection(sel_start + static_cast<int>(prefix.size()),
                          sel_end + static_cast<int>(prefix.size()));
    editor_->EndUndoAction();
}

// #20 Smart backspace in pairs — delete matching pair
void EditorPanel::SmartBackspace()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();
    if (pos < 1 || pos >= editor_->GetLength())
        return;

    auto ch_before = static_cast<char>(editor_->GetCharAt(pos - 1));
    auto ch_after = static_cast<char>(editor_->GetCharAt(pos));

    // Check if we're between a matching pair
    bool is_pair = (ch_before == '(' && ch_after == ')') || (ch_before == '[' && ch_after == ']') ||
                   (ch_before == '{' && ch_after == '}') || (ch_before == '"' && ch_after == '"') ||
                   (ch_before == '\'' && ch_after == '\'') || (ch_before == '`' && ch_after == '`');

    if (is_pair)
    {
        editor_->BeginUndoAction();
        editor_->SetTargetStart(pos - 1);
        editor_->SetTargetEnd(pos + 1);
        editor_->ReplaceTarget("");
        editor_->EndUndoAction();
    }
    else
    {
        // Normal backspace
        editor_->DeleteBack();
    }
}

// ═══════════════════════════════════════════════════════════════════════════
// Phase 8: 20 More VS Code-Inspired Improvements
// ═══════════════════════════════════════════════════════════════════════════

// #1 Fold current region — collapse the fold region at cursor
void EditorPanel::FoldCurrentRegion()
{
    if (editor_ == nullptr)
        return;
    int line = editor_->GetCurrentLine();
    if (editor_->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG)
    {
        if (editor_->GetFoldExpanded(line))
        {
            editor_->ToggleFold(line);
        }
    }
    else
    {
        // Walk upward to find the parent fold header
        int parent = editor_->GetFoldParent(line);
        if (parent >= 0 && editor_->GetFoldExpanded(parent))
        {
            editor_->ToggleFold(parent);
        }
    }
}

// #2 Unfold current region — expand the fold region at cursor
void EditorPanel::UnfoldCurrentRegion()
{
    if (editor_ == nullptr)
        return;
    int line = editor_->GetCurrentLine();
    if (editor_->GetFoldLevel(line) & wxSTC_FOLDLEVELHEADERFLAG)
    {
        if (!editor_->GetFoldExpanded(line))
        {
            editor_->ToggleFold(line);
        }
    }
    else
    {
        int parent = editor_->GetFoldParent(line);
        if (parent >= 0 && !editor_->GetFoldExpanded(parent))
        {
            editor_->ToggleFold(parent);
        }
    }
}

// #3 Fold all regions — collapse every fold header
void EditorPanel::FoldAllRegions()
{
    if (editor_ == nullptr)
        return;
    int line_count = editor_->GetLineCount();
    for (int i = 0; i < line_count; ++i)
    {
        if ((editor_->GetFoldLevel(i) & wxSTC_FOLDLEVELHEADERFLAG) && editor_->GetFoldExpanded(i))
        {
            editor_->ToggleFold(i);
        }
    }
}

// #4 Unfold all regions — expand every fold header
void EditorPanel::UnfoldAllRegions()
{
    if (editor_ == nullptr)
        return;
    int line_count = editor_->GetLineCount();
    for (int i = 0; i < line_count; ++i)
    {
        if ((editor_->GetFoldLevel(i) & wxSTC_FOLDLEVELHEADERFLAG) && !editor_->GetFoldExpanded(i))
        {
            editor_->ToggleFold(i);
        }
    }
}

// #5 Expand line selection — select the entire current line (Ctrl+L in VS Code)
void EditorPanel::ExpandLineSelection()
{
    if (editor_ == nullptr)
        return;
    int line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(line);
    int line_end = (line + 1 < editor_->GetLineCount()) ? editor_->PositionFromLine(line + 1)
                                                        : editor_->GetLength();
    editor_->SetSelection(line_start, line_end);
}

// #6 Delete current line — remove the line the cursor is on
void EditorPanel::DeleteCurrentLine()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(line);
    int line_end = (line + 1 < editor_->GetLineCount()) ? editor_->PositionFromLine(line + 1)
                                                        : editor_->GetLength();
    // If last line, also remove the preceding newline
    if (line + 1 >= editor_->GetLineCount() && line > 0)
    {
        line_start = editor_->GetLineEndPosition(line - 1);
    }
    editor_->SetTargetStart(line_start);
    editor_->SetTargetEnd(line_end);
    editor_->ReplaceTarget("");
    editor_->EndUndoAction();
}

// #7 Toggle render whitespace — cycle between none, boundary, all
void EditorPanel::ToggleRenderWhitespace()
{
    if (editor_ == nullptr)
        return;
    int current = editor_->GetViewWhiteSpace();
    if (current == wxSTC_WS_INVISIBLE)
    {
        editor_->SetViewWhiteSpace(wxSTC_WS_VISIBLEAFTERINDENT); // boundary
        show_whitespace_ = true;
    }
    else if (current == wxSTC_WS_VISIBLEAFTERINDENT)
    {
        editor_->SetViewWhiteSpace(wxSTC_WS_VISIBLEALWAYS); // all
    }
    else
    {
        editor_->SetViewWhiteSpace(wxSTC_WS_INVISIBLE); // none
        show_whitespace_ = false;
    }
}

// #8 Toggle line numbers — show/hide the line number gutter
void EditorPanel::ToggleLineNumbers()
{
    if (editor_ == nullptr)
        return;
    show_line_numbers_ = !show_line_numbers_;
    if (show_line_numbers_)
    {
        UpdateLineNumberMargin();
    }
    else
    {
        editor_->SetMarginWidth(0, 0);
    }
}

// #9 Jump to matching bracket — navigate to the matching bracket
void EditorPanel::JumpToMatchingBracket()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();

    // Try the character at pos and pos-1
    int match = editor_->BraceMatch(pos);
    if (match == wxSTC_INVALID_POSITION && pos > 0)
    {
        match = editor_->BraceMatch(pos - 1);
    }
    if (match != wxSTC_INVALID_POSITION)
    {
        editor_->GotoPos(match);
        editor_->EnsureCaretVisible();
    }
}

// #10 Select to matching bracket — select from cursor to matching bracket
void EditorPanel::SelectToMatchingBracket()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();

    int brace_pos = pos;
    int match = editor_->BraceMatch(pos);
    if (match == wxSTC_INVALID_POSITION && pos > 0)
    {
        brace_pos = pos - 1;
        match = editor_->BraceMatch(brace_pos);
    }
    if (match != wxSTC_INVALID_POSITION)
    {
        // Select including both brackets
        int sel_start = std::min(brace_pos, match);
        int sel_end = std::max(brace_pos, match) + 1;
        editor_->SetSelection(sel_start, sel_end);
    }
}

// #11 Remove surrounding brackets — delete the bracket pair around cursor
void EditorPanel::RemoveSurroundingBrackets()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();

    // Find the enclosing bracket by checking at pos and pos-1
    int brace_pos = -1;
    int match = -1;

    // Check various positions around cursor
    for (int check : {pos, pos - 1})
    {
        if (check < 0 || check >= editor_->GetLength())
            continue;
        auto ch = static_cast<char>(editor_->GetCharAt(check));
        if (ch == '(' || ch == '[' || ch == '{' || ch == ')' || ch == ']' || ch == '}')
        {
            int m = editor_->BraceMatch(check);
            if (m != wxSTC_INVALID_POSITION)
            {
                brace_pos = check;
                match = m;
                break;
            }
        }
    }

    if (brace_pos >= 0 && match >= 0)
    {
        editor_->BeginUndoAction();
        // Delete the later position first to preserve earlier position
        int first = std::min(brace_pos, match);
        int second = std::max(brace_pos, match);
        editor_->SetTargetStart(second);
        editor_->SetTargetEnd(second + 1);
        editor_->ReplaceTarget("");
        editor_->SetTargetStart(first);
        editor_->SetTargetEnd(first + 1);
        editor_->ReplaceTarget("");
        editor_->EndUndoAction();
    }
}

// #12 Duplicate selection or current line
void EditorPanel::DuplicateSelectionOrLine()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    if (editor_->GetSelectionEmpty())
    {
        // Duplicate entire current line
        editor_->SelectionDuplicate();
    }
    else
    {
        // Duplicate the selection inline
        std::string selected = editor_->GetSelectedText().ToStdString();
        int sel_end = editor_->GetSelectionEnd();
        editor_->InsertText(sel_end, selected);
        // Select the duplicated text
        editor_->SetSelection(sel_end, sel_end + static_cast<int>(selected.size()));
    }
    editor_->EndUndoAction();
}

// #13 Transform to uppercase
void EditorPanel::TransformToUppercase()
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;
    editor_->BeginUndoAction();
    editor_->UpperCase();
    editor_->EndUndoAction();
}

// #14 Transform to lowercase
void EditorPanel::TransformToLowercase()
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;
    editor_->BeginUndoAction();
    editor_->LowerCase();
    editor_->EndUndoAction();
}

// #15 Transform to title case — capitalize the first letter of each word
void EditorPanel::TransformToTitleCase()
{
    if (editor_ == nullptr || editor_->GetSelectionEmpty())
        return;
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    std::string text = editor_->GetSelectedText().ToStdString();

    bool capitalize_next = true;
    for (auto& ch : text)
    {
        if (std::isalpha(static_cast<unsigned char>(ch)))
        {
            if (capitalize_next)
            {
                ch = static_cast<char>(std::toupper(static_cast<unsigned char>(ch)));
                capitalize_next = false;
            }
            else
            {
                ch = static_cast<char>(std::tolower(static_cast<unsigned char>(ch)));
            }
        }
        else
        {
            capitalize_next = (ch == ' ' || ch == '\t' || ch == '-' || ch == '_');
        }
    }

    editor_->BeginUndoAction();
    editor_->SetTargetStart(sel_start);
    editor_->SetTargetEnd(sel_end);
    editor_->ReplaceTarget(text);
    editor_->SetSelection(sel_start, sel_start + static_cast<int>(text.size()));
    editor_->EndUndoAction();
}

// #16 Sort lines ascending — sort selected lines alphabetically A→Z
void EditorPanel::SortLinesAscending()
{
    if (editor_ == nullptr)
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int first_line = editor_->LineFromPosition(sel_start);
    int last_line = editor_->LineFromPosition(sel_end);

    if (first_line == last_line)
        return; // nothing to sort

    // Collect lines
    std::vector<std::string> lines;
    for (int line = first_line; line <= last_line; ++line)
    {
        int ls = editor_->PositionFromLine(line);
        int le = editor_->GetLineEndPosition(line);
        lines.push_back(editor_->GetTextRange(ls, le).ToStdString());
    }

    std::sort(lines.begin(), lines.end());

    // Rebuild
    std::string joined;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        joined += lines[i];
        if (i + 1 < lines.size())
            joined += '\n';
    }

    editor_->BeginUndoAction();
    int range_start = editor_->PositionFromLine(first_line);
    int range_end = editor_->GetLineEndPosition(last_line);
    editor_->SetTargetStart(range_start);
    editor_->SetTargetEnd(range_end);
    editor_->ReplaceTarget(joined);
    editor_->EndUndoAction();
}

// #17 Sort lines descending — sort selected lines Z→A
void EditorPanel::SortLinesDescending()
{
    if (editor_ == nullptr)
        return;

    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int first_line = editor_->LineFromPosition(sel_start);
    int last_line = editor_->LineFromPosition(sel_end);

    if (first_line == last_line)
        return;

    std::vector<std::string> lines;
    for (int line = first_line; line <= last_line; ++line)
    {
        int ls = editor_->PositionFromLine(line);
        int le = editor_->GetLineEndPosition(line);
        lines.push_back(editor_->GetTextRange(ls, le).ToStdString());
    }

    std::sort(lines.begin(), lines.end(), std::greater<std::string>());

    std::string joined;
    for (size_t i = 0; i < lines.size(); ++i)
    {
        joined += lines[i];
        if (i + 1 < lines.size())
            joined += '\n';
    }

    editor_->BeginUndoAction();
    int range_start = editor_->PositionFromLine(first_line);
    int range_end = editor_->GetLineEndPosition(last_line);
    editor_->SetTargetStart(range_start);
    editor_->SetTargetEnd(range_end);
    editor_->ReplaceTarget(joined);
    editor_->EndUndoAction();
}

// #18 Insert line above — insert a blank line above the current line
void EditorPanel::InsertLineAbove()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(line);
    editor_->InsertText(line_start, "\n");
    editor_->GotoPos(line_start);
    editor_->EndUndoAction();
}

// #19 Trim trailing whitespace now — strip trailing whitespace from all lines
void EditorPanel::TrimTrailingWhitespaceNow()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int line_count = editor_->GetLineCount();
    for (int i = 0; i < line_count; ++i)
    {
        int line_end = editor_->GetLineEndPosition(i);
        int line_start = editor_->PositionFromLine(i);

        // Walk backward from end to find last non-whitespace
        int trim_start = line_end;
        while (trim_start > line_start)
        {
            auto ch = static_cast<char>(editor_->GetCharAt(trim_start - 1));
            if (ch != ' ' && ch != '\t')
                break;
            --trim_start;
        }

        if (trim_start < line_end)
        {
            editor_->SetTargetStart(trim_start);
            editor_->SetTargetEnd(line_end);
            editor_->ReplaceTarget("");
        }
    }
    editor_->EndUndoAction();
}

// #20 Toggle minimap visibility
void EditorPanel::ToggleMinimapVisibility()
{
    if (editor_ == nullptr)
        return;
    minimap_visible_ = !minimap_visible_;
    if (minimap_ != nullptr)
    {
        minimap_->Show(minimap_visible_);
        Layout();
    }
}

// ── Phase 9: 20 More VS Code-Inspired Improvements ──

// #1 Copy line up – duplicate current line and place copy above
void EditorPanel::CopyLineUp()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int line = editor_->GetCurrentLine();
    int col = editor_->GetColumn(editor_->GetCurrentPos());
    wxString text = editor_->GetLine(line);
    // Remove trailing newline if present so we can insert cleanly
    if (text.EndsWith("\n"))
        text.RemoveLast();
    int line_start = editor_->PositionFromLine(line);
    editor_->InsertText(line_start, text + "\n");
    // Keep cursor on original line (now shifted down by 1)
    int new_pos = editor_->FindColumn(line, col);
    editor_->GotoPos(new_pos);
    editor_->EndUndoAction();
}

// #2 Copy line down – duplicate current line and place copy below
void EditorPanel::CopyLineDown()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int line = editor_->GetCurrentLine();
    int col = editor_->GetColumn(editor_->GetCurrentPos());
    wxString text = editor_->GetLine(line);
    if (text.EndsWith("\n"))
        text.RemoveLast();
    int line_end = editor_->GetLineEndPosition(line);
    editor_->InsertText(line_end, "\n" + text);
    // Move cursor to the new copy
    int new_pos = editor_->FindColumn(line + 1, col);
    editor_->GotoPos(new_pos);
    editor_->EndUndoAction();
}

// #3 Delete all left of cursor on current line
void EditorPanel::DeleteAllLeft()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int pos = editor_->GetCurrentPos();
    int line = editor_->GetCurrentLine();
    int line_start = editor_->PositionFromLine(line);
    if (pos > line_start)
    {
        editor_->SetTargetStart(line_start);
        editor_->SetTargetEnd(pos);
        editor_->ReplaceTarget("");
    }
    editor_->EndUndoAction();
}

// #4 Delete all right of cursor on current line
void EditorPanel::DeleteAllRight()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int pos = editor_->GetCurrentPos();
    int line = editor_->GetCurrentLine();
    int line_end = editor_->GetLineEndPosition(line);
    if (pos < line_end)
    {
        editor_->SetTargetStart(pos);
        editor_->SetTargetEnd(line_end);
        editor_->ReplaceTarget("");
    }
    editor_->EndUndoAction();
}

// #5 Add line comment – force-add HTML comment wrapper around each selected line
void EditorPanel::AddLineComment()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int first_line = editor_->LineFromPosition(sel_start);
    int last_line = editor_->LineFromPosition(sel_end);
    if (sel_end == editor_->PositionFromLine(last_line) && last_line > first_line)
        --last_line;

    // Process lines from bottom to top so positions remain stable
    for (int i = last_line; i >= first_line; --i)
    {
        int ls = editor_->PositionFromLine(i);
        int le = editor_->GetLineEndPosition(i);
        wxString line_text = editor_->GetTextRange(ls, le);
        wxString commented = "<!-- " + line_text + " -->";
        editor_->SetTargetStart(ls);
        editor_->SetTargetEnd(le);
        editor_->ReplaceTarget(commented);
    }
    editor_->EndUndoAction();
}

// #6 Remove line comment – strip HTML comment wrapper from each selected line
void EditorPanel::RemoveLineComment()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int sel_start = editor_->GetSelectionStart();
    int sel_end = editor_->GetSelectionEnd();
    int first_line = editor_->LineFromPosition(sel_start);
    int last_line = editor_->LineFromPosition(sel_end);
    if (sel_end == editor_->PositionFromLine(last_line) && last_line > first_line)
        --last_line;

    for (int i = last_line; i >= first_line; --i)
    {
        int ls = editor_->PositionFromLine(i);
        int le = editor_->GetLineEndPosition(i);
        wxString line_text = editor_->GetTextRange(ls, le);
        wxString trimmed = line_text;
        trimmed.Trim(false); // trim leading whitespace for matching
        trimmed.Trim(true);
        if (trimmed.StartsWith("<!-- ") && trimmed.EndsWith(" -->"))
        {
            // Find the <!-- and --> in the original text
            size_t open_pos = static_cast<size_t>(line_text.Find("<!-- "));
            size_t close_pos = static_cast<size_t>(line_text.Find(" -->"));
            if (open_pos != wxString::npos && close_pos != wxString::npos)
            {
                wxString inner = line_text.Mid(open_pos + 5, close_pos - open_pos - 5);
                // Preserve any leading whitespace before the comment
                wxString prefix = line_text.Left(open_pos);
                editor_->SetTargetStart(ls);
                editor_->SetTargetEnd(le);
                editor_->ReplaceTarget(prefix + inner);
            }
        }
    }
    editor_->EndUndoAction();
}

// #7 Toggle auto-indent
void EditorPanel::ToggleAutoIndent()
{
    if (editor_ == nullptr)
        return;
    auto_indent_ = !auto_indent_;
}

// #8 Toggle bracket matching
void EditorPanel::ToggleBracketMatching()
{
    if (editor_ == nullptr)
        return;
    bracket_matching_ = !bracket_matching_;
    if (!bracket_matching_)
    {
        // Clear existing highlight
        editor_->BraceHighlight(wxSTC_INVALID_POSITION, wxSTC_INVALID_POSITION);
    }
}

// #9 Toggle code folding – show/hide fold margin
void EditorPanel::ToggleCodeFolding()
{
    if (editor_ == nullptr)
        return;
    int current_width = editor_->GetMarginWidth(2); // Margin 2 is typically fold margin
    if (current_width > 0)
    {
        editor_->SetMarginWidth(2, 0);
        // Unfold everything when disabling
        for (int i = 0; i < editor_->GetLineCount(); ++i)
        {
            if (!editor_->GetFoldExpanded(i))
                editor_->ToggleFold(i);
        }
    }
    else
    {
        editor_->SetMarginWidth(2, 16);
    }
}

// #10 Toggle indentation guides
void EditorPanel::ToggleIndentationGuides()
{
    if (editor_ == nullptr)
        return;
    indentation_guides_ = !indentation_guides_;
    editor_->SetIndentationGuides(indentation_guides_ ? wxSTC_IV_LOOKBOTH : wxSTC_IV_NONE);
}

// #11 Select word at cursor (like VS Code Ctrl+D without find)
void EditorPanel::SelectWordAtCursor()
{
    if (editor_ == nullptr)
        return;
    int pos = editor_->GetCurrentPos();
    int word_start = editor_->WordStartPosition(pos, true);
    int word_end = editor_->WordEndPosition(pos, true);
    if (word_start < word_end)
    {
        editor_->SetSelection(word_start, word_end);
    }
}

// #12 Select current paragraph (text between blank lines)
void EditorPanel::SelectCurrentParagraph()
{
    if (editor_ == nullptr)
        return;
    int current_line = editor_->GetCurrentLine();
    int total_lines = editor_->GetLineCount();

    // Find start of paragraph (first non-blank line going upward)
    int para_start = current_line;
    while (para_start > 0)
    {
        wxString line = editor_->GetLine(para_start - 1);
        line.Trim(false);
        line.Trim(true);
        if (line.IsEmpty())
            break;
        --para_start;
    }

    // Find end of paragraph (first non-blank line going downward)
    int para_end = current_line;
    while (para_end < total_lines - 1)
    {
        wxString line = editor_->GetLine(para_end + 1);
        line.Trim(false);
        line.Trim(true);
        if (line.IsEmpty())
            break;
        ++para_end;
    }

    int start_pos = editor_->PositionFromLine(para_start);
    int end_pos = editor_->GetLineEndPosition(para_end);
    editor_->SetSelection(start_pos, end_pos);
}

// #13 Toggle read-only mode
void EditorPanel::ToggleReadOnly()
{
    if (editor_ == nullptr)
        return;
    bool currently_readonly = editor_->GetReadOnly();
    editor_->SetReadOnly(!currently_readonly);
}

// #14 Convert indentation to spaces – replace leading tabs with spaces
void EditorPanel::ConvertIndentationToSpaces()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int tab_width = editor_->GetTabWidth();
    wxString spaces(static_cast<size_t>(tab_width), ' ');

    for (int i = 0; i < editor_->GetLineCount(); ++i)
    {
        int ls = editor_->PositionFromLine(i);
        int le = editor_->GetLineEndPosition(i);
        wxString line = editor_->GetTextRange(ls, le);

        // Only replace leading tabs
        wxString new_line;
        bool in_leading = true;
        for (size_t c = 0; c < line.length(); ++c)
        {
            if (in_leading && line[c] == '\t')
            {
                new_line += spaces;
            }
            else
            {
                in_leading = false;
                new_line += line[c];
            }
        }

        if (new_line != line)
        {
            editor_->SetTargetStart(ls);
            editor_->SetTargetEnd(le);
            editor_->ReplaceTarget(new_line);
        }
    }
    editor_->SetUseTabs(false);
    editor_->EndUndoAction();
}

// #20 Convert indentation to tabs – replace leading spaces with tabs
void EditorPanel::ConvertIndentationToTabs()
{
    if (editor_ == nullptr)
        return;
    editor_->BeginUndoAction();
    int tab_width = editor_->GetTabWidth();

    for (int i = 0; i < editor_->GetLineCount(); ++i)
    {
        int ls = editor_->PositionFromLine(i);
        int le = editor_->GetLineEndPosition(i);
        wxString line = editor_->GetTextRange(ls, le);

        // Count leading spaces and replace with tabs
        int leading_spaces = 0;
        for (size_t c = 0; c < line.length(); ++c)
        {
            if (line[c] == ' ')
                ++leading_spaces;
            else
                break;
        }

        if (leading_spaces >= tab_width)
        {
            int num_tabs = leading_spaces / tab_width;
            int remaining = leading_spaces % tab_width;
            wxString new_leading = wxString(static_cast<size_t>(num_tabs), '\t') +
                                   wxString(static_cast<size_t>(remaining), ' ');
            wxString new_line = new_leading + line.Mid(static_cast<size_t>(leading_spaces));

            if (new_line != line)
            {
                editor_->SetTargetStart(ls);
                editor_->SetTargetEnd(le);
                editor_->ReplaceTarget(new_line);
            }
        }
    }
    editor_->SetUseTabs(true);
    editor_->EndUndoAction();
}

// R4 Fix 1: Editor right-click context menu
void EditorPanel::OnRightDown(wxMouseEvent& event)
{
    if (editor_ == nullptr)
    {
        event.Skip();
        return;
    }

    // Position cursor at click if no selection
    if (editor_->GetSelectionStart() == editor_->GetSelectionEnd())
    {
        const auto pos = editor_->PositionFromPoint(event.GetPosition());
        editor_->SetCurrentPos(pos);
        editor_->SetSelection(pos, pos);
    }

    ShowEditorContextMenu();
}

void EditorPanel::ShowEditorContextMenu()
{
    constexpr int kCtxUndo = 200;
    constexpr int kCtxRedo = 201;
    constexpr int kCtxCut = 202;
    constexpr int kCtxCopy = 203;
    constexpr int kCtxPaste = 204;
    constexpr int kCtxSelectAll = 205;
    constexpr int kCtxGoToLine = 206;
    constexpr int kCtxFind = 207;
    constexpr int kCtxWordWrap = 208;
    constexpr int kCtxUppercase = 209;
    constexpr int kCtxLowercase = 210;
    constexpr int kCtxTitleCase = 211;

    wxMenu menu;
    menu.Append(kCtxUndo, "Undo\tCtrl+Z");
    menu.Append(kCtxRedo, "Redo\tCtrl+Shift+Z");
    menu.AppendSeparator();
    menu.Append(kCtxCut, "Cut\tCtrl+X");
    menu.Append(kCtxCopy, "Copy\tCtrl+C");
    menu.Append(kCtxPaste, "Paste\tCtrl+V");
    menu.AppendSeparator();
    menu.Append(kCtxSelectAll, "Select All\tCtrl+A");
    menu.AppendSeparator();

    // R14 Fix 13-14: Go to Line and Find
    menu.Append(kCtxGoToLine, "Go to Line...\tCtrl+G");
    menu.Append(kCtxFind, "Find...\tCtrl+F");
    menu.AppendSeparator();

    // R14 Fix 15: Toggle Word Wrap
    menu.AppendCheckItem(kCtxWordWrap, "Word Wrap");
    menu.Check(kCtxWordWrap, editor_->GetWrapMode() != wxSTC_WRAP_NONE);

    // R14 Fix 16: Format submenu
    auto* format_menu = new wxMenu();
    format_menu->Append(kCtxUppercase, "UPPERCASE");
    format_menu->Append(kCtxLowercase, "lowercase");
    format_menu->Append(kCtxTitleCase, "Title Case");
    menu.AppendSubMenu(format_menu, "Format");

    // R15 Fixes 17-20: Additional context menu items
    menu.AppendSeparator();
    constexpr int kCtxSortLines = 212;
    constexpr int kCtxDuplicateLine = 213;
    constexpr int kCtxDeleteLine = 214;
    constexpr int kCtxToggleComment = 215;
    menu.Append(kCtxSortLines, "Sort Lines");
    menu.Append(kCtxDuplicateLine, "Duplicate Line");
    menu.Append(kCtxDeleteLine, "Delete Line");
    menu.Append(kCtxToggleComment, "Toggle Comment");

    // Enable/disable based on state
    menu.Enable(kCtxUndo, editor_->CanUndo());
    menu.Enable(kCtxRedo, editor_->CanRedo());

    const bool has_selection = editor_->GetSelectionStart() != editor_->GetSelectionEnd();
    menu.Enable(kCtxCut, has_selection);
    menu.Enable(kCtxCopy, has_selection);
    menu.Enable(kCtxPaste, editor_->CanPaste());
    format_menu->Enable(kCtxUppercase, has_selection);
    format_menu->Enable(kCtxLowercase, has_selection);
    format_menu->Enable(kCtxTitleCase, has_selection);

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& cmd_event)
              {
                  switch (cmd_event.GetId())
                  {
                      case 200:
                          editor_->Undo();
                          break;
                      case 201:
                          editor_->Redo();
                          break;
                      case 202:
                          editor_->Cut();
                          break;
                      case 203:
                          editor_->Copy();
                          break;
                      case 204:
                          editor_->Paste();
                          break;
                      case 205:
                          editor_->SelectAll();
                          break;
                      case 206:
                          GoToLineDialog();
                          break;
                      case 207:
                      {
                          core::events::FindRequestEvent evt;
                          event_bus_.publish(evt);
                          break;
                      }
                      case 208:
                          ToggleWordWrap();
                          break;
                      case 209:
                          TransformToUppercase();
                          break;
                      case 210:
                          TransformToLowercase();
                          break;
                      case 211:
                          TransformToTitleCase();
                          break;
                      case 212:
                          SortSelectedLines();
                          break;
                      case 213:
                          DuplicateLine();
                          break;
                      case 214:
                          DeleteCurrentLine();
                          break;
                      case 215:
                          ToggleLineComment();
                          break;
                      default:
                          break;
                  }
              });

    PopupMenu(&menu);
}

} // namespace markamp::ui
