#include "TableSortFilter.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstdlib>
#include <iterator>
#include <numeric>

namespace markamp::canvas
{

namespace
{

auto to_lower(const std::string& str) -> std::string
{
    std::string lower;
    lower.reserve(str.size());
    for (const auto character : str)
    {
        lower.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(character))));
    }
    return lower;
}

auto try_parse_double(const std::string& str) -> std::pair<bool, double>
{
    if (str.empty())
    {
        return {false, 0.0};
    }

    char* end_ptr = nullptr;
    const double value = std::strtod(str.c_str(), &end_ptr);

    // Check if entire string was consumed and no error occurred.
    if (end_ptr == str.c_str() + str.size() && !std::isinf(value) && !std::isnan(value))
    {
        return {true, value};
    }
    return {false, 0.0};
}

auto contains_substring(const std::string& haystack, const std::string& needle, bool case_sensitive)
    -> bool
{
    if (needle.empty())
    {
        return true;
    }

    if (case_sensitive)
    {
        return haystack.find(needle) != std::string::npos;
    }

    const auto lower_haystack = to_lower(haystack);
    const auto lower_needle_str = to_lower(needle);
    return lower_haystack.find(lower_needle_str) != std::string::npos;
}

} // anonymous namespace

auto TableSortFilter::sort_by_column(TableObject& table, size_t col_idx, SortDirection dir) -> void
{
    if (col_idx >= table.column_count())
    {
        return;
    }

    auto& rows = table.rows_mut();

    std::stable_sort(
        rows.begin(),
        rows.end(),
        [col_idx, dir](const std::vector<TableCell>& lhs, const std::vector<TableCell>& rhs)
        {
            const auto& lhs_text = lhs.at(col_idx).text;
            const auto& rhs_text = rhs.at(col_idx).text;

            if (dir == SortDirection::kAscending)
            {
                return lhs_text < rhs_text;
            }
            return lhs_text > rhs_text;
        });
}

auto TableSortFilter::sort_numeric(TableObject& table, size_t col_idx, SortDirection dir) -> void
{
    if (col_idx >= table.column_count())
    {
        return;
    }

    auto& rows = table.rows_mut();

    std::stable_sort(
        rows.begin(),
        rows.end(),
        [col_idx, dir](const std::vector<TableCell>& lhs, const std::vector<TableCell>& rhs)
        {
            const auto [lhs_ok, lhs_val] = try_parse_double(lhs.at(col_idx).text);
            const auto [rhs_ok, rhs_val] = try_parse_double(rhs.at(col_idx).text);

            // Non-numeric values fall to the end.
            if (!lhs_ok && !rhs_ok)
            {
                return false;
            }
            if (!lhs_ok)
            {
                return false; // lhs is non-numeric -> after rhs
            }
            if (!rhs_ok)
            {
                return true; // rhs is non-numeric -> after lhs
            }

            if (dir == SortDirection::kAscending)
            {
                return lhs_val < rhs_val;
            }
            return lhs_val > rhs_val;
        });
}

auto TableSortFilter::filter_rows(const TableObject& table, const FilterPredicate& pred)
    -> std::vector<size_t>
{
    std::vector<size_t> matching;

    if (pred.column_index >= table.column_count())
    {
        return matching;
    }

    for (size_t ri = 0; ri < table.row_count(); ++ri)
    {
        const auto& cell_text = table.get_cell(ri, pred.column_index).text;
        if (contains_substring(cell_text, pred.pattern, pred.case_sensitive))
        {
            matching.push_back(ri);
        }
    }

    return matching;
}

auto TableSortFilter::filter_rows_multi(const TableObject& table,
                                        const std::vector<FilterPredicate>& preds)
    -> std::vector<size_t>
{
    if (preds.empty())
    {
        // No predicates → all rows match.
        std::vector<size_t> all(table.row_count());
        std::iota(all.begin(), all.end(), 0);
        return all;
    }

    // Start with first predicate, then intersect.
    auto result = filter_rows(table, preds[0]);

    for (size_t pi = 1; pi < preds.size(); ++pi)
    {
        const auto filtered = filter_rows(table, preds[pi]);

        std::vector<size_t> intersected;
        std::set_intersection(result.begin(),
                              result.end(),
                              filtered.begin(),
                              filtered.end(),
                              std::back_inserter(intersected));
        result = std::move(intersected);
    }

    return result;
}

auto TableSortFilter::remove_filtered_rows(TableObject& table,
                                           const std::vector<size_t>& keep_indices) -> void
{
    if (keep_indices.empty())
    {
        // Remove all rows.
        while (table.row_count() > 0)
        {
            table.remove_row(table.row_count() - 1);
        }
        return;
    }

    // Build a set of indices to keep for O(1) lookup.
    std::vector<bool> keep(table.row_count(), false);
    for (const auto keep_idx : keep_indices)
    {
        if (keep_idx < table.row_count())
        {
            keep[keep_idx] = true;
        }
    }

    // Remove from back to front to preserve indices.
    for (size_t ri = table.row_count(); ri > 0; --ri)
    {
        if (!keep[ri - 1])
        {
            table.remove_row(ri - 1);
        }
    }
}

} // namespace markamp::canvas
