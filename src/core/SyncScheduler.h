// ============================================================================
// File: src/core/SyncScheduler.h
// Phase 27: Cloud Sync & Collaboration — Auto-sync scheduling
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;
class CloudSyncService;

// SyncScheduler — manages auto-sync timing, triggers, and offline queueing.
class SyncScheduler
{
public:
    SyncScheduler(EventBus& event_bus, Config& config);

    // Schedule configuration.
    auto set_interval(SyncScheduleInterval interval) -> void;
    [[nodiscard]] auto interval() const -> SyncScheduleInterval;
    [[nodiscard]] auto interval_minutes() const -> int;

    // Pause/resume auto-sync.
    auto pause() -> void;
    auto resume() -> void;
    [[nodiscard]] auto is_paused() const -> bool;

    // Manual sync trigger.
    auto sync_now() -> void;

    // Sync-on-save debounce.
    auto notify_file_saved(const std::string& file_path) -> void;
    auto set_save_debounce_ms(int64_t debounce_ms) -> void;
    [[nodiscard]] auto save_debounce_ms() const -> int64_t;

    // Sync on workspace close.
    auto sync_on_close() -> void;
    auto set_sync_on_close_enabled(bool enabled) -> void;
    [[nodiscard]] auto sync_on_close_enabled() const -> bool;

    // Online/offline state.
    auto set_online(bool online) -> void;
    [[nodiscard]] auto is_online() const -> bool;

    // Offline queue management.
    auto queue_offline_change(const std::string& file_path) -> void;
    [[nodiscard]] auto pending_count() const -> int32_t;
    [[nodiscard]] auto pending_files() const -> std::vector<std::string>;
    auto drain_offline_queue() -> void;
    auto clear_offline_queue() -> void;

    // Timing info.
    [[nodiscard]] auto last_sync_time() const -> int64_t;
    [[nodiscard]] auto next_sync_time() const -> int64_t;
    auto set_last_sync_time(int64_t timestamp) -> void;

    // Check if a sync should trigger now.
    [[nodiscard]] auto should_sync(int64_t current_time) const -> bool;

    // Sync state for the current cloud config.
    auto set_cloud_config(const CloudSyncConfig& cloud_config) -> void;
    [[nodiscard]] auto has_cloud_config() const -> bool;

    // Bandwidth configuration.
    auto set_bandwidth_config(const BandwidthConfig& bandwidth_config) -> void;
    [[nodiscard]] auto bandwidth_config() const -> const BandwidthConfig&;

private:
    EventBus& event_bus_;
    Config& config_;

    SyncScheduleInterval interval_{SyncScheduleInterval::k30Min};
    bool paused_{false};
    bool online_{true};
    bool sync_on_close_enabled_{true};
    int64_t save_debounce_ms_{30000}; // 30 seconds
    int64_t last_sync_time_{0};
    int64_t last_save_notify_time_{0};

    std::vector<std::string> offline_queue_;
    CloudSyncConfig cloud_config_;
    bool has_cloud_config_{false};
    BandwidthConfig bandwidth_config_;
};

} // namespace markamp::core
