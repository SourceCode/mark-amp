#include "core/BlockTreeIndex.h"

#include "core/BlockDatabase.h"

namespace markamp::core
{

// -- Initialization --

auto BlockTreeIndex::init(BlockDatabase& database) -> void
{
    std::unique_lock lock(mutex_);

    entries_.clear();
    root_index_.clear();
    box_index_.clear();
    path_index_.clear();

    // Load all document blocks from the database to build the index
    auto doc_blocks = database.get_blocks_by_type(BlockType::Document);
    for (const auto& block : doc_blocks)
    {
        BlockTreeEntry entry;
        entry.id = block.id;
        entry.root_id = block.root_id;
        entry.parent_id = block.parent_id;
        entry.box_id = block.box;
        entry.path = block.path;
        entry.hpath = block.hpath;
        entry.updated = block.updated_str;
        entry.type = block.type;

        entries_[entry.id] = entry;
        add_to_indexes(entry);
    }
}

auto BlockTreeIndex::clear() -> void
{
    std::unique_lock lock(mutex_);
    entries_.clear();
    root_index_.clear();
    box_index_.clear();
    path_index_.clear();
}

// -- Single-Entry Operations --

auto BlockTreeIndex::get(const std::string& block_id) const -> std::optional<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    auto found = entries_.find(block_id);
    if (found != entries_.end())
    {
        return found->second;
    }
    return std::nullopt;
}

auto BlockTreeIndex::upsert(const BlockTreeEntry& entry) -> void
{
    std::unique_lock lock(mutex_);

    // Remove old indexes if updating
    auto existing = entries_.find(entry.id);
    if (existing != entries_.end())
    {
        remove_from_indexes(existing->second);
    }

    entries_[entry.id] = entry;
    add_to_indexes(entry);
}

auto BlockTreeIndex::remove(const std::string& block_id) -> void
{
    std::unique_lock lock(mutex_);
    auto found = entries_.find(block_id);
    if (found != entries_.end())
    {
        remove_from_indexes(found->second);
        entries_.erase(found);
    }
}

// -- Bulk Operations --

auto BlockTreeIndex::index_tree(const Block& root_block) -> void
{
    std::unique_lock lock(mutex_);
    index_block_recursive(root_block, root_block.id);
}

auto BlockTreeIndex::remove_by_root(const std::string& root_id) -> void
{
    std::unique_lock lock(mutex_);
    auto root_it = root_index_.find(root_id);
    if (root_it == root_index_.end())
    {
        return;
    }

    // Copy the set since we'll be mutating it
    auto block_ids = root_it->second;
    for (const auto& block_id : block_ids)
    {
        auto entry_it = entries_.find(block_id);
        if (entry_it != entries_.end())
        {
            remove_from_indexes(entry_it->second);
            entries_.erase(entry_it);
        }
    }
}

auto BlockTreeIndex::remove_by_box(const std::string& box_id) -> void
{
    std::unique_lock lock(mutex_);
    auto box_it = box_index_.find(box_id);
    if (box_it == box_index_.end())
    {
        return;
    }

    auto block_ids = box_it->second;
    for (const auto& block_id : block_ids)
    {
        auto entry_it = entries_.find(block_id);
        if (entry_it != entries_.end())
        {
            remove_from_indexes(entry_it->second);
            entries_.erase(entry_it);
        }
    }
}

// -- Queries --

auto BlockTreeIndex::get_by_root(const std::string& root_id) const -> std::vector<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    std::vector<BlockTreeEntry> result;

    auto root_it = root_index_.find(root_id);
    if (root_it == root_index_.end())
    {
        return result;
    }

    result.reserve(root_it->second.size());
    for (const auto& block_id : root_it->second)
    {
        auto entry_it = entries_.find(block_id);
        if (entry_it != entries_.end())
        {
            result.push_back(entry_it->second);
        }
    }
    return result;
}

auto BlockTreeIndex::get_root_by_path(const std::string& box_id, const std::string& path) const
    -> std::optional<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    auto key = box_id + ":" + path;
    auto found = path_index_.find(key);
    if (found == path_index_.end())
    {
        return std::nullopt;
    }

    auto entry_it = entries_.find(found->second);
    if (entry_it != entries_.end())
    {
        return entry_it->second;
    }
    return std::nullopt;
}

auto BlockTreeIndex::get_by_box(const std::string& box_id) const -> std::vector<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    std::vector<BlockTreeEntry> result;

    auto box_it = box_index_.find(box_id);
    if (box_it == box_index_.end())
    {
        return result;
    }

    result.reserve(box_it->second.size());
    for (const auto& block_id : box_it->second)
    {
        auto entry_it = entries_.find(block_id);
        if (entry_it != entries_.end())
        {
            result.push_back(entry_it->second);
        }
    }
    return result;
}

auto BlockTreeIndex::get_by_type(BlockType type) const -> std::vector<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    std::vector<BlockTreeEntry> result;

    for (const auto& [block_id, entry] : entries_)
    {
        if (entry.type == type)
        {
            result.push_back(entry);
        }
    }
    return result;
}

// -- Statistics --

auto BlockTreeIndex::count() const -> int64_t
{
    std::shared_lock lock(mutex_);
    return static_cast<int64_t>(entries_.size());
}

auto BlockTreeIndex::count_by_box(const std::string& box_id) const -> int64_t
{
    std::shared_lock lock(mutex_);
    auto box_it = box_index_.find(box_id);
    if (box_it == box_index_.end())
    {
        return 0;
    }
    return static_cast<int64_t>(box_it->second.size());
}

auto BlockTreeIndex::count_documents() const -> int64_t
{
    std::shared_lock lock(mutex_);
    int64_t doc_count = 0;
    for (const auto& [block_id, entry] : entries_)
    {
        if (entry.type == BlockType::Document)
        {
            ++doc_count;
        }
    }
    return doc_count;
}

// -- Validation --

auto BlockTreeIndex::contains(const std::string& block_id) const -> bool
{
    std::shared_lock lock(mutex_);
    return entries_.contains(block_id);
}

auto BlockTreeIndex::find_orphans() const -> std::vector<BlockTreeEntry>
{
    std::shared_lock lock(mutex_);
    std::vector<BlockTreeEntry> orphans;

    for (const auto& [block_id, entry] : entries_)
    {
        // Skip document blocks (they are roots)
        if (entry.type == BlockType::Document)
        {
            continue;
        }

        // An orphan has a parent_id that doesn't exist in the index
        if (!entry.parent_id.empty() && !entries_.contains(entry.parent_id))
        {
            orphans.push_back(entry);
        }
    }
    return orphans;
}

// -- Internal Helpers --

auto BlockTreeIndex::add_to_indexes(const BlockTreeEntry& entry) -> void
{
    // Root index
    if (!entry.root_id.empty())
    {
        root_index_[entry.root_id].insert(entry.id);
    }

    // Box index
    if (!entry.box_id.empty())
    {
        box_index_[entry.box_id].insert(entry.id);
    }

    // Path index (only for document/root blocks)
    if (entry.type == BlockType::Document && !entry.box_id.empty() && !entry.path.empty())
    {
        path_index_[entry.box_id + ":" + entry.path] = entry.id;
    }
}

auto BlockTreeIndex::remove_from_indexes(const BlockTreeEntry& entry) -> void
{
    // Root index
    if (!entry.root_id.empty())
    {
        auto root_it = root_index_.find(entry.root_id);
        if (root_it != root_index_.end())
        {
            root_it->second.erase(entry.id);
            if (root_it->second.empty())
            {
                root_index_.erase(root_it);
            }
        }
    }

    // Box index
    if (!entry.box_id.empty())
    {
        auto box_it = box_index_.find(entry.box_id);
        if (box_it != box_index_.end())
        {
            box_it->second.erase(entry.id);
            if (box_it->second.empty())
            {
                box_index_.erase(box_it);
            }
        }
    }

    // Path index
    if (entry.type == BlockType::Document && !entry.box_id.empty() && !entry.path.empty())
    {
        path_index_.erase(entry.box_id + ":" + entry.path);
    }
}

auto BlockTreeIndex::index_block_recursive(const Block& block, const std::string& root_id) -> void
{
    BlockTreeEntry entry;
    entry.id = block.id;
    entry.root_id = root_id;
    entry.parent_id = block.parent_id;
    entry.box_id = block.box;
    entry.path = block.path;
    entry.hpath = block.hpath;
    entry.updated = block.updated_str;
    entry.type = block.type;

    // Remove old if present
    auto existing = entries_.find(entry.id);
    if (existing != entries_.end())
    {
        remove_from_indexes(existing->second);
    }

    entries_[entry.id] = entry;
    add_to_indexes(entry);

    // Recurse into children
    for (const auto& child_ptr : block.children)
    {
        if (child_ptr)
        {
            index_block_recursive(*child_ptr, root_id);
        }
    }
}

} // namespace markamp::core
