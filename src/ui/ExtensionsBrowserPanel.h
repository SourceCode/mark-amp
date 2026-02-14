#pragma once

#include "core/EventBus.h"
#include "core/ExtensionManagement.h"
#include "core/ExtensionManifest.h"
#include "core/GalleryService.h"
#include "core/ThemeEngine.h"

#include <wx/button.h>
#include <wx/panel.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/srchctrl.h>
#include <wx/stattext.h>

#include <string>
#include <vector>

namespace markamp::ui
{

class ExtensionCard;
class ExtensionDetailPanel;

/// Extensions browser panel shown in the sidebar.
/// Allows searching, browsing, and managing extensions.
class ExtensionsBrowserPanel : public wxPanel
{
public:
    ExtensionsBrowserPanel(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           core::IExtensionManagementService& mgmt_service,
                           core::IExtensionGalleryService& gallery_service);

    /// Refresh the installed extensions list.
    void ShowInstalledExtensions();

    /// Search extensions in the gallery.
    void SearchExtensions(const std::string& query);

    /// Apply current theme styling.
    void ApplyTheme();

    /// The view mode for the browser.
    enum class ViewMode
    {
        kInstalled,
        kSearchResults
    };

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::IExtensionManagementService& mgmt_service_;
    core::IExtensionGalleryService& gallery_service_;

    ViewMode view_mode_{ViewMode::kInstalled};

    // UI elements
    wxSearchCtrl* search_ctrl_{nullptr};
    wxPanel* tab_bar_{nullptr};
    wxButton* installed_tab_{nullptr};
    wxButton* search_tab_{nullptr};
    wxScrolledWindow* card_scroll_{nullptr};
    wxBoxSizer* card_sizer_{nullptr};
    ExtensionDetailPanel* detail_panel_{nullptr};

    // State
    std::vector<ExtensionCard*> cards_;
    std::vector<core::LocalExtension> installed_extensions_;

    // Subscriptions
    core::Subscription install_sub_;
    core::Subscription uninstall_sub_;

    void CreateLayout();
    void ClearCards();
    void PopulateInstalledCards();
    void PopulateSearchCards(const std::vector<core::GalleryExtension>& results);
    void OnSearchChanged(wxCommandEvent& event);
    void OnTabClicked(ViewMode mode);
    void OnCardClicked(const std::string& extension_id);
    void OnCardAction(const std::string& extension_id, bool is_installed);
    void ShowCardList();
    void ShowDetailView(const std::string& extension_id);
    void UpdateTabStyles();

    [[nodiscard]] auto IsExtensionInstalled(const std::string& extension_id) const -> bool;

    static constexpr int kSearchBarHeight = 28;
    static constexpr int kTabBarHeight = 32;
};

} // namespace markamp::ui
