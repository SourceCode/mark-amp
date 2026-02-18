/// @file CommandExecutionLog.cpp
/// @brief Implementation of CommandExecutionLog.

#include "CommandExecutionLog.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>

namespace markamp::core
{

void CommandExecutionLog::record(ExecutionLogEntry entry)
{
    entry.timestamp = std::chrono::system_clock::now();
    entries_.push_back(std::move(entry));
    while (entries_.size() > kMaxEntries)
    {
        entries_.pop_front();
    }
}

auto CommandExecutionLog::recent(int count) const -> std::vector<ExecutionLogEntry>
{
    std::vector<ExecutionLogEntry> result;
    const auto total = static_cast<int>(entries_.size());
    const int start_idx = std::max(0, total - count);
    for (int idx = total - 1; idx >= start_idx; --idx)
    {
        result.push_back(entries_[static_cast<std::size_t>(idx)]);
    }
    return result;
}

auto CommandExecutionLog::filter_by_command(const std::string& command_id) const
    -> std::vector<ExecutionLogEntry>
{
    std::vector<ExecutionLogEntry> result;
    for (const auto& entry : entries_)
    {
        if (entry.command_id == command_id)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto CommandExecutionLog::filter_by_source(const std::string& source) const
    -> std::vector<ExecutionLogEntry>
{
    std::vector<ExecutionLogEntry> result;
    for (const auto& entry : entries_)
    {
        if (entry.source == source)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto CommandExecutionLog::failures() const -> std::vector<ExecutionLogEntry>
{
    std::vector<ExecutionLogEntry> result;
    for (const auto& entry : entries_)
    {
        if (!entry.success)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto CommandExecutionLog::stats_for(const std::string& command_id) const -> CommandExecStats
{
    CommandExecStats stats;
    stats.command_id = command_id;
    int64_t total_duration = 0;

    for (const auto& entry : entries_)
    {
        if (entry.command_id == command_id)
        {
            stats.total_runs++;
            if (entry.success)
            {
                stats.success_count++;
            }
            else
            {
                stats.failure_count++;
            }
            total_duration += entry.duration_ms;
            if (entry.duration_ms > stats.max_duration_ms)
            {
                stats.max_duration_ms = entry.duration_ms;
            }
        }
    }
    if (stats.total_runs > 0)
    {
        stats.avg_duration_ms = total_duration / stats.total_runs;
    }
    return stats;
}

auto CommandExecutionLog::all_stats() const -> std::vector<CommandExecStats>
{
    std::unordered_map<std::string, bool> seen;
    std::vector<CommandExecStats> result;
    for (const auto& entry : entries_)
    {
        if (!seen.count(entry.command_id))
        {
            seen[entry.command_id] = true;
            result.push_back(stats_for(entry.command_id));
        }
    }
    return result;
}

auto CommandExecutionLog::export_json() const -> std::string
{
    std::ostringstream oss;
    oss << "[\n";
    bool first = true;
    for (const auto& entry : entries_)
    {
        if (!first)
        {
            oss << ",\n";
        }
        first = false;
        oss << "  {\"command_id\":\"" << entry.command_id << "\",\"source\":\"" << entry.source
            << "\",\"success\":" << (entry.success ? "true" : "false")
            << ",\"duration_ms\":" << entry.duration_ms << "}";
    }
    oss << "\n]";
    return oss.str();
}

void CommandExecutionLog::clear()
{
    entries_.clear();
}

auto CommandExecutionLog::entry_count() const -> std::size_t
{
    return entries_.size();
}

} // namespace markamp::core
