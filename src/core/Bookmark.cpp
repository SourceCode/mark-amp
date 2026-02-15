#include "Bookmark.h"

#include <algorithm>

namespace markamp::core
{

// ════════════════════════════════════════════════════════════
// BookmarkIndex
// ════════════════════════════════════════════════════════════

void BookmarkIndex::add(const BlockId& block_id, const std::string& label)
{
    // If block already has a bookmark, remove it first
    auto existing = block_to_label_.find(block_id);
    if (existing != block_to_label_.end())
    {
        auto& old_set = label_to_blocks_[existing->second];
        old_set.erase(block_id);
        if (old_set.empty())
        {
            label_to_blocks_.erase(existing->second);
        }
    }

    block_to_label_[block_id] = label;
    label_to_blocks_[label].insert(block_id);
}

void BookmarkIndex::remove(const BlockId& block_id)
{
    auto iter = block_to_label_.find(block_id);
    if (iter == block_to_label_.end())
    {
        return;
    }

    auto& label_set = label_to_blocks_[iter->second];
    label_set.erase(block_id);
    if (label_set.empty())
    {
        label_to_blocks_.erase(iter->second);
    }
    block_to_label_.erase(iter);
}

auto BookmarkIndex::is_bookmarked(const BlockId& block_id) const -> bool
{
    return block_to_label_.find(block_id) != block_to_label_.end();
}

auto BookmarkIndex::get_label(const BlockId& block_id) const -> std::string
{
    auto iter = block_to_label_.find(block_id);
    if (iter == block_to_label_.end())
    {
        return {};
    }
    return iter->second;
}

auto BookmarkIndex::get_blocks_with_label(const std::string& label) const -> std::vector<BlockId>
{
    auto iter = label_to_blocks_.find(label);
    if (iter == label_to_blocks_.end())
    {
        return {};
    }
    return {iter->second.begin(), iter->second.end()};
}

auto BookmarkIndex::get_all_labels() const -> std::vector<std::string>
{
    std::vector<std::string> labels;
    labels.reserve(label_to_blocks_.size());
    for (const auto& [label, blocks] : label_to_blocks_)
    {
        labels.push_back(label);
    }
    std::sort(labels.begin(), labels.end());
    return labels;
}

auto BookmarkIndex::rename_label(const std::string& old_label, const std::string& new_label) -> int
{
    auto iter = label_to_blocks_.find(old_label);
    if (iter == label_to_blocks_.end())
    {
        return 0;
    }

    auto blocks = std::move(iter->second);
    label_to_blocks_.erase(iter);

    int count = 0;
    for (const auto& block_id : blocks)
    {
        block_to_label_[block_id] = new_label;
        label_to_blocks_[new_label].insert(block_id);
        count++;
    }
    return count;
}

auto BookmarkIndex::remove_label(const std::string& label) -> int
{
    auto iter = label_to_blocks_.find(label);
    if (iter == label_to_blocks_.end())
    {
        return 0;
    }

    int count = 0;
    for (const auto& block_id : iter->second)
    {
        block_to_label_.erase(block_id);
        count++;
    }
    label_to_blocks_.erase(iter);
    return count;
}

auto BookmarkIndex::total_count() const -> std::size_t
{
    return block_to_label_.size();
}

void BookmarkIndex::clear()
{
    block_to_label_.clear();
    label_to_blocks_.clear();
}

} // namespace markamp::core
