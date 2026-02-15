#pragma once

#include "BlockRef.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Information about a single tag (leaf or intermediate node in tag tree).
struct TagInfo
{
    /// The tag label (just the leaf component, e.g., "work" in "project/work").
    std::string label;

    /// The full hierarchical path (e.g., "project/work").
    std::string full_path;

    /// Number of blocks directly tagged with this exact tag.
    int direct_count{0};

    /// Total count including all descendant tags.
    int total_count{0};

    /// Child tags (next level in hierarchy).
    std::vector<TagInfo> children;

    /// Whether this node has children.
    [[nodiscard]] auto has_children() const -> bool
    {
        return !children.empty();
    }

    /// Depth in the hierarchy (0 = root level).
    int depth{0};
};

/// A parsed tag occurrence found in block content.
struct ParsedTag
{
    /// The full tag path as written (e.g., "project/work/alpha").
    std::string full_path;

    /// Character offset of the opening `#` in the source content.
    std::size_t source_offset{0};

    /// Character length of the entire tag syntax including delimiters.
    std::size_t source_length{0};

    /// Individual path components (e.g., ["project", "work", "alpha"]).
    [[nodiscard]] auto components() const -> std::vector<std::string>;
};

/// The complete tag tree for the workspace.
struct TagTree
{
    /// Root-level tags (no parent).
    std::vector<TagInfo> roots;

    /// Total number of unique tag paths.
    int unique_tag_count{0};

    /// Total number of tag usages across all blocks.
    int total_usage_count{0};

    /// Whether the tree is empty.
    [[nodiscard]] auto is_empty() const -> bool
    {
        return roots.empty();
    }
};

/// Association between a block and its tags.
struct BlockTagEntry
{
    BlockId block_id;
    std::vector<std::string> tags; // Full paths of all tags on this block
};

/// Parses tag syntax from block content.
class TagParser
{
public:
    /// Extract all `#tag#` occurrences from content.
    /// Pattern: `#([a-zA-Z0-9_/\-]+)#` with boundary checks to avoid
    /// matching markdown headings (`# Heading`).
    [[nodiscard]] auto parse_tags(std::string_view content) const -> std::vector<ParsedTag>;

    /// Validate a tag path (no empty components, valid characters).
    [[nodiscard]] auto validate_tag(std::string_view tag_path) const -> bool;

    /// Split a hierarchical tag into components.
    [[nodiscard]] static auto split_path(std::string_view tag_path) -> std::vector<std::string>;

    /// Join components into a hierarchical tag path.
    [[nodiscard]] static auto join_path(const std::vector<std::string>& components) -> std::string;
};

/// In-memory tag index for fast lookups.
class TagIndex
{
public:
    /// Add a tag association for a block.
    void add_tag(const BlockId& block_id, const std::string& tag_path);

    /// Remove a specific tag from a block.
    void remove_tag(const BlockId& block_id, const std::string& tag_path);

    /// Remove all tags for a block.
    void remove_block(const BlockId& block_id);

    /// Get all blocks with a specific tag (exact match).
    [[nodiscard]] auto get_blocks_with_tag(const std::string& tag_path) const
        -> std::vector<BlockId>;

    /// Get all blocks with a tag or any of its descendants.
    [[nodiscard]] auto get_blocks_with_tag_recursive(const std::string& tag_path) const
        -> std::vector<BlockId>;

    /// Get all tags for a specific block.
    [[nodiscard]] auto get_tags_for_block(const BlockId& block_id) const
        -> std::vector<std::string>;

    /// Get the count of blocks with a specific tag.
    [[nodiscard]] auto tag_count(const std::string& tag_path) const -> int;

    /// Build the complete tag tree from the index.
    [[nodiscard]] auto build_tree() const -> TagTree;

    /// Search for tags matching a query (prefix match).
    [[nodiscard]] auto search_tags(const std::string& query) const -> std::vector<TagInfo>;

    /// Get all unique tag paths.
    [[nodiscard]] auto all_tag_paths() const -> std::vector<std::string>;

    /// Clear the entire index.
    void clear();

private:
    /// tag_path -> set of block IDs
    std::unordered_map<std::string, std::unordered_set<BlockId, BlockIdHash>> tag_to_blocks_;

    /// block_id -> set of tag paths
    std::unordered_map<BlockId, std::unordered_set<std::string>, BlockIdHash> block_to_tags_;

    /// Build a TagInfo subtree rooted at the given path prefix.
    [[nodiscard]] auto build_subtree(const std::string& prefix,
                                     const std::vector<std::string>& all_paths) const
        -> std::vector<TagInfo>;
};

} // namespace markamp::core
