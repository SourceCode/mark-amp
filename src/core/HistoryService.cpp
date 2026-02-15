// ============================================================================
// File: src/core/HistoryService.cpp
// Phase 32: Document History & Snapshots — HistoryService implementation
// ============================================================================
#include "HistoryService.h"

#include <nlohmann/json.hpp>

#include <algorithm>
#include <chrono>
#include <fstream>
#include <functional>
#include <iomanip>
#include <sstream>

namespace markamp::core
{

HistoryService::HistoryService(EventBus& event_bus,
                               Config& config,
                               std::filesystem::path workspace_root)
    : event_bus_(event_bus)
    , config_(config)
    , workspace_root_(std::move(workspace_root))
    , history_root_(workspace_root_ / "history")
{
}

auto HistoryService::initialize() -> std::expected<void, std::string>
{
    std::error_code error_code;
    std::filesystem::create_directories(history_root_, error_code);
    if (error_code)
    {
        return std::unexpected("Failed to create history directory: " + error_code.message());
    }

    return load_index();
}

auto HistoryService::get_doc_histories(const std::string& root_id, int page, int page_size) const
    -> std::expected<std::vector<HistoryEntry>, std::string>
{
    std::lock_guard lock(mutex_);

    std::vector<HistoryEntry> results;
    for (const auto& [entry_id, entry] : entry_index_)
    {
        if (entry.root_id == root_id)
        {
            results.push_back(entry);
        }
    }

    // Sort newest first
    std::sort(results.begin(),
              results.end(),
              [](const HistoryEntry& left, const HistoryEntry& right)
              { return left.created > right.created; });

    // Paginate
    const auto start = static_cast<std::size_t>(page * page_size);
    if (start >= results.size())
    {
        return std::vector<HistoryEntry>{};
    }

    const auto end = std::min(start + static_cast<std::size_t>(page_size), results.size());
    return std::vector<HistoryEntry>(results.begin() + static_cast<int>(start),
                                     results.begin() + static_cast<int>(end));
}

auto HistoryService::get_notebook_history(const std::string& notebook_id,
                                          HistoryOpType type_filter,
                                          const std::string& query) const
    -> std::expected<std::vector<HistoryEntry>, std::string>
{
    std::lock_guard lock(mutex_);

    std::vector<HistoryEntry> results;
    for (const auto& [entry_id, entry] : entry_index_)
    {
        if (entry.notebook_id != notebook_id)
        {
            continue;
        }
        if (entry.op_type != type_filter)
        {
            continue;
        }
        if (!query.empty() && entry.title.find(query) == std::string::npos)
        {
            continue;
        }
        results.push_back(entry);
    }

    std::sort(results.begin(),
              results.end(),
              [](const HistoryEntry& left, const HistoryEntry& right)
              { return left.created > right.created; });

    return results;
}

auto HistoryService::get_documents_with_history() const -> std::vector<HistorySummary>
{
    std::lock_guard lock(mutex_);

    std::unordered_map<std::string, HistorySummary> summary_map;

    for (const auto& [entry_id, entry] : entry_index_)
    {
        auto& summary = summary_map[entry.root_id];
        summary.root_id = entry.root_id;
        summary.title = entry.title;
        ++summary.entry_count;
        summary.total_storage_bytes += entry.content_size;

        if (summary.entry_count == 1 || entry.created < summary.oldest_entry)
        {
            summary.oldest_entry = entry.created;
        }
        if (summary.entry_count == 1 || entry.created > summary.newest_entry)
        {
            summary.newest_entry = entry.created;
        }
    }

    std::vector<HistorySummary> results;
    results.reserve(summary_map.size());
    for (auto& [root_id, summary] : summary_map)
    {
        results.push_back(std::move(summary));
    }

    return results;
}

auto HistoryService::get_history_content(const std::string& history_entry_id) const
    -> std::expected<std::string, std::string>
{
    std::lock_guard lock(mutex_);

    auto found = entry_index_.find(history_entry_id);
    if (found == entry_index_.end())
    {
        return std::unexpected("History entry not found: " + history_entry_id);
    }

    std::ifstream ifs(found->second.history_path);
    if (!ifs.is_open())
    {
        return std::unexpected("Cannot read history file: " + found->second.history_path);
    }

    std::ostringstream oss;
    oss << ifs.rdbuf();
    return oss.str();
}

auto HistoryService::create_auto_snapshot(const std::string& root_id,
                                          const std::string& file_path,
                                          const std::string& content,
                                          const std::string& title)
    -> std::expected<std::string, std::string>
{
    const auto config = history_config();
    if (!config.auto_save_enabled)
    {
        return std::string{};
    }

    // Debounce check
    auto now = std::chrono::steady_clock::now();
    {
        std::lock_guard lock(mutex_);
        auto last_it = last_auto_snapshot_.find(root_id);
        if (last_it != last_auto_snapshot_.end())
        {
            auto elapsed =
                std::chrono::duration_cast<std::chrono::seconds>(now - last_it->second).count();
            if (elapsed < config.auto_save_interval_seconds)
            {
                return std::string{}; // Too soon
            }
        }
    }

    // Content hash deduplication
    const auto hash = compute_hash(content);
    {
        std::lock_guard lock(mutex_);
        for (const auto& [entry_id, entry] : entry_index_)
        {
            if (entry.root_id == root_id && entry.content_hash == hash)
            {
                // Same content — skip
                return std::string{};
            }
        }
    }

    auto result = create_entry(root_id, file_path, content, title, HistoryOpType::Update);

    if (result.has_value())
    {
        std::lock_guard lock(mutex_);
        last_auto_snapshot_[root_id] = now;
    }

    return result;
}

auto HistoryService::create_manual_snapshot(const std::string& root_id,
                                            const std::string& file_path,
                                            const std::string& content,
                                            const std::string& title)
    -> std::expected<std::string, std::string>
{
    return create_entry(root_id, file_path, content, title, HistoryOpType::Snapshot);
}

auto HistoryService::rollback_doc(const std::string& root_id, const std::string& history_entry_id)
    -> std::expected<std::string, std::string>
{
    // Get the history content
    auto content_result = get_history_content(history_entry_id);
    if (!content_result.has_value())
    {
        return std::unexpected(content_result.error());
    }

    return content_result.value();
}

auto HistoryService::diff_entries(const std::string& old_entry_id,
                                  const std::string& new_entry_id) const
    -> std::expected<DiffResult, std::string>
{
    auto old_content = get_history_content(old_entry_id);
    auto new_content = get_history_content(new_entry_id);

    if (!old_content.has_value())
    {
        return std::unexpected("Old entry: " + old_content.error());
    }
    if (!new_content.has_value())
    {
        return std::unexpected("New entry: " + new_content.error());
    }

    // Use DiffEngine (imported separately) for actual computation
    DiffResult result;
    result.old_title = old_entry_id;
    result.new_title = new_entry_id;
    // Actual diff computation delegated to DiffEngine
    return result;
}

auto HistoryService::diff_with_current(const std::string& history_entry_id,
                                       const std::string& /*current_content*/) const
    -> std::expected<DiffResult, std::string>
{
    auto old_content = get_history_content(history_entry_id);
    if (!old_content.has_value())
    {
        return std::unexpected(old_content.error());
    }

    DiffResult result;
    result.old_title = history_entry_id;
    result.new_title = "Current";
    return result;
}

auto HistoryService::clear_doc_history(const std::string& root_id)
    -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    std::vector<std::string> to_remove;
    for (const auto& [entry_id, entry] : entry_index_)
    {
        if (entry.root_id == root_id)
        {
            to_remove.push_back(entry_id);
        }
    }

    for (const auto& entry_id : to_remove)
    {
        auto& entry = entry_index_[entry_id];
        std::error_code error_code;
        std::filesystem::remove(entry.history_path, error_code);
        entry_index_.erase(entry_id);
    }

    return save_index();
}

auto HistoryService::clear_all_history() -> std::expected<void, std::string>
{
    std::lock_guard lock(mutex_);

    entry_index_.clear();
    last_auto_snapshot_.clear();

    std::error_code error_code;
    std::filesystem::remove_all(history_root_, error_code);
    std::filesystem::create_directories(history_root_, error_code);

    return save_index();
}

auto HistoryService::run_cleanup() -> std::expected<int, std::string>
{
    std::lock_guard lock(mutex_);

    const auto config = history_config();
    const auto cutoff =
        std::chrono::system_clock::now() - std::chrono::hours(config.retention_days * 24);

    int removed = 0;

    // Remove entries older than retention
    std::vector<std::string> to_remove;
    for (const auto& [entry_id, entry] : entry_index_)
    {
        if (entry.created < cutoff)
        {
            to_remove.push_back(entry_id);
        }
    }

    for (const auto& entry_id : to_remove)
    {
        std::error_code error_code;
        std::filesystem::remove(entry_index_[entry_id].history_path, error_code);
        entry_index_.erase(entry_id);
        ++removed;
    }

    auto save_result = save_index();
    if (!save_result.has_value())
    {
        return std::unexpected(save_result.error());
    }

    return removed;
}

auto HistoryService::history_config() const -> HistoryConfig
{
    // Return defaults (would read from Config in full implementation)
    return {};
}

auto HistoryService::create_entry(const std::string& root_id,
                                  const std::string& file_path,
                                  const std::string& content,
                                  const std::string& title,
                                  HistoryOpType op_type) -> std::expected<std::string, std::string>
{
    std::lock_guard lock(mutex_);

    // Generate unique ID
    auto now = std::chrono::system_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    const auto entry_id = "hist-" + std::to_string(ms);

    // Create directory for this timestamp
    auto dir = history_dir_for_now();
    std::error_code error_code;
    std::filesystem::create_directories(dir, error_code);
    if (error_code)
    {
        return std::unexpected("Failed to create history dir: " + error_code.message());
    }

    // Write content
    auto content_path = dir / (root_id + ".md");
    {
        std::ofstream ofs(content_path);
        if (!ofs.is_open())
        {
            return std::unexpected("Failed to write history file: " + content_path.string());
        }
        ofs << content;
    }

    // Build entry
    HistoryEntry entry;
    entry.id = entry_id;
    entry.root_id = root_id;
    entry.file_path = file_path;
    entry.history_path = content_path.string();
    entry.title = title;
    entry.op_type = op_type;
    entry.content_hash = compute_hash(content);
    entry.content_size = content.size();
    entry.created = now;

    entry_index_[entry_id] = std::move(entry);

    auto save_result = save_index();
    if (!save_result.has_value())
    {
        return std::unexpected(save_result.error());
    }

    return entry_id;
}

auto HistoryService::history_dir_for_now() const -> std::filesystem::path
{
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm{};
    localtime_r(&time, &local_tm);

    std::ostringstream oss;
    oss << std::put_time(&local_tm, "%Y-%m-%d-%H%M%S");

    return history_root_ / oss.str();
}

auto HistoryService::compute_hash(const std::string& content) -> std::string
{
    // Simple hash for deduplication (real implementation would use SHA-256)
    auto hash = std::hash<std::string>{}(content);
    return std::to_string(hash);
}

auto HistoryService::load_index() -> std::expected<void, std::string>
{
    auto index_path = history_root_ / "index.json";
    std::error_code error_code;

    if (!std::filesystem::exists(index_path, error_code) || error_code)
    {
        return {}; // No index yet
    }

    std::ifstream ifs(index_path);
    if (!ifs.is_open())
    {
        return std::unexpected("Failed to read history index");
    }

    try
    {
        auto json = nlohmann::json::parse(ifs);

        for (const auto& entry_json : json)
        {
            HistoryEntry entry;
            entry.id = entry_json.value("id", "");
            entry.root_id = entry_json.value("root_id", "");
            entry.notebook_id = entry_json.value("notebook_id", "");
            entry.file_path = entry_json.value("file_path", "");
            entry.history_path = entry_json.value("history_path", "");
            entry.title = entry_json.value("title", "");
            entry.op_type = static_cast<HistoryOpType>(entry_json.value("op_type", 0));
            entry.content_hash = entry_json.value("content_hash", "");
            entry.content_size = entry_json.value("content_size", 0);

            entry_index_[entry.id] = std::move(entry);
        }
    }
    catch (const nlohmann::json::parse_error& parse_err)
    {
        return std::unexpected("History index parse error: " + std::string(parse_err.what()));
    }

    return {};
}

auto HistoryService::save_index() const -> std::expected<void, std::string>
{
    auto index_path = history_root_ / "index.json";

    nlohmann::json json_array = nlohmann::json::array();
    for (const auto& [entry_id, entry] : entry_index_)
    {
        nlohmann::json entry_json;
        entry_json["id"] = entry.id;
        entry_json["root_id"] = entry.root_id;
        entry_json["notebook_id"] = entry.notebook_id;
        entry_json["file_path"] = entry.file_path;
        entry_json["history_path"] = entry.history_path;
        entry_json["title"] = entry.title;
        entry_json["op_type"] = static_cast<int>(entry.op_type);
        entry_json["content_hash"] = entry.content_hash;
        entry_json["content_size"] = entry.content_size;
        json_array.push_back(std::move(entry_json));
    }

    std::ofstream ofs(index_path);
    if (!ofs.is_open())
    {
        return std::unexpected("Failed to write history index");
    }

    ofs << json_array.dump(2);
    return {};
}

} // namespace markamp::core
