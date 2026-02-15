#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"
#include "AttributeViewStore.h"

#include <expected>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Resolved Relation: fully resolved with content for display
// ════════════════════════════════════════════════════════════

struct ResolvedRelation
{
    std::string block_id;
    std::string title;
    std::string av_id;
};

// ════════════════════════════════════════════════════════════
// Relation Service
// ════════════════════════════════════════════════════════════

class AVRelationService
{
public:
    explicit AVRelationService(AttributeViewStore& store);

    /// Add a relation from source row to target row.
    [[nodiscard]] auto add_relation(const std::string& source_av_id,
                                    const std::string& source_block_id,
                                    const std::string& relation_key_id,
                                    const std::string& target_block_id)
        -> std::expected<void, std::string>;

    /// Remove a relation.
    [[nodiscard]] auto remove_relation(const std::string& source_av_id,
                                       const std::string& source_block_id,
                                       const std::string& relation_key_id,
                                       const std::string& target_block_id)
        -> std::expected<void, std::string>;

    /// Resolve relation block IDs to display content.
    [[nodiscard]] auto resolve_relations(const std::string& av_id,
                                         const std::string& block_id,
                                         const std::string& key_id) const
        -> std::vector<ResolvedRelation>;

    /// Create a two-way relation pair.
    [[nodiscard]] auto create_two_way_relation(const std::string& source_av_id,
                                               const std::string& source_key_id,
                                               const std::string& target_av_id)
        -> std::expected<std::string, std::string>;

    /// Sync back-relations when a relation changes.
    [[nodiscard]] auto sync_back_relations(const std::string& source_av_id,
                                           const std::string& source_block_id,
                                           const std::string& source_key_id)
        -> std::expected<void, std::string>;

private:
    AttributeViewStore& store_;
};

} // namespace markamp::core::av
