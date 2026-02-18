#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/dialog.h>
#include <wx/stattext.h>
#include <wx/stc/stc.h>

#include <string>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

/// Modal dialog providing a raw JSON text editor for all settings.
/// Supports syntax-highlighted editing, parse-on-save validation, and
/// bidirectional sync with the Config system.
///
/// Batch 5C Task 11: Provides an alternative to the GUI settings panel
/// for power users who prefer editing settings as JSON directly.
class SettingsJsonEditor : public wxDialog
{
public:
    SettingsJsonEditor(wxWindow* parent,
                       core::ThemeEngine& theme_engine,
                       core::EventBus& event_bus,
                       core::Config& config);

    /// Reload editor content from current Config values.
    void ReloadFromConfig();

    /// Validate JSON in the editor and report errors.
    /// Returns true if JSON is valid.
    [[nodiscard]] auto ValidateJson() const -> bool;

private:
    core::ThemeEngine& theme_engine_;
    [[maybe_unused]] core::EventBus& event_bus_;
    core::Config& config_;

    wxStyledTextCtrl* editor_{nullptr};
    wxStaticText* status_label_{nullptr};

    void CreateLayout();
    void OnSave(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);
    void OnFormat(wxCommandEvent& event);
    void ApplyTheme();

    /// Build a JSON string from all Config key-value pairs.
    [[nodiscard]] auto ConfigToJson() const -> std::string;

    /// Parse JSON text and apply values to Config.
    /// Returns true on success, false with error message on failure.
    auto ApplyJsonToConfig(const std::string& json_text) -> bool;
};

} // namespace markamp::ui
