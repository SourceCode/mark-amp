#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Text alignment for table cells (re-using the same concept from TextBox).
enum class TableCellAlignment : uint8_t
{
    kLeft,
    kCenter,
    kRight
};

/// A single cell in a TableObject.
struct TableCell
{
    std::string text;
    CanvasColor fill_color{255, 255, 255, 0}; // Transparent by default.
    CanvasColor text_color{0, 0, 0, 255};     // Black text.
    bool bold{false};
    TableCellAlignment alignment{TableCellAlignment::kLeft};

    // ── Round 2 Batch 2 (#18-20) ──────────────────────────────────

    /// (#18) Whether the cell text is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return text.empty();
    }

    /// (#19) Whether the cell is bold.
    [[nodiscard]] auto is_bold() const noexcept -> bool
    {
        return bold;
    }

    /// (#20) Whether the cell has a non-transparent fill.
    [[nodiscard]] auto has_fill() const noexcept -> bool
    {
        return fill_color.a > 0;
    }
};

/// Column definition for a TableObject.
struct TableColumn
{
    std::string header;
    double width{120.0};
    bool sortable{true};
};

/// A data grid table canvas object with typed cells and configurable rows/columns.
class TableObject : public CanvasObject
{
public:
    TableObject();

    // ── Columns ───────────────────────────────────────────────

    auto add_column(const std::string& header, double width = 120.0) -> void;
    auto remove_column(size_t col_idx) -> void;
    auto resize_column(size_t col_idx, double width) -> void;
    [[nodiscard]] auto column_count() const -> size_t;
    [[nodiscard]] auto column(size_t col_idx) const -> const TableColumn&;

    // ── Rows ──────────────────────────────────────────────────

    auto add_row() -> void;
    auto insert_row(size_t row_idx) -> void;
    auto remove_row(size_t row_idx) -> void;
    [[nodiscard]] auto row_count() const -> size_t;

    // ── Cells ─────────────────────────────────────────────────

    auto set_cell(size_t row, size_t col, const std::string& text) -> void;
    [[nodiscard]] auto get_cell(size_t row, size_t col) const -> const TableCell&;
    auto set_cell_style(size_t row, size_t col, CanvasColor fill, CanvasColor text_col, bool bold)
        -> void;

    // ── Row access (for sorting) ──────────────────────────────

    [[nodiscard]] auto rows() const -> const std::vector<std::vector<TableCell>>&;
    auto rows_mut() -> std::vector<std::vector<TableCell>>&;

    // ── Visual settings ───────────────────────────────────────

    [[nodiscard]] auto row_height() const -> double;
    auto set_row_height(double height) -> void;
    [[nodiscard]] auto header_height() const -> double;
    auto set_header_height(double height) -> void;
    [[nodiscard]] auto border_color() const -> const CanvasColor&;
    auto set_border_color(CanvasColor color) -> void;
    [[nodiscard]] auto header_bg_color() const -> const CanvasColor&;
    auto set_header_bg_color(CanvasColor color) -> void;
    [[nodiscard]] auto alternate_row_color() const -> const CanvasColor&;
    auto set_alternate_row_color(CanvasColor color) -> void;

    // ── Computed dimensions ───────────────────────────────────

    /// Total width = sum of all column widths.
    [[nodiscard]] auto total_width() const -> double;
    /// Total height = header_height + row_count * row_height.
    [[nodiscard]] auto total_height() const -> double;

    // ── CanvasObject overrides ────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

    // ── Round 2 Batch 2 (#11-17) ──────────────────────────────────

    /// (#11) Whether the table has no data rows.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return rows_.empty();
    }

    /// (#12) Whether the table has data rows.
    [[nodiscard]] auto has_rows() const noexcept -> bool
    {
        return !rows_.empty();
    }

    /// (#13) Total number of cells (rows × columns).
    [[nodiscard]] auto cell_count() const noexcept -> size_t
    {
        return rows_.size() * columns_.size();
    }

    /// (#14) Whether the table has columns defined.
    [[nodiscard]] auto has_columns() const noexcept -> bool
    {
        return !columns_.empty();
    }

    /// (#15) Total area of the table (width × height).
    [[nodiscard]] auto total_area() const noexcept -> double
    {
        return total_width() * total_height();
    }

    /// (#16) Whether the table has exactly one column.
    [[nodiscard]] auto is_single_column() const noexcept -> bool
    {
        return columns_.size() == 1;
    }

    /// (#17) Whether the table has exactly one row.
    [[nodiscard]] auto is_single_row() const noexcept -> bool
    {
        return rows_.size() == 1;
    }

private:
    std::vector<TableColumn> columns_;
    std::vector<std::vector<TableCell>> rows_;
    double row_height_{32.0};
    double header_height_{36.0};
    CanvasColor border_color_{180, 180, 180, 255};
    CanvasColor header_bg_color_{240, 240, 240, 255};
    CanvasColor alternate_row_color_{248, 248, 248, 255};

    static constexpr double kMinColumnWidth = 40.0;
    static constexpr double kMinRowHeight = 20.0;

    /// Create a default row with empty cells matching column count.
    [[nodiscard]] auto make_empty_row() const -> std::vector<TableCell>;
};

} // namespace markamp::canvas
