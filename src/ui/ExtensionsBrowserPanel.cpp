#include "ExtensionsBrowserPanel.h"

#include "ExtensionCard.h"
#include "ExtensionDetailPanel.h"
#include "SidebarSkeletonPlaceholder.h"
#include "core/Events.h"
#include "ui/PanelHeader.h"
#include "ui/SidebarSection.h"

#include <wx/menu.h>
#include <wx/msgdlg.h>

#include <spdlog/spdlog.h>

namespace markamp::ui
{

ExtensionsBrowserPanel::ExtensionsBrowserPanel(wxWindow* parent,
                                               core::ThemeEngine& theme_engine,
                                               core::EventBus& event_bus,
                                               core::IExtensionManagementService& mgmt_service,
                                               core::IExtensionGalleryService& gallery_service,
                                               DesignSystemContext& ds,
                                               IconManager& icon_manager)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , mgmt_service_(mgmt_service)
    , gallery_service_(gallery_service)
    , ds_(ds)
    , icon_manager_(icon_manager)
{
    CreateLayout();
    ApplyTheme();

    // Subscribe to extension events to auto-refresh
    install_sub_ = event_bus_.subscribe<core::events::ExtensionInstalledEvent>(
        [this]([[maybe_unused]] const core::events::ExtensionInstalledEvent& evt)
        {
            if (search_ctrl_ && search_ctrl_->GetValue().empty())
            {
                ShowInstalledExtensions();
            }
            else if (search_ctrl_)
            {
                SearchExtensions(search_ctrl_->GetValue().ToStdString());
            }
        });

    uninstall_sub_ = event_bus_.subscribe<core::events::ExtensionUninstalledEvent>(
        [this]([[maybe_unused]] const core::events::ExtensionUninstalledEvent& evt)
        {
            if (search_ctrl_ && search_ctrl_->GetValue().empty())
            {
                ShowInstalledExtensions();
            }
            else if (search_ctrl_)
            {
                SearchExtensions(search_ctrl_->GetValue().ToStdString());
            }
        });

    header_action_sub_ = event_bus_.subscribe<core::events::PanelHeaderActionEvent>(
        [this](const core::events::PanelHeaderActionEvent& evt)
        {
            if (evt.action_id == "extensions.filter")
            {
                ShowFilterMenu();
            }
        });

    // Start with installed view
    ShowInstalledExtensions();
}

void ExtensionsBrowserPanel::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // 1. Panel Header
    auto* header = new PanelHeader(this, ds_, icon_manager_, event_bus_);
    header->set_title("EXTENSIONS");
    const std::vector<PanelHeader::ActionIcon> actions = {
        {"extensions.filter", "filter", "Filter Extensions"}};
    header->set_actions(actions);
    main_sizer->Add(header, 0, wxEXPAND);

    auto* toolbar_panel = new wxPanel(header, wxID_ANY);
    toolbar_panel->SetBackgroundColour(GetBackgroundColour());
    auto* toolbar_sizer = new wxBoxSizer(wxVERTICAL);
    toolbar_panel->SetSizer(toolbar_sizer);

    // Search bar
    search_ctrl_ = new wxSearchCtrl(
        toolbar_panel, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, kSearchBarHeight));
    search_ctrl_->SetDescriptiveText("Search extensions\u2026");
    search_ctrl_->ShowCancelButton(true);
    search_ctrl_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular));

    search_ctrl_->Bind(wxEVT_TEXT, [this](wxCommandEvent& evt) { OnSearchChanged(evt); });

    search_ctrl_->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           search_ctrl_->Clear();
                           ShowInstalledExtensions();
                       });

    toolbar_sizer->Add(search_ctrl_, 0, wxEXPAND | wxALL, 0);
    header->set_toolbar(toolbar_panel);

    // Scrollable section list
    card_scroll_ = new wxScrolledWindow(this, wxID_ANY);
    card_scroll_->SetScrollRate(0, 10);
    card_sizer_ = new wxBoxSizer(wxVERTICAL);
    card_scroll_->SetSizer(card_sizer_);

    main_sizer->Add(card_scroll_, 1, wxEXPAND);

    // 1. Installed Section
    installed_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, "INSTALLED");
    auto* inst_panel = new wxPanel(installed_section_);
    installed_card_sizer_ = new wxBoxSizer(wxVERTICAL);
    inst_panel->SetSizer(installed_card_sizer_);
    installed_section_->set_content(inst_panel);
    card_sizer_->Add(installed_section_, 0, wxEXPAND);

    // 2. Recommended Section
    recommended_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, "RECOMMENDED");
    auto* rec_panel = new wxPanel(recommended_section_);
    recommended_card_sizer_ = new wxBoxSizer(wxVERTICAL);
    rec_panel->SetSizer(recommended_card_sizer_);
    recommended_section_->set_content(rec_panel);
    card_sizer_->Add(recommended_section_, 0, wxEXPAND);

    // 3. Search / Marketplace Section
    search_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, "MARKETPLACE");
    auto* search_panel = new wxPanel(search_section_);
    search_card_sizer_ = new wxBoxSizer(wxVERTICAL);
    search_panel->SetSizer(search_card_sizer_);
    search_section_->set_content(search_panel);
    card_sizer_->Add(search_section_, 0, wxEXPAND);

    // Scrollable card list
    card_scroll_ = new wxScrolledWindow(this, wxID_ANY);
    card_scroll_->SetScrollRate(0, 10);
    card_sizer_ = new wxBoxSizer(wxVERTICAL);
    card_scroll_->SetSizer(card_sizer_);

    main_sizer->Add(card_scroll_, 1, wxEXPAND);

    // Detail panel (initially hidden)
    detail_panel_ = new ExtensionDetailPanel(this, theme_engine_, event_bus_);
    detail_panel_->Hide();

    // Skeleton placeholder (initially hidden)
    loading_skeleton_ = new SidebarSkeletonPlaceholder(
        this, theme_engine_, SidebarSkeletonPlaceholder::Style::kCards);
    loading_skeleton_->Hide();

    detail_panel_->SetOnBack([this]() { ShowCardList(); });

    detail_panel_->SetOnInstall([this](const std::string& ext_id) { OnCardAction(ext_id, false); });

    detail_panel_->SetOnUninstall([this](const std::string& ext_id)
                                  { OnCardAction(ext_id, true); });

    detail_panel_->SetOnUpdate(
        [this](const std::string& ext_id)
        {
            auto result = mgmt_service_.update(ext_id);
            if (result.has_value())
            {
                spdlog::info("Extension updated: {}", ext_id);
                if (search_ctrl_ && search_ctrl_->GetValue().empty())
                {
                    ShowInstalledExtensions();
                }
                else if (search_ctrl_)
                {
                    SearchExtensions(search_ctrl_->GetValue().ToStdString());
                }
                ShowCardList();
            }
            else
            {
                spdlog::error("Failed to update extension {}: {}", ext_id, result.error());
            }
        });

    main_sizer->Add(detail_panel_, 1, wxEXPAND);
    main_sizer->Add(loading_skeleton_, 1, wxEXPAND);

    SetSizer(main_sizer);
}

void ExtensionsBrowserPanel::ShowInstalledExtensions()
{
    installed_extensions_ = mgmt_service_.get_installed();
    ClearInstalledCards();
    PopulateInstalledSection();

    installed_section_->Show();
    // We can hide others if not searching, or show recommended.
    // For now, let's just make sure installed is visible and search is hidden.
    recommended_section_->Hide();
    search_section_->Hide();

    card_scroll_->Layout();
}

void ExtensionsBrowserPanel::SearchExtensions(const std::string& query)
{
    if (query.empty())
    {
        ShowInstalledExtensions();
        return;
    }

    // Show skeleton while loading
    card_scroll_->Hide();
    detail_panel_->Hide();
    loading_skeleton_->ShowAndAnimate();
    Layout();

    // Yield to let the UI draw the skeleton
    wxYieldIfNeeded();

    core::GalleryQueryOptions options;
    options.filters.push_back({core::GalleryFilterType::kSearchText, query});
    if (!gallery_category_.empty())
    {
        options.filters.push_back({core::GalleryFilterType::kCategory, gallery_category_});
    }
    options.sort_by = gallery_sort_;
    options.page_size = 20;

    auto result = gallery_service_.query(options);

    loading_skeleton_->HideAndStop();
    card_scroll_->Show();

    if (result.has_value())
    {
        ClearSearchCards();
        PopulateSearchSection(result.value().extensions);
    }
    else
    {
        spdlog::warn("Gallery search failed: {}", result.error());
        ClearSearchCards();
    }

    installed_section_->Hide();
    recommended_section_->Hide();
    search_section_->Show();
    card_scroll_->Layout();
}

void ExtensionsBrowserPanel::ClearInstalledCards()
{
    installed_card_sizer_->Clear(true);
    installed_cards_.clear();
}

void ExtensionsBrowserPanel::ClearRecommendedCards()
{
    recommended_card_sizer_->Clear(true);
    recommended_cards_.clear();
}

void ExtensionsBrowserPanel::ClearSearchCards()
{
    search_card_sizer_->Clear(true); // Destroy children
    search_cards_.clear();
}

void ExtensionsBrowserPanel::PopulateInstalledSection()
{
    if (installed_extensions_.empty())
    {
        auto* empty_label =
            new wxStaticText(installed_section_->get_content(),
                             wxID_ANY,
                             "No extensions installed.\nSearch the gallery to find new features.");
        empty_label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        auto font = empty_label->GetFont();
        font.SetPointSize(font.GetPointSize() - 1);
        empty_label->SetFont(font);
        installed_card_sizer_->AddSpacer(8);
        installed_card_sizer_->Add(empty_label, 0, wxALIGN_CENTER | wxALL, 16);
        installed_card_sizer_->AddSpacer(8);
        installed_section_->get_content()->Layout();
        return;
    }

    for (const auto& ext : installed_extensions_)
    {
        const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
        auto* card = new ExtensionCard(installed_section_->get_content(),
                                       theme_engine_,
                                       icon_manager_,
                                       ext_id,
                                       ext.manifest.name,
                                       ext.manifest.publisher,
                                       ext.manifest.version,
                                       ext.manifest.description,
                                       ExtensionCard::State::Installed);

        // Update card click handlers
        card->SetOnClick([this](const std::string& card_ext_id) { OnCardClicked(card_ext_id); });
        card->SetOnAction(
            [this](const std::string& card_ext_id, [[maybe_unused]] ExtensionCard::State card_state)
            { OnCardAction(card_ext_id, true); });

        installed_card_sizer_->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2);
        installed_cards_.push_back(card);
    }

    installed_section_->get_content()->Layout();
}

void ExtensionsBrowserPanel::PopulateRecommendedSection()
{
    // Currently unimplemented, placeholder space
}

void ExtensionsBrowserPanel::PopulateSearchSection(
    const std::vector<core::GalleryExtension>& results)
{
    installed_extensions_ = mgmt_service_.get_installed();

    if (results.empty())
    {
        auto* empty_label = new wxStaticText(
            search_section_->get_content(), wxID_ANY, "No extensions found matching your search.");
        empty_label->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        auto font = empty_label->GetFont();
        font.SetPointSize(font.GetPointSize() - 1);
        empty_label->SetFont(font);
        search_card_sizer_->AddSpacer(8);
        search_card_sizer_->Add(empty_label, 0, wxALIGN_CENTER | wxALL, 16);
        search_card_sizer_->AddSpacer(8);
        search_section_->get_content()->Layout();
        return;
    }

    for (const auto& gallery_ext : results)
    {
        const bool installed = IsExtensionInstalled(gallery_ext.identifier);
        auto state =
            installed ? ExtensionCard::State::Installed : ExtensionCard::State::NotInstalled;

        auto display_name =
            gallery_ext.display_name.empty() ? gallery_ext.identifier : gallery_ext.display_name;

        auto* card = new ExtensionCard(search_section_->get_content(),
                                       theme_engine_,
                                       icon_manager_,
                                       gallery_ext.identifier,
                                       display_name,
                                       gallery_ext.publisher_display,
                                       gallery_ext.version,
                                       gallery_ext.description,
                                       state);

        card->SetOnClick([this](const std::string& card_ext_id) { OnCardClicked(card_ext_id); });
        card->SetOnAction([this, installed](const std::string& card_ext_id,
                                            [[maybe_unused]] ExtensionCard::State card_state)
                          { OnCardAction(card_ext_id, installed); });

        search_card_sizer_->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2);
        search_cards_.push_back(card);
    }

    search_section_->get_content()->Layout();
}

void ExtensionsBrowserPanel::OnSearchChanged(wxCommandEvent& /*event*/)
{
    auto query = search_ctrl_->GetValue().ToStdString();
    if (query.length() >= 2)
    {
        SearchExtensions(query);
    }
    else if (query.empty())
    {
        ShowInstalledExtensions();
    }
}

void ExtensionsBrowserPanel::OnCardClicked(const std::string& extension_id)
{
    ShowDetailView(extension_id);
}

void ExtensionsBrowserPanel::OnCardAction(const std::string& extension_id, bool is_installed)
{
    if (is_installed)
    {
        auto result = mgmt_service_.uninstall(extension_id);
        if (result.has_value())
        {
            spdlog::info("Extension uninstalled: {}", extension_id);
        }
        else
        {
            spdlog::error("Failed to uninstall {}: {}", extension_id, result.error());
        }
    }
    else
    {
        // For gallery installs, we need a GalleryExtension object
        auto gallery_result = gallery_service_.get_extensions({extension_id});
        if (gallery_result.has_value() && !gallery_result.value().empty())
        {
            auto install_result =
                mgmt_service_.install_from_gallery(gallery_result.value().front());
            if (install_result.has_value())
            {
                spdlog::info("Extension installed: {}", extension_id);
            }
            else
            {
                spdlog::error("Failed to install {}: {}", extension_id, install_result.error());
            }
        }
        else
        {
            spdlog::error("Extension not found in gallery: {}", extension_id);
        }
    }

    // Refresh view
    if (search_ctrl_->GetValue().empty())
    {
        ShowInstalledExtensions();
    }
}

void ExtensionsBrowserPanel::ShowCardList()
{
    if (loading_skeleton_)
        loading_skeleton_->HideAndStop();
    detail_panel_->Hide();
    card_scroll_->Show();
    search_ctrl_->Show();
    Layout();
}

void ExtensionsBrowserPanel::ShowDetailView(const std::string& extension_id)
{
    // Try to find in installed extensions first
    for (const auto& ext : installed_extensions_)
    {
        const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
        if (ext_id == extension_id)
        {
            detail_panel_->ShowExtension(ext);
            card_scroll_->Hide();
            detail_panel_->Show();
            Layout();
            return;
        }
    }

    // Not found in installed — try gallery
    auto gallery_result = gallery_service_.get_extensions({extension_id});
    if (gallery_result.has_value() && !gallery_result.value().empty())
    {
        bool installed = IsExtensionInstalled(extension_id);
        detail_panel_->ShowGalleryExtension(gallery_result.value().front(), installed);
        card_scroll_->Hide();
        search_ctrl_->Hide();
        detail_panel_->Show();
        Layout();
    }
}

void ExtensionsBrowserPanel::ShowFilterMenu()
{
    wxMenu menu;
    auto* sort_menu = new wxMenu();
    sort_menu->AppendRadioItem(1001, "Installs")
        ->Check(gallery_sort_ == core::GallerySortBy::kInstallCount);
    sort_menu->AppendRadioItem(1002, "Rating")
        ->Check(gallery_sort_ == core::GallerySortBy::kAverageRating);
    sort_menu->AppendRadioItem(1003, "Name")->Check(gallery_sort_ == core::GallerySortBy::kTitle);
    sort_menu->AppendRadioItem(1004, "Published Date")
        ->Check(gallery_sort_ == core::GallerySortBy::kPublishedDate);

    auto* category_menu = new wxMenu();
    category_menu->AppendRadioItem(2000, "All")->Check(gallery_category_.empty());
    category_menu->AppendRadioItem(2001, "Themes")->Check(gallery_category_ == "Themes");
    category_menu->AppendRadioItem(2002, "Snippets")->Check(gallery_category_ == "Snippets");
    category_menu->AppendRadioItem(2003, "Linters")->Check(gallery_category_ == "Linters");

    menu.AppendSubMenu(sort_menu, "Sort By");
    menu.AppendSubMenu(category_menu, "Category");

    menu.Bind(wxEVT_MENU,
              [this](wxCommandEvent& event)
              {
                  int id = event.GetId();
                  bool changed = false;

                  switch (id)
                  {
                      case 1001:
                          gallery_sort_ = core::GallerySortBy::kInstallCount;
                          changed = true;
                          break;
                      case 1002:
                          gallery_sort_ = core::GallerySortBy::kAverageRating;
                          changed = true;
                          break;
                      case 1003:
                          gallery_sort_ = core::GallerySortBy::kTitle;
                          changed = true;
                          break;
                      case 1004:
                          gallery_sort_ = core::GallerySortBy::kPublishedDate;
                          changed = true;
                          break;
                      case 2000:
                          gallery_category_ = "";
                          changed = true;
                          break;
                      case 2001:
                          gallery_category_ = "Themes";
                          changed = true;
                          break;
                      case 2002:
                          gallery_category_ = "Snippets";
                          changed = true;
                          break;
                      case 2003:
                          gallery_category_ = "Linters";
                          changed = true;
                          break;
                  }

                  if (changed && search_ctrl_ && !search_ctrl_->GetValue().empty())
                  {
                      SearchExtensions(search_ctrl_->GetValue().ToStdString());
                  }
              });

    PopupMenu(&menu);
}

auto ExtensionsBrowserPanel::IsExtensionInstalled(const std::string& extension_id) const -> bool
{
    for (const auto& ext : installed_extensions_)
    {
        const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
        if (ext_id == extension_id)
        {
            return true;
        }
    }
    return false;
}

void ExtensionsBrowserPanel::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);

    if (card_scroll_ != nullptr)
    {
        card_scroll_->SetBackgroundColour(bg_color);
    }

    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        search_ctrl_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }

    // Update cards
    for (auto* card : installed_cards_)
    {
        card->ApplyTheme(theme_engine_);
    }
    for (auto* card : recommended_cards_)
    {
        card->ApplyTheme(theme_engine_);
    }
    for (auto* card : search_cards_)
    {
        card->ApplyTheme(theme_engine_);
    }

    // Update detail panel
    if (detail_panel_ != nullptr)
    {
        detail_panel_->ApplyTheme();
    }

    Refresh();
}

} // namespace markamp::ui
