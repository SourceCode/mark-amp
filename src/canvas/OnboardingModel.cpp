#include "OnboardingModel.h"

#include <algorithm>

namespace markamp::canvas
{

void OnboardingModel::set_steps(std::vector<WalkthroughStep> steps)
{
    steps_ = std::move(steps);
}
auto OnboardingModel::steps() const -> const std::vector<WalkthroughStep>&
{
    return steps_;
}

void OnboardingModel::complete_step(const std::string& step_id)
{
    for (auto& step : steps_)
    {
        if (step.step_id == step_id)
        {
            step.completed = true;
            break;
        }
    }
}

auto OnboardingModel::progress_percent() const -> int
{
    if (steps_.empty())
    {
        return 100;
    }
    const int completed = static_cast<int>(std::count_if(steps_.begin(),
                                                         steps_.end(),
                                                         [](const WalkthroughStep& wt_step)
                                                         { return wt_step.completed; }));
    return (completed * 100) / static_cast<int>(steps_.size());
}

auto OnboardingModel::is_complete() const -> bool
{
    return progress_percent() == 100;
}

void OnboardingModel::dismiss_hint(const std::string& hint_id)
{
    if (std::find(dismissed_hints_.begin(), dismissed_hints_.end(), hint_id) ==
        dismissed_hints_.end())
    {
        dismissed_hints_.push_back(hint_id);
    }
}

auto OnboardingModel::is_hint_dismissed(const std::string& hint_id) const -> bool
{
    return std::find(dismissed_hints_.begin(), dismissed_hints_.end(), hint_id) !=
           dismissed_hints_.end();
}

void OnboardingModel::set_starter_templates(std::vector<StarterTemplate> templates)
{
    starter_templates_ = std::move(templates);
}

auto OnboardingModel::starter_templates() const -> const std::vector<StarterTemplate>&
{
    return starter_templates_;
}

void OnboardingModel::set_reference_visible(bool visible)
{
    reference_visible_ = visible;
}
auto OnboardingModel::reference_visible() const -> bool
{
    return reference_visible_;
}

} // namespace markamp::canvas
