/// @file NotebookSearchIndex.h
/// @brief V8 Phase 15 – Full-text search across notebook cells and outputs.

#pragma once

#include "core/EventBus.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Search options
// ============================================================================

/// Options controlling notebook cell search behavior.
struct CellSearchOptions
{
    bool case_sensitive{false};
    bool regex{false};
    bool search_code_cells{true};
    bool search_markdown_cells{true};
    bool search_raw_cells{true};
    bool search_outputs{false};
    int max_results{100};
};

// ============================================================================
// Search result
// ============================================================================

/// A single search result within a notebook cell.
struct CellSearchResult
{
    std::string cell_id;
    int cell_index{0};
    int line_number{0};
    std::string match_text;   ///< The matched substring
    std::string context_line; ///< Full line containing the match
    std::string cell_type;    ///< "code", "markdown", "raw"
    bool in_output{false};    ///< true if match is in output, not source
};

// ============================================================================
// Indexed cell entry (internal)
// ============================================================================

/// Internal representation of an indexed cell for search.
struct IndexedCell
{
    std::string cell_id;
    int cell_index{0};
    std::string cell_type;
    std::string source;
    std::string output_text;
    std::vector<std::string> source_lines;
    std::vector<std::string> output_lines;
};

// ============================================================================
// NotebookSearchIndex
// ============================================================================

class NotebookSearchIndex
{
public:
    explicit NotebookSearchIndex(EventBus& event_bus);

    /// Build the search index from a list of cells.
    auto index_notebook(const std::vector<IndexedCell>& cells) -> void;

    /// Add a single cell to the index.
    auto add_cell(const IndexedCell& cell) -> void;

    /// Search across indexed cells with the given query and options.
    [[nodiscard]] auto search(const std::string& query, const CellSearchOptions& options = {}) const
        -> std::vector<CellSearchResult>;

    /// Search only within cell outputs.
    [[nodiscard]] auto search_in_outputs(const std::string& query) const
        -> std::vector<CellSearchResult>;

    /// Find-and-replace across cell sources (returns count of replacements).
    auto replace_in_cells(const std::string& query, const std::string& replacement) -> int;

    /// Clear the entire search index.
    auto clear_index() -> void;

    /// Get the total number of indexed cells.
    [[nodiscard]] auto indexed_cell_count() const -> int;

    /// Get the total number of indexed lines (source + output).
    [[nodiscard]] auto total_lines() const -> int;

private:
    EventBus& event_bus_;
    std::vector<IndexedCell> indexed_cells_;

    /// Split text into lines for line-by-line searching.
    [[nodiscard]] static auto split_lines(const std::string& text) -> std::vector<std::string>;

    /// Case-insensitive string find.
    [[nodiscard]] static auto find_case_insensitive(const std::string& haystack,
                                                    const std::string& needle,
                                                    size_t start_pos = 0) -> std::optional<size_t>;

    /// Check if a cell type matches the search options.
    [[nodiscard]] static auto cell_type_matches(const std::string& cell_type,
                                                const CellSearchOptions& options) -> bool;
};

} // namespace markamp::core
