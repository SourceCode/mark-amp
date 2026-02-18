/// @file DataTableEngine.h
/// @brief V9 Phase 48 — In-document data table management.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Column data type.
enum class ColumnType : uint8_t
{
    kString = 0,
    kNumber = 1,
    kBoolean = 2,
    kDate = 3,
};

/// Column definition.
struct ColumnDef
{
    std::string name;
    ColumnType type{ColumnType::kString};
    int width{120};
};

/// A row of string-encoded cell values.
struct DataRow
{
    std::vector<std::string> cells;
};

/// A complete data table.
struct DataTable
{
    std::string table_id;
    std::string name;
    std::vector<ColumnDef> columns;
    std::vector<DataRow> rows;
};

/// Manages multiple data tables with sort and filter operations.
class DataTableEngine
{
public:
    DataTableEngine() = default;

    // ── Table lifecycle ───────────────────────────────────────────────
    auto create_table(const std::string& name) -> std::string;
    auto delete_table(const std::string& table_id) -> bool;
    [[nodiscard]] auto get_table(const std::string& table_id) const -> const DataTable*;

    // ── Schema ────────────────────────────────────────────────────────
    auto add_column(const std::string& table_id, ColumnDef col) -> bool;

    // ── Row operations ────────────────────────────────────────────────
    auto add_row(const std::string& table_id, DataRow row) -> bool;
    auto remove_row(const std::string& table_id, int row_index) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    auto sort_table(const std::string& table_id, const std::string& col_name, bool ascending = true)
        -> bool;
    [[nodiscard]] auto filter_table(const std::string& table_id,
                                    const std::string& col_name,
                                    const std::string& value) const -> std::vector<DataRow>;

    // ── Statistics ────────────────────────────────────────────────────
    [[nodiscard]] auto table_count() const -> int;
    void clear();

private:
    std::vector<DataTable> tables_;
    int next_id_{1};

    auto find_table_mut(const std::string& table_id) -> DataTable*;
    [[nodiscard]] auto find_column_index(const DataTable& table, const std::string& col_name) const
        -> int;
};

} // namespace markamp::core
