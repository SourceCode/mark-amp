#include "AVImportExport.h"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <regex>
#include <sstream>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// CSV Field Escaping
// ════════════════════════════════════════════════════════════

auto AVImportExport::escape_csv_field(const std::string& field) -> std::string
{
    bool needs_quoting = false;
    for (char ch : field)
    {
        if (ch == ',' || ch == '"' || ch == '\n' || ch == '\r')
        {
            needs_quoting = true;
            break;
        }
    }

    if (!needs_quoting)
    {
        return field;
    }

    std::string result = "\"";
    for (char ch : field)
    {
        if (ch == '"')
        {
            result += "\"\"";
        }
        else
        {
            result += ch;
        }
    }
    result += '"';
    return result;
}

auto AVImportExport::escape_json_string(const std::string& str) -> std::string
{
    std::string result;
    result.reserve(str.size() + 2);
    result += '"';
    for (char ch : str)
    {
        switch (ch)
        {
            case '"':
                result += "\\\"";
                break;
            case '\\':
                result += "\\\\";
                break;
            case '\n':
                result += "\\n";
                break;
            case '\r':
                result += "\\r";
                break;
            case '\t':
                result += "\\t";
                break;
            default:
                result += ch;
        }
    }
    result += '"';
    return result;
}

// ════════════════════════════════════════════════════════════
// CSV Parsing (RFC 4180)
// ════════════════════════════════════════════════════════════

auto AVImportExport::parse_csv_row(const std::string& line) -> std::vector<std::string>
{
    std::vector<std::string> fields;
    std::string field;
    bool in_quotes = false;
    size_t idx = 0;
    const size_t len = line.size();

    while (idx < len)
    {
        char ch = line[idx];

        if (in_quotes)
        {
            if (ch == '"')
            {
                if (idx + 1 < len && line[idx + 1] == '"')
                {
                    field += '"';
                    ++idx;
                }
                else
                {
                    in_quotes = false;
                }
            }
            else
            {
                field += ch;
            }
        }
        else
        {
            if (ch == '"')
            {
                in_quotes = true;
            }
            else if (ch == ',')
            {
                fields.push_back(field);
                field.clear();
            }
            else
            {
                field += ch;
            }
        }
        ++idx;
    }
    fields.push_back(field);
    return fields;
}

auto AVImportExport::parse_csv_rows(const std::string& csv_data)
    -> std::pair<std::vector<std::string>, std::vector<std::vector<std::string>>>
{
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;

    std::istringstream stream(csv_data);
    std::string line;
    bool first = true;

    while (std::getline(stream, line))
    {
        // Strip trailing \r
        if (!line.empty() && line.back() == '\r')
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        auto fields = parse_csv_row(line);

        if (first)
        {
            headers = fields;
            first = false;
        }
        else
        {
            rows.push_back(fields);
        }
    }

    return {headers, rows};
}

// ════════════════════════════════════════════════════════════
// Type Inference
// ════════════════════════════════════════════════════════════

auto AVImportExport::infer_type(const std::vector<std::string>& sample_values) -> AVKeyType
{
    if (sample_values.empty())
    {
        return AVKeyType::Text;
    }

    int number_count = 0;
    int bool_count = 0;
    int url_count = 0;
    int email_count = 0;
    int non_empty_count = 0;

    for (const auto& val : sample_values)
    {
        if (val.empty())
        {
            continue;
        }
        ++non_empty_count;

        // Check number
        try
        {
            size_t parsed_chars = 0;
            std::stod(val, &parsed_chars);
            if (parsed_chars == val.size())
            {
                ++number_count;
                continue;
            }
        }
        catch (...)
        {
            // Not a number
        }

        // Check bool
        std::string lower_val = val;
        std::transform(lower_val.begin(), lower_val.end(), lower_val.begin(), ::tolower);
        if (lower_val == "true" || lower_val == "false" || lower_val == "yes" || lower_val == "no")
        {
            ++bool_count;
            continue;
        }

        // Check URL
        if (val.starts_with("http://") || val.starts_with("https://"))
        {
            ++url_count;
            continue;
        }

        // Check email (simple heuristic)
        if (val.find('@') != std::string::npos && val.find('.') != std::string::npos)
        {
            ++email_count;
            continue;
        }
    }

    if (non_empty_count == 0)
    {
        return AVKeyType::Text;
    }

    // If >70% match a type, use that type
    double threshold = 0.7;
    if (static_cast<double>(number_count) / static_cast<double>(non_empty_count) >= threshold)
    {
        return AVKeyType::Number;
    }
    if (static_cast<double>(bool_count) / static_cast<double>(non_empty_count) >= threshold)
    {
        return AVKeyType::Checkbox;
    }
    if (static_cast<double>(url_count) / static_cast<double>(non_empty_count) >= threshold)
    {
        return AVKeyType::URL;
    }
    if (static_cast<double>(email_count) / static_cast<double>(non_empty_count) >= threshold)
    {
        return AVKeyType::Email;
    }

    return AVKeyType::Text;
}

auto AVImportExport::detect_column_types(const std::vector<std::vector<std::string>>& rows,
                                         const std::vector<std::string>& headers)
    -> std::vector<AVImportColumnMapping>
{
    std::vector<AVImportColumnMapping> mappings;

    for (size_t col = 0; col < headers.size(); ++col)
    {
        std::vector<std::string> column_values;
        // Sample up to 100 rows for inference
        size_t sample_count = std::min(rows.size(), static_cast<size_t>(100));
        for (size_t row = 0; row < sample_count; ++row)
        {
            if (col < rows[row].size())
            {
                column_values.push_back(rows[row][col]);
            }
        }

        AVImportColumnMapping mapping;
        mapping.source_header = headers[col];
        mapping.inferred_type = infer_type(column_values);
        mappings.push_back(mapping);
    }

    return mappings;
}

// ════════════════════════════════════════════════════════════
// Set a typed value from raw string input
// ════════════════════════════════════════════════════════════

void AVImportExport::set_typed_value(AttributeView& attribute_view,
                                     const std::string& key_id,
                                     const std::string& block_id,
                                     const std::string& raw_value,
                                     AVKeyType column_type)
{
    switch (column_type)
    {
        case AVKeyType::Number:
        {
            AVValueNumber num;
            try
            {
                num.content = std::stod(raw_value);
                num.is_not_empty = !raw_value.empty();
            }
            catch (...)
            {
                num.content = 0.0;
                num.is_not_empty = false;
            }
            attribute_view.set_value(key_id, block_id, num);
            break;
        }
        case AVKeyType::Checkbox:
        {
            std::string lower = raw_value;
            std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);
            AVValueCheckbox chk;
            chk.checked = (lower == "true" || lower == "yes" || lower == "1");
            attribute_view.set_value(key_id, block_id, chk);
            break;
        }
        case AVKeyType::URL:
        {
            AVValueURL url_val;
            url_val.content = raw_value;
            attribute_view.set_value(key_id, block_id, url_val);
            break;
        }
        case AVKeyType::Email:
        {
            AVValueEmail email;
            email.content = raw_value;
            attribute_view.set_value(key_id, block_id, email);
            break;
        }
        default:
        {
            AVValueText txt;
            txt.content = raw_value;
            attribute_view.set_value(key_id, block_id, txt);
            break;
        }
    }
}

// ════════════════════════════════════════════════════════════
// CSV Import
// ════════════════════════════════════════════════════════════

auto AVImportExport::import_from_csv(const std::string& csv_data, AttributeView& attribute_view)
    -> std::expected<AVImportResult, std::string>
{
    auto [headers, rows] = parse_csv_rows(csv_data);

    if (headers.empty())
    {
        return std::unexpected("CSV data has no headers");
    }

    AVImportResult result;

    // Detect column types
    auto mappings = detect_column_types(rows, headers);

    // Create keys for each column
    for (auto& mapping : mappings)
    {
        // Check if key already exists
        bool found = false;
        for (const auto& kv : attribute_view.key_values)
        {
            if (kv.key.name == mapping.source_header)
            {
                mapping.target_key_id = kv.key.id;
                found = true;
                break;
            }
        }

        if (!found)
        {
            AVKey new_key;
            new_key.name = mapping.source_header;
            new_key.type = mapping.inferred_type;
            mapping.target_key_id = attribute_view.add_key(new_key);
            ++result.columns_created;
        }
    }

    // Import rows
    for (const auto& row : rows)
    {
        // Create a new row (block)
        std::string block_id = "imported_" + std::to_string(result.rows_imported);
        (void)attribute_view.add_row(block_id);

        for (size_t col = 0; col < mappings.size() && col < row.size(); ++col)
        {
            if (!row[col].empty())
            {
                set_typed_value(attribute_view,
                                mappings[col].target_key_id,
                                block_id,
                                row[col],
                                mappings[col].inferred_type);
            }
        }

        ++result.rows_imported;
    }

    return result;
}

// ════════════════════════════════════════════════════════════
// JSON Import
// ════════════════════════════════════════════════════════════

auto AVImportExport::import_from_json(const std::string& json_data, AttributeView& attribute_view)
    -> std::expected<AVImportResult, std::string>
{
    // Simple JSON array-of-objects parser
    // Expects: [{"key": "value", ...}, ...]
    AVImportResult result;

    // Find the array start
    auto arr_start = json_data.find('[');
    if (arr_start == std::string::npos)
    {
        return std::unexpected("JSON data must be an array");
    }

    // Extract headers and values by scanning for key-value pairs
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;

    // Simple state machine parser
    size_t pos = arr_start + 1;
    const size_t len = json_data.size();

    while (pos < len)
    {
        // Skip whitespace
        while (pos < len && std::isspace(static_cast<unsigned char>(json_data[pos])) != 0)
        {
            ++pos;
        }

        if (pos >= len || json_data[pos] == ']')
        {
            break;
        }

        if (json_data[pos] == ',')
        {
            ++pos;
            continue;
        }

        if (json_data[pos] != '{')
        {
            return std::unexpected("Expected '{' in JSON array element");
        }
        ++pos;

        std::vector<std::string> row_values;

        while (pos < len && json_data[pos] != '}')
        {
            // Skip whitespace
            while (pos < len && std::isspace(static_cast<unsigned char>(json_data[pos])) != 0)
            {
                ++pos;
            }

            if (pos >= len || json_data[pos] == '}')
            {
                break;
            }

            if (json_data[pos] == ',')
            {
                ++pos;
                continue;
            }

            // Parse key
            if (json_data[pos] != '"')
            {
                return std::unexpected("Expected '\"' for JSON key");
            }
            ++pos;
            std::string key;
            while (pos < len && json_data[pos] != '"')
            {
                if (json_data[pos] == '\\' && pos + 1 < len)
                {
                    ++pos;
                }
                key += json_data[pos];
                ++pos;
            }
            if (pos < len)
            {
                ++pos; // skip closing quote
            }

            // Skip colon
            while (pos < len && json_data[pos] != ':')
            {
                ++pos;
            }
            if (pos < len)
            {
                ++pos;
            }

            // Skip whitespace
            while (pos < len && std::isspace(static_cast<unsigned char>(json_data[pos])) != 0)
            {
                ++pos;
            }

            // Parse value
            std::string value;
            if (pos < len && json_data[pos] == '"')
            {
                ++pos;
                while (pos < len && json_data[pos] != '"')
                {
                    if (json_data[pos] == '\\' && pos + 1 < len)
                    {
                        ++pos;
                    }
                    value += json_data[pos];
                    ++pos;
                }
                if (pos < len)
                {
                    ++pos; // skip closing quote
                }
            }
            else
            {
                // Number, bool, or null
                while (pos < len && json_data[pos] != ',' && json_data[pos] != '}' &&
                       std::isspace(static_cast<unsigned char>(json_data[pos])) == 0)
                {
                    value += json_data[pos];
                    ++pos;
                }
                if (value == "null")
                {
                    value.clear();
                }
            }

            // Track header ordering
            auto header_it = std::find(headers.begin(), headers.end(), key);
            size_t col_index = 0;
            if (header_it == headers.end())
            {
                col_index = headers.size();
                headers.push_back(key);
            }
            else
            {
                col_index = static_cast<size_t>(std::distance(headers.begin(), header_it));
            }

            // Grow row to fit
            while (row_values.size() <= col_index)
            {
                row_values.emplace_back();
            }
            row_values[col_index] = value;
        }

        if (pos < len)
        {
            ++pos; // skip '}'
        }

        rows.push_back(row_values);
    }

    // Detect types and create columns
    auto mappings = detect_column_types(rows, headers);

    for (auto& mapping : mappings)
    {
        bool found = false;
        for (const auto& kv : attribute_view.key_values)
        {
            if (kv.key.name == mapping.source_header)
            {
                mapping.target_key_id = kv.key.id;
                found = true;
                break;
            }
        }

        if (!found)
        {
            AVKey new_key;
            new_key.name = mapping.source_header;
            new_key.type = mapping.inferred_type;
            mapping.target_key_id = attribute_view.add_key(new_key);
            ++result.columns_created;
        }
    }

    // Import rows
    for (const auto& row : rows)
    {
        std::string block_id = "json_" + std::to_string(result.rows_imported);
        (void)attribute_view.add_row(block_id);

        for (size_t col = 0; col < mappings.size() && col < row.size(); ++col)
        {
            if (!row[col].empty())
            {
                set_typed_value(attribute_view,
                                mappings[col].target_key_id,
                                block_id,
                                row[col],
                                mappings[col].inferred_type);
            }
        }

        ++result.rows_imported;
    }

    return result;
}

// ════════════════════════════════════════════════════════════
// CSV Export
// ════════════════════════════════════════════════════════════

auto AVImportExport::export_csv(const AttributeView& attribute_view) -> std::string
{
    std::ostringstream oss;

    // Header row
    for (size_t idx = 0; idx < attribute_view.key_values.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ',';
        }
        oss << escape_csv_field(attribute_view.key_values[idx].key.name);
    }
    oss << '\n';

    // Data rows
    auto block_ids = attribute_view.row_block_ids();
    for (const auto& block_id : block_ids)
    {
        for (size_t col = 0; col < attribute_view.key_values.size(); ++col)
        {
            if (col > 0)
            {
                oss << ',';
            }
            const auto* val =
                attribute_view.get_value(attribute_view.key_values[col].key.id, block_id);
            if (val != nullptr && !val->is_empty())
            {
                oss << escape_csv_field(val->to_display_string());
            }
        }
        oss << '\n';
    }

    return oss.str();
}

// ════════════════════════════════════════════════════════════
// JSON Export
// ════════════════════════════════════════════════════════════

auto AVImportExport::export_json(const AttributeView& attribute_view) -> std::string
{
    std::ostringstream oss;
    oss << "[\n";

    auto block_ids = attribute_view.row_block_ids();
    for (size_t row = 0; row < block_ids.size(); ++row)
    {
        if (row > 0)
        {
            oss << ",\n";
        }
        oss << "  {";

        for (size_t col = 0; col < attribute_view.key_values.size(); ++col)
        {
            if (col > 0)
            {
                oss << ", ";
            }
            oss << escape_json_string(attribute_view.key_values[col].key.name) << ": ";

            const auto* val =
                attribute_view.get_value(attribute_view.key_values[col].key.id, block_ids[row]);
            if (val != nullptr && !val->is_empty())
            {
                if (const auto* num = val->as_number())
                {
                    oss << num->content;
                }
                else if (const auto* chk = val->as_checkbox())
                {
                    oss << (chk->checked ? "true" : "false");
                }
                else
                {
                    oss << escape_json_string(val->to_display_string());
                }
            }
            else
            {
                oss << "null";
            }
        }

        oss << "}";
    }

    oss << "\n]\n";
    return oss.str();
}

// ════════════════════════════════════════════════════════════
// Markdown Export
// ════════════════════════════════════════════════════════════

auto AVImportExport::export_markdown(const AttributeView& attribute_view) -> std::string
{
    std::ostringstream oss;

    // Header
    oss << '|';
    for (const auto& kv : attribute_view.key_values)
    {
        oss << ' ' << kv.key.name << " |";
    }
    oss << '\n';

    // Separator
    oss << '|';
    for (size_t idx = 0; idx < attribute_view.key_values.size(); ++idx)
    {
        oss << " --- |";
    }
    oss << '\n';

    // Data rows
    auto block_ids = attribute_view.row_block_ids();
    for (const auto& block_id : block_ids)
    {
        oss << '|';
        for (const auto& kv : attribute_view.key_values)
        {
            const auto* val = attribute_view.get_value(kv.key.id, block_id);
            oss << ' ';
            if (val != nullptr && !val->is_empty())
            {
                // Escape pipe characters in value
                std::string display = val->to_display_string();
                std::string escaped;
                for (char ch : display)
                {
                    if (ch == '|')
                    {
                        escaped += "\\|";
                    }
                    else
                    {
                        escaped += ch;
                    }
                }
                oss << escaped;
            }
            oss << " |";
        }
        oss << '\n';
    }

    return oss.str();
}

// ════════════════════════════════════════════════════════════
// Public export dispatcher
// ════════════════════════════════════════════════════════════

auto AVImportExport::export_to_string(const AttributeView& attribute_view, AVExportFormat format)
    -> std::string
{
    switch (format)
    {
        case AVExportFormat::CSV:
            return export_csv(attribute_view);
        case AVExportFormat::JSON:
            return export_json(attribute_view);
        case AVExportFormat::MarkdownTable:
            return export_markdown(attribute_view);
    }
    return "";
}

} // namespace markamp::core::av
