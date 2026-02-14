#include "WalkthroughPanel.h"

#include <algorithm>
#include <utility>

namespace markamp::ui
{

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
    auto found = std::find_if(walkthroughs_.begin(),
                              walkthroughs_.end(),
                              [&](const core::ExtensionWalkthrough& walk)
                              { return walk.walkthrough_id == walkthrough_id; });
    if (found == walkthroughs_.end() || found->steps.empty())
    {
        return 0.0;
    }

    int completed = 0;
    for (const auto& step : found->steps)
    {
        if (is_step_completed(walkthrough_id, step.step_id))
        {
            ++completed;
        }
    }
    return static_cast<double>(completed) / static_cast<double>(found->steps.size());
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
    // Remove all completed steps for this walkthrough
    auto found = std::find_if(walkthroughs_.begin(),
                              walkthroughs_.end(),
                              [&](const core::ExtensionWalkthrough& walk)
                              { return walk.walkthrough_id == walkthrough_id; });
    if (found != walkthroughs_.end())
    {
        for (const auto& step : found->steps)
        {
            completed_steps_.erase(walkthrough_id + ":" + step.step_id);
        }
    }
}

} // namespace markamp::ui
