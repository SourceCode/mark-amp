#include "LinkPreviewPopover.h"

#include "core/Color.h"
#include "core/Events.h"

#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

LinkPreviewPopover::LinkPreviewPopover(wxWindow* parent,
                                       core::ThemeEngine& theme_engine,
                                       core::EventBus& event_bus)
    : wxPopupTransientWindow(parent, wxBORDER_SIMPLE)
    , theme_engine_(theme_engine)
    , event_bus_(event_bus)
{
    CreateLayout();
    ApplyTheme();

    theme_sub_ = event_bus_.subscribe<core::events::ThemeChangedEvent>(
        [this](const core::events::ThemeChangedEvent& /*evt*/) { ApplyTheme(); });
}

void LinkPreviewPopover::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    link_label_ = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    auto link_font = link_label_->GetFont();
    link_font.SetWeight(wxFONTWEIGHT_BOLD);
    link_label_->SetFont(link_font);

    url_label_ = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxSize(kMaxWidth, -1));
    url_label_->SetMaxSize(wxSize(kMaxWidth, -1));

    open_btn_ = new wxButton(this, wxID_ANY, "Open in browser", wxDefaultPosition, wxDefaultSize);
    open_btn_->Bind(wxEVT_BUTTON, &LinkPreviewPopover::OnOpenInBrowser, this);

    sizer->AddSpacer(kPadding);
    sizer->Add(link_label_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->AddSpacer(4);
    sizer->Add(url_label_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->AddSpacer(6);
    sizer->Add(open_btn_, 0, wxLEFT | wxRIGHT, kPadding);
    sizer->AddSpacer(kPadding);

    SetSizer(sizer);
}

void LinkPreviewPopover::SetLink(const std::string& link_text, const std::string& url)
{
    current_url_ = url;
    link_label_->SetLabel(wxString::FromUTF8(link_text));
    url_label_->SetLabel(wxString::FromUTF8(url));
    url_label_->Wrap(kMaxWidth - 2 * kPadding);

    GetSizer()->Fit(this);
}

void LinkPreviewPopover::ApplyTheme()
{
    auto bg_color = theme_engine_.color(core::ThemeColorToken::BgPanel);
    auto fg_color = theme_engine_.color(core::ThemeColorToken::TextMain);
    auto muted = theme_engine_.color(core::ThemeColorToken::TextMuted);
    auto accent = theme_engine_.color(core::ThemeColorToken::AccentPrimary);

    SetBackgroundColour(bg_color);
    link_label_->SetForegroundColour(fg_color);
    url_label_->SetForegroundColour(accent);
    open_btn_->SetBackgroundColour(bg_color);
    open_btn_->SetForegroundColour(muted);

    Refresh();
}

void LinkPreviewPopover::OnOpenInBrowser(wxCommandEvent& /*event*/)
{
    if (!current_url_.empty())
    {
        wxLaunchDefaultBrowser(wxString::FromUTF8(current_url_));
    }
    Dismiss();
}

} // namespace markamp::ui
