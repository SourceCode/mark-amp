#pragma once

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SettingsCatalog.h"
#include "core/ThemeEngine.h"

#include <wx/choice.h>
#include <wx/dialog.h>
#include <wx/stattext.h>

#include <string>

namespace markamp::core
{
class Config;
class ConfigProfileManager;
} // namespace markamp::core

namespace markamp::ui
{

class SettingsPanel;

/// Modal dialog wrapping SettingsPanel with staged-edit lifecycle (OK/Cancel/Apply).
class SettingsDialog : public wxDialog
{
public:
    SettingsDialog(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   core::EventBus& event_bus,
                   core::Config& config,
                   core::SettingsCatalog& catalog);

    /// Open the dialog scrolled to a specific setting by ID.
    void OpenToSetting(const std::string& setting_id);

    /// Open the dialog with a pre-filled search query.
    void OpenWithQuery(const std::string& query);

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Config& config_;
    [[maybe_unused]] core::SettingsCatalog& catalog_;
    SettingsPanel* settings_panel_{nullptr};

    void CreateLayout();
    void OnOK(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);

    // Batch 7 enhancements
    void UpdateUnsavedIndicator();
    void OnClose(wxCloseEvent& event);
    void ApplyTheme();

    wxStaticText* unsaved_indicator_{nullptr};
    wxChoice* scope_selector_{nullptr};
    wxStaticText* search_count_label_{nullptr};
    wxChoice* profile_selector_{nullptr};

    // Batch 5D Task 14: Profile manager (nullable; not owned)
    core::ConfigProfileManager* profile_manager_{nullptr};

    // Batch 5D Task 13-15: Toolbar action handlers
    void OnImportSettings(wxCommandEvent& event);
    void OnExportSettings(wxCommandEvent& event);
    void OnProfileChanged(wxCommandEvent& event);
    void OnUndo(wxCommandEvent& event);
    void OnRedo(wxCommandEvent& event);
};

} // namespace markamp::ui
