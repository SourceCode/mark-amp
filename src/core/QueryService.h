#pragma once

#include "EventBus.h"
#include "SqlQuery.h"

#include <deque>
#include <mutex>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;

/// Service for user-facing SQL queries against the block database.
/// Provides read-only access with validation, timeout, and row limiting.
class QueryService
{
public:
    QueryService(EventBus& event_bus, Config& config);

    /// Execute a SQL query (SELECT only). Validates, enforces limits.
    [[nodiscard]] auto execute_sql(const std::string& sql) -> QueryResult;

    /// Validate that a SQL string is safe to execute.
    [[nodiscard]] auto validate_sql(const std::string& sql) -> bool;

    /// Return metadata about all available tables.
    [[nodiscard]] auto get_schema_info() -> SchemaInfo;

    /// Return detailed column information for a specific table.
    [[nodiscard]] auto get_table_info(const std::string& table_name) -> std::optional<TableInfo>;

    /// Execute a parameterized query using SQLite prepared statements.
    [[nodiscard]] auto execute_with_params(const std::string& sql,
                                           const std::vector<std::string>& params) -> QueryResult;

    /// Set the path to the SQLite database file.
    void set_database_path(const std::string& path);

    /// Return the most recent N queries.
    [[nodiscard]] auto get_query_history() -> std::vector<std::string>;

    /// Add a query to the history.
    void add_to_history(const std::string& sql);

    /// Format a QueryResult as a markdown table.
    [[nodiscard]] auto format_result_as_markdown(const QueryResult& result) -> std::string;

    /// Run EXPLAIN QUERY PLAN and return the execution plan.
    [[nodiscard]] auto estimate_query_cost(const std::string& sql) -> std::string;

private:
    EventBus& event_bus_;
    Config& config_;
    SqlValidator validator_;
    std::string database_path_;

    mutable std::mutex history_mutex_;
    std::deque<std::string> query_history_;
    static constexpr int kDefaultHistorySize = 50;
};

} // namespace markamp::core
