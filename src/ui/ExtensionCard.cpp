#include "ExtensionCard.h"

#include "ui/IconManager.h"

#include <wx/dcbuffer.h>

#include <cstdio>

namespace markamp::ui
{

ExtensionCard::ExtensionCard(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             IconManager& icon_manager,
                             const std::string& extension_id,
                             const std::string& name,
                             const std::string& publisher,
                             const std::string& version,
                             const std::string& description,
                             State state)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, kCardHeight))
    , theme_engine_(theme_engine)
    , icon_manager_(icon_manager)
    , extension_id_(extension_id)
    , state_(state)
{
    SetMinSize(wxSize(-1, kCardHeight));
    SetMaxSize(wxSize(-1, kCardHeight));
    CreateLayout(name, publisher, version, description);
    ApplyTheme(theme_engine);

    // Bind mouse events for hover highlighting and click
    Bind(wxEVT_ENTER_WINDOW, &ExtensionCard::OnMouseEnter, this);
    Bind(wxEVT_LEAVE_WINDOW, &ExtensionCard::OnMouseLeave, this);
    Bind(wxEVT_LEFT_UP, &ExtensionCard::OnClick, this);

    // Also bind from child windows so clicks pass through
    if (info_panel_ != nullptr)
    {
        info_panel_->Bind(wxEVT_LEFT_UP, &ExtensionCard::OnClick, this);
        info_panel_->Bind(wxEVT_ENTER_WINDOW, &ExtensionCard::OnMouseEnter, this);
        info_panel_->Bind(wxEVT_LEAVE_WINDOW, &ExtensionCard::OnMouseLeave, this);
    }
}

void ExtensionCard::CreateLayout(const std::string& name,
                                 const std::string& publisher,
                                 const std::string& version,
                                 const std::string& description)
{
    auto* main_sizer = new wxBoxSizer(wxHORIZONTAL);
    main_sizer->AddSpacer(kCardPadding);

    // Icon (left)
    icon_bitmap_ = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
    main_sizer->Add(icon_bitmap_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);

    // Info area (middle, takes most space)
    info_panel_ = new wxPanel(this, wxID_ANY);
    auto* info_sizer = new wxBoxSizer(wxVERTICAL);

    // Row 1: Name (bold) + version
    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);

    name_label_ = new wxStaticText(info_panel_, wxID_ANY, name);
    name_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Bold().Scaled(1.0F));

    version_label_ = new wxStaticText(info_panel_, wxID_ANY, "v" + version);
    version_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.80F));

    header_sizer->Add(name_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    header_sizer->Add(version_label_, 0, wxALIGN_CENTER_VERTICAL);

    info_sizer->Add(header_sizer, 0, wxEXPAND | wxBOTTOM, 2);

    // Row 2: Publisher
    publisher_label_ = new wxStaticText(info_panel_, wxID_ANY, publisher);
    publisher_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    info_sizer->Add(publisher_label_, 0, wxEXPAND | wxBOTTOM, 2);

    // Row 2b: Rating and downloads (Phase 20 Task 2)
    auto* rating_sizer = new wxBoxSizer(wxHORIZONTAL);
    rating_label_ = new wxStaticText(info_panel_, wxID_ANY, "");
    rating_label_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    downloads_label_ = new wxStaticText(info_panel_, wxID_ANY, "");
    downloads_label_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
    rating_sizer->Add(rating_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 12);
    rating_sizer->Add(downloads_label_, 0, wxALIGN_CENTER_VERTICAL);
    info_sizer->Add(rating_sizer, 0, wxEXPAND | wxBOTTOM, 2);

    // Row 3: Description (truncated)
    auto truncated_desc = description;
    constexpr size_t kMaxDescLen = 80;
    if (truncated_desc.length() > kMaxDescLen)
    {
        truncated_desc = truncated_desc.substr(0, kMaxDescLen) + "…";
    }

    description_label_ = new wxStaticText(info_panel_, wxID_ANY, truncated_desc);
    description_label_->SetFont(
        theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    info_sizer->Add(description_label_, 0, wxEXPAND);

    info_panel_->SetSizer(info_sizer);

    main_sizer->Add(info_panel_, 1, wxALIGN_CENTER_VERTICAL | wxTOP | wxBOTTOM, 8);

    // Action button (right side)
    action_button_ =
        new wxButton(this, wxID_ANY, "", wxDefaultPosition, wxSize(80, 28), wxBORDER_NONE);
    action_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));
    UpdateActionButton();

    action_button_->Bind(wxEVT_BUTTON,
                         [this](wxCommandEvent& /*evt*/)
                         {
                             if (on_action_)
                             {
                                 on_action_(extension_id_, state_);
                             }
                         });

    // Phase 20 Task 8: Settings gear button (visible for installed extensions)
    gear_button_ = new wxButton(
        this, wxID_ANY, "\xE2\x9A\x99", wxDefaultPosition, wxSize(28, 28), wxBORDER_NONE);
    gear_button_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(1.0F));
    gear_button_->SetToolTip("Extension Settings");
    gear_button_->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           if (on_settings_)
                           {
                               on_settings_(extension_id_);
                           }
                       });
    gear_button_->Show(state_ == State::Installed || state_ == State::UpdateAvailable);

    main_sizer->Add(gear_button_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
    main_sizer->Add(action_button_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, kCardPadding);

    action_button_->Bind(wxEVT_SET_FOCUS,
                         [this](wxFocusEvent& evt)
                         {
                             auto base_col = action_button_->GetBackgroundColour();
                             action_button_->SetBackgroundColour(base_col.ChangeLightness(140));
                             action_button_->Refresh();
                             evt.Skip();
                         });

    action_button_->Bind(wxEVT_KILL_FOCUS,
                         [this](wxFocusEvent& evt)
                         {
                             UpdateActionButton();
                             evt.Skip();
                         });

    SetSizer(main_sizer);
}

void ExtensionCard::UpdateActionButton()
{
    switch (state_)
    {
        case State::NotInstalled:
        {
            action_button_->SetLabel("Install");
            action_button_->SetBackgroundColour(
                theme_engine_.color(core::ThemeColorToken::AccentPrimary));
            action_button_->SetForegroundColour(wxColour(255, 255, 255));
            break;
        }
        case State::Installed:
        {
            action_button_->SetLabel("Uninstall");
            action_button_->SetBackgroundColour(
                theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
            action_button_->SetForegroundColour(
                theme_engine_.color(core::ThemeColorToken::TextMain));
            break;
        }
        case State::UpdateAvailable:
        {
            action_button_->SetLabel("Update");
            action_button_->SetBackgroundColour(
                theme_engine_.color(core::ThemeColorToken::AccentPrimary));
            action_button_->SetForegroundColour(wxColour(255, 255, 255));
            break;
        }
        case State::Installing:
        {
            action_button_->SetLabel("Installing");
            action_button_->SetBackgroundColour(
                theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(100));
            action_button_->SetForegroundColour(
                theme_engine_.color(core::ThemeColorToken::TextMuted));
            action_button_->Disable();
            break;
        }
    }
}

void ExtensionCard::SetOnClick(std::function<void(const std::string&)> callback)
{
    on_click_ = std::move(callback);
}

void ExtensionCard::SetOnAction(std::function<void(const std::string&, State)> callback)
{
    on_action_ = std::move(callback);
}

void ExtensionCard::ApplyTheme(core::ThemeEngine& theme_engine)
{
    auto bg_color = theme_engine.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);
    if (info_panel_ != nullptr)
    {
        info_panel_->SetBackgroundColour(bg_color);
    }

    if (icon_bitmap_ != nullptr)
    {
        auto icon_color = theme_engine.color(core::ThemeColorToken::TextMuted);
        icon_bitmap_->SetBitmap(
            icon_manager_.get_icon_bitmap("extensions", wxSize(36, 36), icon_color));
    }

    if (name_label_ != nullptr)
    {
        name_label_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMain));
    }
    if (publisher_label_ != nullptr)
    {
        publisher_label_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMuted));
    }
    if (version_label_ != nullptr)
    {
        version_label_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMuted));
    }
    if (description_label_ != nullptr)
    {
        description_label_->SetForegroundColour(
            theme_engine.color(core::ThemeColorToken::TextMuted));
    }

    // Phase 20 Task 24: Theme gear button and rating/downloads labels
    if (gear_button_ != nullptr)
    {
        gear_button_->SetBackgroundColour(bg_color);
        gear_button_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMuted));
    }
    if (rating_label_ != nullptr)
    {
        rating_label_->SetForegroundColour(
            theme_engine.color(core::ThemeColorToken::AccentPrimary));
    }
    if (downloads_label_ != nullptr)
    {
        downloads_label_->SetForegroundColour(theme_engine.color(core::ThemeColorToken::TextMuted));
    }

    // Phase 20 Task 25: Accessibility names for screen readers
    SetName("Extension: " + extension_id_);
    if (action_button_ != nullptr)
    {
        action_button_->SetName("Action for " + extension_id_);
    }
    if (gear_button_ != nullptr)
    {
        gear_button_->SetName("Settings for " + extension_id_);
    }

    UpdateActionButton();
    Refresh();
}

auto ExtensionCard::GetExtensionId() const -> const std::string&
{
    return extension_id_;
}

void ExtensionCard::SetState(State new_state)
{
    state_ = new_state;
    UpdateActionButton();
    Refresh();
}

void ExtensionCard::OnMouseEnter(wxMouseEvent& /*event*/)
{
    auto hover_color = theme_engine_.color(core::ThemeColorToken::BgPanel).ChangeLightness(115);
    SetBackgroundColour(hover_color);
    if (info_panel_ != nullptr)
    {
        info_panel_->SetBackgroundColour(hover_color);
    }
    Refresh();
}

void ExtensionCard::OnMouseLeave(wxMouseEvent& /*event*/)
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    SetBackgroundColour(bg_color);
    if (info_panel_ != nullptr)
    {
        info_panel_->SetBackgroundColour(bg_color);
    }
    Refresh();
}

void ExtensionCard::OnClick(wxMouseEvent& /*event*/)
{
    if (on_click_)
    {
        on_click_(extension_id_);
    }
}

void ExtensionCard::SetRatingAndDownloads(double rating, int download_count)
{
    if (rating_label_ != nullptr)
    {
        // Build star string: ★ for filled, ☆ for empty
        std::string stars;
        for (int i = 1; i <= 5; ++i)
        {
            if (static_cast<double>(i) <= rating)
            {
                stars += "\xE2\x98\x85"; // ★
            }
            else
            {
                stars += "\xE2\x98\x86"; // ☆
            }
        }
        // Show numeric rating too
        char buf[8];
        std::snprintf(buf, sizeof(buf), " %.1f", rating);
        stars += buf;
        rating_label_->SetLabel(stars);
        rating_label_->SetForegroundColour(wxColour(255, 193, 7)); // Gold/amber
    }

    if (downloads_label_ != nullptr)
    {
        std::string dl_text;
        if (download_count >= 1000000)
        {
            char buf[16];
            std::snprintf(
                buf, sizeof(buf), "%.1fM", static_cast<double>(download_count) / 1000000.0);
            dl_text = buf;
        }
        else if (download_count >= 1000)
        {
            char buf[16];
            std::snprintf(buf, sizeof(buf), "%.1fK", static_cast<double>(download_count) / 1000.0);
            dl_text = buf;
        }
        else
        {
            dl_text = std::to_string(download_count);
        }
        dl_text = "\xE2\x87\xA9 " + dl_text; // ⇩ prefix
        downloads_label_->SetLabel(dl_text);
    }
}

void ExtensionCard::SetUpdateVersion(const std::string& current_ver, const std::string& new_ver)
{
    if (update_version_label_ == nullptr)
    {
        update_version_label_ = new wxStaticText(info_panel_, wxID_ANY, "");
        update_version_label_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        update_version_label_->SetForegroundColour(
            theme_engine_.color(core::ThemeColorToken::AccentPrimary));
        auto* sizer = info_panel_->GetSizer();
        if (sizer != nullptr)
        {
            sizer->Add(update_version_label_, 0, wxEXPAND | wxBOTTOM, 2);
            info_panel_->Layout();
        }
    }

    std::string text = "v";
    text += current_ver;
    text += " \xE2\x86\x92 v"; // → arrow
    text += new_ver;
    update_version_label_->SetLabel(text);
    update_version_label_->Show();
}

void ExtensionCard::SetInstallProgress(int percent, const std::string& status_text)
{
    if (progress_gauge_ == nullptr)
    {
        progress_gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 6));
        progress_label_ = new wxStaticText(this, wxID_ANY, "");
        progress_label_->SetFont(theme_engine_.font(core::ThemeFontToken::UISmall));
        progress_label_->SetForegroundColour(theme_engine_.color(core::ThemeColorToken::TextMuted));

        auto* sizer = GetSizer();
        if (sizer != nullptr)
        {
            sizer->Add(progress_label_, 0, wxEXPAND | wxLEFT | wxRIGHT, kCardPadding);
            sizer->Add(progress_gauge_, 0, wxEXPAND | wxLEFT | wxRIGHT | wxBOTTOM, kCardPadding);
            Layout();
        }
    }

    progress_gauge_->SetValue(percent);
    progress_label_->SetLabel(status_text);
    progress_gauge_->Show();
    progress_label_->Show();
    Layout();
}

} // namespace markamp::ui
