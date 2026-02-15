#include "core/Backlink.h"

#include <algorithm>

namespace markamp::core
{

// ============================================================================
// BacklinkIndex Implementation
// ============================================================================

void BacklinkIndex::on_ref_added(const BlockId& source, const BlockId& def, RefType /*type*/)
{
    std::lock_guard<std::mutex> lock(mutex_);

    BacklinkItem item;
    item.ref_block_id = source;
    item.def_block_id = def;
    item.type = BacklinkType::DirectRef;
    item.updated_at = std::chrono::system_clock::now();

    cache_[def].push_back(std::move(item));
}

void BacklinkIndex::on_ref_removed(const BlockId& source, const BlockId& def)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(def);
    if (it == cache_.end())
    {
        return;
    }

    auto& items = it->second;
    items.erase(std::remove_if(items.begin(),
                               items.end(),
                               [&](const BacklinkItem& item)
                               { return item.ref_block_id == source; }),
                items.end());

    if (items.empty())
    {
        cache_.erase(it);
    }
}

auto BacklinkIndex::get_backlinks(const BlockId& def_block_id) const -> std::vector<BacklinkItem>
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(def_block_id);
    if (it == cache_.end())
    {
        return {};
    }
    return it->second;
}

auto BacklinkIndex::get_tree_backlinks(const BlockId& def_block_id) const
    -> std::vector<BacklinkTreeNode>
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(def_block_id);
    if (it == cache_.end())
    {
        return {};
    }

    // Group by root document
    std::unordered_map<std::string, BacklinkTreeNode> grouped;
    for (const auto& item : it->second)
    {
        auto& node = grouped[item.ref_root_id];
        node.doc_id = item.ref_root_id;
        node.doc_title = item.ref_doc_title;
        node.backlinks.push_back(item);
    }

    std::vector<BacklinkTreeNode> result;
    result.reserve(grouped.size());
    for (auto& [_, node] : grouped)
    {
        result.push_back(std::move(node));
    }
    return result;
}

auto BacklinkIndex::backlink_count(const BlockId& def_block_id) const -> std::size_t
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto it = cache_.find(def_block_id);
    if (it == cache_.end())
    {
        return 0;
    }
    return it->second.size();
}

void BacklinkIndex::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
}

void BacklinkIndex::rebuild_from(const RefIndex& ref_index)
{
    // Note: this is a simplified rebuild. In production, we'd iterate
    // all entries in the ref_index. For now, we just clear.
    std::lock_guard<std::mutex> lock(mutex_);
    cache_.clear();
    (void)ref_index; // Full rebuild requires iterating ref_index internals
}

} // namespace markamp::core
