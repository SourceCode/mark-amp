#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Sort direction.
enum class SortDirection : uint8_t
{
    kNone,
    kAscending,
    kDescending,
};

/// Table cell.
struct TableCell
{
    std::string value;
    std::string format{"text"}; ///< "text", "number", "date"

    // ── Round 6 Batch 10 (#91-93) ───────────────────────────────

    /// (#91) Whether a value is set.
    [[nodiscard]] auto has_value() const noexcept -> bool
    {
        return !value.empty();
    }

    /// (#92) Whether format is text.
    [[nodiscard]] auto is_text() const noexcept -> bool
    {
        return format == "text";
    }

    /// (#93) Whether format is number.
    [[nodiscard]] auto is_number() const noexcept -> bool
    {
        return format == "number";
    }
};

/// Column sort state.
struct ColumnSort
{
    int column{-1};
    SortDirection direction{SortDirection::kNone};

    // ── Round 6 Batch 10 (#94-96) ───────────────────────────────

    /// (#94) Whether sorting is active.
    [[nodiscard]] auto is_sorted() const noexcept -> bool
    {
        return direction != SortDirection::kNone;
    }

    /// (#95) Whether sorting is ascending.
    [[nodiscard]] auto is_ascending() const noexcept -> bool
    {
        return direction == SortDirection::kAscending;
    }

    /// (#96) Whether sorting is descending.
    [[nodiscard]] auto is_descending() const noexcept -> bool
    {
        return direction == SortDirection::kDescending;
    }
};

/// Testable model for Table Object Editing (Phase 56).
///
/// Encapsulates:
/// - Grid of cells with row/column operations
/// - Cell editing with tab navigation
/// - Column sorting
/// - CSV import/export
/// - Formatting (header row, alternate row styling)
class TableModel
{
public:
    // ── Grid ────────────────────────────────────────────────────────

    void set_grid(int rows, int cols);
    [[nodiscard]] auto rows() const -> int;
    [[nodiscard]] auto cols() const -> int;

    void set_cell(int row, int col, const std::string& value);
    [[nodiscard]] auto cell(int row, int col) const -> std::string;

    void insert_row(int at);
    void insert_column(int at);
    void delete_row(int at);
    void delete_column(int at);

    // ── Navigation ──────────────────────────────────────────────────

    void set_active_cell(int row, int col);
    [[nodiscard]] auto active_row() const -> int;
    [[nodiscard]] auto active_col() const -> int;
    void tab_next();

    // ── Sort ────────────────────────────────────────────────────────

    void set_sort(ColumnSort sort);
    [[nodiscard]] auto sort() const -> const ColumnSort&;

    // ── Formatting ──────────────────────────────────────────────────

    void set_header_row(bool enabled);
    [[nodiscard]] auto has_header_row() const -> bool;

    void set_alternate_rows(bool enabled);
    [[nodiscard]] auto alternate_rows() const -> bool;

    // ── CSV ─────────────────────────────────────────────────────────

    [[nodiscard]] auto to_csv() const -> std::string;
    void from_csv(const std::string& csv);

private:
    std::vector<std::vector<TableCell>> grid_;
    int rows_{0};
    int cols_{0};
    int active_row_{0};
    int active_col_{0};
    ColumnSort sort_;
    bool header_row_{true};
    bool alternate_rows_{false};

    // ── Round 6 Batch 10 (#97-100) ───────────────────────────────

    /// (#97) Whether the table has no data.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return rows_ == 0 || cols_ == 0;
    }

    /// (#98) Total number of cells.
    [[nodiscard]] auto cell_count() const noexcept -> int
    {
        return rows_ * cols_;
    }

    /// (#99) Whether active cell is in the first row.
    [[nodiscard]] auto is_first_row() const noexcept -> bool
    {
        return active_row_ == 0;
    }

    /// (#100) Whether active cell is in the first column.
    [[nodiscard]] auto is_first_col() const noexcept -> bool
    {
        return active_col_ == 0;
    }
};

} // namespace markamp::canvas
