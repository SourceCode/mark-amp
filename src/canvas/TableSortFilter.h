#pragma once

#include "canvas/TableObject.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

/// Sort direction for TableSortFilter.
enum class SortDirection : uint8_t
{
    kAscending,
    kDescending
};

/// A filter predicate for matching table rows.
struct FilterPredicate
{
    size_t column_index{0};
    std::string pattern;
    bool case_sensitive{false};
};

/// Static utility class for sorting and filtering TableObject data.
class TableSortFilter
{
public:
    TableSortFilter() = delete;

    /// Sort rows by text in the given column (stable sort, lexicographic).
    static auto sort_by_column(TableObject& table, size_t col_idx, SortDirection dir) -> void;

    /// Sort rows by numeric interpretation of cell text.
    /// Non-numeric values fall to the end.
    static auto sort_numeric(TableObject& table, size_t col_idx, SortDirection dir) -> void;

    /// Return indices of rows matching the given predicate (substring match).
    [[nodiscard]] static auto filter_rows(const TableObject& table, const FilterPredicate& pred)
        -> std::vector<size_t>;

    /// Return indices of rows matching ALL given predicates (AND logic).
    [[nodiscard]] static auto filter_rows_multi(const TableObject& table,
                                                const std::vector<FilterPredicate>& preds)
        -> std::vector<size_t>;

    /// Keep only the rows at the given indices; remove all others.
    static auto remove_filtered_rows(TableObject& table, const std::vector<size_t>& keep_indices)
        -> void;
};

} // namespace markamp::canvas
