/// @file NotebookSearchIndex.cpp
/// @brief V8 Phase 15 – Full-text search across notebook cells implementation.

#include "core/NotebookSearchIndex.h"

#include "core/Events.h"

#include <algorithm>
#include <cctype>
#include <regex>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookSearchIndex::NotebookSearchIndex(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// Indexing
// ============================================================================

auto NotebookSearchIndex::index_notebook(const std::vector<IndexedCell>& cells) -> void
{
    indexed_cells_.clear();
    for (const auto& cell : cells)
    {
        add_cell(cell);
    }
}

auto NotebookSearchIndex::add_cell(const IndexedCell& cell) -> void
{
    IndexedCell indexed = cell;
    indexed.source_lines = split_lines(cell.source);
    indexed.output_lines = split_lines(cell.output_text);
    indexed_cells_.push_back(std::move(indexed));
}

// ============================================================================
// Search
// ============================================================================

auto NotebookSearchIndex::search(const std::string& query, const CellSearchOptions& options) const
    -> std::vector<CellSearchResult>
{
    std::vector<CellSearchResult> results;

    if (query.empty())
    {
        return results;
    }

    // Prepare case-insensitive query if needed.
    std::string lower_query;
    if (!options.case_sensitive)
    {
        lower_query.resize(query.size());
        std::transform(
            query.begin(),
            query.end(),
            lower_query.begin(),
            [](char character)
            { return static_cast<char>(std::tolower(static_cast<unsigned char>(character))); });
    }

    std::optional<std::regex> regex_pattern;
    if (options.regex)
    {
        auto flags = std::regex::ECMAScript;
        if (!options.case_sensitive)
        {
            flags |= std::regex::icase;
        }
        regex_pattern.emplace(query, flags);
    }

    for (const auto& cell : indexed_cells_)
    {
        if (!cell_type_matches(cell.cell_type, options))
        {
            continue;
        }

        // Search source lines.
        for (int line_idx = 0; line_idx < static_cast<int>(cell.source_lines.size()); ++line_idx)
        {
            if (static_cast<int>(results.size()) >= options.max_results)
            {
                break;
            }

            const auto& line = cell.source_lines[static_cast<size_t>(line_idx)];
            bool found = false;
            std::string match_text;

            if (options.regex && regex_pattern.has_value())
            {
                std::smatch match;
                if (std::regex_search(line, match, regex_pattern.value()))
                {
                    found = true;
                    match_text = match.str();
                }
            }
            else if (options.case_sensitive)
            {
                auto pos = line.find(query);
                if (pos != std::string::npos)
                {
                    found = true;
                    match_text = query;
                }
            }
            else
            {
                auto pos = find_case_insensitive(line, lower_query);
                if (pos.has_value())
                {
                    found = true;
                    match_text = line.substr(pos.value(), query.size());
                }
            }

            if (found)
            {
                CellSearchResult result;
                result.cell_id = cell.cell_id;
                result.cell_index = cell.cell_index;
                result.line_number = line_idx + 1;
                result.match_text = match_text;
                result.context_line = line;
                result.cell_type = cell.cell_type;
                result.in_output = false;
                results.push_back(std::move(result));
            }
        }

        // Search output lines if requested.
        if (options.search_outputs)
        {
            for (int line_idx = 0; line_idx < static_cast<int>(cell.output_lines.size());
                 ++line_idx)
            {
                if (static_cast<int>(results.size()) >= options.max_results)
                {
                    break;
                }

                const auto& line = cell.output_lines[static_cast<size_t>(line_idx)];
                bool found = false;
                std::string match_text;

                if (options.case_sensitive)
                {
                    auto pos = line.find(query);
                    if (pos != std::string::npos)
                    {
                        found = true;
                        match_text = query;
                    }
                }
                else
                {
                    auto pos = find_case_insensitive(line, lower_query);
                    if (pos.has_value())
                    {
                        found = true;
                        match_text = line.substr(pos.value(), query.size());
                    }
                }

                if (found)
                {
                    CellSearchResult result;
                    result.cell_id = cell.cell_id;
                    result.cell_index = cell.cell_index;
                    result.line_number = line_idx + 1;
                    result.match_text = match_text;
                    result.context_line = line;
                    result.cell_type = cell.cell_type;
                    result.in_output = true;
                    results.push_back(std::move(result));
                }
            }
        }
    }

    // Publish search completed event.
    events::NotebookSearchCompletedEvent evt;
    evt.query = query;
    evt.result_count = static_cast<int>(results.size());
    event_bus_.publish(evt);

    return results;
}

auto NotebookSearchIndex::search_in_outputs(const std::string& query) const
    -> std::vector<CellSearchResult>
{
    CellSearchOptions options;
    options.search_outputs = true;

    // We still need to iterate, but only collect output matches.
    std::vector<CellSearchResult> results;
    if (query.empty())
    {
        return results;
    }

    std::string lower_query;
    lower_query.resize(query.size());
    std::transform(
        query.begin(),
        query.end(),
        lower_query.begin(),
        [](char character)
        { return static_cast<char>(std::tolower(static_cast<unsigned char>(character))); });

    for (const auto& cell : indexed_cells_)
    {
        for (int line_idx = 0; line_idx < static_cast<int>(cell.output_lines.size()); ++line_idx)
        {
            const auto& line = cell.output_lines[static_cast<size_t>(line_idx)];
            auto pos = find_case_insensitive(line, lower_query);
            if (pos.has_value())
            {
                CellSearchResult result;
                result.cell_id = cell.cell_id;
                result.cell_index = cell.cell_index;
                result.line_number = line_idx + 1;
                result.match_text = line.substr(pos.value(), query.size());
                result.context_line = line;
                result.cell_type = cell.cell_type;
                result.in_output = true;
                results.push_back(std::move(result));
            }
        }
    }

    return results;
}

// ============================================================================
// Replace
// ============================================================================

auto NotebookSearchIndex::replace_in_cells(const std::string& query, const std::string& replacement)
    -> int
{
    int count = 0;

    for (auto& cell : indexed_cells_)
    {
        size_t pos = 0;
        while ((pos = cell.source.find(query, pos)) != std::string::npos)
        {
            cell.source.replace(pos, query.size(), replacement);
            pos += replacement.size();
            ++count;
        }

        // Re-split lines after replacement.
        if (count > 0)
        {
            cell.source_lines = split_lines(cell.source);
        }
    }

    return count;
}

// ============================================================================
// Index management
// ============================================================================

auto NotebookSearchIndex::clear_index() -> void
{
    indexed_cells_.clear();
}

auto NotebookSearchIndex::indexed_cell_count() const -> int
{
    return static_cast<int>(indexed_cells_.size());
}

auto NotebookSearchIndex::total_lines() const -> int
{
    int total = 0;
    for (const auto& cell : indexed_cells_)
    {
        total += static_cast<int>(cell.source_lines.size());
        total += static_cast<int>(cell.output_lines.size());
    }
    return total;
}

// ============================================================================
// Static helpers
// ============================================================================

auto NotebookSearchIndex::split_lines(const std::string& text) -> std::vector<std::string>
{
    std::vector<std::string> lines;
    if (text.empty())
    {
        return lines;
    }

    std::istringstream stream(text);
    std::string line;
    while (std::getline(stream, line))
    {
        lines.push_back(std::move(line));
    }
    return lines;
}

auto NotebookSearchIndex::find_case_insensitive(const std::string& haystack,
                                                const std::string& needle,
                                                size_t start_pos) -> std::optional<size_t>
{
    if (needle.empty() || haystack.size() < needle.size())
    {
        return std::nullopt;
    }

    std::string lower_haystack;
    lower_haystack.resize(haystack.size());
    std::transform(
        haystack.begin(),
        haystack.end(),
        lower_haystack.begin(),
        [](char character)
        { return static_cast<char>(std::tolower(static_cast<unsigned char>(character))); });

    auto pos = lower_haystack.find(needle, start_pos);
    if (pos != std::string::npos)
    {
        return pos;
    }
    return std::nullopt;
}

auto NotebookSearchIndex::cell_type_matches(const std::string& cell_type,
                                            const CellSearchOptions& options) -> bool
{
    if (cell_type == "code")
    {
        return options.search_code_cells;
    }
    if (cell_type == "markdown")
    {
        return options.search_markdown_cells;
    }
    if (cell_type == "raw")
    {
        return options.search_raw_cells;
    }
    return true;
}

} // namespace markamp::core
