#pragma once

#include <cstdint>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

/// Unique block identifier following SiYuan's timestamp-hash format.
/// Format: "YYYYMMDDHHmmss-xxxxxxx" (14 digits + dash + 7 alphanumeric chars).
struct BlockId
{
    std::string value;

    [[nodiscard]] auto operator==(const BlockId& other) const -> bool = default;
    [[nodiscard]] auto operator<=>(const BlockId& other) const = default;

    /// Validate that this ID matches the expected format.
    [[nodiscard]] auto is_valid() const -> bool;

    /// Generate a new unique block ID using current timestamp + random suffix.
    [[nodiscard]] static auto generate() -> BlockId;
};

/// Hash support for BlockId in unordered containers.
struct BlockIdHash
{
    [[nodiscard]] auto operator()(const BlockId& id) const noexcept -> std::size_t
    {
        return std::hash<std::string>{}(id.value);
    }
};

/// Type of block reference.
enum class RefType : std::uint8_t
{
    BlockRef,          // ((id "text"))
    BlockEmbed,        // {{id}}
    FileAnnotationRef, // <<path/annotation "text">>
    VirtualRef         // auto-detected name mention
};

/// A single parsed reference found in a block's content.
struct ParsedRef
{
    RefType type{RefType::BlockRef};

    /// The ID of the block being referenced (the definition/target).
    BlockId def_block_id;

    /// Optional anchor text provided in the reference syntax.
    std::optional<std::string> anchor_text;

    /// For file annotation refs: the asset path component.
    std::optional<std::string> asset_path;

    /// Character offset of the reference start within the source block's content.
    std::size_t source_offset{0};

    /// Character length of the entire reference syntax in source.
    std::size_t source_length{0};
};

/// A resolved reference with full context for display.
struct ResolvedRef
{
    ParsedRef parsed;

    /// The source block that contains this reference.
    BlockId source_block_id;

    /// The root document ID containing the source block.
    std::string source_root_id;

    /// Resolved display text (anchor text if provided, else target block's first line).
    std::string display_text;

    /// Whether the target block was found in the index.
    bool target_exists{false};

    /// If target exists, its block type.
    std::string target_type;

    /// If target exists, a content snippet (first 100 chars).
    std::string target_snippet;
};

/// Entry in the reference index.
struct RefIndexEntry
{
    BlockId def_block_id;

    /// All source block IDs that reference this definition.
    std::unordered_set<BlockId, BlockIdHash> referencing_block_ids;

    /// Total reference count.
    std::size_t total_ref_count{0};
};

/// In-memory reference index providing O(1) lookups by def or source block.
class RefIndex
{
public:
    /// Add a reference from source_block to def_block.
    void add_ref(const BlockId& source_block, const BlockId& def_block, RefType type);

    /// Remove all references originating from the given source block.
    void remove_refs_by_source(const BlockId& source_block);

    /// Get all blocks that reference the given definition block.
    [[nodiscard]] auto get_refs_to(const BlockId& def_block) const -> std::vector<BlockId>;

    /// Get all blocks that the given source block references.
    [[nodiscard]] auto get_refs_from(const BlockId& source_block) const -> std::vector<BlockId>;

    /// Get the reference count for a definition block.
    [[nodiscard]] auto ref_count(const BlockId& def_block) const -> std::size_t;

    /// Check if a circular reference exists between two blocks.
    [[nodiscard]] auto has_circular_ref(const BlockId& block_a, const BlockId& block_b) const
        -> bool;

    /// Clear the entire index.
    void clear();

    /// Total number of unique references tracked.
    [[nodiscard]] auto total_refs() const -> std::size_t;

private:
    /// def_block_id -> set of source_block_ids
    std::unordered_map<BlockId, std::unordered_set<BlockId, BlockIdHash>, BlockIdHash>
        forward_index_;

    /// source_block_id -> set of def_block_ids
    std::unordered_map<BlockId, std::unordered_set<BlockId, BlockIdHash>, BlockIdHash>
        reverse_index_;

    std::size_t total_ref_count_{0};
};

} // namespace markamp::core
