/// @file CommandExecutionLog.h
/// @brief V9 Phase 36 Task 16 — Execution log: tracks all command runs for diagnostics.
///
/// Records every command execution with timing, source, success/failure,
/// and provides filtering, searching, and stats. Supports export to JSON.
#pragma once

#include <chrono>
#include <deque>
#include <string>
#include <vector>

namespace markamp::core
{

/// A single command execution log entry.
struct ExecutionLogEntry
{
    std::string command_id;    // Which command was executed
    std::string source;        // How it was invoked: "palette", "shortcut", "menu", etc.
    bool success{true};        // Whether it succeeded
    std::string error_message; // Error detail if !success
    int64_t duration_ms{0};    // Duration in milliseconds
    std::chrono::system_clock::time_point timestamp;
};

/// Execution statistics for a single command.
struct CommandExecStats
{
    std::string command_id;
    int total_runs{0};
    int success_count{0};
    int failure_count{0};
    int64_t avg_duration_ms{0};
    int64_t max_duration_ms{0};
};

// ============================================================================
// CommandExecutionLog — records and queries command execution history
// ============================================================================

/// Tracks all command executions for diagnostics, profiling, and debugging.
///
/// Usage:
/// ```cpp
/// CommandExecutionLog log;
/// log.record({.command_id = "file.save", .source = "shortcut",
///             .success = true, .duration_ms = 12});
/// auto recent = log.recent(20);
/// auto stats = log.stats_for("file.save");
/// auto json = log.export_json();
/// ```
class CommandExecutionLog
{
public:
    CommandExecutionLog() = default;

    /// Record a command execution.
    void record(ExecutionLogEntry entry);

    /// Get the N most recent log entries.
    [[nodiscard]] auto recent(int count = 50) const -> std::vector<ExecutionLogEntry>;

    /// Filter log entries by command ID.
    [[nodiscard]] auto filter_by_command(const std::string& command_id) const
        -> std::vector<ExecutionLogEntry>;

    /// Filter log entries by source.
    [[nodiscard]] auto filter_by_source(const std::string& source) const
        -> std::vector<ExecutionLogEntry>;

    /// Get failures only.
    [[nodiscard]] auto failures() const -> std::vector<ExecutionLogEntry>;

    /// Get execution statistics for a specific command.
    [[nodiscard]] auto stats_for(const std::string& command_id) const -> CommandExecStats;

    /// Get execution statistics for all commands.
    [[nodiscard]] auto all_stats() const -> std::vector<CommandExecStats>;

    /// Export all log entries to JSON.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Clear the execution log.
    void clear();

    /// Total number of logged entries.
    [[nodiscard]] auto entry_count() const -> std::size_t;

    /// Maximum number of entries retained (rolling buffer).
    static constexpr std::size_t kMaxEntries = 5000;

private:
    std::deque<ExecutionLogEntry> entries_;
};

} // namespace markamp::core
