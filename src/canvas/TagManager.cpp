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

// ── Batch 8 (#43-46) ──────────────────────────────────────────────

auto TagManager::rename_tag(const std::string& old_name, const std::string& new_name) -> void
{
    auto tag_iter = tags_.find(old_name);
    if (tag_iter == tags_.end() || old_name == new_name)
    {
        return;
    }

    // Move tag info to new name.
    TagInfo renamed_info = tag_iter->second;
    renamed_info.name = new_name;

    // Merge into existing new-name entry if it exists.
    auto dest_iter = tags_.find(new_name);
    if (dest_iter != tags_.end())
    {
        dest_iter->second.usage_count += renamed_info.usage_count;
    }
    else
    {
        tags_[new_name] = renamed_info;
    }
    tags_.erase(tag_iter);

    // Transfer object associations.
    auto obj_iter = tag_to_objects_.find(old_name);
    if (obj_iter != tag_to_objects_.end())
    {
        tag_to_objects_[new_name].insert(obj_iter->second.begin(), obj_iter->second.end());
        tag_to_objects_.erase(obj_iter);
    }
}

auto TagManager::delete_tag(const std::string& tag_name) -> void
{
    tags_.erase(tag_name);
    tag_to_objects_.erase(tag_name);
}

auto TagManager::merge_tags(const std::string& source, const std::string& destination) -> void
{
    if (source == destination)
    {
        return;
    }

    auto src_tag = tags_.find(source);
    auto dst_tag = tags_.find(destination);
    if (src_tag == tags_.end())
    {
        return;
    }

    // Ensure destination exists.
    if (dst_tag == tags_.end())
    {
        tags_[destination].name = destination;
        tags_[destination].usage_count = 0;
    }

    tags_[destination].usage_count += src_tag->second.usage_count;

    // Move object associations.
    auto src_objs = tag_to_objects_.find(source);
    if (src_objs != tag_to_objects_.end())
    {
        tag_to_objects_[destination].insert(src_objs->second.begin(), src_objs->second.end());
        tag_to_objects_.erase(src_objs);
    }

    tags_.erase(source);
}

auto TagManager::most_used_tags(size_t top_n) const -> std::vector<TagInfo>
{
    auto sorted = all_tags(); // Already sorted by usage_count descending.
    if (sorted.size() > top_n)
    {
        sorted.resize(top_n);
    }
    return sorted;
}

// (#94) Check if a specific tag exists in the index.
auto TagManager::has_tag(const std::string& tag_name) const -> bool
{
    return tags_.contains(tag_name);
}

} // namespace markamp::canvas
