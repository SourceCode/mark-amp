#pragma once

#include "core/EventBus.h"
#include "core/IPlugin.h"
#include "core/ThemeEngine.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/collpane.h>
#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>

#include <functional>
#include <set>
#include <string>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Definition of a single configurable setting displayed in the Settings panel.
/// Mirrors VS Code's `IConfigurationPropertySchema`.
struct SettingDefinition
{
    std::string setting_id;  // Unique key, e.g. "editor.fontSize"
    std::string label;       // Display label, e.g. "Font Size"
    std::string description; // Help text
    std::string category;    // "Editor", "Appearance", "Keybindings", "Plugins", "Advanced"
    core::SettingType type{core::SettingType::Boolean};
    std::string default_value;        // Serialized default
    std::vector<std::string> choices; // For choice type only
    int min_int{0};                   // For integer type
    int max_int{100};                 // For integer type
};

/// Settings panel inspired by VS Code's settingsEditor2.
/// Displays configurable settings grouped by category with a search/filter bar.
/// Changes are written to Config immediately and fire SettingChangedEvent.
class SettingsPanel : public wxPanel
{
public:
    SettingsPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::Config& config);

    /// Register a setting definition
    void RegisterSetting(SettingDefinition definition);

    /// Register the built-in settings
    void RegisterBuiltinSettings();

    /// Apply current theme styling
    void ApplyTheme();

    /// Refresh all controls to reflect current Config values
    void RefreshValues();

    static constexpr int kCategoryPadding = 12;
    static constexpr int kSettingRowHeight = 44;
    static constexpr int kMaxVisibleSettings = 50;

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Config& config_;

    wxSearchCtrl* search_ctrl_{nullptr};
    wxListBox* category_list_{nullptr};
    wxScrolledWindow* scroll_area_{nullptr};
    wxBoxSizer* settings_sizer_{nullptr};
    std::string active_category_; // empty = show all

    std::vector<SettingDefinition> definitions_;
    std::vector<wxWindow*> setting_widgets_;

    void CreateLayout();
    void RebuildSettingsList();
    void OnSearchChanged(wxCommandEvent& event);
    void OnCategorySelected(wxCommandEvent& event);
    void OnSettingChanged(const std::string& setting_id, const std::string& new_value);
    void ResetSettingToDefault(const std::string& setting_id, const std::string& default_val);
    auto CreateResetButton(wxWindow* parent, const SettingDefinition& def) -> wxButton*;

    // Phase 9 settings panel improvements
    void ExportSettings();
    void ImportSettings();
    [[nodiscard]] auto IsSettingModified(const SettingDefinition& def) const -> bool;
    void OnCollapsibleToggle(const std::string& category);

    // Widget creation per setting type
    auto CreateBooleanSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateIntegerSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateStringSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateChoiceSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;

    // Phase 9: collapsible state
    std::set<std::string> collapsed_categories_;

    // Theme subscription
    core::Subscription theme_sub_;
};

} // namespace markamp::ui
