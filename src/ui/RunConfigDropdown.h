/// @file RunConfigDropdown.h
/// @brief Custom-drawn dropdown for selecting run configurations.
///
/// Features:
///   - Themed dropdown with configuration name display
///   - Click to open dropdown list showing all configurations
///   - Recent configurations shown at top
///   - "Edit Configurations..." item at bottom
///
/// @see RunConfigService, Toolbar

#pragma once

#include "ThemeAwareWindow.h"
#include "core/RunConfigService.h"

#include <functional>
#include <string>

namespace markamp::ui
{

/// Custom-drawn dropdown widget for run configuration selection.
class RunConfigDropdown : public ThemeAwareWindow
{
public:
    RunConfigDropdown(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      core::RunConfigService& config_service);

    /// Get the currently selected configuration name.
    [[nodiscard]] auto selected_name() const -> const std::string&;

    /// Set a callback for when the user selects a configuration.
    using SelectionCallback = std::function<void(const std::string&)>;
    void set_on_selection(SelectionCallback callback);

    /// Set a callback for when "Edit Configurations..." is clicked.
    using EditCallback = std::function<void()>;
    void set_on_edit(EditCallback callback);

    static constexpr int kDropdownWidth = 160;
    static constexpr int kDropdownHeight = 24;
    static constexpr int kItemHeight = 26;
    static constexpr int kMaxVisibleItems = 10;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::RunConfigService& config_service_;
    std::string selected_name_;
    SelectionCallback on_selection_;
    EditCallback on_edit_;

    bool is_open_{false};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);

    void ShowDropdownPopup();
    void CloseDropdownPopup();
};

} // namespace markamp::ui
