/// @file DataFrameRenderer.cpp
/// @brief V4 Phase 32 – Rich DataFrame Display implementation.

#include "ui/DataFrameRenderer.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <cstdlib>
#include <sstream>

namespace markamp::ui
{

// ============================================================================
// DataFrameData accessors
// ============================================================================

auto DataFrameData::column_count() const -> int
{
    return static_cast<int>(columns.size());
}

auto DataFrameData::row_count() const -> int
{
    return static_cast<int>(rows.size());
}

// ============================================================================
// Constructor
// ============================================================================

DataFrameEngine::DataFrameEngine(markamp::core::EventBus& event_bus)
    : event_bus_(event_bus)
{
}

// ============================================================================
// HTML table parsing
// ============================================================================

auto DataFrameEngine::parse_html_table(const std::string& html) -> DataFrameData
{
    DataFrameData result;

    // Simple HTML table parser: extract <th> for headers, <td> for data.
    // This handles pandas-generated HTML output.

    // Extract column headers from <th> tags.
    {
        size_t pos = 0;
        // Look for <thead> section first.
        const size_t thead_start = html.find("<thead>");
        const size_t thead_end = html.find("</thead>");

        const std::string header_section =
            (thead_start != std::string::npos && thead_end != std::string::npos)
                ? html.substr(thead_start, thead_end - thead_start)
                : html;

        pos = 0;
        while (true)
        {
            const size_t th_open = header_section.find("<th>", pos);
            if (th_open == std::string::npos)
            {
                break;
            }
            const size_t content_start = th_open + 4;
            const size_t th_close = header_section.find("</th>", content_start);
            if (th_close == std::string::npos)
            {
                break;
            }

            DataFrameColumn col;
            col.name = header_section.substr(content_start, th_close - content_start);
            col.dtype = "object"; // Default; could be inferred later.
            result.columns.push_back(std::move(col));
            pos = th_close + 5;
        }
    }

    // Extract row data from <td> tags within <tbody>.
    {
        const size_t tbody_start = html.find("<tbody>");
        const size_t tbody_end = html.find("</tbody>");

        const std::string body_section =
            (tbody_start != std::string::npos && tbody_end != std::string::npos)
                ? html.substr(tbody_start, tbody_end - tbody_start)
                : html;

        // Find each <tr>.
        size_t tr_pos = 0;
        while (true)
        {
            const size_t tr_open = body_section.find("<tr>", tr_pos);
            if (tr_open == std::string::npos)
            {
                break;
            }
            const size_t tr_close = body_section.find("</tr>", tr_open);
            if (tr_close == std::string::npos)
            {
                break;
            }

            const std::string row_section = body_section.substr(tr_open, tr_close - tr_open);

            std::vector<std::string> row;
            size_t td_pos = 0;
            while (true)
            {
                const size_t td_open = row_section.find("<td>", td_pos);
                if (td_open == std::string::npos)
                {
                    break;
                }
                const size_t content_start = td_open + 4;
                const size_t td_close = row_section.find("</td>", content_start);
                if (td_close == std::string::npos)
                {
                    break;
                }

                row.push_back(row_section.substr(content_start, td_close - content_start));
                td_pos = td_close + 5;
            }

            if (!row.empty())
            {
                result.rows.push_back(std::move(row));
            }

            tr_pos = tr_close + 5;
        }
    }

    result.total_rows = static_cast<int>(result.rows.size());
    return result;
}

auto DataFrameEngine::load_from_html(const std::string& html) -> void
{
    data_ = parse_html_table(html);
    is_filtered_ = false;
    filtered_rows_.clear();
    active_filter_.clear();

    core::events::DataFrameRenderedEvent event;
    event.cell_id = "";
    event.rows = data_.row_count();
    event.columns = data_.column_count();
    event_bus_.publish(event);
}

auto DataFrameEngine::load_from_json(const std::string& json) -> void
{
    // Simple JSON table parser for format:
    // {"columns": ["A","B"], "data": [["1","x"],["2","y"]]}
    DataFrameData result;

    // Extract column names from "columns": [...]
    const size_t cols_start = json.find("\"columns\"");
    if (cols_start != std::string::npos)
    {
        const size_t arr_start = json.find('[', cols_start);
        const size_t arr_end = json.find(']', arr_start);
        if (arr_start != std::string::npos && arr_end != std::string::npos)
        {
            const std::string arr = json.substr(arr_start + 1, arr_end - arr_start - 1);
            // Parse quoted strings.
            size_t pos = 0;
            while (true)
            {
                const size_t q1 = arr.find('"', pos);
                if (q1 == std::string::npos)
                {
                    break;
                }
                const size_t q2 = arr.find('"', q1 + 1);
                if (q2 == std::string::npos)
                {
                    break;
                }
                DataFrameColumn col;
                col.name = arr.substr(q1 + 1, q2 - q1 - 1);
                col.dtype = "object";
                result.columns.push_back(std::move(col));
                pos = q2 + 1;
            }
        }
    }

    // Extract rows from "data": [[...], [...]]
    const size_t data_start = json.find("\"data\"");
    if (data_start != std::string::npos)
    {
        const size_t outer_start = json.find('[', data_start);
        if (outer_start != std::string::npos)
        {
            // Find each inner array.
            size_t pos = outer_start + 1;
            while (true)
            {
                const size_t inner_start = json.find('[', pos);
                if (inner_start == std::string::npos)
                {
                    break;
                }
                const size_t inner_end = json.find(']', inner_start);
                if (inner_end == std::string::npos)
                {
                    break;
                }

                const std::string inner = json.substr(inner_start + 1, inner_end - inner_start - 1);
                std::vector<std::string> row;
                size_t vpos = 0;
                while (true)
                {
                    const size_t q1 = inner.find('"', vpos);
                    if (q1 == std::string::npos)
                    {
                        break;
                    }
                    const size_t q2 = inner.find('"', q1 + 1);
                    if (q2 == std::string::npos)
                    {
                        break;
                    }
                    row.push_back(inner.substr(q1 + 1, q2 - q1 - 1));
                    vpos = q2 + 1;
                }

                if (!row.empty())
                {
                    result.rows.push_back(std::move(row));
                }
                pos = inner_end + 1;
            }
        }
    }

    result.total_rows = static_cast<int>(result.rows.size());
    data_ = std::move(result);
    is_filtered_ = false;
    filtered_rows_.clear();
    active_filter_.clear();

    core::events::DataFrameRenderedEvent event;
    event.cell_id = "";
    event.rows = data_.row_count();
    event.columns = data_.column_count();
    event_bus_.publish(event);
}

// ============================================================================
// Sorting
// ============================================================================

auto DataFrameEngine::is_numeric_column(const std::vector<std::vector<std::string>>& rows,
                                        int col_index) -> bool
{
    if (rows.empty())
    {
        return false;
    }

    int numeric_count = 0;
    const int sample_size = std::min(static_cast<int>(rows.size()), 10);

    for (int idx = 0; idx < sample_size; ++idx)
    {
        const auto& row = rows[static_cast<size_t>(idx)];
        if (col_index >= static_cast<int>(row.size()))
        {
            continue;
        }
        const auto& val = row[static_cast<size_t>(col_index)];
        if (val.empty())
        {
            continue;
        }
        char* end_ptr = nullptr;
        std::strtod(val.c_str(), &end_ptr);
        if (end_ptr != val.c_str() && *end_ptr == '\0')
        {
            ++numeric_count;
        }
    }

    return numeric_count > sample_size / 2;
}

auto DataFrameEngine::sort_by_column(int col_index, bool ascending) -> void
{
    if (col_index < 0 || col_index >= data_.column_count())
    {
        return;
    }

    auto& rows = is_filtered_ ? filtered_rows_ : data_.rows;
    const bool is_numeric = is_numeric_column(rows, col_index);

    std::sort(rows.begin(),
              rows.end(),
              [col_index, ascending, is_numeric](const std::vector<std::string>& left,
                                                 const std::vector<std::string>& right) -> bool
              {
                  const auto col = static_cast<size_t>(col_index);
                  const auto& l_val = col < left.size() ? left[col] : "";
                  const auto& r_val = col < right.size() ? right[col] : "";

                  if (is_numeric)
                  {
                      const double l_num = std::strtod(l_val.c_str(), nullptr);
                      const double r_num = std::strtod(r_val.c_str(), nullptr);
                      return ascending ? (l_num < r_num) : (l_num > r_num);
                  }

                  return ascending ? (l_val < r_val) : (l_val > r_val);
              });
}

// ============================================================================
// Filtering
// ============================================================================

auto DataFrameEngine::filter(const std::string& query) -> void
{
    if (query.empty())
    {
        clear_filter();
        return;
    }

    active_filter_ = query;
    is_filtered_ = true;
    filtered_rows_.clear();

    for (const auto& row : data_.rows)
    {
        for (const auto& cell : row)
        {
            if (cell.find(query) != std::string::npos)
            {
                filtered_rows_.push_back(row);
                break;
            }
        }
    }
}

auto DataFrameEngine::clear_filter() -> void
{
    is_filtered_ = false;
    filtered_rows_.clear();
    active_filter_.clear();
}

// ============================================================================
// CSV export
// ============================================================================

auto DataFrameEngine::escape_csv_field(const std::string& field) -> std::string
{
    const bool needs_quoting = field.find(',') != std::string::npos ||
                               field.find('"') != std::string::npos ||
                               field.find('\n') != std::string::npos;

    if (!needs_quoting)
    {
        return field;
    }

    std::string escaped = "\"";
    for (const char ch : field)
    {
        if (ch == '"')
        {
            escaped += "\"\"";
        }
        else
        {
            escaped += ch;
        }
    }
    escaped += '"';
    return escaped;
}

auto DataFrameEngine::export_csv() const -> std::string
{
    std::ostringstream oss;

    // Header row.
    for (size_t idx = 0; idx < data_.columns.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ',';
        }
        oss << escape_csv_field(data_.columns[idx].name);
    }
    oss << '\n';

    // Data rows.
    const auto& rows = is_filtered_ ? filtered_rows_ : data_.rows;
    for (const auto& row : rows)
    {
        for (size_t idx = 0; idx < row.size(); ++idx)
        {
            if (idx > 0)
            {
                oss << ',';
            }
            oss << escape_csv_field(row[idx]);
        }
        oss << '\n';
    }

    return oss.str();
}

// ============================================================================
// Accessors
// ============================================================================

auto DataFrameEngine::data() const -> const DataFrameData&
{
    return data_;
}

auto DataFrameEngine::filtered_rows() const -> const std::vector<std::vector<std::string>>&
{
    return filtered_rows_;
}

auto DataFrameEngine::is_filtered() const -> bool
{
    return is_filtered_;
}

// ============================================================================
// Chart
// ============================================================================

auto DataFrameEngine::load_chart(const ChartData& chart) -> void
{
    chart_ = chart;
    has_chart_ = true;

    core::events::ChartRenderedEvent event;
    event.cell_id = "";
    event.format = chart.format;
    event_bus_.publish(event);
}

auto DataFrameEngine::chart() const -> const ChartData&
{
    return chart_;
}

auto DataFrameEngine::has_chart() const -> bool
{
    return has_chart_;
}

} // namespace markamp::ui
