#pragma once

#include "ThemePreviewCard.h"
#include "core/ThemeEngine.h"
#include "core/ThemeRegistry.h"

#include <wx/dialog.h>
#include <wx/scrolwin.h>
#include <wx/stattext.h>
#include <wx/timer.h>

#include <string>
#include <vector>

namespace markamp::ui
{

/// Modal theme gallery dialog that displays all available themes as
/// miniature live preview cards. Supports instant theme selection,
/// theme import/export, and responsive grid layout.
class ThemeGallery : public wxDialog
{
public:
    ThemeGallery(wxWindow* parent, core::ThemeEngine& theme_engine, core::ThemeRegistry& registry);

    /// Show the gallery as a modal dialog.
    void ShowGallery();

    /// Returns the selected theme ID (or empty if cancelled).
    [[nodiscard]] auto GetSelectedThemeId() const -> std::string;

    /// Calculate how many grid columns fit in a given width.
    [[nodiscard]] static auto CalculateColumns(int available_width) -> int;

    /// Generate export filename from theme name.
    [[nodiscard]] static auto ExportFilename(const std::string& theme_name) -> std::string;

    static constexpr int kDialogWidth = 896;
    static constexpr int kCardGap = 24;
    static constexpr int kGridPadding = 24;
    static constexpr int kHeaderHeight = 72;
    static constexpr int kToolbarHeight = 48;

private:
    core::ThemeEngine& theme_engine_;
    core::ThemeRegistry& registry_;

    // UI components
    wxPanel* header_panel_{nullptr};
    wxPanel* toolbar_panel_{nullptr};
    wxScrolledWindow* grid_panel_{nullptr};
    wxStaticText* theme_count_label_{nullptr};
    wxStaticText* error_label_{nullptr};
    wxTimer error_timer_;
    std::vector<ThemePreviewCard*> preview_cards_;

    // State
    std::string selected_theme_id_;

    // Layout
    void CreateHeader();
    void CreateToolbar();
    void CreateGrid();
    void PopulateGrid();
    void UpdateActiveIndicators();
    void RefreshThemeCount();
    void ShowError(const std::string& message);
    void ClearError();

    // Event handlers
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnThemeCardClicked(const std::string& theme_id);
    void OnExportTheme(const std::string& theme_id);
    void OnDeleteTheme(const std::string& theme_id);
    void OnImportClicked(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnErrorTimer(wxTimerEvent& event);
};

} // namespace markamp::ui
