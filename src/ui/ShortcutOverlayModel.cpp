#include "ShortcutOverlayModel.h"

#include <algorithm>
#include <cctype>
#include <set>

namespace markamp::ui
{

void ShortcutOverlayModel::set_entries(std::vector<ShortcutEntry> entries)
{
    entries_ = std::move(entries);
}

auto ShortcutOverlayModel::entries() const -> const std::vector<ShortcutEntry>&
{
    return entries_;
}

auto ShortcutOverlayModel::search(const std::string& query) const -> std::vector<ShortcutEntry>
{
    if (query.empty())
    {
        return entries_;
    }

    std::string lower_query = query;
    std::transform(lower_query.begin(),
                   lower_query.end(),
                   lower_query.begin(),
                   [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

    std::vector<ShortcutEntry> results;
    for (const auto& entry : entries_)
    {
        std::string lower_label = entry.label;
        std::transform(lower_label.begin(),
                       lower_label.end(),
                       lower_label.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        std::string lower_shortcut = entry.shortcut;
        std::transform(lower_shortcut.begin(),
                       lower_shortcut.end(),
                       lower_shortcut.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        std::string lower_category = entry.category;
        std::transform(lower_category.begin(),
                       lower_category.end(),
                       lower_category.begin(),
                       [](unsigned char chr) { return static_cast<char>(std::tolower(chr)); });

        if (lower_label.find(lower_query) != std::string::npos ||
            lower_shortcut.find(lower_query) != std::string::npos ||
            lower_category.find(lower_query) != std::string::npos)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto ShortcutOverlayModel::for_context(const std::string& mode) const -> std::vector<ShortcutEntry>
{
    std::vector<ShortcutEntry> results;
    for (const auto& entry : entries_)
    {
        if (entry.context == mode || entry.context.empty())
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto ShortcutOverlayModel::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique;
    for (const auto& entry : entries_)
    {
        unique.insert(entry.category);
    }
    return {unique.begin(), unique.end()};
}

auto ShortcutOverlayModel::by_category(const std::string& category) const
    -> std::vector<ShortcutEntry>
{
    std::vector<ShortcutEntry> results;
    for (const auto& entry : entries_)
    {
        if (entry.category == category)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto ShortcutOverlayModel::format_tooltip(const std::string& label, const std::string& shortcut)
    -> std::string
{
    if (shortcut.empty())
    {
        return label;
    }
    return label + " (" + shortcut + ")";
}

} // namespace markamp::ui
