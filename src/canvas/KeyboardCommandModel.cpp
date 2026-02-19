#include "KeyboardCommandModel.h"

#include <algorithm>
#include <map>

namespace markamp::canvas
{

void KeyboardCommandModel::set_bindings(std::vector<KeyBinding> bindings)
{
    bindings_ = std::move(bindings);
}
auto KeyboardCommandModel::bindings() const -> const std::vector<KeyBinding>&
{
    return bindings_;
}

auto KeyboardCommandModel::bindings_in_category(const std::string& category) const
    -> std::vector<KeyBinding>
{
    std::vector<KeyBinding> result;
    for (const auto& binding : bindings_)
    {
        if (binding.category == category)
        {
            result.push_back(binding);
        }
    }
    return result;
}

auto KeyboardCommandModel::find_binding(const std::string& command_id) const -> KeyBinding
{
    for (const auto& binding : bindings_)
    {
        if (binding.command_id == command_id)
        {
            return binding;
        }
    }
    return {"", "", "", "", false};
}

auto KeyboardCommandModel::has_conflicts() const -> bool
{
    return !conflicts().empty();
}

auto KeyboardCommandModel::conflicts() const -> std::vector<std::string>
{
    std::map<std::string, int> shortcut_counts;
    for (const auto& binding : bindings_)
    {
        if (binding.active && !binding.shortcut.empty())
        {
            ++shortcut_counts[binding.shortcut];
        }
    }
    std::vector<std::string> dupes;
    for (const auto& [key, count] : shortcut_counts)
    {
        if (count > 1)
        {
            dupes.push_back(key);
        }
    }
    return dupes;
}

void KeyboardCommandModel::set_nudge_step(double pixels)
{
    nudge_step_ = std::max(0.5, pixels);
}
auto KeyboardCommandModel::nudge_step() const -> double
{
    return nudge_step_;
}
void KeyboardCommandModel::set_grid_nudge(bool enabled)
{
    grid_nudge_ = enabled;
}
auto KeyboardCommandModel::grid_nudge() const -> bool
{
    return grid_nudge_;
}

} // namespace markamp::canvas
