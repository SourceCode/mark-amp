// ============================================================================
// File: src/core/SyncHistoryLogger.h
// Phase 27: Cloud Sync & Collaboration — Sync history tracking
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

// SyncHistoryLogger — records and queries sync operation history.
class SyncHistoryLogger
{
public:
    explicit SyncHistoryLogger(EventBus& event_bus);

    // Log a completed sync operation.
    auto log_sync(const CloudSyncResult& result,
                  const std::string& operation,
                  const std::string& device_name = "") -> void;

    // Log a custom entry.
    auto log_entry(const SyncHistoryEntry& entry) -> void;

    // Query history.
    [[nodiscard]] auto get_history() const -> const std::vector<SyncHistoryEntry>&;
    [[nodiscard]] auto entry_count() const -> int32_t;
    [[nodiscard]] auto is_empty() const -> bool;

    // Get the most recent entry.
    [[nodiscard]] auto latest() const -> std::optional<SyncHistoryEntry>;

    // Search by date range (epoch seconds).
    [[nodiscard]] auto search_by_date(int64_t from_timestamp, int64_t to_timestamp) const
        -> std::vector<SyncHistoryEntry>;

    // Search by operation type ("sync", "push", "pull").
    [[nodiscard]] auto search_by_operation(const std::string& operation) const
        -> std::vector<SyncHistoryEntry>;

    // Search by file name (checks errors for file references).
    [[nodiscard]] auto search_by_file(const std::string& file_name) const
        -> std::vector<SyncHistoryEntry>;

    // Get entries that had errors.
    [[nodiscard]] auto error_entries() const -> std::vector<SyncHistoryEntry>;

    // Get entries for a specific device.
    [[nodiscard]] auto device_entries(const std::string& device_name) const
        -> std::vector<SyncHistoryEntry>;

    // Export history to JSON format.
    [[nodiscard]] auto export_json() const -> std::string;

    // Statistics.
    [[nodiscard]] auto total_files_synced() const -> int64_t;
    [[nodiscard]] auto total_errors() const -> int32_t;

    // Management.
    auto clear() -> void;
    auto set_max_entries(int32_t max_entries) -> void;
    [[nodiscard]] auto max_entries() const -> int32_t;

private:
    [[maybe_unused]] EventBus& event_bus_;

    std::vector<SyncHistoryEntry> history_;
    int32_t max_entries_{200};
    int32_t next_id_{1};

    // Trim history to max_entries_.
    auto trim_history() -> void;
};

} // namespace markamp::core
