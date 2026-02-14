# Phase 15 -- SQL Query Interface for Blocks

**Priority:** Medium (power-user feature)
**Estimated Scope:** ~8 files created/modified
**Dependencies:** Phase 14 (SearchService, SQLite FTS5 block database); Existing EventBus, Config, PluginContext

---

## Objective

Implement a user-facing SQL query interface that allows direct SQL queries against the block database. SiYuan exposes `SELECT * FROM blocks WHERE ...` as a first-class feature, allowing power users to write custom queries. The query interface includes SQL validation (only SELECT allowed), timeout enforcement, row limiting, and schema introspection.

This is a powerful extensibility mechanism: plugins and advanced users can query the entire block database with the full expressiveness of SQL, including JOINs across blocks, refs, and attributes tables.

---

## Prerequisites

- Phase 14 completed: block database with SQLite tables (blocks, refs, block_attrs, assets)
- `src/core/EventBus.h` -- publish/subscribe for query events
- `src/core/Config.h` -- query timeout and row limit settings
- `src/core/PluginContext.h` -- service injection
- SQLite3 library linked (already available from Phase 14)

---

## SiYuan Source Reference

| SiYuan File | Relevant Functions | MarkAmp Mapping |
|---|---|---|
| `kernel/api/query.go` | `sql` endpoint -- accepts raw SQL SELECT | `QueryService::execute_sql()` |
| `kernel/model/search.go` | `QuerySQL` | `QueryService::execute_sql()` internal |
| `kernel/sql/database.go` | Schema definition, table creation | `QueryService::get_schema_info()` |
| `kernel/sql/block.go` | Block table schema | `blocks` table definition |
| `kernel/sql/block_ref.go` | Refs table schema | `refs` table definition |

---

## MarkAmp Integration Points

1. **SQLite block database** -- The query service operates directly on the same SQLite database used by the SearchService (Phase 14). It provides read-only access through a shared database connection.
2. **Config** -- Timeout and row limit settings prevent runaway queries.
3. **PluginContext** -- `QueryService*` pointer added for extension access. Plugins can execute custom queries to build custom views.
4. **Phase 16** -- SearchPanel includes a "SQL" mode tab that sends queries through this service.
5. **EventBus** -- Publishes `QueryExecutedEvent` for logging and diagnostics.

---

## Data Structures to Implement

All structures go in `src/core/SqlQuery.h`:

```cpp
#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Information about a database column.
struct ColumnInfo
{
    /// Column name.
    std::string name;

    /// Column type (e.g., "TEXT", "INTEGER", "REAL").
    std::string type;

    /// Whether the column allows NULL values.
    bool nullable{true};

    /// Whether this column is part of the primary key.
    bool is_primary_key{false};

    /// Default value expression (empty if none).
    std::string default_value;
};

/// Information about a database table.
struct TableInfo
{
    /// Table name.
    std::string name;

    /// Column definitions.
    std::vector<ColumnInfo> columns;

    /// Approximate row count (from sqlite_stat1 or COUNT(*)).
    int64_t row_count{0};

    /// Whether this is a virtual table (e.g., FTS5).
    bool is_virtual{false};

    /// Type of virtual table (e.g., "fts5") if applicable.
    std::string virtual_type;
};

/// Result of a SQL query execution.
struct QueryResult
{
    /// Column names in result order.
    std::vector<std::string> columns;

    /// Row data: each row is a vector of string-serialized cell values.
    /// NULL values are represented as empty strings with a corresponding null flag.
    std::vector<std::vector<std::string>> rows;

    /// Per-cell null indicators (rows[i][j] is NULL if null_flags[i][j] is true).
    std::vector<std::vector<bool>> null_flags;

    /// Number of rows returned.
    [[nodiscard]] auto row_count() const -> std::size_t
    {
        return rows.size();
    }

    /// Number of columns.
    [[nodiscard]] auto column_count() const -> std::size_t
    {
        return columns.size();
    }

    /// Execution time in milliseconds.
    double elapsed_ms{0.0};

    /// Whether the result was truncated due to max_rows limit.
    bool truncated{false};

    /// Total rows that would have been returned without limit.
    int64_t total_rows_available{0};

    /// Error message (empty if query succeeded).
    std::string error;

    /// Whether the query executed successfully.
    [[nodiscard]] auto is_success() const -> bool
    {
        return error.empty();
    }
};

/// SQL statement classification for security validation.
enum class SqlStatementType : std::uint8_t
{
    Select,
    Insert,
    Update,
    Delete,
    Create,
    Drop,
    Alter,
    Pragma,
    Attach,
    Detach,
    Unknown
};

/// SQL validator that ensures only safe (read-only) queries are executed.
class SqlValidator
{
public:
    /// Classify a SQL statement by its type.
    [[nodiscard]] auto classify(std::string_view sql) const -> SqlStatementType;

    /// Validate that a SQL string is a safe SELECT query.
    /// Returns an error message if invalid, or empty string if valid.
    [[nodiscard]] auto validate(std::string_view sql) const -> std::string;

    /// Check if the statement is read-only (SELECT or safe PRAGMA).
    [[nodiscard]] auto is_read_only(std::string_view sql) const -> bool;

    /// Sanitize a SQL string by removing comments and normalizing whitespace.
    [[nodiscard]] auto sanitize(std::string_view sql) const -> std::string;

private:
    /// Check for common SQL injection patterns.
    [[nodiscard]] auto has_injection_pattern(std::string_view sql) const -> bool;

    /// Extract the first keyword from a SQL statement.
    [[nodiscard]] auto first_keyword(std::string_view sql) const -> std::string;
};

/// Database schema information for user reference.
struct SchemaInfo
{
    /// All available tables.
    std::vector<TableInfo> tables;

    /// SQLite version string.
    std::string sqlite_version;

    /// Database file size in bytes.
    int64_t database_size_bytes{0};

    /// Total indexed blocks count.
    int64_t total_blocks{0};
};

} // namespace markamp::core
```

---

## Key Functions to Implement

### QueryService class (`src/core/QueryService.h` / `src/core/QueryService.cpp`)

Constructor: `QueryService(EventBus& event_bus, Config& config)`

1. **`auto execute_sql(const std::string& sql) -> QueryResult`** -- Execute a SQL query against the block database. Validates the SQL first (must be SELECT only). Enforces timeout and row limit from config. Returns `QueryResult` with column names, row data, and execution metrics. On validation failure, returns `QueryResult` with `error` populated. On timeout, returns partial results with `error` set. Publishes `QueryExecutedEvent`.

2. **`auto validate_sql(const std::string& sql) -> bool`** -- Validate that a SQL string is safe to execute. Uses `SqlValidator` to check: (a) statement is SELECT, (b) no write keywords (INSERT/UPDATE/DELETE/DROP/ALTER/CREATE), (c) no ATTACH/DETACH (prevent accessing other databases), (d) no dangerous PRAGMA statements.

3. **`auto get_schema_info() -> SchemaInfo`** -- Return metadata about all available tables, their columns, types, and approximate row counts. Useful for building query assistance UI (autocomplete, schema explorer). Queries `sqlite_master` and `PRAGMA table_info()` for each table.

4. **`auto get_table_info(const std::string& table_name) -> std::optional<TableInfo>`** -- Return detailed column information for a specific table. Includes column names, types, nullability, and primary key flags.

5. **`auto execute_with_params(const std::string& sql, const std::vector<std::string>& params) -> QueryResult`** -- Execute a parameterized query using SQLite prepared statements. Parameters are bound by position (`?` placeholders). Prevents SQL injection for programmatic queries.

6. **`void set_database_path(const std::string& path)`** -- Set the path to the SQLite database file. Called during initialization.

7. **`auto get_query_history() -> std::vector<std::string>`** -- Return the most recent N queries (configurable via `knowledgebase.query.history_size`). Stored in memory, not persisted.

8. **`void add_to_history(const std::string& sql)`** -- Add a query to the history, deduplicating and trimming to max size.

9. **`auto format_result_as_markdown(const QueryResult& result) -> std::string`** -- Format a `QueryResult` as a markdown table for display in the preview panel or export.

10. **`auto estimate_query_cost(const std::string& sql) -> std::string`** -- Run `EXPLAIN QUERY PLAN` on the SQL and return the human-readable execution plan. Useful for query optimization hints.

---

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// SQL query events (Phase 15)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QueryExecutedEvent)
std::string sql;
double elapsed_ms{0.0};
int row_count{0};
bool success{true};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(QueryErrorEvent)
std::string sql;
std::string error_message;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT(QueryPanelOpenRequestEvent);
```

---

## Config Keys to Add

Add to `resources/config_defaults.json`:

| Key | Type | Default | Description |
|---|---|---|---|
| `knowledgebase.query.timeout_ms` | int | `5000` | Maximum execution time for a SQL query in milliseconds |
| `knowledgebase.query.max_rows` | int | `1000` | Maximum number of rows returned from a query |
| `knowledgebase.query.history_size` | int | `50` | Number of recent queries to remember |
| `knowledgebase.query.enable_sql_mode` | bool | `true` | Enable user-facing SQL query interface |
| `knowledgebase.query.show_execution_plan` | bool | `false` | Show EXPLAIN QUERY PLAN output for each query |

---

## Test Cases

File: `tests/unit/test_sql_query.cpp`

```cpp
#include "core/SqlQuery.h"
#include "core/QueryService.h"
#include "core/EventBus.h"
#include "core/Config.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
```

1. **Valid SELECT query** -- Execute `"SELECT id, content FROM blocks WHERE type = 'p' LIMIT 10"`. Verify: `QueryResult::is_success() == true`. `columns` contains "id" and "content". `row_count()` is between 0 and 10. `elapsed_ms > 0`.

2. **Reject INSERT statement** -- Validate `"INSERT INTO blocks VALUES (...)"`. Verify: `SqlValidator::validate()` returns non-empty error string. `classify()` returns `SqlStatementType::Insert`. `execute_sql()` returns result with `error` populated.

3. **Reject DROP statement** -- Validate `"DROP TABLE blocks"`. Verify: `classify()` returns `SqlStatementType::Drop`. `validate()` returns error. Not executed.

4. **Complex JOIN query** -- Execute `"SELECT b.id, b.content, r.def_block_id FROM blocks b JOIN refs r ON b.id = r.block_id WHERE b.type = 'h' LIMIT 5"`. Verify: result has 3 columns. Query succeeds.

5. **Query with LIKE** -- Execute `"SELECT id, content FROM blocks WHERE content LIKE '%search term%'"`. Verify: result contains only blocks whose content includes "search term".

6. **Query with ORDER BY** -- Execute `"SELECT id, content FROM blocks ORDER BY created DESC LIMIT 10"`. Verify: results are in descending creation order.

7. **Timeout handling** -- Configure `timeout_ms = 1`. Execute a query that would scan all rows (e.g., `"SELECT * FROM blocks"`). Verify: `QueryResult::error` contains timeout message. Partial results may be returned.

8. **Max rows limit** -- Configure `max_rows = 5`. Execute `"SELECT * FROM blocks"` (assuming more than 5 blocks exist). Verify: `row_count() <= 5`. `truncated == true`.

9. **Schema info retrieval** -- Call `get_schema_info()`. Verify: `SchemaInfo::tables` contains at least "blocks" and "refs". Each `TableInfo` has non-empty `columns` with correct types. "blocks" table has columns: id (TEXT), content (TEXT), type (TEXT), root_id (TEXT), parent_id (TEXT), etc.

10. **Empty result set** -- Execute `"SELECT * FROM blocks WHERE id = 'nonexistent_id_12345'"`. Verify: `QueryResult::is_success() == true`. `row_count() == 0`. `columns` still populated with correct names.

---

## Acceptance Criteria

- [ ] `QueryService::execute_sql()` executes valid SELECT queries and returns correct results
- [ ] `SqlValidator::validate()` rejects INSERT, UPDATE, DELETE, DROP, ALTER, CREATE, ATTACH, DETACH
- [ ] `SqlValidator::classify()` correctly identifies all statement types
- [ ] Timeout enforcement terminates long-running queries after configured duration
- [ ] Row limit enforcement truncates results and sets `truncated == true`
- [ ] `get_schema_info()` returns complete table and column metadata
- [ ] `execute_with_params()` correctly binds positional parameters
- [ ] Query history is maintained with deduplication and size limit
- [ ] `format_result_as_markdown()` produces valid markdown table output
- [ ] `estimate_query_cost()` returns EXPLAIN QUERY PLAN output
- [ ] `QueryExecutedEvent` fires with sql, elapsed_ms, row_count for every execution
- [ ] `QueryErrorEvent` fires for validation failures and timeouts
- [ ] NULL values are correctly represented in `QueryResult` via `null_flags`
- [ ] All 10 Catch2 test cases pass
- [ ] No use of `catch(...)` -- all exception handlers use typed catches
- [ ] All query methods marked `[[nodiscard]]`

---

## Files to Create/Modify

### New Files

| File | Description |
|---|---|
| `src/core/SqlQuery.h` | `ColumnInfo`, `TableInfo`, `QueryResult`, `SqlValidator`, `SchemaInfo`, enums |
| `src/core/SqlQuery.cpp` | `SqlValidator` implementations |
| `src/core/QueryService.h` | `QueryService` class declaration |
| `src/core/QueryService.cpp` | All 10 service methods, SQLite execution, timeout, history |
| `tests/unit/test_sql_query.cpp` | All 10 Catch2 test cases |

### Modified Files

| File | Change |
|---|---|
| `src/core/Events.h` | Add `QueryExecutedEvent`, `QueryErrorEvent`, `QueryPanelOpenRequestEvent` |
| `src/core/PluginContext.h` | Add `QueryService* query_service{nullptr};` pointer |
| `src/CMakeLists.txt` | Add `core/SqlQuery.cpp`, `core/QueryService.cpp` to sources |
| `tests/CMakeLists.txt` | Add `test_sql_query` test target |
| `resources/config_defaults.json` | Add 5 `knowledgebase.query.*` config keys |
