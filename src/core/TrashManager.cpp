/// @file TrashManager.cpp
/// @brief Phase 20 – Soft-delete trash manager implementation.

#include "core/TrashManager.h"

#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <filesystem>

namespace markamp::core
{

// ============================================================================
// Construction
// ============================================================================

TrashManager::TrashManager(EventBus& event_bus)
    : event_bus_(event_bus)
{
    config_.trash_directory = ".markamp/trash";
}

// ============================================================================
// Trash Operations
// ============================================================================

auto TrashManager::trash(const std::string& path) -> std::expected<std::string, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Empty path");
    }

    auto trash_id = generate_id();

    TrashedItem item;
    item.id = trash_id;
    item.original_path = path;
    item.trash_path = config_.trash_directory + "/" + trash_id;
    item.trashed_at = current_timestamp();
    item.size_bytes = 0; // In production, would compute actual file size.
    item.is_directory = false;

    items_[trash_id] = std::move(item);

    // Publish event.
    events::FilesTrashedEvent evt;
    evt.paths = {path};
    event_bus_.publish(evt);

    return trash_id;
}

auto TrashManager::restore(const std::string& trash_id) -> std::expected<void, std::string>
{
    auto iter = items_.find(trash_id);
    if (iter == items_.end())
    {
        return std::unexpected("Trashed item not found: " + trash_id);
    }

    auto original_path = iter->second.original_path;
    auto trash_path = iter->second.trash_path;

    // Attempt filesystem restore only if the trash file actually exists on disk.
    std::error_code error_code;
    if (!trash_path.empty() && !original_path.empty() &&
        std::filesystem::exists(trash_path, error_code))
    {
        // Create parent directories if they were deleted.
        auto parent_path = std::filesystem::path(original_path).parent_path();
        if (!parent_path.empty())
        {
            std::filesystem::create_directories(parent_path, error_code);
            // Non-fatal if directory creation fails (directory may already exist).
        }

        // Move file from trash to original location.
        std::filesystem::rename(trash_path, original_path, error_code);
        if (error_code)
        {
            // If rename fails (e.g., cross-device), try copy + remove.
            std::filesystem::copy(trash_path, original_path,
                                  std::filesystem::copy_options::overwrite_existing, error_code);
            if (error_code)
            {
                return std::unexpected("Failed to restore file: " + error_code.message());
            }
            std::filesystem::remove_all(trash_path, error_code);
            // Non-fatal if cleanup fails.
        }
    }

    items_.erase(iter);

    // Publish event.
    events::FilesRestoredEvent evt;
    evt.paths = {original_path};
    event_bus_.publish(evt);

    return {};
}

auto TrashManager::permanent_delete(const std::string& trash_id) -> bool
{
    return items_.erase(trash_id) > 0;
}

// ============================================================================
// Listing
// ============================================================================

auto TrashManager::list_trashed() const -> std::vector<TrashedItem>
{
    std::vector<TrashedItem> result;
    result.reserve(items_.size());
    for (const auto& [trash_id, item] : items_)
    {
        result.push_back(item);
    }
    // Sort newest first.
    std::sort(result.begin(),
              result.end(),
              [](const TrashedItem& item_a, const TrashedItem& item_b)
              { return item_a.trashed_at > item_b.trashed_at; });
    return result;
}

auto TrashManager::find_by_original_path(const std::string& path) const
    -> std::optional<TrashedItem>
{
    for (const auto& [trash_id, item] : items_)
    {
        if (item.original_path == path)
        {
            return item;
        }
    }
    return std::nullopt;
}

auto TrashManager::find_by_id(const std::string& trash_id) const -> std::optional<TrashedItem>
{
    auto iter = items_.find(trash_id);
    if (iter != items_.end())
    {
        return iter->second;
    }
    return std::nullopt;
}

// ============================================================================
// Bulk Operations
// ============================================================================

auto TrashManager::empty_trash() -> int
{
    auto count = static_cast<int>(items_.size());
    items_.clear();
    return count;
}

auto TrashManager::auto_purge_older_than(int days) -> int
{
    if (days <= 0)
    {
        return 0;
    }

    // Simple age-based purge using timestamp comparison.
    auto cutoff = current_timestamp(); // Simplified; would compute actual cutoff in production.
    int purged = 0;

    std::vector<std::string> to_remove;
    for (const auto& [trash_id, item] : items_)
    {
        // In production, would parse timestamps and compare.
        // For now, this is a structural placeholder.
        if (item.trashed_at < cutoff)
        {
            to_remove.push_back(trash_id);
        }
    }

    for (const auto& remove_id : to_remove)
    {
        items_.erase(remove_id);
        ++purged;
    }

    return purged;
}

// ============================================================================
// Statistics
// ============================================================================

auto TrashManager::trashed_count() const -> std::size_t
{
    return items_.size();
}

auto TrashManager::total_trash_size() const -> std::uint64_t
{
    std::uint64_t total = 0;
    for (const auto& [trash_id, item] : items_)
    {
        total += item.size_bytes;
    }
    return total;
}

// ============================================================================
// Configuration
// ============================================================================

void TrashManager::set_config(const TrashConfig& new_config)
{
    config_ = new_config;
}

auto TrashManager::config() const -> const TrashConfig&
{
    return config_;
}

void TrashManager::clear()
{
    items_.clear();
}

// ============================================================================
// Private Helpers
// ============================================================================

auto TrashManager::generate_id() -> std::string
{
    return "trash-" + std::to_string(next_id_++);
}

auto TrashManager::current_timestamp() -> std::string
{
    auto now = std::chrono::system_clock::now();
    auto time_t_now = std::chrono::system_clock::to_time_t(now);
    std::array<char, 32> buffer{};
    struct tm time_info
    {
    };
    gmtime_r(&time_t_now, &time_info);
    std::strftime(buffer.data(), buffer.size(), "%Y-%m-%dT%H:%M:%SZ", &time_info);
    return {buffer.data()};
}

} // namespace markamp::core
