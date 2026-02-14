#include "OutputPanel.h"

#include <wx/button.h>
#include <wx/sizer.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

OutputPanel::OutputPanel()
    : wxPanel()
{
}

// ── UI constructor ──

OutputPanel::OutputPanel(wxWindow* parent, core::OutputChannelService* service)
    : wxPanel(parent, wxID_ANY)
    , service_(service)
{
    CreateLayout();
    if (service_ != nullptr)
    {
        auto names = service_->channel_names();
        if (!names.empty())
        {
            active_channel_ = names.front();
        }
    }
    RefreshContent();
}

void OutputPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Top bar: channel selector + buttons ──
    auto* top_bar = new wxBoxSizer(wxHORIZONTAL);

    channel_selector_ = new wxChoice(this, wxID_ANY);
    top_bar->Add(channel_selector_, 1, wxEXPAND | wxRIGHT, 4);

    auto* clear_btn =
        new wxButton(this, wxID_ANY, "Clear", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    clear_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { clear_active_channel(); });
    top_bar->Add(clear_btn, 0, wxRIGHT, 4);

    auto* lock_btn =
        new wxButton(this, wxID_ANY, "Lock", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    lock_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { auto_scroll_ = !auto_scroll_; });
    top_bar->Add(lock_btn, 0);

    sizer->Add(top_bar, 0, wxEXPAND | wxALL, 4);

    // ── Text display area ──
    text_area_ = new wxTextCtrl(this,
                                wxID_ANY,
                                wxEmptyString,
                                wxDefaultPosition,
                                wxDefaultSize,
                                wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH2 | wxBORDER_NONE);
    text_area_->SetFont(wxFont(wxFontInfo(11).Family(wxFONTFAMILY_TELETYPE).FaceName("Menlo")));
    sizer->Add(text_area_, 1, wxEXPAND);

    SetSizer(sizer);

    // Wire channel selector
    channel_selector_->Bind(wxEVT_CHOICE, [this](wxCommandEvent& evt) { OnChannelChanged(evt); });
}

void OutputPanel::OnChannelChanged(wxCommandEvent& /*event*/)
{
    if (channel_selector_ == nullptr)
    {
        return;
    }
    const int sel = channel_selector_->GetSelection();
    if (sel != wxNOT_FOUND)
    {
        active_channel_ =
            channel_selector_->GetString(static_cast<unsigned int>(sel)).ToStdString();
        RefreshContent();
    }
}

void OutputPanel::RefreshContent()
{
    if (text_area_ == nullptr)
    {
        return;
    }

    // Update channel selector items
    if (channel_selector_ != nullptr && service_ != nullptr)
    {
        auto names = service_->channel_names();
        channel_selector_->Clear();
        int active_idx = 0;
        for (std::size_t idx = 0; idx < names.size(); ++idx)
        {
            channel_selector_->Append(wxString(names[idx]));
            if (names[idx] == active_channel_)
            {
                active_idx = static_cast<int>(idx);
            }
        }
        if (!names.empty())
        {
            channel_selector_->SetSelection(active_idx);
        }
    }

    // Update text content
    text_area_->SetValue(wxString(active_content()));

    if (auto_scroll_)
    {
        text_area_->ShowPosition(text_area_->GetLastPosition());
    }
}

void OutputPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (text_area_ != nullptr)
    {
        text_area_->SetBackgroundColour(bg_colour);
        text_area_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void OutputPanel::set_service(core::OutputChannelService* service)
{
    service_ = service;
    if (service_ != nullptr)
    {
        auto names = service_->channel_names();
        if (!names.empty() && active_channel_.empty())
        {
            active_channel_ = names.front();
        }
    }
}

auto OutputPanel::active_channel() const -> const std::string&
{
    return active_channel_;
}

void OutputPanel::set_active_channel(const std::string& channel_name)
{
    active_channel_ = channel_name;
}

auto OutputPanel::channel_names() const -> std::vector<std::string>
{
    return service_ != nullptr ? service_->channel_names() : std::vector<std::string>{};
}

auto OutputPanel::active_content() const -> std::string
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return {};
    }
    auto* channel = service_->get_channel(active_channel_);
    return channel != nullptr ? channel->content() : std::string{};
}

auto OutputPanel::auto_scroll() const -> bool
{
    return auto_scroll_;
}

void OutputPanel::set_auto_scroll(bool enabled)
{
    auto_scroll_ = enabled;
}

void OutputPanel::clear_active_channel()
{
    if (service_ == nullptr || active_channel_.empty())
    {
        return;
    }
    auto* channel = service_->get_channel(active_channel_);
    if (channel != nullptr)
    {
        channel->clear();
    }
    RefreshContent();
}

void OutputPanel::lock_scroll()
{
    auto_scroll_ = false;
}

void OutputPanel::unlock_scroll()
{
    auto_scroll_ = true;
}

} // namespace markamp::ui
