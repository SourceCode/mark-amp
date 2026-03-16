#include "core/BacklinkService.h"

#include "core/Config.h"

#include <algorithm>
#include <queue>
#include <unordered_set>

namespace markamp::core
{

BacklinkService::BacklinkService(EventBus& event_bus,
                                 Config& config,
                                 RefIndex& ref_index,
                                 BacklinkIndex& backlink_index)
    : event_bus_(event_bus)
    , config_(config)
    , ref_index_(ref_index)
    , backlink_index_(backlink_index)
{
}

auto BacklinkService::get_backlinks(const BlockId& block_id) -> std::vector<BacklinkItem>
{
    return backlink_index_.get_backlinks(block_id);
}

// (#74) Implement basic mention detection via [[wiki-link]] scanning.
auto BacklinkService::get_mentions(const BlockId& block_id) -> std::vector<MentionItem>
{
    // Scan all backlink sources for wiki-link mentions of this block.
    const auto backlinks = backlink_index_.get_backlinks(block_id);
    std::vector<MentionItem> mentions;
    mentions.reserve(backlinks.size());
    for (const auto& backlink : backlinks)
    {
        MentionItem mention;
        mention.source_block_id = backlink.ref_block_id;
        mention.source_root_id = backlink.ref_root_id;
        mention.source_doc_title = backlink.ref_doc_title;
        mention.matched_name = backlink.ref_block_content;
        mentions.push_back(std::move(mention));
    }
    return mentions;
}

auto BacklinkService::get_backlink_count(const BlockId& block_id) -> std::size_t
{
    return backlink_index_.backlink_count(block_id);
}

auto BacklinkService::get_mention_count(const BlockId& block_id) -> std::size_t
{
    auto mentions = get_mentions(block_id);
    return mentions.size();
}

auto BacklinkService::build_tree_backlinks(const BlockId& block_id) -> std::vector<BacklinkTreeNode>
{
    return backlink_index_.get_tree_backlinks(block_id);
}

auto BacklinkService::get_backlink_documents(const BlockId& block_id) -> std::vector<std::string>
{
    auto backlinks = backlink_index_.get_backlinks(block_id);

    std::unordered_set<std::string> doc_ids;
    for (const auto& backlink : backlinks)
    {
        if (!backlink.ref_root_id.empty())
        {
            doc_ids.insert(backlink.ref_root_id);
        }
    }

    return {doc_ids.begin(), doc_ids.end()};
}

auto BacklinkService::get_graph_data(const BlockId& focal_block_id, int depth) -> GraphData
{
    GraphData data;

    if (depth < 0)
    {
        return data;
    }

    // BFS from the focal block
    std::unordered_set<BlockId, BlockIdHash> visited;
    std::queue<std::pair<BlockId, int>> bfs_queue;
    bfs_queue.emplace(focal_block_id, 0);
    visited.insert(focal_block_id);

    while (!bfs_queue.empty())
    {
        auto [current_id, current_depth] = bfs_queue.front();
        bfs_queue.pop();

        // Add node
        GraphNode node;
        node.block_id = current_id;
        node.label = current_id.value;
        node.is_focal = (current_id == focal_block_id);
        node.ref_count = static_cast<int>(ref_index_.ref_count(current_id));
        data.nodes.push_back(std::move(node));

        if (current_depth >= depth)
        {
            continue;
        }

        // Add outgoing edges (blocks this block references)
        auto outgoing = ref_index_.get_refs_from(current_id);
        for (const auto& target : outgoing)
        {
            GraphEdge edge;
            edge.source_id = current_id;
            edge.target_id = target;
            data.edges.push_back(edge);

            if (!visited.contains(target))
            {
                visited.insert(target);
                bfs_queue.emplace(target, current_depth + 1);
            }
        }

        // Add incoming edges (blocks that reference this block)
        auto incoming = ref_index_.get_refs_to(current_id);
        for (const auto& source : incoming)
        {
            GraphEdge edge;
            edge.source_id = source;
            edge.target_id = current_id;
            data.edges.push_back(edge);

            if (!visited.contains(source))
            {
                visited.insert(source);
                bfs_queue.emplace(source, current_depth + 1);
            }
        }
    }

    return data;
}

void BacklinkService::rebuild_backlink_index()
{
    backlink_index_.rebuild_from(ref_index_);
}

void BacklinkService::on_block_ref_created(const BlockId& source, const BlockId& def)
{
    backlink_index_.on_ref_added(source, def, RefType::BlockRef);
}

void BacklinkService::on_block_ref_deleted(const BlockId& source, const BlockId& def)
{
    backlink_index_.on_ref_removed(source, def);
}

// ── Batch 23-25 (#153-154) ──

auto BacklinkService::has_backlinks(const BlockId& block_id) -> bool
{
    return get_backlink_count(block_id) > 0;
}

auto BacklinkService::total_link_count(const BlockId& block_id) -> std::size_t
{
    return get_backlink_count(block_id) + get_mention_count(block_id);
}

// (#178) Return the mention-to-total-link ratio for a block.
auto BacklinkService::mention_percentage(const BlockId& block_id) -> double
{
    auto total = total_link_count(block_id);
    if (total == 0)
    {
        return 0.0;
    }
    auto mentions = get_mention_count(block_id);
    return (static_cast<double>(mentions) / static_cast<double>(total)) * 100.0;
}

// (#179) Return the number of blocks that have any links (backlinks + mentions).
auto BacklinkService::linked_block_count() -> std::size_t
{
    // Count total links via the reference index
    return ref_index_.total_refs();
}

} // namespace markamp::core
