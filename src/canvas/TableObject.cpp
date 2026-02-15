#include "TableObject.h"

#include <algorithm>
#include <numeric>
#include <sstream>
#include <stdexcept>

namespace markamp::canvas
{

TableObject::TableObject()
    : CanvasObject(CanvasObjectType::Table)
{
    // Default 3×3 table.
    columns_.push_back({"Column A", 120.0, true});
    columns_.push_back({"Column B", 120.0, true});
    columns_.push_back({"Column C", 120.0, true});

    for (int row = 0; row < 3; ++row)
    {
        rows_.push_back(make_empty_row());
    }
}

// ── Columns ───────────────────────────────────────────────────

auto TableObject::add_column(const std::string& header, double width) -> void
{
    const double clamped_w = std::max(width, kMinColumnWidth);
    columns_.push_back({header, clamped_w, true});

    // Add an empty cell to each existing row.
    for (auto& row : rows_)
    {
        row.push_back(TableCell{});
    }
    mark_dirty();
}

auto TableObject::remove_column(size_t col_idx) -> void
{
    if (col_idx >= columns_.size() || columns_.size() <= 1)
    {
        return; // Must keep at least one column.
    }

    columns_.erase(columns_.begin() + static_cast<std::ptrdiff_t>(col_idx));
    for (auto& row : rows_)
    {
        if (col_idx < row.size())
        {
            row.erase(row.begin() + static_cast<std::ptrdiff_t>(col_idx));
        }
    }
    mark_dirty();
}

auto TableObject::resize_column(size_t col_idx, double width) -> void
{
    if (col_idx >= columns_.size())
    {
        return;
    }
    columns_[col_idx].width = std::max(width, kMinColumnWidth);
    mark_dirty();
}

auto TableObject::column_count() const -> size_t
{
    return columns_.size();
}

auto TableObject::column(size_t col_idx) const -> const TableColumn&
{
    return columns_.at(col_idx);
}

// ── Rows ──────────────────────────────────────────────────────

auto TableObject::add_row() -> void
{
    rows_.push_back(make_empty_row());
    mark_dirty();
}

auto TableObject::insert_row(size_t row_idx) -> void
{
    if (row_idx > rows_.size())
    {
        row_idx = rows_.size();
    }
    rows_.insert(rows_.begin() + static_cast<std::ptrdiff_t>(row_idx), make_empty_row());
    mark_dirty();
}

auto TableObject::remove_row(size_t row_idx) -> void
{
    if (row_idx >= rows_.size())
    {
        return;
    }
    rows_.erase(rows_.begin() + static_cast<std::ptrdiff_t>(row_idx));
    mark_dirty();
}

auto TableObject::row_count() const -> size_t
{
    return rows_.size();
}

// ── Cells ─────────────────────────────────────────────────────

auto TableObject::set_cell(size_t row, size_t col, const std::string& text) -> void
{
    if (row >= rows_.size() || col >= columns_.size())
    {
        return;
    }
    rows_[row][col].text = text;
    mark_dirty();
}

auto TableObject::get_cell(size_t row, size_t col) const -> const TableCell&
{
    return rows_.at(row).at(col);
}

auto TableObject::set_cell_style(
    size_t row, size_t col, CanvasColor fill, CanvasColor text_col, bool bold) -> void
{
    if (row >= rows_.size() || col >= columns_.size())
    {
        return;
    }
    auto& cell = rows_[row][col];
    cell.fill_color = fill;
    cell.text_color = text_col;
    cell.bold = bold;
    mark_dirty();
}

// ── Row access ────────────────────────────────────────────────

auto TableObject::rows() const -> const std::vector<std::vector<TableCell>>&
{
    return rows_;
}

auto TableObject::rows_mut() -> std::vector<std::vector<TableCell>>&
{
    return rows_;
}

// ── Visual settings ───────────────────────────────────────────

auto TableObject::row_height() const -> double
{
    return row_height_;
}

auto TableObject::set_row_height(double height) -> void
{
    row_height_ = std::max(height, kMinRowHeight);
    mark_dirty();
}

auto TableObject::header_height() const -> double
{
    return header_height_;
}

auto TableObject::set_header_height(double height) -> void
{
    header_height_ = std::max(height, kMinRowHeight);
    mark_dirty();
}

auto TableObject::border_color() const -> const CanvasColor&
{
    return border_color_;
}
auto TableObject::set_border_color(CanvasColor color) -> void
{
    border_color_ = color;
    mark_dirty();
}

auto TableObject::header_bg_color() const -> const CanvasColor&
{
    return header_bg_color_;
}
auto TableObject::set_header_bg_color(CanvasColor color) -> void
{
    header_bg_color_ = color;
    mark_dirty();
}

auto TableObject::alternate_row_color() const -> const CanvasColor&
{
    return alternate_row_color_;
}
auto TableObject::set_alternate_row_color(CanvasColor color) -> void
{
    alternate_row_color_ = color;
    mark_dirty();
}

// ── Computed dimensions ───────────────────────────────────────

auto TableObject::total_width() const -> double
{
    return std::accumulate(columns_.begin(),
                           columns_.end(),
                           0.0,
                           [](double sum, const TableColumn& col) { return sum + col.width; });
}

auto TableObject::total_height() const -> double
{
    return header_height_ + static_cast<double>(rows_.size()) * row_height_;
}

// ── CanvasObject overrides ────────────────────────────────────

auto TableObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, total_width(), total_height()};
}

auto TableObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<TableObject>();
    copy->set_name(name());
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_locked(is_locked());
    copy->set_visible(is_visible());
    copy->set_opacity(opacity());

    copy->columns_ = columns_;
    copy->rows_ = rows_;
    copy->row_height_ = row_height_;
    copy->header_height_ = header_height_;
    copy->border_color_ = border_color_;
    copy->header_bg_color_ = header_bg_color_;
    copy->alternate_row_color_ = alternate_row_color_;

    return copy;
}

auto TableObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Table\"";
    oss << ",\"id\":" << id();
    oss << ",\"name\":\"" << name() << "\"";
    oss << ",\"z_index\":" << z_index();
    oss << ",\"row_height\":" << row_height_;
    oss << ",\"header_height\":" << header_height_;

    oss << ",\"columns\":[";
    for (size_t ci = 0; ci < columns_.size(); ++ci)
    {
        if (ci > 0)
            oss << ",";
        oss << "{\"header\":\"" << columns_[ci].header << "\"";
        oss << ",\"width\":" << columns_[ci].width;
        oss << ",\"sortable\":" << (columns_[ci].sortable ? "true" : "false") << "}";
    }
    oss << "]";

    oss << ",\"rows\":[";
    for (size_t ri = 0; ri < rows_.size(); ++ri)
    {
        if (ri > 0)
            oss << ",";
        oss << "[";
        for (size_t ci = 0; ci < rows_[ri].size(); ++ci)
        {
            if (ci > 0)
                oss << ",";
            oss << "{\"text\":\"" << rows_[ri][ci].text << "\"";
            oss << ",\"bold\":" << (rows_[ri][ci].bold ? "true" : "false") << "}";
        }
        oss << "]";
    }
    oss << "]";

    oss << "}";
    return oss.str();
}

auto TableObject::from_json(const std::string& /*json*/) -> void
{
    // Deserialization stub.
}

// ── Private ───────────────────────────────────────────────────

auto TableObject::make_empty_row() const -> std::vector<TableCell>
{
    std::vector<TableCell> row;
    row.resize(columns_.size());
    return row;
}

} // namespace markamp::canvas
