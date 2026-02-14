#include "ExtensionsBrowserPanel.h"

#include "ExtensionCard.h"
#include "ExtensionDetailPanel.h"
#include "core/Events.h"

#include <spdlog/spdlog.h>

namespace markamp::ui
{

ExtensionsBrowserPanel::ExtensionsBrowserPanel(wxWindow* parent,
                                               core::ThemeEngine& theme_engine,
                                               core::EventBus& event_bus,
                                               core::IExtensionManagementService& mgmt_service,
                                               core::IExtensionGalleryService& gallery_service)
    : wxPanel(parent, wxID_ANY)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , mgmt_service_(mgmt_service)
    , gallery_service_(gallery_service)
{
    CreateLayout();
    ApplyTheme();

    // Subscribe to extension events to auto-refresh
    install_sub_ = event_bus_.subscribe<core::events::ExtensionInstalledEvent>(
        [this]([[maybe_unused]] const core::events::ExtensionInstalledEvent& evt)
        {
            // Refresh installed list after an installation
            if (view_mode_ == ViewMode::kInstalled)
            {
                ShowInstalledExtensions();
            }
        });

    uninstall_sub_ = event_bus_.subscribe<core::events::ExtensionUninstalledEvent>(
        [this]([[maybe_unused]] const core::events::ExtensionUninstalledEvent& evt)
        {
            // Refresh installed list after an uninstall
            if (view_mode_ == ViewMode::kInstalled)
            {
                ShowInstalledExtensions();
            }
        });

    // Start with installed view
    ShowInstalledExtensions();
}

void ExtensionsBrowserPanel::CreateLayout()
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Search bar
    auto* search_sizer = new wxBoxSizer(wxHORIZONTAL);
    search_ctrl_ = new wxSearchCtrl(
        this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, kSearchBarHeight));
    search_ctrl_->SetDescriptiveText("Search extensions\u2026");
    search_ctrl_->ShowCancelButton(true);
    search_ctrl_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular));

    search_ctrl_->Bind(wxEVT_TEXT, [this](wxCommandEvent& evt) { OnSearchChanged(evt); });

    search_ctrl_->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           search_ctrl_->Clear();
                           OnTabClicked(ViewMode::kInstalled);
                       });

    search_sizer->AddSpacer(8);
    search_sizer->Add(search_ctrl_, 1, wxALIGN_CENTER_VERTICAL);
    search_sizer->AddSpacer(8);
    main_sizer->Add(search_sizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 6);

    // Tab bar: "Installed" | "Search Results"
    tab_bar_ = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxSize(-1, kTabBarHeight));
    auto* tab_sizer = new wxBoxSizer(wxHORIZONTAL);

    installed_tab_ = new wxButton(
        tab_bar_, wxID_ANY, "Installed", wxDefaultPosition, wxSize(-1, 26), wxBORDER_NONE);
    installed_tab_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    installed_tab_->Bind(wxEVT_BUTTON,
                         [this](wxCommandEvent& /*evt*/) { OnTabClicked(ViewMode::kInstalled); });

    search_tab_ = new wxButton(
        tab_bar_, wxID_ANY, "Search Results", wxDefaultPosition, wxSize(-1, 26), wxBORDER_NONE);
    search_tab_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    search_tab_->Bind(wxEVT_BUTTON,
                      [this](wxCommandEvent& /*evt*/) { OnTabClicked(ViewMode::kSearchResults); });

    tab_sizer->AddSpacer(8);
    tab_sizer->Add(installed_tab_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    tab_sizer->Add(search_tab_, 0, wxALIGN_CENTER_VERTICAL);
    tab_bar_->SetSizer(tab_sizer);

    main_sizer->Add(tab_bar_, 0, wxEXPAND);

    // Scrollable card list
    card_scroll_ = new wxScrolledWindow(this, wxID_ANY);
    card_scroll_->SetScrollRate(0, 10);
    card_sizer_ = new wxBoxSizer(wxVERTICAL);
    card_scroll_->SetSizer(card_sizer_);

    main_sizer->Add(card_scroll_, 1, wxEXPAND);

    // Detail panel (initially hidden)
    detail_panel_ = new ExtensionDetailPanel(this, theme_engine_, event_bus_);
    detail_panel_->Hide();

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
                ShowInstalledExtensions();
                ShowCardList();
            }
            else
            {
                spdlog::error("Failed to update extension {}: {}", ext_id, result.error());
            }
        });

    main_sizer->Add(detail_panel_, 1, wxEXPAND);

    SetSizer(main_sizer);
    UpdateTabStyles();
}

void ExtensionsBrowserPanel::ShowInstalledExtensions()
{
    view_mode_ = ViewMode::kInstalled;
    installed_extensions_ = mgmt_service_.get_installed();
    ClearCards();
    PopulateInstalledCards();
    UpdateTabStyles();
}

void ExtensionsBrowserPanel::SearchExtensions(const std::string& query)
{
    if (query.empty())
    {
        ShowInstalledExtensions();
        return;
    }

    view_mode_ = ViewMode::kSearchResults;

    core::GalleryQueryOptions options;
    options.filters.push_back({core::GalleryFilterType::kSearchText, query});
    options.page_size = 20;

    auto result = gallery_service_.query(options);
    if (result.has_value())
    {
        ClearCards();
        PopulateSearchCards(result.value().extensions);
    }
    else
    {
        spdlog::warn("Gallery search failed: {}", result.error());
        ClearCards();
    }

    UpdateTabStyles();
}

void ExtensionsBrowserPanel::ClearCards()
{
    card_sizer_->Clear(true); // Destroy children
    cards_.clear();
}

void ExtensionsBrowserPanel::PopulateInstalledCards()
{
    for (const auto& ext : installed_extensions_)
    {
        const auto ext_id = ext.manifest.publisher + "." + ext.manifest.name;
        auto* card = new ExtensionCard(card_scroll_,
                                       theme_engine_,
                                       ext_id,
                                       ext.manifest.name,
                                       ext.manifest.publisher,
                                       ext.manifest.version,
                                       ext.manifest.description,
                                       ExtensionCard::State::Installed);

        card->SetOnClick([this](const std::string& card_ext_id) { OnCardClicked(card_ext_id); });

        card->SetOnAction(
            [this](const std::string& card_ext_id, [[maybe_unused]] ExtensionCard::State card_state)
            { OnCardAction(card_ext_id, true); });

        card_sizer_->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2);
        cards_.push_back(card);
    }

    card_scroll_->FitInside();
    card_scroll_->Layout();
}

void ExtensionsBrowserPanel::PopulateSearchCards(const std::vector<core::GalleryExtension>& results)
{
    // Refresh installed list for comparison
    installed_extensions_ = mgmt_service_.get_installed();

    for (const auto& gallery_ext : results)
    {
        bool installed = IsExtensionInstalled(gallery_ext.identifier);
        auto state =
            installed ? ExtensionCard::State::Installed : ExtensionCard::State::NotInstalled;

        auto display_name =
            gallery_ext.display_name.empty() ? gallery_ext.identifier : gallery_ext.display_name;

        auto* card = new ExtensionCard(card_scroll_,
                                       theme_engine_,
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

        card_sizer_->Add(card, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, 2);
        cards_.push_back(card);
    }

    card_scroll_->FitInside();
    card_scroll_->Layout();
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

void ExtensionsBrowserPanel::OnTabClicked(ViewMode mode)
{
    if (mode == ViewMode::kInstalled)
    {
        ShowInstalledExtensions();
        ShowCardList();
    }
    else
    {
        // If there's search text, re-search; otherwise just switch tab style
        auto query = search_ctrl_->GetValue().ToStdString();
        if (!query.empty())
        {
            SearchExtensions(query);
        }
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
    if (view_mode_ == ViewMode::kInstalled)
    {
        ShowInstalledExtensions();
    }
}

void ExtensionsBrowserPanel::ShowCardList()
{
    detail_panel_->Hide();
    card_scroll_->Show();
    tab_bar_->Show();
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
            tab_bar_->Hide();
            search_ctrl_->Hide();
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
        tab_bar_->Hide();
        search_ctrl_->Hide();
        detail_panel_->Show();
        Layout();
    }
}

void ExtensionsBrowserPanel::UpdateTabStyles()
{
    auto active_bg = theme_engine_.color(core::ThemeColorToken::AccentPrimary);
    auto inactive_bg = theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(110);
    auto active_fg = wxColour(255, 255, 255);
    auto inactive_fg = theme_engine_.color(core::ThemeColorToken::TextMuted);

    if (installed_tab_ != nullptr)
    {
        installed_tab_->SetBackgroundColour(view_mode_ == ViewMode::kInstalled ? active_bg
                                                                               : inactive_bg);
        installed_tab_->SetForegroundColour(view_mode_ == ViewMode::kInstalled ? active_fg
                                                                               : inactive_fg);
    }

    if (search_tab_ != nullptr)
    {
        search_tab_->SetBackgroundColour(view_mode_ == ViewMode::kSearchResults ? active_bg
                                                                                : inactive_bg);
        search_tab_->SetForegroundColour(view_mode_ == ViewMode::kSearchResults ? active_fg
                                                                                : inactive_fg);
    }

    if (tab_bar_ != nullptr)
    {
        tab_bar_->Refresh();
    }
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

    if (tab_bar_ != nullptr)
    {
        tab_bar_->SetBackgroundColour(bg_color);
    }

    if (search_ctrl_ != nullptr)
    {
        search_ctrl_->SetBackgroundColour(
            theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        search_ctrl_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMain));
    }

    // Update cards
    for (auto* card : cards_)
    {
        card->ApplyTheme(theme_engine_);
    }

    // Update detail panel
    if (detail_panel_ != nullptr)
    {
        detail_panel_->ApplyTheme();
    }

    UpdateTabStyles();
    Refresh();
}

} // namespace markamp::ui
