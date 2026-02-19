#include "SettingsNavModel.h"

#include <algorithm>
#include <cctype>
#include <map>
#include <set>

namespace markamp::ui
{

void SettingsNavModel::set_entries(std::vector<SettingEntry> entries)
{
    entries_ = std::move(entries);
}

auto SettingsNavModel::entries() const -> const std::vector<SettingEntry>&
{
    return entries_;
}

auto SettingsNavModel::category_tree() const -> std::vector<SettingsCategoryNode>
{
    // Build unique group/subgroup pairs with counts
    struct GroupKey
    {
        std::string group;
        std::string subgroup;
        auto operator<(const GroupKey& other) const -> bool
        {
            if (group != other.group)
            {
                return group < other.group;
            }
            return subgroup < other.subgroup;
        }
    };

    std::map<GroupKey, int> counts;
    for (const auto& entry : entries_)
    {
        ++counts[{entry.group, entry.subgroup}];
    }

    std::vector<SettingsCategoryNode> result;
    result.reserve(counts.size());
    for (const auto& [key, count] : counts)
    {
        result.push_back({key.group, key.subgroup, true, count});
    }
    return result;
}

auto SettingsNavModel::match_score(const std::string& query, const std::string& text) -> int
{
    if (query.empty() || text.empty())
    {
        return 0;
    }

    std::string lower_query = query;
    std::string lower_text = text;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });
    std::transform(lower_text.begin(),
                   lower_text.end(),
                   lower_text.begin(),
                   [](unsigned char character)
                   { return static_cast<char>(std::tolower(character)); });

    if (lower_text.find(lower_query) == 0)
    {
        return 100;
    }
    if (lower_text.find(lower_query) != std::string::npos)
    {
        return 50;
    }
    return 0;
}

auto SettingsNavModel::search(const std::string& query) const -> std::vector<SettingsSearchMatch>
{
    std::vector<SettingsSearchMatch> results;

    for (int idx = 0; idx < static_cast<int>(entries_.size()); ++idx)
    {
        const auto& entry = entries_[static_cast<std::size_t>(idx)];

        int best_score = match_score(query, entry.label);
        std::string context = entry.label;

        const int id_score = match_score(query, entry.setting_id);
        if (id_score > best_score)
        {
            best_score = id_score;
            context = entry.setting_id;
        }

        const int desc_score = match_score(query, entry.description);
        if (desc_score > best_score)
        {
            best_score = desc_score;
            context = entry.description;
        }

        for (const auto& keyword : entry.keywords)
        {
            const int kw_score = match_score(query, keyword);
            if (kw_score > best_score)
            {
                best_score = kw_score;
                context = keyword;
            }
        }

        if (best_score > 0)
        {
            results.push_back({idx, entry.setting_id, entry.label, context, best_score});
        }
    }

    std::sort(results.begin(),
              results.end(),
              [](const SettingsSearchMatch& lhs, const SettingsSearchMatch& rhs)
              { return lhs.score > rhs.score; });

    return results;
}

auto SettingsNavModel::breadcrumb_for(const std::string& setting_id) const
    -> std::optional<SettingsBreadcrumb>
{
    for (const auto& entry : entries_)
    {
        if (entry.setting_id == setting_id)
        {
            return SettingsBreadcrumb{entry.group, entry.subgroup, entry.setting_id};
        }
    }
    return std::nullopt;
}

auto SettingsNavModel::badge_for(const std::string& setting_id) const -> SettingBadge
{
    for (const auto& entry : entries_)
    {
        if (entry.setting_id == setting_id)
        {
            if (entry.deprecated)
            {
                return SettingBadge::kDeprecated;
            }
            if (entry.restart_required)
            {
                return SettingBadge::kRestartRequired;
            }
            if (entry.experimental)
            {
                return SettingBadge::kExperimental;
            }
            return SettingBadge::kNone;
        }
    }
    return SettingBadge::kNone;
}

auto SettingsNavModel::badge_tooltip(SettingBadge badge) -> std::string
{
    switch (badge)
    {
        case SettingBadge::kNone:
            return "";
        case SettingBadge::kRestartRequired:
            return "Changing this setting requires a restart";
        case SettingBadge::kExperimental:
            return "This is an experimental feature";
        case SettingBadge::kDeprecated:
            return "This setting is deprecated";
    }
    return "";
}

} // namespace markamp::ui
