#include "ThemeTokenEditor.h"

#include "core/Logger.h"

#include <wx/button.h>
#include <wx/colordlg.h>
#include <wx/dcmemory.h>
#include <wx/sizer.h>

namespace markamp::ui
{

// ---------------------------------------------------------------------------
// Construction
// ---------------------------------------------------------------------------

ThemeTokenEditor::ThemeTokenEditor(wxWindow* parent, core::ThemeEngine& engine)
    : wxDialog(parent,
               wxID_ANY,
               "Theme Token Editor",
               wxDefaultPosition,
               wxSize(kDialogWidth, kDialogHeight),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , engine_(engine)
    , working_theme_(engine.current_theme())
{
    working_theme_.sync_layers_from_colors();
    build_ui();
    populate_chrome_tokens();
    refresh_preview();
    Centre();
}

// ---------------------------------------------------------------------------
// UI construction
// ---------------------------------------------------------------------------

void ThemeTokenEditor::build_ui()
{
    auto* root_sizer = new wxBoxSizer(wxVERTICAL);

    // --- Tab notebook ---
    tabs_ = new wxNotebook(this, wxID_ANY);
    tabs_->AddPage(new wxPanel(tabs_), "Chrome");
    tabs_->AddPage(new wxPanel(tabs_), "Syntax");
    tabs_->AddPage(new wxPanel(tabs_), "Render");
    tabs_->Bind(wxEVT_NOTEBOOK_PAGE_CHANGED, &ThemeTokenEditor::on_tab_changed, this);

    // --- Main content: token list + preview ---
    auto* content_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Token list (left side)
    auto* list_parent = new wxPanel(this, wxID_ANY);
    auto* list_sizer = new wxBoxSizer(wxVERTICAL);
    token_list_ = new wxListCtrl(
        list_parent, wxID_ANY, wxDefaultPosition, wxSize(280, -1), wxLC_REPORT | wxLC_SINGLE_SEL);
    token_list_->InsertColumn(0, "Token", wxLIST_FORMAT_LEFT, 180);
    token_list_->InsertColumn(1, "Color", wxLIST_FORMAT_LEFT, 80);
    token_list_->Bind(wxEVT_LIST_ITEM_ACTIVATED, &ThemeTokenEditor::on_token_clicked, this);
    list_sizer->Add(token_list_, 1, wxEXPAND);
    list_parent->SetSizer(list_sizer);

    // Preview panel (right side)
    auto* preview_parent = new wxPanel(this, wxID_ANY);
    preview_panel_ = build_preview_panel(preview_parent);
    auto* preview_sizer = new wxBoxSizer(wxVERTICAL);
    preview_sizer->Add(preview_panel_, 1, wxEXPAND);
    preview_parent->SetSizer(preview_sizer);

    content_sizer->Add(list_parent, 0, wxEXPAND | wxRIGHT, 8);
    content_sizer->Add(preview_parent, 1, wxEXPAND);

    root_sizer->Add(tabs_, 0, wxEXPAND | wxALL, 8);
    root_sizer->Add(content_sizer, 1, wxEXPAND | wxLEFT | wxRIGHT, 8);

    // --- Buttons ---
    auto* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    btn_sizer->AddStretchSpacer();

    auto* apply_btn = new wxButton(this, wxID_APPLY, "Apply");
    auto* save_btn = new wxButton(this, wxID_SAVE, "Save");
    auto* cancel_btn = new wxButton(this, wxID_CANCEL, "Cancel");

    apply_btn->Bind(wxEVT_BUTTON, &ThemeTokenEditor::on_apply, this);
    save_btn->Bind(wxEVT_BUTTON, &ThemeTokenEditor::on_save, this);
    cancel_btn->Bind(wxEVT_BUTTON, &ThemeTokenEditor::on_cancel, this);

    btn_sizer->Add(apply_btn, 0, wxRIGHT, 4);
    btn_sizer->Add(save_btn, 0, wxRIGHT, 4);
    btn_sizer->Add(cancel_btn, 0);

    root_sizer->Add(btn_sizer, 0, wxEXPAND | wxALL, 8);
    SetSizer(root_sizer);
}

void ThemeTokenEditor::build_token_list(wxPanel* /*parent*/, const std::vector<TokenEntry>& entries)
{
    token_list_->DeleteAllItems();
    current_entries_ = entries;

    for (size_t idx = 0; idx < entries.size(); ++idx)
    {
        const auto& entry = entries[idx];
        long item = token_list_->InsertItem(static_cast<long>(idx), entry.label);
        token_list_->SetItem(item, 1, entry.color_ref->to_hex());
    }
}

auto ThemeTokenEditor::build_preview_panel(wxPanel* parent) -> wxPanel*
{
    auto* panel = new wxPanel(parent, wxID_ANY);
    auto* sizer = new wxBoxSizer(wxHORIZONTAL);

    // Sidebar preview
    preview_sidebar_ = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(80, -1));
    preview_sidebar_->SetMinSize(wxSize(80, 100));

    // Right side: editor + rendered
    auto* right_sizer = new wxBoxSizer(wxVERTICAL);
    preview_editor_ = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 100));
    preview_rendered_ = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 100));
    right_sizer->Add(preview_editor_, 1, wxEXPAND | wxBOTTOM, 2);
    right_sizer->Add(preview_rendered_, 1, wxEXPAND);

    sizer->Add(preview_sidebar_, 0, wxEXPAND | wxRIGHT, 2);
    sizer->Add(right_sizer, 1, wxEXPAND);
    panel->SetSizer(sizer);
    return panel;
}

// ---------------------------------------------------------------------------
// Token population
// ---------------------------------------------------------------------------

void ThemeTokenEditor::populate_chrome_tokens()
{
    std::vector<TokenEntry> entries;
    auto& chrome = working_theme_.chrome;

    entries.push_back({"bg_app", core::ThemeColorToken::BgApp, &chrome.bg_app});
    entries.push_back({"bg_panel", core::ThemeColorToken::BgPanel, &chrome.bg_panel});
    entries.push_back({"bg_header", core::ThemeColorToken::BgHeader, &chrome.bg_header});
    entries.push_back({"bg_input", core::ThemeColorToken::BgInput, &chrome.bg_input});
    entries.push_back({"border_light", core::ThemeColorToken::BorderLight, &chrome.border_light});
    entries.push_back({"border_dark", core::ThemeColorToken::BorderDark, &chrome.border_dark});
    entries.push_back(
        {"accent_primary", core::ThemeColorToken::AccentPrimary, &chrome.accent_primary});
    entries.push_back(
        {"accent_secondary", core::ThemeColorToken::AccentSecondary, &chrome.accent_secondary});
    entries.push_back({"list_hover", core::ThemeColorToken::HoverBg, &chrome.list_hover});
    entries.push_back({"list_selected", core::ThemeColorToken::SelectionBg, &chrome.list_selected});
    entries.push_back(
        {"scrollbar_thumb", core::ThemeColorToken::ScrollbarThumb, &chrome.scrollbar_thumb});
    entries.push_back(
        {"scrollbar_track", core::ThemeColorToken::ScrollbarTrack, &chrome.scrollbar_track});

    build_token_list(nullptr, entries);
}

void ThemeTokenEditor::populate_syntax_tokens()
{
    std::vector<TokenEntry> entries;
    auto& syntax = working_theme_.syntax;

    entries.push_back({"editor_bg", core::ThemeColorToken::BgInput, &syntax.editor_bg});
    entries.push_back({"editor_fg", core::ThemeColorToken::TextMain, &syntax.editor_fg});
    entries.push_back({"selection", core::ThemeColorToken::SelectionBg, &syntax.editor_selection});
    entries.push_back(
        {"line_number", core::ThemeColorToken::TextMuted, &syntax.editor_line_number});
    entries.push_back({"cursor", core::ThemeColorToken::AccentPrimary, &syntax.editor_cursor});
    entries.push_back({"gutter", core::ThemeColorToken::BgPanel, &syntax.editor_gutter});
    entries.push_back({"keyword", core::ThemeColorToken::SyntaxKeyword, &syntax.keyword});
    entries.push_back({"string", core::ThemeColorToken::SyntaxString, &syntax.string_literal});
    entries.push_back({"comment", core::ThemeColorToken::SyntaxComment, &syntax.comment});
    entries.push_back({"number", core::ThemeColorToken::SyntaxNumber, &syntax.number});
    entries.push_back({"type", core::ThemeColorToken::SyntaxType, &syntax.type_name});
    entries.push_back({"function", core::ThemeColorToken::SyntaxFunction, &syntax.function_name});
    entries.push_back({"operator", core::ThemeColorToken::SyntaxOperator, &syntax.operator_tok});
    entries.push_back(
        {"preprocessor", core::ThemeColorToken::SyntaxPreprocessor, &syntax.preprocessor});

    build_token_list(nullptr, entries);
}

void ThemeTokenEditor::populate_render_tokens()
{
    std::vector<TokenEntry> entries;
    auto& render = working_theme_.render;

    entries.push_back({"heading", core::ThemeColorToken::RenderHeading, &render.heading});
    entries.push_back({"link", core::ThemeColorToken::RenderLink, &render.link});
    entries.push_back({"code_bg", core::ThemeColorToken::RenderCodeBg, &render.code_bg});
    entries.push_back({"code_fg", core::ThemeColorToken::RenderCodeFg, &render.code_fg});
    entries.push_back({"blockquote_border",
                       core::ThemeColorToken::RenderBlockquoteBorder,
                       &render.blockquote_border});
    entries.push_back(
        {"blockquote_bg", core::ThemeColorToken::RenderBlockquoteBg, &render.blockquote_bg});
    entries.push_back(
        {"table_border", core::ThemeColorToken::RenderTableBorder, &render.table_border});
    entries.push_back(
        {"table_header_bg", core::ThemeColorToken::RenderTableHeaderBg, &render.table_header_bg});

    build_token_list(nullptr, entries);
}

// ---------------------------------------------------------------------------
// Events
// ---------------------------------------------------------------------------

void ThemeTokenEditor::on_tab_changed(wxBookCtrlEvent& /*event*/)
{
    int page = tabs_->GetSelection();
    switch (page)
    {
        case 0:
            populate_chrome_tokens();
            break;
        case 1:
            populate_syntax_tokens();
            break;
        case 2:
            populate_render_tokens();
            break;
        default:
            break;
    }
    refresh_preview();
}

void ThemeTokenEditor::on_token_clicked(wxListEvent& event)
{
    long idx = event.GetIndex();
    if (idx < 0 || static_cast<size_t>(idx) >= current_entries_.size())
    {
        return;
    }

    auto& entry = current_entries_[static_cast<size_t>(idx)];
    wxColourData color_data;
    color_data.SetColour(entry.color_ref->to_wx_colour());

    wxColourDialog dlg(this, &color_data);
    if (dlg.ShowModal() == wxID_OK)
    {
        wxColour chosen = dlg.GetColourData().GetColour();
        *entry.color_ref = core::Color(static_cast<uint8_t>(chosen.Red()),
                                       static_cast<uint8_t>(chosen.Green()),
                                       static_cast<uint8_t>(chosen.Blue()),
                                       static_cast<uint8_t>(chosen.Alpha()));

        // Update list display
        token_list_->SetItem(idx, 1, entry.color_ref->to_hex());
        refresh_preview();
    }
}

void ThemeTokenEditor::on_apply(wxCommandEvent& /*event*/)
{
    // Write working theme layers back to the engine and apply
    applied_ = true;

    // Copy working colors back to the engine's theme
    auto theme = working_theme_;

    // Write chrome layer back to flat ThemeColors
    theme.colors.bg_app = theme.chrome.bg_app;
    theme.colors.bg_panel = theme.chrome.bg_panel;
    theme.colors.bg_header = theme.chrome.bg_header;
    theme.colors.bg_input = theme.chrome.bg_input;
    theme.colors.border_light = theme.chrome.border_light;
    theme.colors.border_dark = theme.chrome.border_dark;
    theme.colors.accent_primary = theme.chrome.accent_primary;
    theme.colors.accent_secondary = theme.chrome.accent_secondary;
    theme.colors.list_hover = theme.chrome.list_hover;
    theme.colors.list_selected = theme.chrome.list_selected;
    theme.colors.scrollbar_thumb = theme.chrome.scrollbar_thumb;
    theme.colors.scrollbar_track = theme.chrome.scrollbar_track;

    // Write syntax layer back
    theme.colors.editor_bg = theme.syntax.editor_bg;
    theme.colors.editor_fg = theme.syntax.editor_fg;
    theme.colors.editor_selection = theme.syntax.editor_selection;
    theme.colors.editor_line_number = theme.syntax.editor_line_number;
    theme.colors.editor_cursor = theme.syntax.editor_cursor;
    theme.colors.editor_gutter = theme.syntax.editor_gutter;

    // Apply layered themes via engine
    engine_.apply_chrome_theme();
    engine_.apply_syntax_theme();
    engine_.apply_render_theme();

    MARKAMP_LOG_INFO("Theme tokens applied via ThemeTokenEditor");
}

void ThemeTokenEditor::on_save(wxCommandEvent& event)
{
    on_apply(event);
    // Write the updated theme to its .md theme file via ThemeEngine
    const auto& theme_id = engine_.current_theme().id;
    auto theme_dir = std::filesystem::path("themes");
    auto theme_path = theme_dir / (theme_id + ".md");
    engine_.export_theme(theme_id, theme_path);
    MARKAMP_LOG_INFO("Theme tokens saved to {}", theme_path.string());
    EndModal(wxID_SAVE);
}

void ThemeTokenEditor::on_cancel(wxCommandEvent& /*event*/)
{
    EndModal(wxID_CANCEL);
}

// ---------------------------------------------------------------------------
// Preview
// ---------------------------------------------------------------------------

void ThemeTokenEditor::refresh_preview()
{
    if (preview_sidebar_ != nullptr)
    {
        preview_sidebar_->SetBackgroundColour(working_theme_.chrome.bg_panel.to_wx_colour());
        preview_sidebar_->Refresh();
    }
    if (preview_editor_ != nullptr)
    {
        preview_editor_->SetBackgroundColour(working_theme_.syntax.editor_bg.to_wx_colour());
        preview_editor_->Refresh();
    }
    if (preview_rendered_ != nullptr)
    {
        preview_rendered_->SetBackgroundColour(working_theme_.render.code_bg.to_wx_colour());
        preview_rendered_->Refresh();
    }
}

} // namespace markamp::ui
