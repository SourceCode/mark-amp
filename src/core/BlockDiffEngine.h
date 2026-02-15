// ============================================================================
// File: src/core/BlockDiffEngine.h
// Phase 33: Version Diff & Comparison — Block-level diff engine
// ============================================================================
#pragma once

#include "DiffTypes.h"

#include <string>
#include <vector>

namespace markamp::core
{

class DiffEngine; // Phase 32 text diff engine (forward declared)

/// Block-level structural diff engine.
/// Compares two documents at the block level, delegating text-level
/// diffing to the Phase 32 `DiffEngine`.
class BlockDiffEngine
{
public:
    BlockDiffEngine() = default;

    /// Compare two documents given their block lists.
    [[nodiscard]] auto
    diff_documents(const std::vector<std::pair<std::string, std::string>>& old_blocks,
                   const std::vector<std::pair<std::string, std::string>>& new_blocks,
                   const DiffOptions& options = {}) const -> BlockDiffResult;

    /// Compare two documents given their full text content.
    /// Splits into blocks using headings/blank lines as delimiters.
    [[nodiscard]] auto diff_text_as_blocks(const std::string& old_text,
                                           const std::string& new_text,
                                           const DiffOptions& options = {}) const
        -> BlockDiffResult;

    /// Compute similarity between two block contents (0.0–1.0).
    [[nodiscard]] static auto compute_similarity(const std::string& left, const std::string& right)
        -> double;

private:
    /// Split text into logical blocks (by headings and paragraph breaks).
    [[nodiscard]] static auto split_into_blocks(const std::string& text)
        -> std::vector<std::pair<std::string, std::string>>; // (block_type, content)

    /// Match old blocks to new blocks using similarity.
    [[nodiscard]] auto
    match_blocks(const std::vector<std::pair<std::string, std::string>>& old_blocks,
                 const std::vector<std::pair<std::string, std::string>>& new_blocks,
                 double threshold = 0.5) const
        -> std::vector<std::pair<int, int>>; // (old_idx, new_idx) matches
};

} // namespace markamp::core
