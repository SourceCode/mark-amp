#pragma once

#include "core/BlockRef.h"
#include "core/EventBus.h"

#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

class BlockDatabase;

/// Resolves parsed references to full block data.
class RefResolver
{
public:
    explicit RefResolver(EventBus& event_bus, BlockDatabase& database, RefIndex& ref_index);

    /// Resolve a single reference to a full ResolvedRef.
    [[nodiscard]] auto resolve_ref(const ParsedRef& ref, const BlockId& source_block)
        -> ResolvedRef;

    /// Resolve a block embed and return rendered HTML content for transclusion.
    [[nodiscard]] auto resolve_embed(const ParsedRef& ref) -> std::optional<std::string>;

    /// Resolve multiple references in a single batch.
    [[nodiscard]] auto batch_resolve(const std::vector<ParsedRef>& refs,
                                     const BlockId& source_block) -> std::vector<ResolvedRef>;

    /// Called when a block's content changes. Re-parses refs and updates RefIndex.
    void on_block_changed(const BlockId& block_id, std::string_view new_content);

    /// Full re-index of all blocks in the workspace.
    void rebuild_index();

    /// Configure maximum transclusion nesting depth (default: 5).
    void set_max_embed_depth(int depth);

private:
    EventBus& event_bus_;
    BlockDatabase& database_;
    RefIndex& ref_index_;
    int max_embed_depth_{5};
};

} // namespace markamp::core
