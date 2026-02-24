#include "EmptyPanelState.h"

#include "DesignSystemContext.h"
#include "IconManager.h"

#include <wx/sizer.h>

namespace markamp::ui
{

EmptyPanelState::EmptyPanelState(wxWindow* parent, DesignSystemContext& ds, IconManager& im)
    : ThemeAwareWindow(parent, ds.theme, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER)
    , im_(im)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    SetSizer(new wxBoxSizer(wxVERTICAL));
}

void EmptyPanelState::set_icon(const std::string& icon_id)
{
    icon_id_ = icon_id;
    UpdateLayout();
}

void EmptyPanelState::set_message(const std::string& message)
{
    if (message_txt_ == nullptr)
    {
        message_txt_ = new wxStaticText(
            this, wxID_ANY, message, wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
        message_txt_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
        message_txt_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
        message_txt_->Wrap(180); // Ensure long messages wrap
    }
    else
    {
        message_txt_->SetLabel(message);
    }
    UpdateLayout();
}

void EmptyPanelState::set_action(const std::string& label, std::function<void()> callback)
{
    action_callback_ = std::move(callback);
    if (action_btn_ == nullptr)
    {
        action_btn_ = new wxButton(this, wxID_ANY, label);
        action_btn_->Bind(wxEVT_BUTTON,
                          [this](wxCommandEvent&)
                          {
                              if (action_callback_ != nullptr)
                              {
                                  action_callback_();
                              }
                          });
    }
    else
    {
        action_btn_->SetLabel(label);
    }
    UpdateLayout();
}

void EmptyPanelState::UpdateLayout()
{
    auto* sizer = GetSizer();
    sizer->Clear(); // clear without deleting windows

    sizer->AddStretchSpacer(1);

    if (!icon_id_.empty())
    {
        if (icon_bmp_ == nullptr)
        {
            icon_bmp_ = new wxStaticBitmap(this, wxID_ANY, wxNullBitmap);
        }
        auto bmp = im_.get_icon_bitmap(
            icon_id_, wxSize(48, 48), theme_engine().color(core::ThemeColorToken::TextMuted));
        icon_bmp_->SetBitmap(bmp);
        sizer->Add(icon_bmp_, 0, wxALIGN_CENTER | wxBOTTOM, 16);
    }

    if (message_txt_)
    {
        sizer->Add(message_txt_, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 16);
    }

    if (action_btn_ != nullptr)
    {
        sizer->Add(action_btn_, 0, wxALIGN_CENTER | wxBOTTOM, 16);
    }

    sizer->AddStretchSpacer(2);
    Layout();
}

void EmptyPanelState::OnThemeChanged(const core::Theme& /*new_theme*/)
{
    SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
    if (message_txt_)
    {
        message_txt_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
        message_txt_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
    }
    if (icon_bmp_ != nullptr && !icon_id_.empty())
    {
        auto bmp = im_.get_icon_bitmap(
            icon_id_, wxSize(48, 48), theme_engine().color(core::ThemeColorToken::TextMuted));
        icon_bmp_->SetBitmap(bmp);
    }
    Refresh();
}

} // namespace markamp::ui
