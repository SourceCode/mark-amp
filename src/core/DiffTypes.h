// ============================================================================
// File: src/core/DiffTypes.h
// Phase 33: Version Diff & Comparison — Block-level diff types
// ============================================================================
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Type of diff change at the block level.
enum class DiffType : uint8_t
{
    Unchanged,
    Added,
    Deleted,
    Modified,
    Moved
};

/// Diff view mode for the UI.
enum class DiffViewMode : uint8_t
{
    Inline,
    SideBySide
};

/// Options for configuring diff computation.
struct DiffOptions
{
    int context_lines{3};          // Number of context lines around changes
    bool ignore_whitespace{false}; // Ignore whitespace differences
    bool ignore_case{false};       // Ignore case differences
    bool word_level_diff{false};   // Compute word-level diffs within lines
    DiffViewMode view_mode{DiffViewMode::Inline};
};

/// Result of comparing a single block between two versions.
struct BlockDiff
{
    std::string block_id;
    DiffType diff_type{DiffType::Unchanged};
    std::string old_content;
    std::string new_content;
    std::string old_block_type; // e.g., "paragraph", "heading"
    std::string new_block_type;
    int old_position{-1};   // Position in old document
    int new_position{-1};   // Position in new document
    double similarity{1.0}; // 0.0 = completely different, 1.0 = identical

    [[nodiscard]] auto is_changed() const -> bool
    {
        return diff_type != DiffType::Unchanged;
    }

    [[nodiscard]] auto is_type_changed() const -> bool
    {
        return old_block_type != new_block_type;
    }
};

/// Aggregate statistics for a diff result.
struct DiffStats
{
    int total_blocks{0};
    int unchanged_blocks{0};
    int added_blocks{0};
    int deleted_blocks{0};
    int modified_blocks{0};
    int moved_blocks{0};

    [[nodiscard]] auto changed_blocks() const -> int
    {
        return added_blocks + deleted_blocks + modified_blocks + moved_blocks;
    }

    [[nodiscard]] auto change_percentage() const -> double
    {
        if (total_blocks == 0)
        {
            return 0.0;
        }
        return (static_cast<double>(changed_blocks()) / static_cast<double>(total_blocks)) * 100.0;
    }

    [[nodiscard]] auto summary() const -> std::string
    {
        return std::to_string(total_blocks) + " blocks, " + std::to_string(added_blocks) +
               " added, " + std::to_string(deleted_blocks) + " deleted, " +
               std::to_string(modified_blocks) + " modified";
    }
};

/// Full result of comparing two documents at the block level.
struct BlockDiffResult
{
    std::string old_doc_id;
    std::string new_doc_id;
    std::vector<BlockDiff> diffs;
    DiffStats stats;
    int64_t compute_time_ms{0};
};

} // namespace markamp::core
