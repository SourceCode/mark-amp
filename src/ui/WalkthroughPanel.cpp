#include "WalkthroughPanel.h"

#include <wx/checkbox.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

namespace markamp::ui
{

// ── Data-only constructor (for tests) ──

WalkthroughPanel::WalkthroughPanel()
    : wxPanel()
{
}

// ── UI constructor ──

WalkthroughPanel::WalkthroughPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY)
{
    CreateLayout();
}

void WalkthroughPanel::CreateLayout()
{
    auto* sizer = new wxBoxSizer(wxVERTICAL);

    // ── Progress gauge ──
    progress_gauge_ = new wxGauge(this, wxID_ANY, 100, wxDefaultPosition, wxSize(-1, 16));
    sizer->Add(progress_gauge_, 0, wxEXPAND | wxALL, 4);

    // ── Scrollable step list ──
    scroll_area_ = new wxScrolledWindow(this, wxID_ANY);
    scroll_area_->SetScrollRate(0, 10);
    sizer->Add(scroll_area_, 1, wxEXPAND);

    SetSizer(sizer);
}

void WalkthroughPanel::RefreshContent()
{
    if (scroll_area_ == nullptr)
    {
        return;
    }

    scroll_area_->DestroyChildren();
    auto* step_sizer = new wxBoxSizer(wxVERTICAL);

    // Find the active walkthrough
    const core::ExtensionWalkthrough* active = nullptr;
    for (const auto& walk : walkthroughs_)
    {
        if (walk.walkthrough_id == active_walkthrough_)
        {
            active = &walk;
            break;
        }
    }

    if (active != nullptr)
    {
        for (const auto& step : active->steps)
        {
            auto* row = new wxBoxSizer(wxHORIZONTAL);

            const bool completed = is_step_completed(active->walkthrough_id, step.step_id);
            const wxString check_label = completed ? wxString::FromUTF8("✓ ") : wxString("○ ");

            auto* label =
                new wxStaticText(scroll_area_, wxID_ANY, check_label + wxString(step.title));
            row->Add(label, 1, wxALIGN_CENTER_VERTICAL | wxALL, 4);

            step_sizer->Add(row, 0, wxEXPAND);

            if (!step.description.empty())
            {
                auto* desc =
                    new wxStaticText(scroll_area_, wxID_ANY, wxString("    " + step.description));
                desc->SetForegroundColour(wxColour(150, 150, 150));
                step_sizer->Add(desc, 0, wxEXPAND | wxLEFT, 20);
            }
        }
    }

    scroll_area_->SetSizer(step_sizer);
    scroll_area_->FitInside();

    // Update progress gauge
    if (progress_gauge_ != nullptr && active != nullptr && !active->steps.empty())
    {
        double progress = completion_progress(active->walkthrough_id);
        progress_gauge_->SetValue(static_cast<int>(progress * 100.0));
    }
}

void WalkthroughPanel::ApplyTheme(const wxColour& bg_colour, const wxColour& fg_colour)
{
    SetBackgroundColour(bg_colour);
    if (scroll_area_ != nullptr)
    {
        scroll_area_->SetBackgroundColour(bg_colour);
        scroll_area_->SetForegroundColour(fg_colour);
    }
    Refresh();
}

// ── Data-layer API (unchanged for test compatibility) ──

void WalkthroughPanel::set_walkthroughs(std::vector<core::ExtensionWalkthrough> walkthroughs)
{
    walkthroughs_ = std::move(walkthroughs);
    if (!walkthroughs_.empty() && active_walkthrough_.empty())
    {
        active_walkthrough_ = walkthroughs_.front().walkthrough_id;
    }
}

auto WalkthroughPanel::walkthroughs() const -> const std::vector<core::ExtensionWalkthrough>&
{
    return walkthroughs_;
}

void WalkthroughPanel::complete_step(const std::string& walkthrough_id, const std::string& step_id)
{
    completed_steps_.insert(walkthrough_id + ":" + step_id);
}

auto WalkthroughPanel::is_step_completed(const std::string& walkthrough_id,
                                         const std::string& step_id) const -> bool
{
    return completed_steps_.contains(walkthrough_id + ":" + step_id);
}

auto WalkthroughPanel::completion_progress(const std::string& walkthrough_id) const -> double
{
    for (const auto& walk : walkthroughs_)
    {
        if (walk.walkthrough_id == walkthrough_id && !walk.steps.empty())
        {
            int completed = 0;
            for (const auto& step : walk.steps)
            {
                if (is_step_completed(walkthrough_id, step.step_id))
                {
                    ++completed;
                }
            }
            return static_cast<double>(completed) / static_cast<double>(walk.steps.size());
        }
    }
    return 0.0;
}

auto WalkthroughPanel::active_walkthrough() const -> const std::string&
{
    return active_walkthrough_;
}

void WalkthroughPanel::set_active_walkthrough(const std::string& walkthrough_id)
{
    active_walkthrough_ = walkthrough_id;
}

void WalkthroughPanel::reset_walkthrough(const std::string& walkthrough_id)
{
    auto iter = completed_steps_.begin();
    const std::string prefix = walkthrough_id + ":";
    while (iter != completed_steps_.end())
    {
        if (iter->substr(0, prefix.size()) == prefix)
        {
            iter = completed_steps_.erase(iter);
        }
        else
        {
            ++iter;
        }
    }
}

} // namespace markamp::ui
