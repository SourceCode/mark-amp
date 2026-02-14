#include "ExtensionCard.h"

#include <wx/dcbuffer.h>

namespace markamp::ui
{

ExtensionCard::ExtensionCard(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             const std::string& extension_id,
                             const std::string& name,
                             const std::string& publisher,
                             const std::string& version,
                             const std::string& description,
                             State state)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(-1, kCardHeight))
    , theme_engine_(theme_engine)
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

    // Info area (left side, takes most space)
    info_panel_ = new wxPanel(this, wxID_ANY);
    auto* info_sizer = new wxBoxSizer(wxVERTICAL);

    // Row 1: Name (bold) + publisher + version
    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);

    name_label_ = new wxStaticText(info_panel_, wxID_ANY, name);
    name_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Bold().Scaled(1.0F));

    publisher_label_ = new wxStaticText(info_panel_, wxID_ANY, publisher);
    publisher_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.85F));

    version_label_ = new wxStaticText(info_panel_, wxID_ANY, "v" + version);
    version_label_->SetFont(theme_engine_.font(core::ThemeFontToken::MonoRegular).Scaled(0.80F));

    header_sizer->Add(name_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    header_sizer->Add(publisher_label_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 6);
    header_sizer->Add(version_label_, 0, wxALIGN_CENTER_VERTICAL);

    info_sizer->Add(header_sizer, 0, wxEXPAND | wxBOTTOM, 2);

    // Row 2: Description (truncated)
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

    main_sizer->AddSpacer(kCardPadding);
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

    main_sizer->Add(action_button_, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, kCardPadding);

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

} // namespace markamp::ui
