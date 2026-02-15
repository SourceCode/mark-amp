/// @file NotebookDiffEngine.cpp
/// @brief V4 Phase 40 – Notebook Diff Engine implementation.

#include "core/NotebookDiffEngine.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <sstream>
#include <unordered_set>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookDiffEngine::NotebookDiffEngine(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Cell-level diff (LCS-based matching)
// ============================================================================

auto NotebookDiffEngine::diff_notebooks(const std::vector<DiffCell>& cells_a,
                                        const std::vector<DiffCell>& cells_b) -> NotebookDiffResult
{
    NotebookDiffResult result;

    // Extract source strings for LCS matching.
    std::vector<std::string> sources_a;
    sources_a.reserve(cells_a.size());
    for (const auto& cell : cells_a)
    {
        sources_a.push_back(cell.source);
    }

    std::vector<std::string> sources_b;
    sources_b.reserve(cells_b.size());
    for (const auto& cell : cells_b)
    {
        sources_b.push_back(cell.source);
    }

    // Build LCS table.
    const size_t num_a = cells_a.size();
    const size_t num_b = cells_b.size();
    std::vector<std::vector<int>> lcs_table(num_a + 1, std::vector<int>(num_b + 1, 0));

    for (size_t idx_a = 1; idx_a <= num_a; ++idx_a)
    {
        for (size_t idx_b = 1; idx_b <= num_b; ++idx_b)
        {
            if (sources_a[idx_a - 1] == sources_b[idx_b - 1])
            {
                lcs_table[idx_a][idx_b] = lcs_table[idx_a - 1][idx_b - 1] + 1;
            }
            else
            {
                lcs_table[idx_a][idx_b] =
                    std::max(lcs_table[idx_a - 1][idx_b], lcs_table[idx_a][idx_b - 1]);
            }
        }
    }

    // Backtrack to produce edit script.
    size_t idx_a = num_a;
    size_t idx_b = num_b;
    std::vector<CellDiff> diffs_reversed;

    while (idx_a > 0 || idx_b > 0)
    {
        if (idx_a > 0 && idx_b > 0 && sources_a[idx_a - 1] == sources_b[idx_b - 1])
        {
            CellDiff diff;
            diff.type = CellDiffType::kUnchanged;
            diff.cell_a = cells_a[idx_a - 1];
            diff.cell_b = cells_b[idx_b - 1];
            diff.similarity = 1.0;
            diffs_reversed.push_back(std::move(diff));
            --idx_a;
            --idx_b;
        }
        else if (idx_b > 0 &&
                 (idx_a == 0 || lcs_table[idx_a][idx_b - 1] >= lcs_table[idx_a - 1][idx_b]))
        {
            // Cell added in B.
            CellDiff diff;
            diff.type = CellDiffType::kAdded;
            diff.cell_b = cells_b[idx_b - 1];
            diff.similarity = 0.0;
            diffs_reversed.push_back(std::move(diff));
            --idx_b;
        }
        else
        {
            // Cell removed from A.
            // Check if the next cell in B is similar (modified).
            if (idx_b > 0 && cell_similarity(cells_a[idx_a - 1], cells_b[idx_b - 1]) > 0.3)
            {
                CellDiff diff;
                diff.type = CellDiffType::kModified;
                diff.cell_a = cells_a[idx_a - 1];
                diff.cell_b = cells_b[idx_b - 1];
                diff.similarity = cell_similarity(cells_a[idx_a - 1], cells_b[idx_b - 1]);
                diff.line_diff =
                    diff_cell_content(cells_a[idx_a - 1].source, cells_b[idx_b - 1].source);
                diffs_reversed.push_back(std::move(diff));
                --idx_a;
                --idx_b;
            }
            else
            {
                CellDiff diff;
                diff.type = CellDiffType::kRemoved;
                diff.cell_a = cells_a[idx_a - 1];
                diff.similarity = 0.0;
                diffs_reversed.push_back(std::move(diff));
                --idx_a;
            }
        }
    }

    // Reverse to get correct order.
    std::reverse(diffs_reversed.begin(), diffs_reversed.end());
    result.cell_diffs = std::move(diffs_reversed);

    // Count summary.
    for (const auto& diff : result.cell_diffs)
    {
        switch (diff.type)
        {
            case CellDiffType::kAdded:
                ++result.cells_added;
                break;
            case CellDiffType::kRemoved:
                ++result.cells_removed;
                break;
            case CellDiffType::kModified:
                ++result.cells_modified;
                break;
            case CellDiffType::kUnchanged:
                ++result.cells_unchanged;
                break;
        }
    }

    events::NotebookDiffComputedEvent event;
    event.cells_changed = result.cells_added + result.cells_removed + result.cells_modified;
    event_bus_.publish(event);

    return result;
}

// ============================================================================
// Line-level diff
// ============================================================================

auto NotebookDiffEngine::diff_cell_content(const std::string& source_a,
                                           const std::string& source_b) const -> std::string
{
    const auto lines_a = split_lines(source_a);
    const auto lines_b = split_lines(source_b);

    std::ostringstream oss;
    oss << "--- a\n+++ b\n";

    const size_t max_lines = std::max(lines_a.size(), lines_b.size());
    for (size_t idx = 0; idx < max_lines; ++idx)
    {
        const bool has_a = idx < lines_a.size();
        const bool has_b = idx < lines_b.size();

        if (has_a && has_b)
        {
            if (lines_a[idx] != lines_b[idx])
            {
                oss << "-" << lines_a[idx] << "\n";
                oss << "+" << lines_b[idx] << "\n";
            }
            else
            {
                oss << " " << lines_a[idx] << "\n";
            }
        }
        else if (has_a)
        {
            oss << "-" << lines_a[idx] << "\n";
        }
        else if (has_b)
        {
            oss << "+" << lines_b[idx] << "\n";
        }
    }

    return oss.str();
}

// ============================================================================
// Three-way merge
// ============================================================================

auto NotebookDiffEngine::three_way_merge(const std::vector<DiffCell>& base,
                                         const std::vector<DiffCell>& local,
                                         const std::vector<DiffCell>& remote) -> MergeResult
{
    MergeResult result;

    // Build source-to-index maps for base.
    std::unordered_map<std::string, size_t> base_map;
    for (size_t idx = 0; idx < base.size(); ++idx)
    {
        base_map[base[idx].source] = idx;
    }

    // Track which base cells are consumed.
    std::vector<bool> base_consumed(base.size(), false);

    // Process local changes.
    std::unordered_set<std::string> local_sources;
    for (const auto& cell : local)
    {
        local_sources.insert(cell.source);
        auto base_it = base_map.find(cell.source);
        if (base_it != base_map.end())
        {
            base_consumed[base_it->second] = true;
        }
    }

    // Process remote changes.
    std::unordered_set<std::string> remote_sources;
    for (const auto& cell : remote)
    {
        remote_sources.insert(cell.source);
    }

    // Merge: take all local cells, then add remote-only cells.
    for (const auto& cell : local)
    {
        result.merged_cells.push_back(cell);
    }

    int conflict_index = 0;
    for (const auto& cell : remote)
    {
        if (local_sources.find(cell.source) == local_sources.end())
        {
            // Remote-only cell: check if it was in base.
            auto base_it = base_map.find(cell.source);
            if (base_it != base_map.end())
            {
                // Was in base but removed locally — conflict.
                MergeConflict conflict;
                conflict.cell_index = conflict_index;
                conflict.base_content = cell.source;
                conflict.local_content = ""; // Removed locally.
                conflict.remote_content = cell.source;
                result.conflicts.push_back(std::move(conflict));
            }
            else
            {
                // New cell in remote, add it.
                result.merged_cells.push_back(cell);
            }
        }
        ++conflict_index;
    }

    // Check for cells modified differently in local and remote.
    for (size_t base_idx = 0; base_idx < base.size(); ++base_idx)
    {
        const auto& base_cell = base[base_idx];
        bool in_local = local_sources.count(base_cell.source) > 0;
        bool in_remote = remote_sources.count(base_cell.source) > 0;

        if (!in_local && !in_remote)
        {
            // Both removed — no conflict, cell is gone.
            continue;
        }
    }

    result.has_conflicts = !result.conflicts.empty();

    events::NotebookMergeCompletedEvent event;
    event.conflicts = static_cast<int>(result.conflicts.size());
    event_bus_.publish(event);

    return result;
}

// ============================================================================
// Utilities
// ============================================================================

auto NotebookDiffEngine::has_conflicts(const MergeResult& result) -> bool
{
    return result.has_conflicts;
}

auto NotebookDiffEngine::format_diff_text(const NotebookDiffResult& result) const -> std::string
{
    std::ostringstream oss;

    for (size_t idx = 0; idx < result.cell_diffs.size(); ++idx)
    {
        const auto& diff = result.cell_diffs[idx];

        switch (diff.type)
        {
            case CellDiffType::kUnchanged:
                oss << "Cell " << idx << ": unchanged\n";
                break;
            case CellDiffType::kAdded:
                oss << "Cell " << idx << ": added\n";
                oss << "+ " << diff.cell_b.source << "\n";
                break;
            case CellDiffType::kRemoved:
                oss << "Cell " << idx << ": removed\n";
                oss << "- " << diff.cell_a.source << "\n";
                break;
            case CellDiffType::kModified:
                oss << "Cell " << idx << ": modified\n";
                oss << diff.line_diff;
                break;
        }
    }

    return oss.str();
}

auto NotebookDiffEngine::format_diff_html(const NotebookDiffResult& result) const -> std::string
{
    std::ostringstream oss;
    oss << "<div class=\"notebook-diff\">\n";

    for (size_t idx = 0; idx < result.cell_diffs.size(); ++idx)
    {
        const auto& diff = result.cell_diffs[idx];
        std::string css_class;

        switch (diff.type)
        {
            case CellDiffType::kUnchanged:
                css_class = "unchanged";
                break;
            case CellDiffType::kAdded:
                css_class = "added";
                break;
            case CellDiffType::kRemoved:
                css_class = "removed";
                break;
            case CellDiffType::kModified:
                css_class = "modified";
                break;
        }

        oss << "<div class=\"cell-diff " << css_class << "\">\n";
        oss << "<span class=\"cell-index\">Cell " << idx << "</span>\n";

        if (diff.type == CellDiffType::kRemoved || diff.type == CellDiffType::kModified)
        {
            oss << "<pre class=\"old\">" << diff.cell_a.source << "</pre>\n";
        }
        if (diff.type == CellDiffType::kAdded || diff.type == CellDiffType::kModified)
        {
            oss << "<pre class=\"new\">" << diff.cell_b.source << "</pre>\n";
        }

        oss << "</div>\n";
    }

    oss << "</div>\n";
    return oss.str();
}

auto NotebookDiffEngine::cell_similarity(const DiffCell& cell_a, const DiffCell& cell_b) -> double
{
    if (cell_a.source == cell_b.source)
    {
        return 1.0;
    }
    if (cell_a.source.empty() && cell_b.source.empty())
    {
        return 1.0;
    }
    if (cell_a.source.empty() || cell_b.source.empty())
    {
        return 0.0;
    }

    // Jaccard similarity on word tokens.
    auto tokenize = [](const std::string& text) -> std::unordered_set<std::string>
    {
        std::unordered_set<std::string> tokens;
        std::istringstream stream(text);
        std::string token;
        while (stream >> token)
        {
            tokens.insert(token);
        }
        return tokens;
    };

    const auto tokens_a = tokenize(cell_a.source);
    const auto tokens_b = tokenize(cell_b.source);

    int intersection_count = 0;
    for (const auto& token : tokens_a)
    {
        if (tokens_b.count(token) > 0)
        {
            ++intersection_count;
        }
    }

    const int union_count =
        static_cast<int>(tokens_a.size() + tokens_b.size()) - intersection_count;
    if (union_count == 0)
    {
        return 1.0;
    }

    return static_cast<double>(intersection_count) / static_cast<double>(union_count);
}

auto NotebookDiffEngine::summarize_diff(const NotebookDiffResult& result) -> std::string
{
    std::ostringstream oss;
    std::vector<std::string> parts;

    if (result.cells_added > 0)
    {
        parts.push_back(std::to_string(result.cells_added) + " cell(s) added");
    }
    if (result.cells_removed > 0)
    {
        parts.push_back(std::to_string(result.cells_removed) + " cell(s) removed");
    }
    if (result.cells_modified > 0)
    {
        parts.push_back(std::to_string(result.cells_modified) + " cell(s) modified");
    }
    if (result.cells_unchanged > 0)
    {
        parts.push_back(std::to_string(result.cells_unchanged) + " cell(s) unchanged");
    }

    for (size_t idx = 0; idx < parts.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ", ";
        }
        oss << parts[idx];
    }

    if (parts.empty())
    {
        oss << "No differences";
    }

    return oss.str();
}

auto NotebookDiffEngine::split_lines(const std::string& text) -> std::vector<std::string>
{
    std::vector<std::string> lines;
    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(line);
    }
    return lines;
}

auto NotebookDiffEngine::lcs_length(const std::vector<std::string>& seq_a,
                                    const std::vector<std::string>& seq_b) -> int
{
    const size_t len_a = seq_a.size();
    const size_t len_b = seq_b.size();
    std::vector<std::vector<int>> table(len_a + 1, std::vector<int>(len_b + 1, 0));

    for (size_t idx_a = 1; idx_a <= len_a; ++idx_a)
    {
        for (size_t idx_b = 1; idx_b <= len_b; ++idx_b)
        {
            if (seq_a[idx_a - 1] == seq_b[idx_b - 1])
            {
                table[idx_a][idx_b] = table[idx_a - 1][idx_b - 1] + 1;
            }
            else
            {
                table[idx_a][idx_b] = std::max(table[idx_a - 1][idx_b], table[idx_a][idx_b - 1]);
            }
        }
    }

    return table[len_a][len_b];
}

} // namespace markamp::core
