/// @file NotebookDiffEngine.h
/// @brief V4 Phase 40 – Notebook Diff Engine.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

// ============================================================================
// Data structures
// ============================================================================

/// Type of cell-level change.
enum class CellDiffType
{
    kUnchanged,
    kAdded,
    kRemoved,
    kModified
};

/// A single cell used as input for diffing.
struct DiffCell
{
    std::string cell_id;
    std::string cell_type; ///< "code" or "markdown"
    std::string source;
    int execution_count{0};
};

/// Result for a single cell in the diff.
struct CellDiff
{
    CellDiffType type{CellDiffType::kUnchanged};
    DiffCell cell_a;        ///< Cell from notebook A (empty if added).
    DiffCell cell_b;        ///< Cell from notebook B (empty if removed).
    std::string line_diff;  ///< Line-level unified diff (for modified cells).
    double similarity{1.0}; ///< Similarity score (0.0 to 1.0).
};

/// Complete diff result between two notebooks.
struct NotebookDiffResult
{
    std::vector<CellDiff> cell_diffs;
    int cells_added{0};
    int cells_removed{0};
    int cells_modified{0};
    int cells_unchanged{0};
};

/// A merge conflict marker.
struct MergeConflict
{
    int cell_index{0};
    std::string base_content;
    std::string local_content;
    std::string remote_content;
};

/// Result of a three-way merge.
struct MergeResult
{
    std::vector<DiffCell> merged_cells;
    std::vector<MergeConflict> conflicts;
    bool has_conflicts{false};
};

// ============================================================================
// NotebookDiffEngine
// ============================================================================

class NotebookDiffEngine
{
public:
    explicit NotebookDiffEngine(EventBus& event_bus);

    /// Compute cell-level diff between two notebooks.
    [[nodiscard]] auto diff_notebooks(const std::vector<DiffCell>& cells_a,
                                      const std::vector<DiffCell>& cells_b) -> NotebookDiffResult;

    /// Compute line-level diff within a single cell.
    [[nodiscard]] auto diff_cell_content(const std::string& source_a,
                                         const std::string& source_b) const -> std::string;

    /// Three-way merge with conflict detection.
    [[nodiscard]] auto three_way_merge(const std::vector<DiffCell>& base,
                                       const std::vector<DiffCell>& local,
                                       const std::vector<DiffCell>& remote) -> MergeResult;

    /// Check if a merge result has conflicts.
    [[nodiscard]] static auto has_conflicts(const MergeResult& result) -> bool;

    /// Render diff as unified diff text.
    [[nodiscard]] auto format_diff_text(const NotebookDiffResult& result) const -> std::string;

    /// Render diff as side-by-side HTML.
    [[nodiscard]] auto format_diff_html(const NotebookDiffResult& result) const -> std::string;

    /// Compute Jaccard similarity between two cell sources.
    [[nodiscard]] static auto cell_similarity(const DiffCell& cell_a, const DiffCell& cell_b)
        -> double;

    /// Human-readable summary of the diff.
    [[nodiscard]] static auto summarize_diff(const NotebookDiffResult& result) -> std::string;

private:
    EventBus& event_bus_;

    /// Split text into lines.
    [[nodiscard]] static auto split_lines(const std::string& text) -> std::vector<std::string>;

    /// Compute LCS length between two sequences.
    [[nodiscard]] static auto lcs_length(const std::vector<std::string>& seq_a,
                                         const std::vector<std::string>& seq_b) -> int;
};

} // namespace markamp::core
