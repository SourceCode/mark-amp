// ============================================================================
// File: src/core/SyncHistoryLogger.cpp
// Phase 27: Cloud Sync & Collaboration — Sync history tracking
// ============================================================================

#include "SyncHistoryLogger.h"

#include "EventBus.h"

#include <algorithm>
#include <chrono>
#include <sstream>

namespace markamp::core
{

SyncHistoryLogger::SyncHistoryLogger(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto SyncHistoryLogger::log_sync(const CloudSyncResult& result,
                                 const std::string& operation,
                                 const std::string& device_name) -> void
{
    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    SyncHistoryEntry entry;
    entry.entry_id = "sync_" + std::to_string(next_id_++);
    entry.timestamp = timestamp;
    entry.operation = operation;
    entry.files_uploaded = result.files_uploaded;
    entry.files_downloaded = result.files_downloaded;
    entry.files_deleted = result.files_deleted;
    entry.elapsed_ms = result.elapsed_ms;
    entry.errors = result.errors;
    entry.device_name = device_name;
    entry.status = result.status;

    history_.push_back(std::move(entry));
    trim_history();
}

auto SyncHistoryLogger::log_entry(const SyncHistoryEntry& entry) -> void
{
    history_.push_back(entry);
    trim_history();
}

auto SyncHistoryLogger::get_history() const -> const std::vector<SyncHistoryEntry>&
{
    return history_;
}

auto SyncHistoryLogger::entry_count() const -> int32_t
{
    return static_cast<int32_t>(history_.size());
}

auto SyncHistoryLogger::is_empty() const -> bool
{
    return history_.empty();
}

auto SyncHistoryLogger::latest() const -> std::optional<SyncHistoryEntry>
{
    if (history_.empty())
    {
        return std::nullopt;
    }
    return history_.back();
}

auto SyncHistoryLogger::search_by_date(int64_t from_timestamp, int64_t to_timestamp) const
    -> std::vector<SyncHistoryEntry>
{
    std::vector<SyncHistoryEntry> results;
    for (const auto& entry : history_)
    {
        if (entry.timestamp >= from_timestamp && entry.timestamp <= to_timestamp)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto SyncHistoryLogger::search_by_operation(const std::string& operation) const
    -> std::vector<SyncHistoryEntry>
{
    std::vector<SyncHistoryEntry> results;
    for (const auto& entry : history_)
    {
        if (entry.operation == operation)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto SyncHistoryLogger::search_by_file(const std::string& file_name) const
    -> std::vector<SyncHistoryEntry>
{
    std::vector<SyncHistoryEntry> results;
    for (const auto& entry : history_)
    {
        // Check if any error message references this file.
        for (const auto& error : entry.errors)
        {
            if (error.find(file_name) != std::string::npos)
            {
                results.push_back(entry);
                break;
            }
        }
    }
    return results;
}

auto SyncHistoryLogger::error_entries() const -> std::vector<SyncHistoryEntry>
{
    std::vector<SyncHistoryEntry> results;
    for (const auto& entry : history_)
    {
        if (!entry.errors.empty())
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto SyncHistoryLogger::device_entries(const std::string& device_name) const
    -> std::vector<SyncHistoryEntry>
{
    std::vector<SyncHistoryEntry> results;
    for (const auto& entry : history_)
    {
        if (entry.device_name == device_name)
        {
            results.push_back(entry);
        }
    }
    return results;
}

auto SyncHistoryLogger::export_json() const -> std::string
{
    std::ostringstream json;
    json << "[\n";

    for (size_t idx = 0; idx < history_.size(); ++idx)
    {
        const auto& entry = history_[idx];
        json << "  {\n";
        json << "    \"entry_id\": \"" << entry.entry_id << "\",\n";
        json << "    \"timestamp\": " << entry.timestamp << ",\n";
        json << "    \"operation\": \"" << entry.operation << "\",\n";
        json << "    \"files_uploaded\": " << entry.files_uploaded << ",\n";
        json << "    \"files_downloaded\": " << entry.files_downloaded << ",\n";
        json << "    \"files_deleted\": " << entry.files_deleted << ",\n";
        json << "    \"conflicts\": " << entry.conflicts << ",\n";
        json << "    \"elapsed_ms\": " << entry.elapsed_ms << ",\n";
        json << "    \"device_name\": \"" << entry.device_name << "\",\n";
        json << "    \"status\": " << static_cast<int>(entry.status) << ",\n";
        json << "    \"errors\": [";

        for (size_t err_idx = 0; err_idx < entry.errors.size(); ++err_idx)
        {
            json << "\"" << entry.errors[err_idx] << "\"";
            if (err_idx + 1 < entry.errors.size())
            {
                json << ", ";
            }
        }

        json << "]\n";
        json << "  }";
        if (idx + 1 < history_.size())
        {
            json << ",";
        }
        json << "\n";
    }

    json << "]\n";
    return json.str();
}

auto SyncHistoryLogger::total_files_synced() const -> int64_t
{
    int64_t total = 0;
    for (const auto& entry : history_)
    {
        total += entry.files_uploaded + entry.files_downloaded;
    }
    return total;
}

auto SyncHistoryLogger::total_errors() const -> int32_t
{
    int32_t total = 0;
    for (const auto& entry : history_)
    {
        total += static_cast<int32_t>(entry.errors.size());
    }
    return total;
}

auto SyncHistoryLogger::clear() -> void
{
    history_.clear();
}

auto SyncHistoryLogger::set_max_entries(int32_t max_entries) -> void
{
    max_entries_ = max_entries;
    trim_history();
}

auto SyncHistoryLogger::max_entries() const -> int32_t
{
    return max_entries_;
}

auto SyncHistoryLogger::trim_history() -> void
{
    while (static_cast<int32_t>(history_.size()) > max_entries_)
    {
        history_.erase(history_.begin());
    }
}

} // namespace markamp::core
