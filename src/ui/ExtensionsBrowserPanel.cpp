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

    // Phase 20 Task 23: Keyboard navigation
    Bind(wxEVT_CHAR_HOOK, &ExtensionsBrowserPanel::OnKeyDown, this);

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

    // Phase 20 Task 1: Category chip bar
    auto* chip_scroll = new wxScrolledWindow(
        toolbar_panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 28), wxHSCROLL | wxBORDER_NONE);
    chip_scroll->SetScrollRate(10, 0);
    chip_scroll->SetBackgroundColour(GetBackgroundColour());
    auto* chip_sizer = new wxBoxSizer(wxHORIZONTAL);
    chip_scroll->SetSizer(chip_sizer);

    static const std::vector<std::string> kCategories = {"All",
                                                         "Themes",
                                                         "Languages",
                                                         "Debuggers",
                                                         "Linters",
                                                         "Formatters",
                                                         "Snippets",
                                                         "Testing",
                                                         "Notebooks",
                                                         "Visualization",
                                                         "Keymaps",
                                                         "Other"};

    for (const auto& cat : kCategories)
    {
        auto* chip = new wxButton(chip_scroll,
                                  wxID_ANY,
                                  cat,
                                  wxDefaultPosition,
                                  wxDefaultSize,
                                  wxBORDER_NONE | wxBU_EXACTFIT);
        auto font = theme_engine_.font(core::ThemeFontToken::UISmall);
        chip->SetFont(font);
        chip->SetMinSize(wxSize(-1, 22));

        bool is_active = (cat == "All" && gallery_category_.empty()) || (cat == gallery_category_);
        if (is_active)
        {
            chip->SetBackgroundColour(theme_engine_.color(core::ThemeColorToken::AccentPrimary));
            chip->SetForegroundColour(*wxWHITE);
        }
        else
        {
            chip->SetBackgroundColour(
                theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(105));
            chip->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));
        }

        chip->Bind(wxEVT_BUTTON,
                   [this, cat](wxCommandEvent& /*evt*/)
                   {
                       gallery_category_ = (cat == "All") ? "" : cat;
                       if (search_ctrl_ && !search_ctrl_->GetValue().empty())
                       {
                           SearchExtensions(search_ctrl_->GetValue().ToStdString());
                       }
                       // Recreate layout to update chip styling
                       Layout();
                   });

        chip_sizer->Add(chip, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);
    }
    chip_sizer->AddSpacer(4);

    toolbar_sizer->Add(chip_scroll, 0, wxEXPAND | wxTOP, 4);

    header->set_toolbar(toolbar_panel);

    // Scrollable section list
    card_scroll_ = new wxScrolledWindow(this, wxID_ANY);
    card_scroll_->SetScrollRate(0, 10);
    card_sizer_ = new wxBoxSizer(wxVERTICAL);
    card_scroll_->SetSizer(card_sizer_);

    main_sizer->Add(card_scroll_, 1, wxEXPAND);

    // 1. Installed Section
    installed_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, nullptr, "INSTALLED");
    auto* inst_panel = new wxPanel(installed_section_);
    installed_card_sizer_ = new wxBoxSizer(wxVERTICAL);
    inst_panel->SetSizer(installed_card_sizer_);
    installed_section_->set_content(inst_panel);
    card_sizer_->Add(installed_section_, 0, wxEXPAND);

    // 2. Recommended Section
    recommended_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, nullptr, "RECOMMENDED");
    auto* rec_panel = new wxPanel(recommended_section_);
    recommended_card_sizer_ = new wxBoxSizer(wxVERTICAL);
    rec_panel->SetSizer(recommended_card_sizer_);
    recommended_section_->set_content(rec_panel);
    card_sizer_->Add(recommended_section_, 0, wxEXPAND);

    // 3. Search / Marketplace Section
    search_section_ =
        new SidebarSection(card_scroll_, ds_, icon_manager_, event_bus_, nullptr, "MARKETPLACE");
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

    // Phase 20 Task 11: @filter prefix parsing
    if (query == "@installed")
    {
        ShowInstalledExtensions();
        return;
    }
    if (query == "@enabled")
    {
        // Show only enabled installed extensions
        ClearSearchCards();
        ClearInstalledCards();
        installed_extensions_ = mgmt_service_.get_installed();
        for (const auto& ext : installed_extensions_)
        {
            if (!ext.enabled)
            {
                continue;
            }
            const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
            auto* card = new ExtensionCard(card_scroll_,
                                           theme_engine_,
                                           icon_manager_,
                                           ext_id,
                                           ext.manifest.name,
                                           ext.manifest.publisher,
                                           ext.manifest.version,
                                           ext.manifest.description,
                                           ExtensionCard::State::Installed);
            card->SetOnClick([this](const std::string& cid) { OnCardClicked(cid); });
            card->SetOnAction([this](const std::string& cid, ExtensionCard::State /*s*/)
                              { OnCardAction(cid, true); });
            card->ApplyTheme(theme_engine_);
            installed_cards_.push_back(card);
            if (installed_card_sizer_ != nullptr)
            {
                installed_card_sizer_->Add(card, 0, wxEXPAND);
            }
        }
        Layout();
        return;
    }
    if (query == "@disabled")
    {
        ClearSearchCards();
        ClearInstalledCards();
        installed_extensions_ = mgmt_service_.get_installed();
        for (const auto& ext : installed_extensions_)
        {
            if (ext.enabled)
            {
                continue;
            }
            const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
            auto* card = new ExtensionCard(card_scroll_,
                                           theme_engine_,
                                           icon_manager_,
                                           ext_id,
                                           ext.manifest.name,
                                           ext.manifest.publisher,
                                           ext.manifest.version,
                                           ext.manifest.description,
                                           ExtensionCard::State::Installed);
            card->SetOnClick([this](const std::string& cid) { OnCardClicked(cid); });
            card->SetOnAction([this](const std::string& cid, ExtensionCard::State /*s*/)
                              { OnCardAction(cid, true); });
            card->ApplyTheme(theme_engine_);
            installed_cards_.push_back(card);
            if (installed_card_sizer_ != nullptr)
            {
                installed_card_sizer_->Add(card, 0, wxEXPAND);
            }
        }
        Layout();
        return;
    }
    if (query == "@outdated")
    {
        // Trigger update check and show results
        auto updates = mgmt_service_.check_updates();
        if (updates.has_value() && !updates.value().empty())
        {
            ClearSearchCards();
            for (const auto& upd : updates.value())
            {
                auto* card = new ExtensionCard(card_scroll_,
                                               theme_engine_,
                                               icon_manager_,
                                               upd.extension_id,
                                               upd.gallery_entry.display_name,
                                               upd.gallery_entry.publisher_display,
                                               upd.available_version,
                                               upd.gallery_entry.description,
                                               ExtensionCard::State::UpdateAvailable);
                card->SetUpdateVersion(upd.current_version, upd.available_version);
                card->SetOnClick([this](const std::string& cid) { OnCardClicked(cid); });
                card->SetOnAction([this](const std::string& cid, ExtensionCard::State /*s*/)
                                  { OnCardAction(cid, true); });
                card->ApplyTheme(theme_engine_);
                search_cards_.push_back(card);
                if (search_card_sizer_ != nullptr)
                {
                    search_card_sizer_->Add(card, 0, wxEXPAND);
                }
            }
            if (search_section_ != nullptr)
            {
                search_section_->Show();
            }
            Layout();
        }
        return;
    }

    // Default: gallery search
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
        // Phase 20 Task 6: Uninstall confirmation dialog
        wxString msg;
        msg << "Uninstall \"" << extension_id << "\"?\n\n"
            << "This will remove:\n"
            << "  \u2022 Extension files\n"
            << "  \u2022 Extension settings\n"
            << "  \u2022 Extension data\n";

        int result =
            wxMessageBox(msg, "Confirm Uninstall", wxYES_NO | wxNO_DEFAULT | wxICON_WARNING, this);
        if (result != wxYES)
        {
            return;
        }

        auto uninstall_result = mgmt_service_.uninstall(extension_id);
        if (uninstall_result.has_value())
        {
            spdlog::info("Extension uninstalled: {}", extension_id);
        }
        else
        {
            spdlog::error("Failed to uninstall {}: {}", extension_id, uninstall_result.error());
        }
    }
    else
    {
        // Phase 20 Task 14: For gallery installs, check for deps and install
        auto gallery_result = gallery_service_.get_extensions({extension_id});
        if (gallery_result.has_value() && !gallery_result.value().empty())
        {
            const auto& gallery_ext = gallery_result.value().front();

            // Attempt install (ExtensionManagementService handles dep resolution)
            auto install_result = mgmt_service_.install_from_gallery(gallery_ext);
            if (install_result.has_value())
            {
                spdlog::info("Extension installed: {}", extension_id);
            }
            else
            {
                spdlog::error("Failed to install {}: {}", extension_id, install_result.error());
                wxMessageBox("Failed to install extension:\n" + install_result.error(),
                             "Install Error",
                             wxOK | wxICON_ERROR,
                             this);
            }
        }
        else
        {
            spdlog::error("Extension not found in gallery: {}", extension_id);
            wxMessageBox("Extension not found in gallery: " + extension_id,
                         "Not Found",
                         wxOK | wxICON_WARNING,
                         this);
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
    category_menu->AppendRadioItem(2002, "Languages")->Check(gallery_category_ == "Languages");
    category_menu->AppendRadioItem(2003, "Debuggers")->Check(gallery_category_ == "Debuggers");
    category_menu->AppendRadioItem(2004, "Linters")->Check(gallery_category_ == "Linters");
    category_menu->AppendRadioItem(2005, "Formatters")->Check(gallery_category_ == "Formatters");
    category_menu->AppendRadioItem(2006, "Snippets")->Check(gallery_category_ == "Snippets");
    category_menu->AppendRadioItem(2007, "Testing")->Check(gallery_category_ == "Testing");
    category_menu->AppendRadioItem(2008, "Notebooks")->Check(gallery_category_ == "Notebooks");
    category_menu->AppendRadioItem(2009, "Visualization")
        ->Check(gallery_category_ == "Visualization");
    category_menu->AppendRadioItem(2010, "Keymaps")->Check(gallery_category_ == "Keymaps");
    category_menu->AppendRadioItem(2011, "Other")->Check(gallery_category_ == "Other");

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
                          gallery_category_ = "Languages";
                          changed = true;
                          break;
                      case 2003:
                          gallery_category_ = "Debuggers";
                          changed = true;
                          break;
                      case 2004:
                          gallery_category_ = "Linters";
                          changed = true;
                          break;
                      case 2005:
                          gallery_category_ = "Formatters";
                          changed = true;
                          break;
                      case 2006:
                          gallery_category_ = "Snippets";
                          changed = true;
                          break;
                      case 2007:
                          gallery_category_ = "Testing";
                          changed = true;
                          break;
                      case 2008:
                          gallery_category_ = "Notebooks";
                          changed = true;
                          break;
                      case 2009:
                          gallery_category_ = "Visualization";
                          changed = true;
                          break;
                      case 2010:
                          gallery_category_ = "Keymaps";
                          changed = true;
                          break;
                      case 2011:
                          gallery_category_ = "Other";
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

auto ExtensionsBrowserPanel::GetVisibleCards() const -> const std::vector<ExtensionCard*>&
{
    // Return whichever card list is currently active
    if (!search_cards_.empty())
    {
        return search_cards_;
    }
    return installed_cards_;
}

void ExtensionsBrowserPanel::OnKeyDown(wxKeyEvent& event)
{
    const auto& cards = GetVisibleCards();
    if (cards.empty())
    {
        event.Skip();
        return;
    }

    const int key = event.GetKeyCode();
    const int card_count = static_cast<int>(cards.size());

    switch (key)
    {
        case WXK_DOWN:
        {
            focused_card_index_ = std::min(focused_card_index_ + 1, card_count - 1);
            if (focused_card_index_ >= 0 && focused_card_index_ < card_count)
            {
                cards[static_cast<size_t>(focused_card_index_)]->SetFocus();
                // Scroll to make the focused card visible
                if (card_scroll_ != nullptr)
                {
                    int scroll_x = 0;
                    int scroll_y = 0;
                    card_scroll_->GetViewStart(&scroll_x, &scroll_y);
                    auto pos = cards[static_cast<size_t>(focused_card_index_)]->GetPosition();
                    card_scroll_->Scroll(-1, pos.y / 10);
                }
            }
            break;
        }
        case WXK_UP:
        {
            focused_card_index_ = std::max(focused_card_index_ - 1, 0);
            if (focused_card_index_ < card_count)
            {
                cards[static_cast<size_t>(focused_card_index_)]->SetFocus();
                if (card_scroll_ != nullptr)
                {
                    auto pos = cards[static_cast<size_t>(focused_card_index_)]->GetPosition();
                    card_scroll_->Scroll(-1, pos.y / 10);
                }
            }
            break;
        }
        case WXK_RETURN:
        case WXK_NUMPAD_ENTER:
        {
            if (focused_card_index_ >= 0 && focused_card_index_ < card_count)
            {
                OnCardClicked(cards[static_cast<size_t>(focused_card_index_)]->GetExtensionId());
            }
            break;
        }
        case WXK_ESCAPE:
        {
            // Return from detail view to list
            if (detail_panel_ != nullptr && detail_panel_->IsShown())
            {
                ShowCardList();
            }
            else
            {
                event.Skip();
            }
            break;
        }
        default:
            event.Skip();
            break;
    }
}

// Phase 20 Task 18: Switch between Installed / Marketplace / Recommended view tabs
void ExtensionsBrowserPanel::SwitchViewTab(int tab_index)
{
    active_view_tab_ = tab_index;

    // Show/hide sections based on active tab
    if (installed_section_ != nullptr)
    {
        installed_section_->Show(tab_index == 0);
    }
    if (trending_section_ != nullptr)
    {
        trending_section_->Show(tab_index == 1);
    }
    if (recommended_section_ != nullptr)
    {
        recommended_section_->Show(tab_index == 2);
    }

    // Tab styling
    auto active_col = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
    auto inactive_col = theme_engine_.color(core::ThemeColorToken::TextMuted);
    if (tab_installed_ != nullptr)
    {
        tab_installed_->SetForegroundColour(tab_index == 0 ? active_col : inactive_col);
    }
    if (tab_marketplace_ != nullptr)
    {
        tab_marketplace_->SetForegroundColour(tab_index == 1 ? active_col : inactive_col);
    }
    if (tab_recommended_ != nullptr)
    {
        tab_recommended_->SetForegroundColour(tab_index == 2 ? active_col : inactive_col);
    }

    // Populate on first switch to Marketplace tab
    if (tab_index == 1 && trending_cards_.empty())
    {
        PopulateTrendingSection();
    }
    // Populate on first switch to Recommended tab
    if (tab_index == 2 && recommended_cards_.empty())
    {
        PopulateRecommendedSection();
    }

    Layout();
    Refresh();
}

// Phase 20 Task 16: Populate trending/popular extensions section
void ExtensionsBrowserPanel::PopulateTrendingSection()
{
    ClearTrendingCards();

    // Phase 20 Task 22: Check marketplace cache validity (5-minute TTL)
    constexpr auto kCacheTtl = std::chrono::minutes(5);
    auto now = std::chrono::steady_clock::now();
    bool cache_valid = !marketplace_cache_.empty() && (now - cache_timestamp_) < kCacheTtl;

    std::vector<core::GalleryExtension> trending;
    if (cache_valid)
    {
        trending = marketplace_cache_;
    }
    else
    {
        // Fetch popular extensions from gallery sorted by install count
        core::GalleryQueryOptions trending_options;
        trending_options.sort_by = core::GallerySortBy::kInstallCount;
        trending_options.page_size = 20;
        auto result = gallery_service_.query(trending_options);
        if (result.has_value())
        {
            trending = result.value().extensions;
            // Update cache
            marketplace_cache_ = trending;
            cache_timestamp_ = now;
        }
    }

    for (const auto& ext : trending)
    {
        auto is_installed = IsExtensionInstalled(ext.identifier);
        auto state =
            is_installed ? ExtensionCard::State::Installed : ExtensionCard::State::NotInstalled;
        auto* card = new ExtensionCard(card_scroll_,
                                       theme_engine_,
                                       icon_manager_,
                                       ext.identifier,
                                       ext.display_name,
                                       ext.publisher_display,
                                       ext.version,
                                       ext.description,
                                       state);
        card->SetOnClick([this](const std::string& cid) { OnCardClicked(cid); });
        card->SetOnAction([this, is_installed](const std::string& cid, ExtensionCard::State /*s*/)
                          { OnCardAction(cid, is_installed); });
        card->ApplyTheme(theme_engine_);
        trending_cards_.push_back(card);
        if (trending_card_sizer_ != nullptr)
        {
            trending_card_sizer_->Add(card, 0, wxEXPAND);
        }
    }

    Layout();
}

// Phase 20 Task 16: Clear trending cards
void ExtensionsBrowserPanel::ClearTrendingCards()
{
    for (auto* card : trending_cards_)
    {
        card->Destroy();
    }
    trending_cards_.clear();
}

} // namespace markamp::ui
