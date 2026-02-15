#include "TagManager.h"

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"

#include <algorithm>

namespace markamp::canvas
{

auto TagManager::rebuild_index(const Board& board) -> void
{
    // Preserve existing tag colors across rebuilds.
    std::unordered_map<std::string, CanvasColor> saved_colors;
    for (const auto& [tag_name, tag_info] : tags_)
    {
        saved_colors[tag_name] = tag_info.color;
    }

    tags_.clear();
    tag_to_objects_.clear();

    for (const auto& obj : board.objects())
    {
        if (!obj)
        {
            continue;
        }
        const ObjectId obj_id = obj->id();
        for (const auto& tag : obj->tags())
        {
            tags_[tag].name = tag;
            tags_[tag].usage_count += 1;
            tag_to_objects_[tag].insert(obj_id);
        }
    }

    // Restore custom colors.
    for (auto& [tag_name, tag_info] : tags_)
    {
        auto color_iter = saved_colors.find(tag_name);
        if (color_iter != saved_colors.end())
        {
            tag_info.color = color_iter->second;
        }
    }
}

auto TagManager::all_tags() const -> std::vector<TagInfo>
{
    std::vector<TagInfo> result;
    result.reserve(tags_.size());
    for (const auto& [name, info] : tags_)
    {
        result.push_back(info);
    }
    // Sort by usage count descending, then alphabetically for ties.
    std::sort(result.begin(),
              result.end(),
              [](const TagInfo& lhs, const TagInfo& rhs)
              {
                  if (lhs.usage_count != rhs.usage_count)
                  {
                      return lhs.usage_count > rhs.usage_count;
                  }
                  return lhs.name < rhs.name;
              });
    return result;
}

auto TagManager::objects_with_tag(const std::string& tag) const -> std::vector<ObjectId>
{
    auto iter = tag_to_objects_.find(tag);
    if (iter == tag_to_objects_.end())
    {
        return {};
    }
    return {iter->second.begin(), iter->second.end()};
}

auto TagManager::set_tag_color(const std::string& tag, const CanvasColor& color) -> void
{
    tags_[tag].name = tag;
    tags_[tag].color = color;
}

auto TagManager::tag_color(const std::string& tag) const -> CanvasColor
{
    auto iter = tags_.find(tag);
    if (iter == tags_.end())
    {
        return {100, 100, 100, 255};
    }
    return iter->second.color;
}

auto TagManager::suggest_tags(const std::string& prefix) const -> std::vector<std::string>
{
    std::vector<std::string> suggestions;
    for (const auto& [tag_name, tag_info] : tags_)
    {
        if (tag_name.size() >= prefix.size() && tag_name.compare(0, prefix.size(), prefix) == 0)
        {
            suggestions.push_back(tag_name);
        }
    }
    std::sort(suggestions.begin(), suggestions.end());
    return suggestions;
}

auto TagManager::tag_count() const -> size_t
{
    return tags_.size();
}

} // namespace markamp::canvas
