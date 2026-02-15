/// @file DataFrameRenderer.h
/// @brief V4 Phase 32 – Rich DataFrame Display (pure-logic engine).
/// Parses HTML tables / JSON into sortable, filterable tabular data.
/// Exports CSV. No wxWidgets dependencies.

#pragma once

#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::ui
{

// ============================================================================
// DataFrame data structures
// ============================================================================

struct DataFrameColumn
{
    std::string name;
    std::string dtype; // "int64", "float64", "object", "datetime64"
    int width{100};
};

struct DataFrameData
{
    std::vector<DataFrameColumn> columns;
    std::vector<std::vector<std::string>> rows;
    int total_rows{0}; // May exceed rows.size() if truncated
    bool truncated{false};

    [[nodiscard]] auto column_count() const -> int;
    [[nodiscard]] auto row_count() const -> int;
};

struct ChartData
{
    std::string format; // "png", "svg", "html"
    std::string data;   // Base64 image or HTML content
    int width{0};
    int height{0};
    bool is_interactive{false};
};

// ============================================================================
// DataFrameEngine – pure-logic table engine
// ============================================================================

class DataFrameEngine
{
public:
    explicit DataFrameEngine(markamp::core::EventBus& event_bus);

    // --- Loading ---

    /// Parse an HTML table (pandas-style) into DataFrameData.
    auto load_from_html(const std::string& html) -> void;

    /// Parse JSON table data.
    auto load_from_json(const std::string& json) -> void;

    // --- Sorting ---

    /// Sort by a column index. Detects numeric vs string sort.
    auto sort_by_column(int col_index, bool ascending = true) -> void;

    // --- Filtering ---

    /// Filter rows where any cell contains the query string.
    auto filter(const std::string& query) -> void;

    /// Clear the active filter.
    auto clear_filter() -> void;

    // --- Export ---

    /// Export visible (filtered) data as CSV with proper escaping.
    [[nodiscard]] auto export_csv() const -> std::string;

    // --- Accessors ---

    [[nodiscard]] auto data() const -> const DataFrameData&;
    [[nodiscard]] auto filtered_rows() const -> const std::vector<std::vector<std::string>>&;
    [[nodiscard]] auto is_filtered() const -> bool;

    // --- Chart ---

    /// Store chart data for rendering.
    auto load_chart(const ChartData& chart) -> void;
    [[nodiscard]] auto chart() const -> const ChartData&;
    [[nodiscard]] auto has_chart() const -> bool;

private:
    markamp::core::EventBus& event_bus_;
    DataFrameData data_;
    std::vector<std::vector<std::string>> filtered_rows_;
    bool is_filtered_{false};
    std::string active_filter_;
    ChartData chart_;
    bool has_chart_{false};

    /// Parse an HTML <table> into DataFrameData.
    [[nodiscard]] static auto parse_html_table(const std::string& html) -> DataFrameData;

    /// Try to determine if a column is numeric by sampling values.
    [[nodiscard]] static auto is_numeric_column(const std::vector<std::vector<std::string>>& rows,
                                                int col_index) -> bool;

    /// Escape a CSV field (quote if contains comma, quote, or newline).
    [[nodiscard]] static auto escape_csv_field(const std::string& field) -> std::string;
};

} // namespace markamp::ui
