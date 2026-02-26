#include "OutputPanel.h"

#include "PanelContainer.h"

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
    CreateLayout(parent);
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

void OutputPanel::CreateLayout(wxWindow* parent)
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    auto* panel_container = dynamic_cast<PanelContainer*>(parent->GetParent());
    wxWindow* toolbar_parent = panel_container ? panel_container->GetActionToolbarArea() : this;

    toolbar_ = new wxWindow(toolbar_parent, wxID_ANY);
    auto* top_bar = new wxBoxSizer(wxHORIZONTAL);

    channel_selector_ = new wxChoice(toolbar_, wxID_ANY);
    top_bar->Add(channel_selector_, 1, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    auto* clear_btn =
        new wxButton(toolbar_, wxID_ANY, "Clear", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    clear_btn->SetToolTip("Clear output console");
    clear_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { clear_active_channel(); });
    top_bar->Add(clear_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);

    auto* lock_btn =
        new wxButton(toolbar_, wxID_ANY, "Lock", wxDefaultPosition, wxDefaultSize, wxBU_EXACTFIT);
    lock_btn->SetToolTip("Toggle auto-scroll lock");
    lock_btn->Bind(wxEVT_BUTTON, [this](wxCommandEvent& /*evt*/) { auto_scroll_ = !auto_scroll_; });
    top_bar->Add(lock_btn, 0, wxALIGN_CENTER_VERTICAL);

    toolbar_->SetSizer(top_bar);

    if (panel_container)
    {
        panel_container->RegisterActionToolbar("output", toolbar_);
    }
    else
    {
        sizer->Add(toolbar_, 0, wxEXPAND | wxALL, 4);
    }

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
    const wxString full_text = wxString::FromUTF8(active_content());
    text_area_->SetValue(full_text);

    // Phase 06 Task 47: Error highlighting for logs
    size_t last_pos = 0;
    while (true)
    {
        const size_t err_pos = full_text.find("[ERROR]", last_pos);
        if (err_pos == wxString::npos)
        {
            break;
        }
        size_t end_line = full_text.find('\n', err_pos);
        if (end_line == wxString::npos)
        {
            end_line = full_text.length();
        }

        wxTextAttr error_style;
        error_style.SetTextColour(wxColour(235, 87, 87)); // Red color
        text_area_->SetStyle(static_cast<long>(err_pos), static_cast<long>(end_line), error_style);

        last_pos = end_line;
    }

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
