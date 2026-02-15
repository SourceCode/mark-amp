#pragma once

#include "core/BlockRef.h"
#include "core/EventBus.h"

#include <chrono>
#include <cstdint>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class Config;

/// Type of backlink relationship.
enum class BacklinkType : std::uint8_t
{
    DirectRef, // Explicit ((id)) reference
    Mention    // Virtual reference (name/alias match in text)
};

/// A single backlink item.
struct BacklinkItem
{
    BlockId ref_block_id;
    BlockId def_block_id;
    std::string ref_root_id;
    std::string ref_doc_title;
    std::string ref_block_content;
    std::string context_before;
    std::string context_after;
    BacklinkType type{BacklinkType::DirectRef};
    std::chrono::system_clock::time_point updated_at;
};

/// A mention item: occurrence of a block's name/alias in another block.
struct MentionItem
{
    BlockId source_block_id;
    std::string source_root_id;
    std::string source_doc_title;
    std::string source_content;
    std::string matched_name;
    std::size_t match_offset{0};
    std::size_t match_length{0};
    std::string context_before;
    std::string context_after;
};

/// Backlinks grouped by source document for tree display.
struct BacklinkTreeNode
{
    std::string doc_id;
    std::string doc_title;
    std::string doc_path;
    std::vector<BacklinkItem> backlinks;

    [[nodiscard]] auto count() const -> std::size_t
    {
        return backlinks.size();
    }
};

/// Graph node for local graph visualization.
struct GraphNode
{
    BlockId block_id;
    std::string label;
    std::string node_type;
    bool is_focal{false};
    int ref_count{0};
};

/// Graph edge for local graph visualization.
struct GraphEdge
{
    BlockId source_id;
    BlockId target_id;
    RefType ref_type{RefType::BlockRef};
};

/// Graph data containing nodes and edges for visualization.
struct GraphData
{
    std::vector<GraphNode> nodes;
    std::vector<GraphEdge> edges;
};

/// Cached backlink index for O(1) lookups.
class BacklinkIndex
{
public:
    void on_ref_added(const BlockId& source, const BlockId& def, RefType type);
    void on_ref_removed(const BlockId& source, const BlockId& def);

    [[nodiscard]] auto get_backlinks(const BlockId& def_block_id) const
        -> std::vector<BacklinkItem>;
    [[nodiscard]] auto get_tree_backlinks(const BlockId& def_block_id) const
        -> std::vector<BacklinkTreeNode>;
    [[nodiscard]] auto backlink_count(const BlockId& def_block_id) const -> std::size_t;

    void clear();
    void rebuild_from(const RefIndex& ref_index);

private:
    std::unordered_map<BlockId, std::vector<BacklinkItem>, BlockIdHash> cache_;
    mutable std::mutex mutex_;
};

} // namespace markamp::core
