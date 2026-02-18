// ============================================================================
// File: src/core/IncrementalParsePolicy.h
// Phase 30: Performance Optimization — Incremental markdown parsing policy
// ============================================================================
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core
{

/// Describes an edit operation in the document.
struct EditRegion
{
    int32_t start_line{0}; ///< First edited line (0-indexed)
    int32_t end_line{0};   ///< Last edited line (inclusive, 0-indexed)
    int32_t lines_inserted{0};
    int32_t lines_deleted{0};
};

/// Describes the range of lines that need to be reparsed.
struct ReparseRange
{
    int32_t start_line{0}; ///< First line to reparse (0-indexed)
    int32_t end_line{0};   ///< Last line to reparse (inclusive, 0-indexed)
    bool is_full_reparse{false};

    /// Number of lines in the reparse range.
    [[nodiscard]] auto line_count() const noexcept -> int32_t
    {
        return end_line - start_line + 1;
    }
};

/// Block boundary type for structural analysis.
enum class BlockBoundaryKind : uint8_t
{
    kNone = 0,
    kCodeFence,     // ``` or ~~~
    kFrontMatter,   // ---
    kMathBlock,     // $$
    kHTMLBlock,     // <div>, <table>, etc.
    kBlockQuote,    // >
    kListItem,      // - / * / 1.
    kThematicBreak, // --- / *** / ___
};

/// Policy for determining minimal reparse ranges after edits.
///
/// Given a document and an edit region, computes the smallest range of
/// lines that must be reparsed to maintain correct AST state. Handles
/// block boundary detection (code fences, front matter, etc.) to determine
/// when a full reparse is necessary.
///
/// Phase 30 Task 14: Incremental markdown parsing.
class IncrementalParsePolicy
{
public:
    static constexpr int32_t kDefaultContextLines = 2;
    static constexpr int32_t kMaxContextLines = 10;

    IncrementalParsePolicy();

    /// Compute the reparse range for a given edit.
    /// @param edit The edit that was performed.
    /// @param total_lines Total number of lines in the document after the edit.
    /// @param lines Document lines after the edit.
    [[nodiscard]] auto compute_reparse_range(const EditRegion& edit,
                                             int32_t total_lines,
                                             const std::vector<std::string>& lines) const
        -> ReparseRange;

    /// Check if a full reparse is required for this edit.
    /// @param edit The edit that was performed.
    /// @param lines Document lines after the edit.
    [[nodiscard]] auto needs_full_reparse(const EditRegion& edit,
                                          const std::vector<std::string>& lines) const -> bool;

    /// Detect the block boundary kind of a given line.
    [[nodiscard]] static auto detect_boundary(std::string_view line) -> BlockBoundaryKind;

    /// Get/set the context window size.
    [[nodiscard]] auto context_lines() const noexcept -> int32_t;
    void set_context_lines(int32_t lines);

private:
    int32_t context_lines_{kDefaultContextLines};

    /// Check if an edit crosses an unclosed block boundary.
    [[nodiscard]] auto crosses_unclosed_block(const EditRegion& edit,
                                              const std::vector<std::string>& lines) const -> bool;

    /// Expand range to include surrounding block context.
    [[nodiscard]] auto expand_to_block_boundary(int32_t start,
                                                int32_t end_val,
                                                int32_t total_lines,
                                                const std::vector<std::string>& lines) const
        -> ReparseRange;
};

} // namespace markamp::core
