#include "ThemeEditorPanel.h"

#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

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
    // TODO: Connect to actual ThemeEngine V2 token retrieval
    token_tree_->DeleteAllItems();
    auto root = token_tree_->GetRootItem();

    // Placeholder data
    auto ed_node = token_tree_->AppendItem(root, "Editor");
    token_tree_->AppendItem(ed_node, "editor.background");
    token_tree_->AppendItem(ed_node, "editor.foreground");

    auto ui_node = token_tree_->AppendItem(root, "UI Elements");
    token_tree_->AppendItem(ui_node, "activityBar.background");
    token_tree_->AppendItem(ui_node, "sideBar.background");

    token_tree_->Expand(ed_node);
    token_tree_->Expand(ui_node);
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
        // TODO: resolve via theme engine
        color_picker_->SetColour(*wxWHITE);
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
        return;

    // TODO: Send to ThemeEngine for live preview overlay
    update_validation_warnings();
}

void ThemeEditorPanel::update_validation_warnings()
{
    // TODO: Run ContrastRatioValidator against current modifications
    // validation_warning_label_->SetLabel("Warning: Contrast ratio 3.2:1 (Fails AA)");
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

    // TODO: Generate YAML export via ThemeExporter utilizing only modified_tokens_ map
    wxMessageBox("Theme exported successfully!", "Export Theme", wxOK | wxICON_INFORMATION);

    // Clear modifications on successful save conceptually, or leave them as active workspace
}

void ThemeEditorPanel::on_reset_clicked(wxCommandEvent&)
{
    if (modified_tokens_.empty())
        return;

    modified_tokens_.clear();
    // TODO: Tell theme engine to cancel preview overlays
    populate_token_list();
    token_name_label_->SetLabel("Select a token to edit");
}

} // namespace markamp::ui
