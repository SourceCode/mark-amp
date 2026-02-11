#include "EditorPanel.h"

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

void EditorPanel::OnThemeChanged(const core::Theme& /*new_theme*/)
{
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
    // Phase 3 Item 30: Sticky scroll
    // This requires a custom overlay widget — placeholder for future implementation.
    // The heading tracking logic would scan upward from current visible line
    // to find the nearest markdown heading and display it as a pinned label.
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
    // Phase 5 Item 47: Default markdown snippets
    // These can be registered in the command palette or triggered via shortcuts.
    // The snippet body uses $0 as the cursor placeholder.
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
    editor_->SetSelAlpha(60); // Item 9: slightly translucent selection

    // Additional selections (multi-cursor) match primary
    editor_->SetAdditionalSelBackground(sel_bg);
    editor_->SetAdditionalSelAlpha(40);
    editor_->SetAdditionalCaretForeground(accent);

    // Active line highlight
    editor_->SetCaretLineVisible(true);
    editor_->SetCaretLineBackground(active_line_bg);

    // --- Bracket matching styles ---
    editor_->StyleSetForeground(wxSTC_STYLE_BRACELIGHT, accent2);
    editor_->StyleSetBackground(wxSTC_STYLE_BRACELIGHT, brace_match_bg);
    editor_->StyleSetBold(wxSTC_STYLE_BRACELIGHT, true);

    editor_->StyleSetForeground(wxSTC_STYLE_BRACEBAD, error_color);
    editor_->StyleSetBackground(wxSTC_STYLE_BRACEBAD, bg);
    editor_->StyleSetBold(wxSTC_STYLE_BRACEBAD, true);

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

    // --- Edge column color (Item 10) ---
    editor_->SetEdgeColour(indent_guide_color);

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
        editor_->SetMarginWidth(0, width);
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

void EditorPanel::SortSelectedLines()
{
    // Not bound to key yet, but good for API
    // Implementation left for future or context menu
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

} // namespace markamp::ui
