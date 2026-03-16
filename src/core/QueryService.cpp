#include "QueryService.h"

#include "Config.h"
#include "Events.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace markamp::core
{

QueryService::QueryService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
}

auto QueryService::execute_sql(const std::string& sql) -> QueryResult
{
    QueryResult result;

    // Validate first
    auto error = validator_.validate(sql);
    if (!error.empty())
    {
        result.error = error;

        events::QueryErrorEvent err_evt;
        err_evt.sql = sql;
        err_evt.error_message = error;
        event_bus_.publish(err_evt);

        return result;
    }

    auto start_time = std::chrono::steady_clock::now();

    // In full implementation:
    // 1. Open database connection (or use shared connection)
    // 2. Prepare statement
    // 3. Set progress handler for timeout enforcement
    // 4. Step through results up to max_rows
    // 5. Collect column names and row data

    auto end_time = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration<double, std::milli>(end_time - start_time).count();

    // Add to history
    add_to_history(sql);

    // Publish event
    events::QueryExecutedEvent evt;
    evt.sql = sql;
    evt.elapsed_ms = result.elapsed_ms;
    evt.row_count = static_cast<int>(result.row_count());
    evt.success = result.is_success();
    event_bus_.publish(evt);

    return result;
}

auto QueryService::validate_sql(const std::string& sql) -> bool
{
    return validator_.validate(sql).empty();
}

auto QueryService::get_schema_info() -> SchemaInfo
{
    SchemaInfo schema;

    // In full implementation: query sqlite_master and PRAGMA table_info()
    // Populate tables, columns, row counts, etc.

    // Return basic structure information about expected tables.
    TableInfo blocks_table;
    blocks_table.name = "blocks";
    blocks_table.columns = {
        {"id", "TEXT", false, true, ""},
        {"content", "TEXT", true, false, ""},
        {"type", "TEXT", true, false, ""},
        {"subtype", "TEXT", true, false, ""},
        {"root_id", "TEXT", true, false, ""},
        {"parent_id", "TEXT", true, false, ""},
        {"sort", "INTEGER", true, false, "0"},
        {"created", "INTEGER", true, false, "0"},
        {"updated", "INTEGER", true, false, "0"},
        {"tag", "TEXT", true, false, ""},
        {"ial", "TEXT", true, false, ""},
    };
    schema.tables.push_back(blocks_table);

    TableInfo refs_table;
    refs_table.name = "refs";
    refs_table.columns = {
        {"block_id", "TEXT", false, false, ""},
        {"def_block_id", "TEXT", false, false, ""},
        {"type", "TEXT", true, false, ""},
        {"root_id", "TEXT", true, false, ""},
    };
    schema.tables.push_back(refs_table);

    TableInfo attrs_table;
    attrs_table.name = "block_attrs";
    attrs_table.columns = {
        {"block_id", "TEXT", false, false, ""},
        {"name", "TEXT", false, false, ""},
        {"value", "TEXT", true, false, ""},
    };
    schema.tables.push_back(attrs_table);

    return schema;
}

auto QueryService::get_table_info(const std::string& table_name) -> std::optional<TableInfo>
{
    auto schema = get_schema_info();
    for (const auto& table : schema.tables)
    {
        if (table.name == table_name)
        {
            return table;
        }
    }
    return std::nullopt;
}

auto QueryService::execute_with_params(const std::string& sql,
                                       const std::vector<std::string>& params) -> QueryResult
{
    // Validate the SQL template
    auto error = validator_.validate(sql);
    if (!error.empty())
    {
        QueryResult result;
        result.error = error;
        return result;
    }

    // In full implementation: use sqlite3_prepare_v2 + sqlite3_bind_text
    (void)params;

    return execute_sql(sql);
}

void QueryService::set_database_path(const std::string& path)
{
    database_path_ = path;
}

auto QueryService::get_query_history() -> std::vector<std::string>
{
    const std::lock_guard<std::mutex> lock(history_mutex_);
    return {query_history_.begin(), query_history_.end()};
}

void QueryService::add_to_history(const std::string& sql)
{
    const std::lock_guard<std::mutex> lock(history_mutex_);

    // Remove duplicate if exists
    auto iter = std::find(query_history_.begin(), query_history_.end(), sql);
    if (iter != query_history_.end())
    {
        query_history_.erase(iter);
    }

    query_history_.push_front(sql);

    // Trim to max size
    const int max_size = config_.get_int("knowledgebase.query.history_size", kDefaultHistorySize);
    while (static_cast<int>(query_history_.size()) > max_size)
    {
        query_history_.pop_back();
    }
}

auto QueryService::format_result_as_markdown(const QueryResult& result) -> std::string
{
    if (result.columns.empty())
    {
        return "*No results*";
    }

    std::ostringstream markdown;

    // Header row
    markdown << "| ";
    for (const auto& col : result.columns)
    {
        markdown << col << " | ";
    }
    markdown << "\n";

    // Separator row
    markdown << "| ";
    for (std::size_t col_idx = 0; col_idx < result.columns.size(); ++col_idx)
    {
        markdown << "--- | ";
    }
    markdown << "\n";

    // Data rows
    for (std::size_t row_idx = 0; row_idx < result.rows.size(); ++row_idx)
    {
        markdown << "| ";
        for (std::size_t col_idx = 0; col_idx < result.rows[row_idx].size(); ++col_idx)
        {
            if (row_idx < result.null_flags.size() && col_idx < result.null_flags[row_idx].size() &&
                result.null_flags[row_idx][col_idx])
            {
                markdown << "*NULL* | ";
            }
            else
            {
                markdown << result.rows[row_idx][col_idx] << " | ";
            }
        }
        markdown << "\n";
    }

    if (result.truncated)
    {
        markdown << "\n*Results truncated. " << result.total_rows_available
                 << " total rows available.*\n";
    }

    return markdown.str();
}

auto QueryService::estimate_query_cost(const std::string& sql) -> std::string
{
    // Generate a synthetic EXPLAIN QUERY PLAN by analyzing the SQL structure.
    std::ostringstream plan;
    plan << "EXPLAIN QUERY PLAN\n";
    plan << "──────────────────\n";

    // Detect tables referenced.
    auto sql_upper = sql;
    for (auto& chr : sql_upper)
    {
        chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
    }

    auto schema = get_schema_info();
    std::vector<std::string> referenced_tables;
    for (const auto& table : schema.tables)
    {
        auto table_upper = table.name;
        for (auto& chr : table_upper)
        {
            chr = static_cast<char>(std::toupper(static_cast<unsigned char>(chr)));
        }
        if (sql_upper.find(table_upper) != std::string::npos)
        {
            referenced_tables.push_back(table.name);
        }
    }

    int step = 0;
    for (const auto& table_name : referenced_tables)
    {
        plan << step++ << "|0|0|SCAN " << table_name << "\n";
    }

    // Detect WHERE clause (filter step).
    if (sql_upper.find("WHERE") != std::string::npos)
    {
        plan << step++ << "|0|0|FILTER using WHERE clause\n";
    }

    // Detect JOIN (join step).
    if (sql_upper.find("JOIN") != std::string::npos)
    {
        plan << step++ << "|0|0|NESTED LOOP JOIN\n";
    }

    // Detect ORDER BY (sort step).
    if (sql_upper.find("ORDER BY") != std::string::npos)
    {
        plan << step++ << "|0|0|USE TEMP B-TREE FOR ORDER BY\n";
    }

    // Detect GROUP BY (aggregate step).
    if (sql_upper.find("GROUP BY") != std::string::npos)
    {
        plan << step++ << "|0|0|USE TEMP B-TREE FOR GROUP BY\n";
    }

    // Detect LIMIT.
    if (sql_upper.find("LIMIT") != std::string::npos)
    {
        plan << step++ << "|0|0|LIMIT applied\n";
    }

    if (step == 0)
    {
        plan << "0|0|0|SCAN (full table scan)\n";
    }

    return plan.str();
}

} // namespace markamp::core
