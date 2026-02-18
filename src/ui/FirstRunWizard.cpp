#include "FirstRunWizard.h"

#include "core/Config.h"
#include "core/ConfigProfile.h"
#include "core/ThemeRegistry.h"

#include <wx/listbox.h>
#include <wx/sizer.h>
#include <wx/statline.h>

namespace markamp::ui
{

FirstRunWizard::FirstRunWizard(wxWindow* parent,
                               core::ThemeEngine& theme_engine,
                               core::EventBus& event_bus,
                               core::Config& config,
                               core::ThemeRegistry& theme_registry,
                               core::ConfigProfileManager& profile_manager)
    : wxDialog(parent,
               wxID_ANY,
               "Welcome to MarkAmp",
               wxDefaultPosition,
               wxSize(600, 500),
               wxDEFAULT_DIALOG_STYLE)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
    , config_(config)
    , theme_registry_(theme_registry)
    , profile_manager_(profile_manager)
{
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);

    // Page container
    pages_ = new wxSimplebook(this, wxID_ANY);
    pages_->AddPage(CreateWelcomePage(), "Welcome");
    pages_->AddPage(CreateThemePage(), "Theme");
    pages_->AddPage(CreateProfilePage(), "Profile");
    pages_->AddPage(CreateExtensionsPage(), "Extensions");
    pages_->SetSelection(0);
    main_sizer->Add(pages_, 1, wxEXPAND | wxALL, 16);

    // Separator
    main_sizer->Add(new wxStaticLine(this), 0, wxEXPAND | wxLEFT | wxRIGHT, 16);

    // Navigation buttons
    auto* nav_sizer = new wxBoxSizer(wxHORIZONTAL);
    back_btn_ = new wxButton(this, wxID_ANY, "← Back");
    back_btn_->Bind(wxEVT_BUTTON, &FirstRunWizard::OnBack, this);
    nav_sizer->Add(back_btn_, 0, wxRIGHT, 8);

    nav_sizer->AddStretchSpacer();

    next_btn_ = new wxButton(this, wxID_ANY, "Next →");
    next_btn_->Bind(wxEVT_BUTTON, &FirstRunWizard::OnNext, this);
    nav_sizer->Add(next_btn_, 0, wxRIGHT, 8);

    finish_btn_ = new wxButton(this, wxID_OK, "Get Started!");
    finish_btn_->Bind(wxEVT_BUTTON, &FirstRunWizard::OnFinish, this);
    nav_sizer->Add(finish_btn_, 0);

    main_sizer->Add(nav_sizer, 0, wxEXPAND | wxALL, 16);

    SetSizer(main_sizer);
    UpdateNavButtons();

    // Apply theme colors
    const auto bg_color = theme_engine_.color(core::ThemeColorToken::BgApp);
    const auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    SetBackgroundColour(bg_color);
    SetForegroundColour(fg_color);
}

auto FirstRunWizard::CreateWelcomePage() -> wxPanel*
{
    auto* page = new wxPanel(pages_);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    sizer->AddSpacer(40);

    auto* heading = new wxStaticText(page, wxID_ANY, "Welcome to MarkAmp!");
    auto heading_font = heading->GetFont();
    heading_font.SetPointSize(24);
    heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    heading->SetFont(heading_font);
    heading->SetName("Welcome heading");
    sizer->Add(heading, 0, wxALIGN_CENTER_HORIZONTAL);

    sizer->AddSpacer(20);

    auto* subtitle = new wxStaticText(
        page, wxID_ANY, "Let's set up your workspace. This will only take a moment.");
    subtitle->SetName("Welcome subtitle");
    sizer->Add(subtitle, 0, wxALIGN_CENTER_HORIZONTAL);

    sizer->AddSpacer(30);

    auto* features = new wxStaticText(page,
                                      wxID_ANY,
                                      "✨ Rich Markdown editing with syntax highlighting\n"
                                      "🎨 Customizable themes and color schemes\n"
                                      "🔌 Extensible plugin architecture\n"
                                      "⌨️ Configurable keyboard shortcuts\n"
                                      "📁 Multi-file workspace management");
    features->SetName("Feature list");
    sizer->Add(features, 0, wxALIGN_CENTER_HORIZONTAL);

    page->SetSizer(sizer);
    return page;
}

auto FirstRunWizard::CreateThemePage() -> wxPanel*
{
    auto* page = new wxPanel(pages_);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* heading = new wxStaticText(page, wxID_ANY, "Choose Your Theme");
    auto heading_font = heading->GetFont();
    heading_font.SetPointSize(18);
    heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    heading->SetFont(heading_font);
    sizer->Add(heading, 0, wxBOTTOM, 16);

    auto* desc = new wxStaticText(
        page, wxID_ANY, "Select a color theme. You can change this later in Settings.");
    sizer->Add(desc, 0, wxBOTTOM, 12);

    // Theme list
    wxArrayString theme_names;
    for (const auto& theme_info : theme_registry_.list_themes())
    {
        theme_names.Add(theme_info.name);
    }
    auto* theme_list =
        new wxListBox(page, wxID_ANY, wxDefaultPosition, wxSize(-1, 200), theme_names, wxLB_SINGLE);
    // Select "Dark Modern" by default if present
    const int default_idx = theme_list->FindString("Dark Modern");
    if (default_idx != wxNOT_FOUND)
    {
        theme_list->SetSelection(default_idx);
    }
    else if (!theme_names.IsEmpty())
    {
        theme_list->SetSelection(0);
    }

    theme_list->Bind(wxEVT_LISTBOX,
                     [this, theme_list](wxCommandEvent& /*evt*/)
                     { selected_theme_ = theme_list->GetStringSelection().ToStdString(); });
    sizer->Add(theme_list, 1, wxEXPAND);

    page->SetSizer(sizer);
    return page;
}

auto FirstRunWizard::CreateProfilePage() -> wxPanel*
{
    auto* page = new wxPanel(pages_);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* heading = new wxStaticText(page, wxID_ANY, "Select a Profile");
    auto heading_font = heading->GetFont();
    heading_font.SetPointSize(18);
    heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    heading->SetFont(heading_font);
    sizer->Add(heading, 0, wxBOTTOM, 16);

    auto* desc = new wxStaticText(
        page, wxID_ANY, "Profiles pre-configure settings for different workflows.");
    sizer->Add(desc, 0, wxBOTTOM, 12);

    wxArrayString profile_names;
    profile_names.Add("(Default)");
    for (const auto& pname : profile_manager_.profile_names())
    {
        profile_names.Add(pname);
    }

    auto* profile_list = new wxListBox(
        page, wxID_ANY, wxDefaultPosition, wxSize(-1, 180), profile_names, wxLB_SINGLE);
    profile_list->SetSelection(0);
    profile_list->Bind(wxEVT_LISTBOX,
                       [this, profile_list](wxCommandEvent& /*evt*/)
                       { selected_profile_ = profile_list->GetStringSelection().ToStdString(); });
    sizer->Add(profile_list, 1, wxEXPAND);

    page->SetSizer(sizer);
    return page;
}

auto FirstRunWizard::CreateExtensionsPage() -> wxPanel*
{
    auto* page = new wxPanel(pages_);
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* heading = new wxStaticText(page, wxID_ANY, "Recommended Extensions");
    auto heading_font = heading->GetFont();
    heading_font.SetPointSize(18);
    heading_font.SetWeight(wxFONTWEIGHT_BOLD);
    heading->SetFont(heading_font);
    sizer->Add(heading, 0, wxBOTTOM, 16);

    auto* desc =
        new wxStaticText(page,
                         wxID_ANY,
                         "These built-in extensions enhance your editing experience.\n"
                         "You can enable or disable them at any time in the Extensions panel.");
    sizer->Add(desc, 0, wxBOTTOM, 12);

    // Static extension recommendations
    const std::vector<std::string> extensions = {
        "✅  Mermaid Diagrams — flowcharts, sequences, and more",
        "✅  Table Editor — visual table editing",
        "✅  Format Bar — rich text formatting toolbar",
        "✅  Theme Gallery — browse and preview themes",
        "✅  Link Preview — URL previews on hover",
        "✅  Image Preview — inline image display",
        "✅  Breadcrumb Navigation — document structure breadcrumbs"};

    for (const auto& ext_text : extensions)
    {
        auto* ext_label = new wxStaticText(page, wxID_ANY, ext_text);
        sizer->Add(ext_label, 0, wxBOTTOM, 6);
    }

    page->SetSizer(sizer);
    return page;
}

void FirstRunWizard::OnNext(wxCommandEvent& /*event*/)
{
    if (current_page_ < kPageCount - 1)
    {
        current_page_++;
        pages_->SetSelection(static_cast<size_t>(current_page_));
        UpdateNavButtons();
    }
}

void FirstRunWizard::OnBack(wxCommandEvent& /*event*/)
{
    if (current_page_ > 0)
    {
        current_page_--;
        pages_->SetSelection(static_cast<size_t>(current_page_));
        UpdateNavButtons();
    }
}

void FirstRunWizard::OnFinish(wxCommandEvent& /*event*/)
{
    // Apply selected theme
    if (!selected_theme_.empty())
    {
        theme_engine_.apply_theme(selected_theme_);
    }

    // Apply selected profile
    if (selected_profile_ != "(Default)" && !selected_profile_.empty())
    {
        profile_manager_.apply_profile(selected_profile_, config_);
    }

    // Mark first run as completed
    config_.set("app.first_run_completed", true);
    (void)config_.save();

    EndModal(wxID_OK);
}

void FirstRunWizard::UpdateNavButtons()
{
    if (back_btn_ != nullptr)
    {
        back_btn_->Enable(current_page_ > 0);
    }
    if (next_btn_ != nullptr)
    {
        next_btn_->Show(current_page_ < kPageCount - 1);
    }
    if (finish_btn_ != nullptr)
    {
        finish_btn_->Show(current_page_ == kPageCount - 1);
    }
    Layout();
}

} // namespace markamp::ui
