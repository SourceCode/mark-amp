/// PluginQuarantine.cpp — V7 Phase 25: Plugin fault quarantine

#include "PluginQuarantine.h"

#include <fstream>
#include <sstream>

namespace markamp::core
{

void PluginQuarantine::set_persistence_path(const std::filesystem::path& path)
{
    std::lock_guard lock(mutex_);
    persistence_path_ = path;
}

void PluginQuarantine::quarantine(const std::string& plugin_id,
                                  const std::string& reason,
                                  int crash_count,
                                  bool manual)
{
    std::lock_guard lock(mutex_);

    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      std::chrono::system_clock::now().time_since_epoch())
                      .count();

    entries_[plugin_id] = QuarantineEntry{
        .plugin_id = plugin_id,
        .reason = reason,
        .quarantined_at_ms = now_ms,
        .crash_count = crash_count,
        .manual = manual,
    };
}

void PluginQuarantine::unquarantine(const std::string& plugin_id)
{
    std::lock_guard lock(mutex_);
    entries_.erase(plugin_id);
    crash_counts_.erase(plugin_id);
}

auto PluginQuarantine::is_quarantined(const std::string& plugin_id) const -> bool
{
    std::lock_guard lock(mutex_);
    return entries_.contains(plugin_id);
}

auto PluginQuarantine::get_entry(const std::string& plugin_id) const
    -> std::optional<QuarantineEntry>
{
    std::lock_guard lock(mutex_);
    auto entry_it = entries_.find(plugin_id);
    if (entry_it == entries_.end())
    {
        return std::nullopt;
    }
    return entry_it->second;
}

auto PluginQuarantine::all_entries() const -> std::vector<QuarantineEntry>
{
    std::lock_guard lock(mutex_);
    std::vector<QuarantineEntry> result;
    result.reserve(entries_.size());
    for (const auto& [plugin_id, entry] : entries_)
    {
        result.push_back(entry);
    }
    return result;
}

auto PluginQuarantine::record_crash(const std::string& plugin_id, int auto_quarantine_threshold)
    -> bool
{
    std::lock_guard lock(mutex_);
    auto& count = crash_counts_[plugin_id];
    count++;

    if (count >= auto_quarantine_threshold && !entries_.contains(plugin_id))
    {
        auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                          std::chrono::system_clock::now().time_since_epoch())
                          .count();

        entries_[plugin_id] = QuarantineEntry{
            .plugin_id = plugin_id,
            .reason = "Auto-quarantined after " + std::to_string(count) + " crashes",
            .quarantined_at_ms = now_ms,
            .crash_count = count,
            .manual = false,
        };
        return true;
    }
    return false;
}

auto PluginQuarantine::crash_count(const std::string& plugin_id) const -> int
{
    std::lock_guard lock(mutex_);
    auto count_it = crash_counts_.find(plugin_id);
    if (count_it == crash_counts_.end())
    {
        return 0;
    }
    return count_it->second;
}

void PluginQuarantine::clear()
{
    std::lock_guard lock(mutex_);
    entries_.clear();
    crash_counts_.clear();
}

auto PluginQuarantine::save() const -> Result<void>
{
    std::lock_guard lock(mutex_);
    if (persistence_path_.empty())
    {
        return std::unexpected(make_io_error("No persistence path set", ErrorCode::IoError));
    }

    // Simple JSON serialization
    std::ostringstream json_stream;
    json_stream << "[\n";
    bool first = true;
    for (const auto& [plugin_id, entry] : entries_)
    {
        if (!first)
        {
            json_stream << ",\n";
        }
        first = false;
        json_stream << "  {\"id\":\"" << entry.plugin_id << "\",\"reason\":\"" << entry.reason
                    << "\",\"ts\":" << entry.quarantined_at_ms
                    << ",\"crashes\":" << entry.crash_count
                    << ",\"manual\":" << (entry.manual ? "true" : "false") << "}";
    }
    json_stream << "\n]\n";

    std::ofstream file(persistence_path_);
    if (!file.is_open())
    {
        return std::unexpected(make_io_error(
            "Cannot open quarantine file: " + persistence_path_.string(), ErrorCode::IoError));
    }
    file << json_stream.str();
    return {};
}

auto PluginQuarantine::load() -> Result<void>
{
    std::lock_guard lock(mutex_);
    if (persistence_path_.empty())
    {
        return std::unexpected(make_io_error("No persistence path set", ErrorCode::IoError));
    }

    if (!std::filesystem::exists(persistence_path_))
    {
        return {}; // No file yet is fine
    }

    std::ifstream file(persistence_path_);
    if (!file.is_open())
    {
        return std::unexpected(make_io_error(
            "Cannot read quarantine file: " + persistence_path_.string(), ErrorCode::IoError));
    }

    // For now, just verify the file can be opened
    // Full JSON parsing would use nlohmann::json in production
    return {};
}

} // namespace markamp::core
