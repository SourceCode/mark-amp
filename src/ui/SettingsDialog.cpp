#include "SettingsDialog.h"

#include "SettingsPanel.h"
#include "core/Config.h"
#include "core/ConfigProfile.h"
#include "core/Events.h"

#include <wx/button.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

namespace markamp::ui
{

SettingsDialog::SettingsDialog(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus,
                               core::Config& config,
                               core::SettingsCatalog& catalog)
    : wxDialog(parent,
               wxID_ANY,
               "Preferences",
               wxDefaultPosition,
               wxSize(900, 650),
               wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , config_(config)
    , catalog_(catalog)
{
    SetMinSize(wxSize(600, 400));

    // Batch 7 #42: Restore last saved dialog size from Config
    const int saved_width = config_.get_int("preferences.dialog.width", 900);
    const int saved_height = config_.get_int("preferences.dialog.height", 650);
    SetSize(wxSize(saved_width, saved_height));

    CreateLayout();
    CentreOnParent();

    // Batch 7 #39: Bind close event for confirmation
    Bind(wxEVT_CLOSE_WINDOW, &SettingsDialog::OnClose, this);

    // Batch 7 #44: Apply theme-aware styling
    ApplyTheme();

    // Batch 7 #48: Auto-focus search on open
    if (settings_panel_ != nullptr)
    {
        settings_panel_->FocusSearch();
    }
}

void SettingsDialog::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Batch 7 #41: Scope selector header row
    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* scope_label = new wxStaticText(this, wxID_ANY, "Scope:");
    header_sizer->Add(scope_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    wxArrayString scope_items;
    scope_items.Add("Application");
    scope_items.Add("Workspace");
    scope_items.Add("Project");
    scope_selector_ = new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(140, -1), scope_items);
    scope_selector_->SetSelection(0);
    header_sizer->Add(scope_selector_, 0, wxALIGN_CENTER_VERTICAL);

    // Batch 7 #46: Scope change fires event
    scope_selector_->Bind(wxEVT_CHOICE,
                          [this](wxCommandEvent& /*evt*/)
                          {
                              const auto sel = scope_selector_->GetStringSelection().ToStdString();
                              event_bus_.publish(core::events::SettingsCategoryChangedEvent(sel));
                          });

    header_sizer->AddStretchSpacer();

    // Batch 7 #47: Search match count label
    search_count_label_ = new wxStaticText(this, wxID_ANY, "");
    search_count_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
    header_sizer->Add(search_count_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);

    main_sizer->Add(header_sizer, 0, wxEXPAND | wxALL, 12);

    // Batch 5D Task 14: Profile switcher
    auto* profile_sizer = new wxBoxSizer(wxHORIZONTAL);
    auto* profile_label = new wxStaticText(this, wxID_ANY, "Profile:");
    profile_sizer->Add(profile_label, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    wxArrayString profile_items;
    profile_items.Add("(Default)");
    // Fill from ConfigProfileManager if available
    if (profile_manager_ != nullptr)
    {
        for (const auto& pname : profile_manager_->profile_names())
        {
            profile_items.Add(pname);
        }
    }
    profile_selector_ =
        new wxChoice(this, wxID_ANY, wxDefaultPosition, wxSize(160, -1), profile_items);
    profile_selector_->SetSelection(0);
    profile_selector_->SetToolTip("Switch between settings profiles");
    profile_selector_->Bind(wxEVT_CHOICE, &SettingsDialog::OnProfileChanged, this);
    profile_sizer->Add(profile_selector_, 0, wxALIGN_CENTER_VERTICAL);
    main_sizer->Add(profile_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT, 12);

    // Use catalog-driven constructor → staged-edit mode
    settings_panel_ = new SettingsPanel(this, theme_engine_, event_bus_, config_, catalog_);
    main_sizer->Add(settings_panel_, 1, wxEXPAND);

    // Action row
    auto* button_sizer = new wxBoxSizer(wxHORIZONTAL);

    // Batch 7 #37: Unsaved changes indicator
    unsaved_indicator_ = new wxStaticText(this, wxID_ANY, "");
    unsaved_indicator_->SetForegroundColour(
        theme_engine_.color(core::ThemeColorToken::EditorGutterWarn));
    button_sizer->Add(unsaved_indicator_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    UpdateUnsavedIndicator();

    button_sizer->AddStretchSpacer();

    // Batch 7 #43: Restore Defaults button
    auto* restore_btn = new wxButton(this, wxID_ANY, "Restore Defaults");
    restore_btn->SetToolTip("Reset all settings to their default values");
    restore_btn->Bind(wxEVT_BUTTON,
                      [this](wxCommandEvent& /*evt*/)
                      {
                          const int answer = wxMessageBox("Reset ALL settings to defaults?",
                                                          "Restore Defaults",
                                                          wxYES_NO | wxICON_WARNING,
                                                          this);
                          if (answer == wxYES && settings_panel_ != nullptr)
                          {
                              settings_panel_->RestoreAllDefaults();
                              UpdateUnsavedIndicator();
                          }
                      });
    button_sizer->Add(restore_btn, 0, wxRIGHT, 16);

    // Batch 5D Task 13: Import/Export buttons
    auto* import_btn = new wxButton(this, wxID_ANY, "Import…", wxDefaultPosition, wxSize(90, -1));
    import_btn->SetToolTip("Import settings from a JSON file");
    import_btn->Bind(wxEVT_BUTTON, &SettingsDialog::OnImportSettings, this);
    button_sizer->Add(import_btn, 0, wxRIGHT, 4);

    auto* export_btn = new wxButton(this, wxID_ANY, "Export…", wxDefaultPosition, wxSize(90, -1));
    export_btn->SetToolTip("Export current settings to a JSON file");
    export_btn->Bind(wxEVT_BUTTON, &SettingsDialog::OnExportSettings, this);
    button_sizer->Add(export_btn, 0, wxRIGHT, 8);

    // Batch 5D Task 15: Undo/Redo buttons
    auto* undo_btn = new wxButton(this, wxID_ANY, "↩ Undo", wxDefaultPosition, wxSize(80, -1));
    undo_btn->SetToolTip("Undo the last setting change (Ctrl+Z)");
    undo_btn->Bind(wxEVT_BUTTON, &SettingsDialog::OnUndo, this);
    button_sizer->Add(undo_btn, 0, wxRIGHT, 4);

    auto* redo_btn = new wxButton(this, wxID_ANY, "↪ Redo", wxDefaultPosition, wxSize(80, -1));
    redo_btn->SetToolTip("Redo the last undone change (Ctrl+Shift+Z)");
    redo_btn->Bind(wxEVT_BUTTON, &SettingsDialog::OnRedo, this);
    button_sizer->Add(redo_btn, 0, wxRIGHT, 16);

    // Batch 7 #45: Keyboard shortcut labels on buttons
    auto* apply_btn = new wxButton(this, wxID_APPLY, "Apply");
    apply_btn->SetToolTip("Apply changes (Ctrl+S)");
    auto* ok_btn = new wxButton(this, wxID_OK, "OK");
    ok_btn->SetToolTip("Apply and close (Enter)");
    auto* cancel_btn = new wxButton(this, wxID_CANCEL, "Cancel");
    cancel_btn->SetToolTip("Discard changes (Escape)");

    button_sizer->Add(apply_btn, 0, wxRIGHT, 8);
    button_sizer->Add(ok_btn, 0, wxRIGHT, 8);
    button_sizer->Add(cancel_btn, 0);

    main_sizer->Add(button_sizer, 0, wxEXPAND | wxALL, 12);
    SetSizer(main_sizer);

    Bind(wxEVT_BUTTON, &SettingsDialog::OnOK, this, wxID_OK);
    Bind(wxEVT_BUTTON, &SettingsDialog::OnCancel, this, wxID_CANCEL);
    Bind(wxEVT_BUTTON, &SettingsDialog::OnApply, this, wxID_APPLY);
}

void SettingsDialog::OpenToSetting(const std::string& setting_id)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->SetSearchText(setting_id);
    }
    ShowModal();
}

void SettingsDialog::OpenWithQuery(const std::string& query)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->SetSearchText(query);
    }
    ShowModal();
}

void SettingsDialog::OnOK(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr && settings_panel_->HasPendingChanges())
    {
        // Batch 5A Task 3: Show commit summary before accepting
        const auto count = settings_panel_->PendingChangeCount();
        wxString summary =
            wxString::Format("Apply %zu setting change%s?\n\n", count, count == 1 ? "" : "s");
        summary += "The following settings will be updated.";

        const int result = wxMessageBox(
            summary, "Confirm Settings Changes", wxOK | wxCANCEL | wxICON_INFORMATION, this);
        if (result != wxOK)
        {
            return; // User cancelled — stay in dialog
        }
        settings_panel_->ApplyPendingChanges();
    }
    // Batch 7 #42: Save dialog size
    const auto size = GetSize();
    config_.set("preferences.dialog.width", size.GetWidth());
    config_.set("preferences.dialog.height", size.GetHeight());
    EndModal(wxID_OK);
}

void SettingsDialog::OnCancel(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->DiscardPendingChanges();
    }
    EndModal(wxID_CANCEL);
}

void SettingsDialog::OnApply(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->ApplyPendingChanges();
    }
    UpdateUnsavedIndicator();
}

// Batch 7 #38: Update unsaved indicator label
void SettingsDialog::UpdateUnsavedIndicator()
{
    if (settings_panel_ == nullptr || unsaved_indicator_ == nullptr)
    {
        return;
    }
    const auto count = settings_panel_->PendingChangeCount();
    if (count > 0)
    {
        unsaved_indicator_->SetLabel(
            wxString::Format("%zu unsaved change%s", count, count == 1 ? "" : "s"));
    }
    else
    {
        unsaved_indicator_->SetLabel("");
    }
}

// Batch 7 #39: Close confirmation
void SettingsDialog::OnClose(wxCloseEvent& event)
{
    if (settings_panel_ != nullptr && settings_panel_->HasPendingChanges())
    {
        const auto count = settings_panel_->PendingChangeCount();
        const int result = wxMessageBox(
            wxString::Format("Discard %zu unsaved change%s?", count, count == 1 ? "" : "s"),
            "Preferences",
            wxYES_NO | wxICON_QUESTION,
            this);
        if (result != wxYES)
        {
            event.Veto();
            return;
        }
        settings_panel_->DiscardPendingChanges();
    }
    EndModal(wxID_CANCEL);
}

// Batch 7 #44: Apply theme-aware styling
void SettingsDialog::ApplyTheme()
{
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);
    if (scope_selector_ != nullptr)
    {
        scope_selector_->SetBackgroundColour(bg_color);
        scope_selector_->SetForegroundColour(fg_color);
    }
}

// Batch 5D Task 13: Import handler
void SettingsDialog::OnImportSettings(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->ImportSettings();
        UpdateUnsavedIndicator();
    }
}

// Batch 5D Task 13: Export handler
void SettingsDialog::OnExportSettings(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr)
    {
        settings_panel_->ExportSettings();
    }
}

// Batch 5D Task 14: Profile changed handler
void SettingsDialog::OnProfileChanged(wxCommandEvent& /*event*/)
{
    if (profile_selector_ == nullptr || profile_manager_ == nullptr)
    {
        return;
    }
    const auto sel = profile_selector_->GetStringSelection().ToStdString();
    if (sel == "(Default)")
    {
        return; // Default means no profile overlay
    }
    profile_manager_->apply_profile(sel, config_);
    if (settings_panel_ != nullptr)
    {
        settings_panel_->RefreshValues();
    }
    UpdateUnsavedIndicator();
    event_bus_.publish(core::events::SettingsCategoryChangedEvent("profile:" + sel));
}

// Batch 5D Task 15: Undo handler
void SettingsDialog::OnUndo(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr && settings_panel_->CanUndo())
    {
        settings_panel_->UndoLastChange();
        UpdateUnsavedIndicator();
    }
}

// Batch 5D Task 15: Redo handler
void SettingsDialog::OnRedo(wxCommandEvent& /*event*/)
{
    if (settings_panel_ != nullptr && settings_panel_->CanRedo())
    {
        settings_panel_->RedoLastChange();
        UpdateUnsavedIndicator();
    }
}

} // namespace markamp::ui
