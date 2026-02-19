#include "ToolWindowModel.h"

#include <algorithm>

namespace markamp::ui
{

void ToolWindowModel::set_problems(std::vector<ProblemEntry> problems)
{
    problems_ = std::move(problems);
}

auto ToolWindowModel::all_problems() const -> const std::vector<ProblemEntry>&
{
    return problems_;
}

auto ToolWindowModel::by_severity(ProblemSeverity severity) const -> std::vector<ProblemEntry>
{
    std::vector<ProblemEntry> result;
    for (const auto& prob : problems_)
    {
        if (prob.severity == severity)
        {
            result.push_back(prob);
        }
    }
    return result;
}

auto ToolWindowModel::by_source(const std::string& source) const -> std::vector<ProblemEntry>
{
    std::vector<ProblemEntry> result;
    for (const auto& prob : problems_)
    {
        if (prob.source == source)
        {
            result.push_back(prob);
        }
    }
    return result;
}

auto ToolWindowModel::error_count() const -> int
{
    int count = 0;
    for (const auto& prob : problems_)
    {
        if (prob.severity == ProblemSeverity::kError)
        {
            ++count;
        }
    }
    return count;
}

auto ToolWindowModel::warning_count() const -> int
{
    int count = 0;
    for (const auto& prob : problems_)
    {
        if (prob.severity == ProblemSeverity::kWarning)
        {
            ++count;
        }
    }
    return count;
}

auto ToolWindowModel::quick_fix_count() const -> int
{
    int count = 0;
    for (const auto& prob : problems_)
    {
        if (prob.has_quick_fix)
        {
            ++count;
        }
    }
    return count;
}

void ToolWindowModel::set_channels(std::vector<OutputChannel> channels)
{
    channels_ = std::move(channels);
}

auto ToolWindowModel::channels() const -> const std::vector<OutputChannel>&
{
    return channels_;
}

void ToolWindowModel::set_active_channel(const std::string& channel_id)
{
    for (auto& channel : channels_)
    {
        channel.is_active = (channel.channel_id == channel_id);
    }
}

auto ToolWindowModel::active_channel() const -> const OutputChannel*
{
    for (const auto& channel : channels_)
    {
        if (channel.is_active)
        {
            return &channel;
        }
    }
    return nullptr;
}

void ToolWindowModel::set_follow_tail(bool enabled)
{
    follow_tail_ = enabled;
}
auto ToolWindowModel::follow_tail() const -> bool
{
    return follow_tail_;
}

void ToolWindowModel::set_word_wrap(bool enabled)
{
    word_wrap_ = enabled;
}
auto ToolWindowModel::word_wrap() const -> bool
{
    return word_wrap_;
}

void ToolWindowModel::set_panel_visible(const std::string& panel_id, bool visible)
{
    auto iter = std::find(hidden_panels_.begin(), hidden_panels_.end(), panel_id);
    if (visible && iter != hidden_panels_.end())
    {
        hidden_panels_.erase(iter);
    }
    else if (!visible && iter == hidden_panels_.end())
    {
        hidden_panels_.push_back(panel_id);
    }
}

auto ToolWindowModel::is_panel_visible(const std::string& panel_id) const -> bool
{
    return std::find(hidden_panels_.begin(), hidden_panels_.end(), panel_id) ==
           hidden_panels_.end();
}

void ToolWindowModel::reset_layout()
{
    hidden_panels_.clear();
}

} // namespace markamp::ui
