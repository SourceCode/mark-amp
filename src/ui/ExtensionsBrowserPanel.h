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

struct DesignSystemContext;
class IconManager;
class ExtensionCard;
class ExtensionDetailPanel;
class SidebarSection;
class SidebarSkeletonPlaceholder;

/// Extensions browser panel shown in the sidebar.
/// Allows searching, browsing, and managing extensions.
class ExtensionsBrowserPanel : public wxPanel
{
public:
    ExtensionsBrowserPanel(wxWindow* parent,
                           core::ThemeEngine& theme_engine,
                           core::EventBus& event_bus,
                           core::IExtensionManagementService& mgmt_service,
                           core::IExtensionGalleryService& gallery_service,
                           DesignSystemContext& ds,
                           IconManager& icon_manager);

    /// Refresh the installed extensions list.
    void ShowInstalledExtensions();

    /// Search extensions in the gallery.
    void SearchExtensions(const std::string& query);

    /// Apply current theme styling.
    void ApplyTheme();

private:
    core::ThemeEngine& theme_engine_;
    core::EventBus& event_bus_;
    core::IExtensionManagementService& mgmt_service_;
    core::IExtensionGalleryService& gallery_service_;
    DesignSystemContext& ds_;
    IconManager& icon_manager_;

    // UI elements
    wxSearchCtrl* search_ctrl_{nullptr};
    wxScrolledWindow* card_scroll_{nullptr};
    wxBoxSizer* card_sizer_{nullptr};

    SidebarSection* installed_section_{nullptr};
    SidebarSection* recommended_section_{nullptr};
    SidebarSection* search_section_{nullptr};

    wxBoxSizer* installed_card_sizer_{nullptr};
    wxBoxSizer* recommended_card_sizer_{nullptr};
    wxBoxSizer* search_card_sizer_{nullptr};

    ExtensionDetailPanel* detail_panel_{nullptr};
    SidebarSkeletonPlaceholder* loading_skeleton_{nullptr};

    // State
    std::vector<ExtensionCard*> installed_cards_;
    std::vector<ExtensionCard*> recommended_cards_;
    std::vector<ExtensionCard*> search_cards_;
    std::vector<core::LocalExtension> installed_extensions_;
    core::GallerySortBy gallery_sort_{core::GallerySortBy::kInstallCount};
    std::string gallery_category_;

    // Subscriptions
    core::Subscription install_sub_;
    core::Subscription uninstall_sub_;
    core::Subscription header_action_sub_;

    void CreateLayout();
    void ClearInstalledCards();
    void ClearRecommendedCards();
    void ClearSearchCards();
    void PopulateInstalledSection();
    void PopulateRecommendedSection();
    void PopulateSearchSection(const std::vector<core::GalleryExtension>& results);
    void OnSearchChanged(wxCommandEvent& event);
    void OnCardClicked(const std::string& extension_id);
    void OnCardAction(const std::string& extension_id, bool is_installed);
    void ShowCardList();
    void ShowDetailView(const std::string& extension_id);
    void ShowFilterMenu();

    [[nodiscard]] auto IsExtensionInstalled(const std::string& extension_id) const -> bool;

    static constexpr int kSearchBarHeight = 28;
};

} // namespace markamp::ui
