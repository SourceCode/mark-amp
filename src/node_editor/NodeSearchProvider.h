#pragma once

// V11 Phase 27: Node Create Menu Search And Quick Insert
// Fuzzy search over node definitions for add-node menu.

#include "NodeDefinitionRegistry.h"
#include "NodeEditorTypes.h"

#include <string>
#include <vector>

namespace markamp::node_editor
{

/// A single search result entry.
struct NodeSearchResult
{
    std::string type_name;
    std::string display_name;
    std::string category;
    float score{0.0F}; // Higher = better match
};

/// Provides searchable access to node definitions for the add-node menu.
class NodeSearchProvider
{
public:
    explicit NodeSearchProvider(const NodeDefinitionRegistry& registry);

    /// Search for node definitions matching a query string.
    /// Results are sorted by score (highest first), limited to max_results.
    [[nodiscard]] auto search(const std::string& query,
                              GraphMode mode_filter,
                              std::size_t max_results = 20) const -> std::vector<NodeSearchResult>;

    /// Get all definitions grouped by category.
    [[nodiscard]] auto all_by_category() const
        -> std::vector<std::pair<std::string, std::vector<NodeSearchResult>>>;

    /// Get total number of searchable definitions.
    [[nodiscard]] auto total_count() const -> std::size_t;

private:
    const NodeDefinitionRegistry& registry_;

    [[nodiscard]] static auto compute_score(const std::string& query,
                                            const std::string& display_name,
                                            const std::string& type_name,
                                            const std::string& category) -> float;
};

} // namespace markamp::node_editor
