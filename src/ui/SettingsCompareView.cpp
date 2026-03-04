#include "SettingsCompareView.h"

#include <unordered_map>

namespace markamp::ui
{

void SettingsCompareModel::compute(
    const std::vector<std::pair<std::string, std::string>>& left_settings,
    const std::vector<std::pair<std::string, std::string>>& right_settings)
{
    entries_.clear();

    // Build lookup maps
    std::unordered_map<std::string, std::string> left_map;
    std::unordered_map<std::string, std::string> right_map;

    for (const auto& [setting_id, value] : left_settings)
    {
        left_map[setting_id] = value;
    }
    for (const auto& [setting_id, value] : right_settings)
    {
        right_map[setting_id] = value;
    }

    // Process left settings
    for (const auto& [setting_id, left_value] : left_settings)
    {
        SettingCompareEntry entry;
        entry.setting_id = setting_id;
        entry.setting_label = setting_id; // Simplified, could be resolved to proper label
        entry.left_value = left_value;

        auto right_iter = right_map.find(setting_id);
        if (right_iter != right_map.end())
        {
            entry.right_value = right_iter->second;
            entry.status = (left_value == right_iter->second) ? SettingCompareStatus::kSame
                                                              : SettingCompareStatus::kDifferent;
        }
        else
        {
            entry.status = SettingCompareStatus::kOnlyInLeft;
        }

        entries_.push_back(std::move(entry));
    }

    // Process right-only settings
    for (const auto& [setting_id, right_value] : right_settings)
    {
        if (left_map.find(setting_id) == left_map.end())
        {
            SettingCompareEntry entry;
            entry.setting_id = setting_id;
            entry.setting_label = setting_id;
            entry.right_value = right_value;
            entry.status = SettingCompareStatus::kOnlyInRight;
            entries_.push_back(std::move(entry));
        }
    }
}

auto SettingsCompareModel::different_entries() const -> std::vector<SettingCompareEntry>
{
    std::vector<SettingCompareEntry> result;
    for (const auto& entry : entries_)
    {
        if (entry.status != SettingCompareStatus::kSame)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto SettingsCompareModel::diff_count() const -> std::size_t
{
    std::size_t count = 0;
    for (const auto& entry : entries_)
    {
        if (entry.status != SettingCompareStatus::kSame)
        {
            ++count;
        }
    }
    return count;
}

auto SettingsCompareModel::summary_text() const -> std::string
{
    auto diffs = diff_count();
    if (diffs == 0)
    {
        return "No differences found";
    }
    return std::to_string(diffs) + " difference" + (diffs == 1 ? "" : "s") + " found";
}

void SettingsCompareModel::clear()
{
    entries_.clear();
}

} // namespace markamp::ui
