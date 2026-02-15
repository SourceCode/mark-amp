#pragma once

#include "core/Backlink.h"
#include "core/BlockRef.h"
#include "core/EventBus.h"
#include "core/RefParser.h"

#include <string>
#include <vector>

namespace markamp::core
{

class Config;
class BlockDatabase;

/// Service for backlink discovery, mention detection, and graph data generation.
class BacklinkService
{
public:
    explicit BacklinkService(EventBus& event_bus,
                             Config& config,
                             RefIndex& ref_index,
                             BacklinkIndex& backlink_index);

    [[nodiscard]] auto get_backlinks(const BlockId& block_id) -> std::vector<BacklinkItem>;

    [[nodiscard]] auto get_mentions(const BlockId& block_id) -> std::vector<MentionItem>;

    [[nodiscard]] auto get_backlink_count(const BlockId& block_id) -> std::size_t;

    [[nodiscard]] auto get_mention_count(const BlockId& block_id) -> std::size_t;

    [[nodiscard]] auto build_tree_backlinks(const BlockId& block_id)
        -> std::vector<BacklinkTreeNode>;

    [[nodiscard]] auto get_backlink_documents(const BlockId& block_id) -> std::vector<std::string>;

    [[nodiscard]] auto get_graph_data(const BlockId& focal_block_id, int depth = 2) -> GraphData;

    void rebuild_backlink_index();

private:
    EventBus& event_bus_;
    Config& config_;
    RefIndex& ref_index_;
    BacklinkIndex& backlink_index_;

    void on_block_ref_created(const BlockId& source, const BlockId& def);
    void on_block_ref_deleted(const BlockId& source, const BlockId& def);
};

} // namespace markamp::core
