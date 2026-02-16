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
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
class SettingsCatalog;
} // namespace markamp::core

namespace markamp::ui
{

/// Definition of a single configurable preference displayed in the Preferences panel.
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

    // Batch 1: Schema enhancements
    std::vector<std::string> tags; // Searchable keyword tags
    bool deprecated{false};        // Gray out with strikethrough
    bool experimental{false};      // Show [experimental] badge
    bool restart_required{false};  // Show ⚠️ "Requires restart" hint
    double min_double{0.0};        // Range for Double type
    double max_double{100.0};      // Range for Double type
    int order_priority{100};       // Sort weight within category (lower = higher)
};

/// Preferences panel inspired by VS Code's settingsEditor2.
/// Displays configurable preferences grouped by category with a search/filter bar.
/// Changes are written to Config immediately and fire SettingChangedEvent.
class SettingsPanel : public wxPanel
{
public:
    /// Construct with manual setting registration (sidebar usage).
    SettingsPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::Config& config);

    /// Construct with catalog-driven settings (dialog usage, staged edits enabled).
    SettingsPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::Config& config,
                  core::SettingsCatalog& catalog);

    /// Register a setting definition
    void RegisterSetting(SettingDefinition definition);

    /// Register the built-in settings
    void RegisterBuiltinSettings();

    /// Apply current theme styling
    void ApplyTheme();

    /// Refresh all controls to reflect current Config values
    void RefreshValues();

    // ── Staged-edit API ──

    /// Flush all pending changes to Config, publish SettingsBatchChangedEvent, clear buffer.
    void ApplyPendingChanges();

    /// Discard all pending changes and rebuild UI from Config.
    void DiscardPendingChanges();

    /// Whether there are uncommitted changes in the buffer.
    [[nodiscard]] auto HasPendingChanges() const -> bool;

    /// Number of uncommitted changes in the buffer.
    [[nodiscard]] auto PendingChangeCount() const -> std::size_t;

    /// Set the search field text programmatically (for deep-linking).
    void SetSearchText(const std::string& query);

    /// Focus the search control (for auto-focus on dialog open).
    void FocusSearch();

    /// Reset all settings to their default values.
    void RestoreAllDefaults();

    static constexpr int kCategoryPadding = 12;
    static constexpr int kSettingRowHeight = 44;
    static constexpr int kMaxVisibleSettings = 50;

    // ── Batch 6: Preferences query API ──

    /// Total number of registered setting definitions.
    [[nodiscard]] auto setting_count() const -> std::size_t;

    /// Number of currently visible (filtered) settings.
    [[nodiscard]] auto filtered_count() const -> std::size_t;

    /// Number of settings whose current value differs from the default.
    [[nodiscard]] auto modified_count() const -> std::size_t;

    /// Get the current Config value for a setting by ID.
    [[nodiscard]] auto GetSettingValue(const std::string& setting_id) const -> std::string;

    /// Programmatically set a setting value.
    void SetSettingValue(const std::string& setting_id, const std::string& value);

    /// Reset all settings in a category to their default values.
    void ResetCategoryToDefaults(const std::string& category);

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

    /// Catalog-driven mode: pointer is non-null when constructed with SettingsCatalog.
    core::SettingsCatalog* catalog_{nullptr};

    /// Staged-edit buffer: setting_id → pending new value (not yet committed to Config).
    std::unordered_map<std::string, std::string> pending_changes_;

    /// Whether staged-edit mode is active (true when catalog_ is set).
    [[nodiscard]] auto is_staged_mode() const -> bool
    {
        return catalog_ != nullptr;
    }

    /// Populate definitions_ from SettingsCatalog entries.
    void PopulateFromCatalog();

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
    auto CreateDoubleSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateFilePathSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateColorSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateKeyBindingSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;
    auto CreateStringListSetting(wxWindow* parent, const SettingDefinition& def) -> wxPanel*;

    // Phase 9: collapsible state
    std::set<std::string> collapsed_categories_;

    // Batch 6: filtered count tracker
    std::size_t filtered_count_{0};

    // Batch 6: UI enhancement members
    wxStaticText* status_bar_label_{nullptr};
    wxStaticText* breadcrumb_label_{nullptr};
    wxCheckBox* show_modified_only_{nullptr};

    // Theme subscription
    core::Subscription theme_sub_;
};

} // namespace markamp::ui
