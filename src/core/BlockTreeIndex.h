#pragma once

#include "core/Block.h"

#include <cstdint>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace markamp::core
{

class BlockDatabase;
class EventBus;

// Lightweight metadata for the in-memory block tree index.
// This is a subset of Block fields optimized for fast lookups.
struct BlockTreeEntry
{
    std::string id;
    std::string root_id;
    std::string parent_id;
    std::string box_id;
    std::string path;
    std::string hpath;
    std::string updated;
    BlockType type = BlockType::Unknown;
};

// In-memory block tree index providing O(1) lookups on block ID
// with secondary indexes for root, box, path, and type queries.
// Thread-safe via shared_mutex for concurrent reads.
class BlockTreeIndex
{
public:
    BlockTreeIndex() = default;
    ~BlockTreeIndex() = default;

    BlockTreeIndex(const BlockTreeIndex&) = delete;
    auto operator=(const BlockTreeIndex&) -> BlockTreeIndex& = delete;
    BlockTreeIndex(BlockTreeIndex&&) = delete;
    auto operator=(BlockTreeIndex&&) -> BlockTreeIndex& = delete;

    // --- Initialization ---
    auto init(BlockDatabase& database) -> void;
    auto clear() -> void;

    // --- Single-Entry Operations ---
    [[nodiscard]] auto get(const std::string& block_id) const -> std::optional<BlockTreeEntry>;
    auto upsert(const BlockTreeEntry& entry) -> void;
    auto remove(const std::string& block_id) -> void;

    // --- Bulk Operations ---
    auto index_tree(const Block& root_block) -> void;
    auto remove_by_root(const std::string& root_id) -> void;
    auto remove_by_box(const std::string& box_id) -> void;

    // --- Queries ---
    [[nodiscard]] auto get_by_root(const std::string& root_id) const -> std::vector<BlockTreeEntry>;
    [[nodiscard]] auto get_root_by_path(const std::string& box_id, const std::string& path) const
        -> std::optional<BlockTreeEntry>;
    [[nodiscard]] auto get_by_box(const std::string& box_id) const -> std::vector<BlockTreeEntry>;
    [[nodiscard]] auto get_by_type(BlockType type) const -> std::vector<BlockTreeEntry>;

    // --- Statistics ---
    [[nodiscard]] auto count() const -> int64_t;
    [[nodiscard]] auto count_by_box(const std::string& box_id) const -> int64_t;
    [[nodiscard]] auto count_documents() const -> int64_t;

    // --- Validation ---
    [[nodiscard]] auto contains(const std::string& block_id) const -> bool;
    [[nodiscard]] auto find_orphans() const -> std::vector<BlockTreeEntry>;

private:
    // Primary index: block_id -> entry
    std::unordered_map<std::string, BlockTreeEntry> entries_;

    // Secondary indexes
    std::unordered_map<std::string, std::unordered_set<std::string>>
        root_index_; // root_id -> set<block_id>
    std::unordered_map<std::string, std::unordered_set<std::string>>
        box_index_;                                           // box_id -> set<block_id>
    std::unordered_map<std::string, std::string> path_index_; // "box_id:path" -> root block_id

    mutable std::shared_mutex mutex_;

    // Internal helpers (caller must hold lock)
    auto add_to_indexes(const BlockTreeEntry& entry) -> void;
    auto remove_from_indexes(const BlockTreeEntry& entry) -> void;
    auto index_block_recursive(const Block& block, const std::string& root_id) -> void;
};

} // namespace markamp::core
