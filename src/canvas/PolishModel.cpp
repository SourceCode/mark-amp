#include "PolishModel.h"

#include <algorithm>

namespace markamp::canvas
{

void PolishModel::enable_interaction(MicroInteraction interaction)
{
    if (std::find(enabled_.begin(), enabled_.end(), interaction) == enabled_.end())
    {
        enabled_.push_back(interaction);
    }
}

void PolishModel::disable_interaction(MicroInteraction interaction)
{
    enabled_.erase(std::remove(enabled_.begin(), enabled_.end(), interaction), enabled_.end());
}

auto PolishModel::is_enabled(MicroInteraction interaction) const -> bool
{
    return std::find(enabled_.begin(), enabled_.end(), interaction) != enabled_.end();
}

void PolishModel::set_easings(std::vector<EasingPreset> easings)
{
    easings_ = std::move(easings);
}
auto PolishModel::easings() const -> const std::vector<EasingPreset>&
{
    return easings_;
}

auto PolishModel::easing_for(const std::string& name) const -> EasingPreset
{
    for (const auto& easing : easings_)
    {
        if (easing.name == name)
        {
            return easing;
        }
    }
    return {"", 0.0, ""};
}

void PolishModel::set_empty_states(std::vector<EmptyState> states)
{
    empty_states_ = std::move(states);
}
auto PolishModel::empty_states() const -> const std::vector<EmptyState>&
{
    return empty_states_;
}

void PolishModel::set_label_issues(int count)
{
    label_issues_ = std::max(0, count);
}
auto PolishModel::label_issues() const -> int
{
    return label_issues_;
}
auto PolishModel::labels_consistent() const -> bool
{
    return label_issues_ == 0;
}

} // namespace markamp::canvas
