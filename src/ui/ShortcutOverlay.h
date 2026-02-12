#pragma once

#include "ThemeAwareWindow.h"
#include "core/ShortcutManager.h"
#include "core/ThemeEngine.h"

#include <wx/textctrl.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Modal overlay that displays all keyboard shortcuts grouped by category.
/// Triggered by F1, closes on Escape or F1 again.
class ShortcutOverlay : public ThemeAwareWindow
{
public:
    ShortcutOverlay(wxWindow* parent,
                    core::ThemeEngine& theme_engine,
                    const core::ShortcutManager& shortcut_manager);

    void ShowOverlay();
    void HideOverlay();
    [[nodiscard]] auto IsOverlayVisible() const -> bool
    {
        return is_visible_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    const core::ShortcutManager& shortcut_manager_;
    bool is_visible_{false};

    // Search filter (Phase 7B)
    wxTextCtrl* filter_input_{nullptr};
    std::string filter_text_;

    /// Category groupings for display.
    struct ShortcutCategory
    {
        std::string name;
        std::vector<std::pair<std::string, std::string>> entries; // {shortcut_text, description}
    };

    std::vector<ShortcutCategory> categories_;
    std::vector<ShortcutCategory> filtered_categories_;

    void BuildCategories();
    void FilterCategories();
    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnFilterChanged(wxCommandEvent& event);
};

} // namespace markamp::ui
