#pragma once

#include <chrono>
#include <cstdint>
#include <string>
#include <string_view>
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

    /// Approximate row count.
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
    std::vector<std::vector<std::string>> rows;

    /// Per-cell null indicators.
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
