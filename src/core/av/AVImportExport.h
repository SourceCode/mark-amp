#pragma once

#include "AVTypes.h"
#include "AVValue.h"
#include "AttributeView.h"

#include <expected>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Export Format
// ════════════════════════════════════════════════════════════

enum class AVExportFormat : std::uint8_t
{
    CSV,
    JSON,
    MarkdownTable
};

// ════════════════════════════════════════════════════════════
// Import Column Mapping
// ════════════════════════════════════════════════════════════

struct AVImportColumnMapping
{
    std::string source_header;
    std::string target_key_id;
    AVKeyType inferred_type{AVKeyType::Text};
};

// ════════════════════════════════════════════════════════════
// Import Result
// ════════════════════════════════════════════════════════════

struct AVImportResult
{
    int rows_imported{0};
    int rows_skipped{0};
    int columns_created{0};
    std::vector<std::string> warnings;
};

// ════════════════════════════════════════════════════════════
// AV Import/Export Service
// ════════════════════════════════════════════════════════════

class AVImportExport
{
public:
    /// Export an entire AttributeView to a formatted string.
    [[nodiscard]] static auto export_to_string(const AttributeView& attribute_view,
                                               AVExportFormat format) -> std::string;

    /// Import CSV data into an AttributeView (creates keys for new columns).
    [[nodiscard]] static auto import_from_csv(const std::string& csv_data,
                                              AttributeView& attribute_view)
        -> std::expected<AVImportResult, std::string>;

    /// Import JSON array-of-objects into an AttributeView.
    [[nodiscard]] static auto import_from_json(const std::string& json_data,
                                               AttributeView& attribute_view)
        -> std::expected<AVImportResult, std::string>;

    /// Detect column types from sample data.
    [[nodiscard]] static auto detect_column_types(const std::vector<std::vector<std::string>>& rows,
                                                  const std::vector<std::string>& headers)
        -> std::vector<AVImportColumnMapping>;

private:
    /// Export helpers
    [[nodiscard]] static auto export_csv(const AttributeView& attribute_view) -> std::string;
    [[nodiscard]] static auto export_json(const AttributeView& attribute_view) -> std::string;
    [[nodiscard]] static auto export_markdown(const AttributeView& attribute_view) -> std::string;

    /// CSV parsing helpers (RFC 4180)
    [[nodiscard]] static auto parse_csv_row(const std::string& line) -> std::vector<std::string>;
    [[nodiscard]] static auto parse_csv_rows(const std::string& csv_data)
        -> std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>>;

    /// CSV field escaping
    [[nodiscard]] static auto escape_csv_field(const std::string& field) -> std::string;

    /// JSON helpers
    [[nodiscard]] static auto escape_json_string(const std::string& str) -> std::string;

    /// Type inference
    [[nodiscard]] static auto infer_type(const std::vector<std::string>& sample_values)
        -> AVKeyType;

    /// Set a cell value based on detected type
    static void set_typed_value(AttributeView& attribute_view,
                                const std::string& key_id,
                                const std::string& block_id,
                                const std::string& raw_value,
                                AVKeyType column_type);
};

} // namespace markamp::core::av
