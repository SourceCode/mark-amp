# Phase 20 -- Table Sorting and Filtering

## Objective

Add column sorting (ascending/descending by text or numeric value), row filtering (text match, contains, equals, greater/less than), and search-within-table functionality to TableObject. Includes column header dropdown menus and a filter bar UI.

## Prerequisites

- Phase 19 (TableObject with cells, columns, rows)
- Phase 05 (UndoRedoStack)

## Feature References (PRD)

- PRD #71: Table Sorting & Filtering

## Data Structures to Implement

### File: `src/canvas/TableSortFilter.h`

```cpp
#pragma once

#include "TableObject.h"

#include <functional>
#include <optional>
#include <string>
#include <vector>

namespace markamp::canvas
{

enum class SortDirection : uint8_t { Ascending, Descending, None };
enum class FilterOp : uint8_t { Contains, Equals, StartsWith, EndsWith, GreaterThan, LessThan, NotEmpty, Empty };

struct ColumnFilter
{
    int column_index{0};
    FilterOp op{FilterOp::Contains};
    std::string value;
};

struct SortSpec
{
    int column_index{0};
    SortDirection direction{SortDirection::Ascending};
};

class TableSortFilter
{
public:
    /// Sort rows by a column. Returns sorted row indices.
    [[nodiscard]] auto sort_rows(const TableObject& table, const SortSpec& spec) const
        -> std::vector<int>;

    /// Filter rows. Returns indices of rows that pass the filter.
    [[nodiscard]] auto filter_rows(const TableObject& table,
                                    const std::vector<ColumnFilter>& filters) const
        -> std::vector<int>;

    /// Combined sort + filter.
    [[nodiscard]] auto sort_and_filter(const TableObject& table,
                                        const SortSpec& sort,
                                        const std::vector<ColumnFilter>& filters) const
        -> std::vector<int>;

    /// Search all cells for a query string. Returns (row, col) pairs.
    [[nodiscard]] auto search(const TableObject& table, const std::string& query) const
        -> std::vector<std::pair<int, int>>;

private:
    [[nodiscard]] auto compare_cells(const std::string& a, const std::string& b,
                                      SortDirection dir) const -> bool;
    [[nodiscard]] auto matches_filter(const std::string& cell_text,
                                       const ColumnFilter& filter) const -> bool;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `TableSortFilter::sort_rows()` -- Create index vector [0..n]. Sort by comparing cell text at the sort column. Try numeric comparison first; fall back to lexicographic.

2. `TableSortFilter::filter_rows()` -- For each row, check all filters. Row passes if all filters match (AND logic).

3. Column header dropdown: Click column header to show sort/filter menu. Options: Sort Ascending, Sort Descending, Filter by... The TableRenderer renders sort direction indicators in the header.

4. Filter bar: When a filter is active, a small bar below the header shows active filters with X buttons to remove.

## Test Cases

File: `tests/unit/test_table_sort_filter.cpp`

1. **Sort ascending text** -- Sort ["c", "a", "b"], verify order a, b, c.
2. **Sort descending text** -- Verify c, b, a.
3. **Sort numeric** -- Sort ["10", "2", "20"], verify 2, 10, 20 (not lexicographic).
4. **Filter contains** -- Filter for "abc", verify only matching rows.
5. **Filter equals** -- Filter for exact match.
6. **Filter not empty** -- Verify filters out empty cells.
7. **Combined sort + filter** -- Filter then sort, verify correct order of passing rows.
8. **Search** -- Search for "xyz", verify cell coordinates returned.
9. **Multiple filters** -- Two column filters, verify AND logic.
10. **Header row excluded** -- Verify header row not included in sort/filter results.

## Acceptance Criteria

- [ ] Column sorting (ascending, descending, numeric-aware)
- [ ] Row filtering with 8 filter operations
- [ ] Search within table cells
- [ ] Column header dropdown for sort/filter
- [ ] Sort indicator in column header
- [ ] Active filter bar with remove buttons
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/TableSortFilter.h` | Sort/filter engine |
| CREATE | `src/canvas/TableSortFilter.cpp` | Sort/filter implementation |
| MODIFY | `src/canvas/TableRenderer.cpp` | Add sort indicators, filter bar |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_table_sort_filter.cpp` | 10 Catch2 tests |

## Architecture Notes

- Sort and filter produce row index lists rather than modifying the table data. The renderer uses these indices to control display order and visibility.
- Numeric comparison attempts std::stod; if both values parse as numbers, compare numerically.

## Estimated Complexity

**M** -- Sort/filter algorithms, numeric detection, column header UI, filter bar, 10 tests.
