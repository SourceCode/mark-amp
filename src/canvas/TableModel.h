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
};

/// Column sort state.
struct ColumnSort
{
    int column{-1};
    SortDirection direction{SortDirection::kNone};
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
};

} // namespace markamp::canvas
