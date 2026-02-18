/// @file NotebookSerializer.cpp
/// @brief V8 Phase 15 – Jupyter .ipynb format serialization implementation.

#include "core/NotebookSerializer.h"

#include "core/Events.h"

#include <algorithm>
#include <chrono>
#include <random>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor
// ============================================================================

NotebookSerializer::NotebookSerializer(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// parse_ipynb
// ============================================================================

auto NotebookSerializer::parse_ipynb(const std::string& json)
    -> std::expected<NotebookDocument, std::string>
{
    if (json.empty())
    {
        return std::unexpected("Empty notebook JSON");
    }

    NotebookDocument doc;

    // Extract nbformat version from JSON (simplified parser).
    auto nbformat_pos = json.find("\"nbformat\"");
    if (nbformat_pos != std::string::npos)
    {
        auto colon_pos = json.find(':', nbformat_pos);
        if (colon_pos != std::string::npos)
        {
            auto value_start = json.find_first_of("0123456789", colon_pos);
            if (value_start != std::string::npos)
            {
                doc.nbformat = std::stoi(json.substr(value_start, 1));
            }
        }
    }

    // Extract nbformat_minor.
    auto minor_pos = json.find("\"nbformat_minor\"");
    if (minor_pos != std::string::npos)
    {
        auto colon_pos = json.find(':', minor_pos);
        if (colon_pos != std::string::npos)
        {
            auto value_start = json.find_first_of("0123456789", colon_pos);
            if (value_start != std::string::npos)
            {
                doc.nbformat_minor = std::stoi(json.substr(value_start, 1));
            }
        }
    }

    // Extract kernelspec name.
    auto ks_pos = json.find("\"kernelspec\"");
    if (ks_pos != std::string::npos)
    {
        auto name_pos = json.find("\"name\"", ks_pos);
        if (name_pos != std::string::npos && name_pos < ks_pos + 500)
        {
            auto quote1 = json.find('"', json.find(':', name_pos) + 1);
            auto quote2 = json.find('"', quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos)
            {
                doc.kernelspec.name = json.substr(quote1 + 1, quote2 - quote1 - 1);
            }
        }
        auto dn_pos = json.find("\"display_name\"", ks_pos);
        if (dn_pos != std::string::npos && dn_pos < ks_pos + 500)
        {
            auto quote1 = json.find('"', json.find(':', dn_pos) + 1);
            auto quote2 = json.find('"', quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos)
            {
                doc.kernelspec.display_name = json.substr(quote1 + 1, quote2 - quote1 - 1);
            }
        }
        auto lang_pos = json.find("\"language\"", ks_pos);
        if (lang_pos != std::string::npos && lang_pos < ks_pos + 500)
        {
            auto quote1 = json.find('"', json.find(':', lang_pos) + 1);
            auto quote2 = json.find('"', quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos)
            {
                doc.kernelspec.language = json.substr(quote1 + 1, quote2 - quote1 - 1);
            }
        }
    }

    // Extract language_info name.
    auto li_pos = json.find("\"language_info\"");
    if (li_pos != std::string::npos)
    {
        auto name_pos = json.find("\"name\"", li_pos);
        if (name_pos != std::string::npos && name_pos < li_pos + 500)
        {
            auto quote1 = json.find('"', json.find(':', name_pos) + 1);
            auto quote2 = json.find('"', quote1 + 1);
            if (quote1 != std::string::npos && quote2 != std::string::npos)
            {
                doc.language_info.name = json.substr(quote1 + 1, quote2 - quote1 - 1);
            }
        }
    }

    // Parse cells array — find "cells" key and extract cell blocks.
    auto cells_pos = json.find("\"cells\"");
    if (cells_pos != std::string::npos)
    {
        auto arr_start = json.find('[', cells_pos);
        if (arr_start != std::string::npos)
        {
            // Count cells by finding cell_type occurrences within the cells array.
            auto search_pos = arr_start;
            while (true)
            {
                auto ct_pos = json.find("\"cell_type\"", search_pos);
                if (ct_pos == std::string::npos)
                {
                    break;
                }

                SerializedCell cell;
                cell.cell_id = generate_cell_id();

                // Extract cell_type value.
                auto quote1 = json.find('"', json.find(':', ct_pos) + 1);
                auto quote2 = json.find('"', quote1 + 1);
                if (quote1 != std::string::npos && quote2 != std::string::npos)
                {
                    cell.cell_type = json.substr(quote1 + 1, quote2 - quote1 - 1);
                }

                // Extract source — look for "source" key after cell_type.
                auto src_pos = json.find("\"source\"", ct_pos);
                if (src_pos != std::string::npos && src_pos < ct_pos + 2000)
                {
                    // Source can be a string or array of strings. Find opening bracket/quote.
                    auto val_start = json.find_first_of("[\"", json.find(':', src_pos) + 1);
                    if (val_start != std::string::npos)
                    {
                        if (json[val_start] == '"')
                        {
                            // Single string source.
                            auto end_quote = json.find('"', val_start + 1);
                            if (end_quote != std::string::npos)
                            {
                                cell.source = json.substr(val_start + 1, end_quote - val_start - 1);
                            }
                        }
                        else
                        {
                            // Array of strings — concatenate.
                            auto arr_end = json.find(']', val_start);
                            if (arr_end != std::string::npos)
                            {
                                std::string arr_content =
                                    json.substr(val_start + 1, arr_end - val_start - 1);
                                // Extract quoted strings.
                                std::string combined;
                                size_t str_pos = 0;
                                while (true)
                                {
                                    auto sq1 = arr_content.find('"', str_pos);
                                    if (sq1 == std::string::npos)
                                    {
                                        break;
                                    }
                                    auto sq2 = arr_content.find('"', sq1 + 1);
                                    if (sq2 == std::string::npos)
                                    {
                                        break;
                                    }
                                    combined += arr_content.substr(sq1 + 1, sq2 - sq1 - 1);
                                    str_pos = sq2 + 1;
                                }
                                cell.source = combined;
                            }
                        }
                    }
                }

                // Extract execution_count for code cells.
                if (cell.cell_type == "code")
                {
                    auto ec_pos = json.find("\"execution_count\"", ct_pos);
                    if (ec_pos != std::string::npos && ec_pos < ct_pos + 2000)
                    {
                        auto colon_pos = json.find(':', ec_pos);
                        if (colon_pos != std::string::npos)
                        {
                            auto digit_start = json.find_first_of("0123456789", colon_pos);
                            if (digit_start != std::string::npos && digit_start < colon_pos + 20)
                            {
                                auto digit_end = json.find_first_not_of("0123456789", digit_start);
                                cell.execution_count =
                                    std::stoi(json.substr(digit_start, digit_end - digit_start));
                            }
                        }
                    }
                }

                doc.cells.push_back(std::move(cell));
                search_pos = ct_pos + 1;
            }
        }
    }

    // Publish event.
    events::NotebookParsedEvent evt;
    evt.notebook_id = doc.kernelspec.name;
    evt.cell_count = static_cast<int>(doc.cells.size());
    evt.nbformat_version = doc.nbformat;
    event_bus_.publish(evt);

    return doc;
}

// ============================================================================
// serialize_ipynb
// ============================================================================

auto NotebookSerializer::serialize_ipynb(const NotebookDocument& doc) const -> std::string
{
    std::ostringstream oss;
    oss << "{\n";
    oss << "  \"nbformat\": " << doc.nbformat << ",\n";
    oss << "  \"nbformat_minor\": " << doc.nbformat_minor << ",\n";

    // Metadata.
    oss << "  \"metadata\": {\n";
    oss << "    \"kernelspec\": {\n";
    oss << "      \"name\": \"" << doc.kernelspec.name << "\",\n";
    oss << "      \"display_name\": \"" << doc.kernelspec.display_name << "\",\n";
    oss << "      \"language\": \"" << doc.kernelspec.language << "\"\n";
    oss << "    },\n";
    oss << "    \"language_info\": {\n";
    oss << "      \"name\": \"" << doc.language_info.name << "\"\n";
    oss << "    }\n";
    oss << "  },\n";

    // Cells.
    oss << "  \"cells\": [\n";
    for (size_t idx = 0; idx < doc.cells.size(); ++idx)
    {
        oss << serialize_cell(doc.cells[idx]);
        if (idx + 1 < doc.cells.size())
        {
            oss << ",";
        }
        oss << "\n";
    }
    oss << "  ]\n";
    oss << "}";

    return oss.str();
}

// ============================================================================
// validate_notebook
// ============================================================================

auto NotebookSerializer::validate_notebook(const NotebookDocument& doc) const -> ValidationResult
{
    ValidationResult result;

    if (doc.nbformat < 4)
    {
        result.valid = false;
        result.issues.push_back({ValidationIssue::Severity::kError,
                                 "Unsupported nbformat version: " + std::to_string(doc.nbformat),
                                 -1});
    }

    if (doc.cells.empty())
    {
        result.issues.push_back({ValidationIssue::Severity::kWarning, "Notebook has no cells", -1});
    }

    for (int idx = 0; idx < static_cast<int>(doc.cells.size()); ++idx)
    {
        const auto& cell = doc.cells[static_cast<size_t>(idx)];
        if (cell.cell_type != "code" && cell.cell_type != "markdown" && cell.cell_type != "raw")
        {
            result.valid = false;
            result.issues.push_back(
                {ValidationIssue::Severity::kError, "Unknown cell type: " + cell.cell_type, idx});
        }
        if (cell.source.empty() && cell.cell_type == "code")
        {
            result.issues.push_back({ValidationIssue::Severity::kWarning, "Empty code cell", idx});
        }
    }

    if (doc.kernelspec.name.empty())
    {
        result.issues.push_back(
            {ValidationIssue::Severity::kWarning, "Missing kernelspec name", -1});
    }

    return result;
}

// ============================================================================
// upgrade_notebook
// ============================================================================

auto NotebookSerializer::upgrade_notebook(const NotebookDocument& doc, int target_version) const
    -> std::expected<NotebookDocument, std::string>
{
    if (doc.nbformat >= target_version)
    {
        return doc; // Already at or above target version.
    }

    if (target_version != 4)
    {
        return std::unexpected("Only upgrade to nbformat 4 is supported");
    }

    NotebookDocument upgraded = doc;
    upgraded.nbformat = 4;
    upgraded.nbformat_minor = 5;

    // Ensure all cells have IDs.
    for (auto& cell : upgraded.cells)
    {
        if (cell.cell_id.empty())
        {
            cell.cell_id = generate_cell_id();
        }
    }

    return upgraded;
}

// ============================================================================
// strip_outputs
// ============================================================================

auto NotebookSerializer::strip_outputs(const NotebookDocument& doc) const -> NotebookDocument
{
    NotebookDocument stripped = doc;
    for (auto& cell : stripped.cells)
    {
        cell.outputs.clear();
        if (cell.cell_type == "code")
        {
            cell.execution_count = 0;
        }
    }
    return stripped;
}

// ============================================================================
// Static helpers
// ============================================================================

auto NotebookSerializer::code_cell_count(const NotebookDocument& doc) -> int
{
    return static_cast<int>(std::count_if(doc.cells.begin(),
                                          doc.cells.end(),
                                          [](const SerializedCell& cell)
                                          { return cell.cell_type == "code"; }));
}

auto NotebookSerializer::markdown_cell_count(const NotebookDocument& doc) -> int
{
    return static_cast<int>(std::count_if(doc.cells.begin(),
                                          doc.cells.end(),
                                          [](const SerializedCell& cell)
                                          { return cell.cell_type == "markdown"; }));
}

auto NotebookSerializer::summarize(const NotebookDocument& doc) -> std::string
{
    std::ostringstream oss;
    oss << "Notebook: nbformat " << doc.nbformat << "." << doc.nbformat_minor;
    oss << ", " << doc.cells.size() << " cells";
    oss << " (" << code_cell_count(doc) << " code, " << markdown_cell_count(doc) << " markdown)";
    if (!doc.kernelspec.display_name.empty())
    {
        oss << ", kernel: " << doc.kernelspec.display_name;
    }
    return oss.str();
}

// ============================================================================
// Private helpers
// ============================================================================

auto NotebookSerializer::parse_cell_block(const std::string& block, int /*index*/) const
    -> SerializedCell
{
    SerializedCell cell;
    cell.cell_id = generate_cell_id();

    auto ct_pos = block.find("\"cell_type\"");
    if (ct_pos != std::string::npos)
    {
        auto q1 = block.find('"', block.find(':', ct_pos) + 1);
        auto q2 = block.find('"', q1 + 1);
        if (q1 != std::string::npos && q2 != std::string::npos)
        {
            cell.cell_type = block.substr(q1 + 1, q2 - q1 - 1);
        }
    }

    return cell;
}

auto NotebookSerializer::serialize_cell(const SerializedCell& cell) const -> std::string
{
    std::ostringstream oss;
    oss << "    {\n";
    oss << "      \"cell_type\": \"" << cell.cell_type << "\",\n";
    oss << "      \"id\": \"" << cell.cell_id << "\",\n";
    oss << "      \"source\": [\"" << cell.source << "\"],\n";
    oss << "      \"metadata\": {},\n";

    if (cell.cell_type == "code")
    {
        if (cell.execution_count > 0)
        {
            oss << "      \"execution_count\": " << cell.execution_count << ",\n";
        }
        else
        {
            oss << "      \"execution_count\": null,\n";
        }
        oss << "      \"outputs\": []\n";
    }
    else
    {
        // No outputs for markdown/raw cells — remove trailing comma from source.
    }

    oss << "    }";
    return oss.str();
}

auto NotebookSerializer::generate_cell_id() -> std::string
{
    static std::mt19937 rng(
        static_cast<unsigned>(std::chrono::steady_clock::now().time_since_epoch().count()));
    static const char chars[] = "abcdefghijklmnopqrstuvwxyz0123456789";
    std::string cell_id = "cell-";
    std::uniform_int_distribution<int> dist(0, sizeof(chars) - 2);
    for (int idx = 0; idx < 8; ++idx)
    {
        cell_id += chars[dist(rng)];
    }
    return cell_id;
}

} // namespace markamp::core
