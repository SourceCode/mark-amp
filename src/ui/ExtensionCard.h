#pragma once

#include "core/EventBus.h"
#include "core/ExtensionManagement.h"
#include "core/ExtensionManifest.h"
#include "core/GalleryService.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/gauge.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/statbmp.h>
#include <wx/stattext.h>

#include <functional>
#include <string>

namespace markamp::ui
{

class IconManager;

/// Compact card widget for displaying an extension in the scrolling list.
/// Shows name, publisher, version, description, and an action button.
class ExtensionCard : public wxPanel
{
public:
    /// Visual state of the card
    enum class State
    {
        NotInstalled,
        Installed,
        UpdateAvailable,
        Installing // Phase 20 Task 17: In-progress install
    };

    ExtensionCard(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  IconManager& icon_manager,
                  const std::string& extension_id,
                  const std::string& name,
                  const std::string& publisher,
                  const std::string& version,
                  const std::string& description,
                  State state);

    /// Set callback for when the card body is clicked (open detail view).
    void SetOnClick(std::function<void(const std::string&)> callback);

    /// Set callback for when the action button is clicked.
    void SetOnAction(std::function<void(const std::string&, State)> callback);

    /// Apply current theme colours.
    void ApplyTheme(core::ThemeEngine& theme_engine);

    /// Get the extension ID this card represents.
    [[nodiscard]] auto GetExtensionId() const -> const std::string&;

    /// Update the visual state and button text.
    void SetState(State new_state);

private:
    core::ThemeEngine& theme_engine_;
    IconManager& icon_manager_;
    std::string extension_id_;
    State state_;

    wxStaticBitmap* icon_bitmap_{nullptr};
    wxStaticText* name_label_{nullptr};
    wxStaticText* publisher_label_{nullptr};
    wxStaticText* version_label_{nullptr};
    wxStaticText* description_label_{nullptr};
    wxStaticText* rating_label_{nullptr};
    wxStaticText* downloads_label_{nullptr};
    wxStaticText* update_version_label_{nullptr}; // Task 7: "v1.2 → v1.3"
    wxGauge* progress_gauge_{nullptr};            // Task 17: install progress
    wxStaticText* progress_label_{nullptr};       // Task 17: "Downloading..."
    wxButton* action_button_{nullptr};
    wxButton* gear_button_{nullptr}; // Task 8: settings gear
    wxPanel* info_panel_{nullptr};

    std::function<void(const std::string&)> on_click_;
    std::function<void(const std::string&, State)> on_action_;
    std::function<void(const std::string&)> on_settings_;

    void CreateLayout(const std::string& name,
                      const std::string& publisher,
                      const std::string& version,
                      const std::string& description);
    void UpdateActionButton();
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnClick(wxMouseEvent& event);

    static constexpr int kCardHeight = 88;
    static constexpr int kCardPadding = 10;

public:
    /// Phase 20 Task 2: Set rating (0.0-5.0) and download count.
    void SetRatingAndDownloads(double rating, int download_count);

    /// Phase 20 Task 7: Set the available update version string.
    void SetUpdateVersion(const std::string& current_ver, const std::string& new_ver);

    /// Phase 20 Task 17: Set install progress (0-100) and status text.
    void SetInstallProgress(int percent, const std::string& status_text);

    /// Phase 20 Task 8: Set settings gear click callback.
    void SetOnSettings(std::function<void(const std::string&)> callback)
    {
        on_settings_ = std::move(callback);
    }
};

} // namespace markamp::ui
