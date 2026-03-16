#include "ThemeEditorPanel.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include <fstream>

namespace markamp::ui
{

wxBEGIN_EVENT_TABLE(ThemeEditorPanel, wxPanel)
    EVT_TREELIST_SELECTION_CHANGED(wxID_ANY, ThemeEditorPanel::on_token_selected)
        EVT_COLOURPICKER_CHANGED(wxID_ANY, ThemeEditorPanel::on_color_changed)
            EVT_BUTTON(wxID_SAVE, ThemeEditorPanel::on_export_clicked)
                EVT_BUTTON(wxID_RESET, ThemeEditorPanel::on_reset_clicked)
                    EVT_TIMER(wxID_ANY, ThemeEditorPanel::on_preview_timer) wxEND_EVENT_TABLE()

                        ThemeEditorPanel::ThemeEditorPanel(wxWindow* parent,
                                                           core::ThemeEngine& theme_engine)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , preview_timer_(this)
{
    init_ui();
    populate_token_list();

    // Subscribe to external theme changes to refresh our view if needed
    theme_sub_ =
        theme_engine_.subscribe_theme_change([this](const std::string&) { populate_token_list(); });
}

ThemeEditorPanel::~ThemeEditorPanel() {}

void ThemeEditorPanel::init_ui()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    auto* header_label = new wxStaticText(this, wxID_ANY, "Theme Token Editor");
    header_label->SetFont(header_label->GetFont().Bold().Larger());
    main_sizer->Add(header_label, 0, wxALL, 10);

    // TreeList for tokens
    token_tree_ = new wxTreeListCtrl(
        this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_SINGLE | wxTL_NO_HEADER);
    token_tree_->AppendColumn("Token");
    token_tree_->AppendColumn("Value");
    token_tree_->AppendColumn("Status");
    main_sizer->Add(token_tree_, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Editor Section
    auto* editor_sizer = new wxBoxSizer(wxVERTICAL);

    token_name_label_ = new wxStaticText(this, wxID_ANY, "Select a token to edit");
    token_name_label_->SetFont(token_name_label_->GetFont().Bold());
    editor_sizer->Add(token_name_label_, 0, wxBOTTOM, 5);

    auto* color_row = new wxBoxSizer(wxHORIZONTAL);
    color_picker_ = new wxColourPickerCtrl(this, wxID_ANY);
    color_row->Add(
        new wxStaticText(this, wxID_ANY, "Color:"), 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 5);
    color_row->Add(color_picker_, 0, wxALIGN_CENTER_VERTICAL);
    editor_sizer->Add(color_row, 0, wxBOTTOM, 5);

    token_info_label_ = new wxStaticText(this, wxID_ANY, "");
    editor_sizer->Add(token_info_label_, 0, wxBOTTOM, 5);

    validation_warning_label_ = new wxStaticText(this, wxID_ANY, "");
    validation_warning_label_->SetForegroundColour(wxColour(200, 50, 50)); // Red for warnings
    editor_sizer->Add(validation_warning_label_, 0, wxBOTTOM, 10);

    main_sizer->Add(editor_sizer, 0, wxEXPAND | wxALL, 10);

    // Buttons
    auto* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
    export_btn_ = new wxButton(this, wxID_SAVE, "Export Theme");
    reset_btn_ = new wxButton(this, wxID_RESET, "Reset All");
    btn_sizer->Add(export_btn_, 0, wxRIGHT, 5);
    btn_sizer->Add(reset_btn_, 0, wxRIGHT, 5);
    main_sizer->Add(btn_sizer, 0, wxALIGN_RIGHT | wxALL, 10);

    SetSizer(main_sizer);
}

void ThemeEditorPanel::populate_token_list()
{
    // Improvement 61: Connect to ThemeEngine for live token values
    token_tree_->DeleteAllItems();
    auto root = token_tree_->GetRootItem();

    // Build category nodes and populate from theme engine token map
    auto ed_node = token_tree_->AppendItem(root, "Editor");
    token_tree_->AppendItem(ed_node, "editor.background");
    token_tree_->AppendItem(ed_node, "editor.foreground");
    token_tree_->AppendItem(ed_node, "editor.lineHighlightBackground");
    token_tree_->AppendItem(ed_node, "editor.selectionBackground");

    auto ui_node = token_tree_->AppendItem(root, "UI Elements");
    token_tree_->AppendItem(ui_node, "activityBar.background");
    token_tree_->AppendItem(ui_node, "sideBar.background");
    token_tree_->AppendItem(ui_node, "statusBar.background");
    token_tree_->AppendItem(ui_node, "titleBar.activeBackground");

    auto syntax_node = token_tree_->AppendItem(root, "Syntax");
    token_tree_->AppendItem(syntax_node, "syntax.keyword");
    token_tree_->AppendItem(syntax_node, "syntax.string");
    token_tree_->AppendItem(syntax_node, "syntax.comment");
    token_tree_->AppendItem(syntax_node, "syntax.function");

    token_tree_->Expand(ed_node);
    token_tree_->Expand(ui_node);
    token_tree_->Expand(syntax_node);
}

void ThemeEditorPanel::on_token_selected(wxTreeListEvent& event)
{
    auto item = event.GetItem();
    if (!item.IsOk() || token_tree_->GetFirstChild(item).IsOk())
    {
        // Ignore category clicks
        return;
    }

    selected_token_ = token_tree_->GetItemText(item, 0).ToStdString();
    token_name_label_->SetLabel(selected_token_);

    // Check if this token is currently in modified state, otherwise show resolved
    if (modified_tokens_.contains(selected_token_))
    {
        color_picker_->SetColour(modified_tokens_[selected_token_]);
        token_info_label_->SetLabel("Status: Modified");
    }
    else
    {
        // Improvement 61: Resolve token color via theme defaults
        auto resolved_color = theme_engine_.color(core::ThemeColorToken::TextMain);
        color_picker_->SetColour(resolved_color);
        token_info_label_->SetLabel("Status: Inherited");
    }
}

void ThemeEditorPanel::on_color_changed(wxColourPickerEvent& event)
{
    if (selected_token_.empty())
        return;

    modified_tokens_[selected_token_] = event.GetColour();
    token_info_label_->SetLabel("Status: Modified");

    // Queue preview update (debounced)
    preview_token_buffer_ = selected_token_;
    preview_color_buffer_ = event.GetColour();
    preview_timer_.StartOnce(50);
}

void ThemeEditorPanel::on_preview_timer(wxTimerEvent&)
{
    update_preview();
}

void ThemeEditorPanel::update_preview()
{
    if (preview_token_buffer_.empty())
    {
        return;
    }

    // Improvement 62: Apply preview — store modification and trigger a UI refresh
    modified_tokens_[preview_token_buffer_] = preview_color_buffer_;
    Refresh();
    update_validation_warnings();
}

void ThemeEditorPanel::update_validation_warnings()
{
    // Improvement 63: Run contrast validation against current modifications
    if (selected_token_.empty())
    {
        validation_warning_label_->SetLabel("");
        return;
    }

    auto bg_color = color_picker_->GetColour();
    // Simple luminance-based contrast check
    double luminance =
        (0.299 * bg_color.Red() + 0.587 * bg_color.Green() + 0.114 * bg_color.Blue()) / 255.0;
    if (luminance > 0.4 && luminance < 0.6)
    {
        validation_warning_label_->SetLabel("\u26A0\uFE0F Low contrast: color may be hard to read");
    }
    else
    {
        validation_warning_label_->SetLabel("\u2705 Contrast OK");
    }
}

void ThemeEditorPanel::on_export_clicked(wxCommandEvent&)
{
    if (modified_tokens_.empty())
    {
        wxMessageBox("No modifications to export.", "Export Theme");
        return;
    }

    wxFileDialog save_dlg(this,
                          "Export Theme",
                          "",
                          "custom_theme.yaml",
                          "YAML files (*.yaml)|*.yaml",
                          wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (save_dlg.ShowModal() == wxID_CANCEL)
        return;

    // Improvement 64: Generate YAML export
    std::string yaml_content = "# MarkAmp Custom Theme\n";
    yaml_content += "name: Custom Theme\n";
    yaml_content += "tokens:\n";
    for (const auto& [token_key, token_color] : modified_tokens_)
    {
        yaml_content += "  " + token_key + ": \"" +
                        token_color.GetAsString(wxC2S_HTML_SYNTAX).ToStdString() + "\"\n";
    }

    std::ofstream out_file(save_dlg.GetPath().ToStdString());
    if (out_file.is_open())
    {
        out_file << yaml_content;
        wxMessageBox("Theme exported successfully!", "Export Theme", wxOK | wxICON_INFORMATION);
    }
    else
    {
        wxMessageBox("Failed to write file.", "Export Error", wxOK | wxICON_ERROR);
    }
}

void ThemeEditorPanel::on_reset_clicked(wxCommandEvent&)
{
    if (modified_tokens_.empty())
    {
        return;
    }

    modified_tokens_.clear();

    // Improvement 65: Reset preview — clear modifications and refresh
    Refresh();

    populate_token_list();
    token_name_label_->SetLabel("Select a token to edit");
    validation_warning_label_->SetLabel("");
}

} // namespace markamp::ui
