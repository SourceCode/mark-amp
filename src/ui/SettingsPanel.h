#pragma once

#include "core/EventBus.h"
#include "core/IPlugin.h"
#include "core/SettingsCatalog.h"
#include "core/ThemeEngine.h"

#include <wx/checkbox.h>
#include <wx/choice.h>
#include <wx/clrpicker.h>
#include <wx/collpane.h>
#include <wx/colordlg.h>
#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/notebook.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/spinctrl.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>
#include <wx/textctrl.h>
#include <wx/timer.h>
#include <wx/treectrl.h>

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

    // ── Batch 5A: Undo/Redo API ──

    /// Undo the last setting change.
    void UndoLastChange();

    /// Redo the last undone change.
    void RedoLastChange();

    /// Whether there are changes that can be undone.
    [[nodiscard]] auto CanUndo() const -> bool;

    /// Whether there are changes that can be redone.
    [[nodiscard]] auto CanRedo() const -> bool;

    /// Reset all settings to their default values.
    void RestoreAllDefaults();

    /// Export/Import settings via file dialog (Batch 5D Task 13).
    void ExportSettings();
    void ImportSettings();

    [[deprecated(
        "Use DesignSystemContext.spacing limits instead")]] static constexpr int kCategoryPadding =
        12;
    [[deprecated(
        "Use ComponentSizeResolver for list row heights")]] static constexpr int kSettingRowHeight =
        44;
    static constexpr int kMaxVisibleSettings = 50;
    static constexpr int kSearchDebounceMs = 300;

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

    // ── 100 Editor UX/UI Improvements: Batch 10 — Settings Panel Accessors (#91–#100) ──

    /// #91 True when staged-edit mode is active (alias for is_staged_mode).
    [[nodiscard]] inline auto is_staged_edit_mode() const noexcept -> bool
    {
        return catalog_ != nullptr;
    }

    /// #92 True when there is undo history.
    [[nodiscard]] inline auto has_undo_history() const noexcept -> bool
    {
        return !undo_stack_.empty();
    }

    /// #93 True when there is redo history.
    [[nodiscard]] inline auto has_redo_history() const noexcept -> bool
    {
        return !redo_stack_.empty();
    }

    /// #94 Number of entries in the undo stack.
    [[nodiscard]] inline auto undo_stack_size() const noexcept -> std::size_t
    {
        return undo_stack_.size();
    }

    /// #95 Number of entries in the redo stack.
    [[nodiscard]] inline auto redo_stack_size() const noexcept -> std::size_t
    {
        return redo_stack_.size();
    }

    /// #96 Currently active category filter (empty = all).
    [[nodiscard]] inline auto active_category() const noexcept -> const std::string&
    {
        return active_category_;
    }

    /// #97 True when a SettingsCatalog is attached.
    [[nodiscard]] inline auto has_catalog() const noexcept -> bool
    {
        return catalog_ != nullptr;
    }

    /// #98 Number of collapsed categories.
    [[nodiscard]] inline auto collapsed_category_count() const noexcept -> std::size_t
    {
        return collapsed_categories_.size();
    }

    /// #99 Total number of setting definitions.
    [[nodiscard]] inline auto definition_count() const noexcept -> std::size_t
    {
        return definitions_.size();
    }

    /// #100 True when search is active (search control has text).
    [[nodiscard]] inline auto is_search_active() const noexcept -> bool
    {
        return search_ctrl_ != nullptr && !search_ctrl_->IsEmpty();
    }

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::Config& config_;

    wxSearchCtrl* search_ctrl_{nullptr};
    wxTreeCtrl* category_tree_{nullptr};
    wxNotebook* scope_tabs_{nullptr};
    wxScrolledWindow* scroll_area_{nullptr};
    wxBoxSizer* settings_sizer_{nullptr};
    std::string active_category_; // empty = show all
    std::string active_subgroup_; // empty = show all in group
    core::ConfigScope active_scope_{core::ConfigScope::kApplication};

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
    void BuildCategoryTree();
    void RebuildSettingsList();
    void OnSearchChanged(wxCommandEvent& event);
    void OnTreeSelectionChanged(wxTreeEvent& event);
    void OnScopeChanged(wxBookCtrlEvent& event);
    void OnSettingChanged(const std::string& setting_id, const std::string& new_value);
    void ResetSettingToDefault(const std::string& setting_id, const std::string& default_val);
    auto CreateResetButton(wxWindow* parent, const SettingDefinition& def) -> wxButton*;

    /// Compute fuzzy search score for a definition against query (higher = better match).
    static auto FuzzyScore(const SettingDefinition& def, const std::string& query) -> double;

    // Phase 9 settings panel improvements
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

    // Batch 5A: Undo/Redo stack
    struct SettingChange
    {
        std::string setting_id;
        std::string old_value;
        std::string new_value;
    };
    std::vector<SettingChange> undo_stack_;
    std::vector<SettingChange> redo_stack_;

    // Theme subscription
    core::Subscription theme_sub_;

    // Batch 5E Task 19: Debounce timer for search input
    wxTimer search_debounce_timer_;
};

} // namespace markamp::ui
