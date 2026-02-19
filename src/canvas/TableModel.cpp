#include "TableModel.h"

#include <sstream>

namespace markamp::canvas
{

void TableModel::set_grid(int rows, int cols)
{
    rows_ = (rows < 0) ? 0 : rows;
    cols_ = (cols < 0) ? 0 : cols;
    grid_.resize(static_cast<size_t>(rows_));
    for (auto& row : grid_)
    {
        row.resize(static_cast<size_t>(cols_));
    }
}

auto TableModel::rows() const -> int
{
    return rows_;
}
auto TableModel::cols() const -> int
{
    return cols_;
}

void TableModel::set_cell(int row, int col, const std::string& value)
{
    if (row >= 0 && row < rows_ && col >= 0 && col < cols_)
    {
        grid_[static_cast<size_t>(row)][static_cast<size_t>(col)].value = value;
    }
}

auto TableModel::cell(int row, int col) const -> std::string
{
    if (row >= 0 && row < rows_ && col >= 0 && col < cols_)
    {
        return grid_[static_cast<size_t>(row)][static_cast<size_t>(col)].value;
    }
    return "";
}

void TableModel::insert_row(int at)
{
    if (at >= 0 && at <= rows_)
    {
        grid_.insert(grid_.begin() + at, std::vector<TableCell>(static_cast<size_t>(cols_)));
        ++rows_;
    }
}

void TableModel::insert_column(int at)
{
    if (at >= 0 && at <= cols_)
    {
        for (auto& row : grid_)
        {
            row.insert(row.begin() + at, TableCell{});
        }
        ++cols_;
    }
}

void TableModel::delete_row(int at)
{
    if (at >= 0 && at < rows_)
    {
        grid_.erase(grid_.begin() + at);
        --rows_;
    }
}

void TableModel::delete_column(int at)
{
    if (at >= 0 && at < cols_)
    {
        for (auto& row : grid_)
        {
            row.erase(row.begin() + at);
        }
        --cols_;
    }
}

void TableModel::set_active_cell(int row, int col)
{
    active_row_ = (row >= 0 && row < rows_) ? row : 0;
    active_col_ = (col >= 0 && col < cols_) ? col : 0;
}

auto TableModel::active_row() const -> int
{
    return active_row_;
}
auto TableModel::active_col() const -> int
{
    return active_col_;
}

void TableModel::tab_next()
{
    ++active_col_;
    if (active_col_ >= cols_)
    {
        active_col_ = 0;
        ++active_row_;
        if (active_row_ >= rows_)
        {
            active_row_ = 0;
        }
    }
}

void TableModel::set_sort(ColumnSort sort)
{
    sort_ = sort;
}
auto TableModel::sort() const -> const ColumnSort&
{
    return sort_;
}

void TableModel::set_header_row(bool enabled)
{
    header_row_ = enabled;
}
auto TableModel::has_header_row() const -> bool
{
    return header_row_;
}

void TableModel::set_alternate_rows(bool enabled)
{
    alternate_rows_ = enabled;
}
auto TableModel::alternate_rows() const -> bool
{
    return alternate_rows_;
}

auto TableModel::to_csv() const -> std::string
{
    std::ostringstream oss;
    for (int row = 0; row < rows_; ++row)
    {
        for (int col = 0; col < cols_; ++col)
        {
            if (col > 0)
            {
                oss << ',';
            }
            oss << grid_[static_cast<size_t>(row)][static_cast<size_t>(col)].value;
        }
        oss << '\n';
    }
    return oss.str();
}

void TableModel::from_csv(const std::string& csv)
{
    grid_.clear();
    rows_ = 0;
    cols_ = 0;
    std::istringstream iss(csv);
    std::string line;
    while (std::getline(iss, line))
    {
        std::vector<TableCell> row;
        std::istringstream line_stream(line);
        std::string cell_val;
        while (std::getline(line_stream, cell_val, ','))
        {
            row.push_back({cell_val, "text"});
        }
        if (!row.empty())
        {
            if (cols_ == 0)
            {
                cols_ = static_cast<int>(row.size());
            }
            grid_.push_back(std::move(row));
            ++rows_;
        }
    }
}

} // namespace markamp::canvas
