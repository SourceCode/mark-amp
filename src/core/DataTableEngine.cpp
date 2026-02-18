/// @file DataTableEngine.cpp
/// @brief V9 Phase 48 — DataTableEngine implementation.

#include "DataTableEngine.h"

#include <algorithm>

namespace markamp::core
{

auto DataTableEngine::create_table(const std::string& name) -> std::string
{
    DataTable table;
    table.table_id = "tbl_" + std::to_string(next_id_++);
    table.name = name;
    tables_.push_back(std::move(table));
    return tables_.back().table_id;
}

auto DataTableEngine::delete_table(const std::string& table_id) -> bool
{
    auto iter = std::remove_if(tables_.begin(),
                               tables_.end(),
                               [&](const DataTable& tbl) { return tbl.table_id == table_id; });
    if (iter == tables_.end())
    {
        return false;
    }
    tables_.erase(iter, tables_.end());
    return true;
}

auto DataTableEngine::get_table(const std::string& table_id) const -> const DataTable*
{
    for (const auto& tbl : tables_)
    {
        if (tbl.table_id == table_id)
        {
            return &tbl;
        }
    }
    return nullptr;
}

auto DataTableEngine::add_column(const std::string& table_id, ColumnDef col) -> bool
{
    auto* tbl = find_table_mut(table_id);
    if (tbl == nullptr)
    {
        return false;
    }
    tbl->columns.push_back(std::move(col));
    // Extend existing rows with empty cells
    for (auto& row : tbl->rows)
    {
        row.cells.emplace_back();
    }
    return true;
}

auto DataTableEngine::add_row(const std::string& table_id, DataRow row) -> bool
{
    auto* tbl = find_table_mut(table_id);
    if (tbl == nullptr)
    {
        return false;
    }
    // Pad or trim to match column count
    row.cells.resize(tbl->columns.size());
    tbl->rows.push_back(std::move(row));
    return true;
}

auto DataTableEngine::remove_row(const std::string& table_id, int row_index) -> bool
{
    auto* tbl = find_table_mut(table_id);
    if (tbl == nullptr || row_index < 0 || row_index >= static_cast<int>(tbl->rows.size()))
    {
        return false;
    }
    tbl->rows.erase(tbl->rows.begin() + row_index);
    return true;
}

auto DataTableEngine::sort_table(const std::string& table_id,
                                 const std::string& col_name,
                                 bool ascending) -> bool
{
    auto* tbl = find_table_mut(table_id);
    if (tbl == nullptr)
    {
        return false;
    }
    int col_idx = find_column_index(*tbl, col_name);
    if (col_idx < 0)
    {
        return false;
    }

    auto col_index = static_cast<size_t>(col_idx);
    std::sort(tbl->rows.begin(),
              tbl->rows.end(),
              [col_index, ascending](const DataRow& row_a, const DataRow& row_b)
              {
                  const auto& val_a = col_index < row_a.cells.size() ? row_a.cells[col_index] : "";
                  const auto& val_b = col_index < row_b.cells.size() ? row_b.cells[col_index] : "";
                  return ascending ? val_a < val_b : val_a > val_b;
              });
    return true;
}

auto DataTableEngine::filter_table(const std::string& table_id,
                                   const std::string& col_name,
                                   const std::string& value) const -> std::vector<DataRow>
{
    std::vector<DataRow> result;
    const auto* tbl = get_table(table_id);
    if (tbl == nullptr)
    {
        return result;
    }
    int col_idx = find_column_index(*tbl, col_name);
    if (col_idx < 0)
    {
        return result;
    }

    auto col_index = static_cast<size_t>(col_idx);
    for (const auto& row : tbl->rows)
    {
        if (col_index < row.cells.size() && row.cells[col_index] == value)
        {
            result.push_back(row);
        }
    }
    return result;
}

auto DataTableEngine::table_count() const -> int
{
    return static_cast<int>(tables_.size());
}

void DataTableEngine::clear()
{
    tables_.clear();
}

auto DataTableEngine::find_table_mut(const std::string& table_id) -> DataTable*
{
    for (auto& tbl : tables_)
    {
        if (tbl.table_id == table_id)
        {
            return &tbl;
        }
    }
    return nullptr;
}

auto DataTableEngine::find_column_index(const DataTable& table, const std::string& col_name) const
    -> int
{
    for (size_t idx = 0; idx < table.columns.size(); ++idx)
    {
        if (table.columns[idx].name == col_name)
        {
            return static_cast<int>(idx);
        }
    }
    return -1;
}

} // namespace markamp::core
