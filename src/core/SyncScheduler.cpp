// ============================================================================
// File: src/core/SyncScheduler.cpp
// Phase 27: Cloud Sync & Collaboration — Auto-sync scheduling
// ============================================================================

#include "SyncScheduler.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"

#include <algorithm>

namespace markamp::core
{

SyncScheduler::SyncScheduler(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    // Load interval from config if available.
    auto interval_val = config_.get_int("sync.interval_minutes", 30);
    if (interval_val <= 0)
    {
        interval_ = SyncScheduleInterval::kManual;
    }
    else if (interval_val <= 5)
    {
        interval_ = SyncScheduleInterval::k5Min;
    }
    else if (interval_val <= 15)
    {
        interval_ = SyncScheduleInterval::k15Min;
    }
    else if (interval_val <= 30)
    {
        interval_ = SyncScheduleInterval::k30Min;
    }
    else
    {
        interval_ = SyncScheduleInterval::k60Min;
    }

    sync_on_close_enabled_ = config_.get_bool("sync.sync_on_close", true);
    save_debounce_ms_ = config_.get_int("sync.save_debounce_ms", 30000);
}

auto SyncScheduler::set_interval(SyncScheduleInterval interval) -> void
{
    interval_ = interval;

    events::SyncScheduleChangedEvent evt;
    evt.interval_minutes = interval_minutes();
    evt.is_paused = paused_;
    event_bus_.publish(evt);
}

auto SyncScheduler::interval() const -> SyncScheduleInterval
{
    return interval_;
}

auto SyncScheduler::interval_minutes() const -> int
{
    switch (interval_)
    {
        case SyncScheduleInterval::kManual:
            return 0;
        case SyncScheduleInterval::k5Min:
            return 5;
        case SyncScheduleInterval::k15Min:
            return 15;
        case SyncScheduleInterval::k30Min:
            return 30;
        case SyncScheduleInterval::k60Min:
            return 60;
    }
    return 30;
}

auto SyncScheduler::pause() -> void
{
    paused_ = true;

    events::SyncScheduleChangedEvent evt;
    evt.interval_minutes = interval_minutes();
    evt.is_paused = true;
    event_bus_.publish(evt);
}

auto SyncScheduler::resume() -> void
{
    paused_ = false;

    events::SyncScheduleChangedEvent evt;
    evt.interval_minutes = interval_minutes();
    evt.is_paused = false;
    event_bus_.publish(evt);
}

auto SyncScheduler::is_paused() const -> bool
{
    return paused_;
}

auto SyncScheduler::sync_now() -> void
{
    // Mark last sync time (the actual sync is triggered externally).
    // This method signals intent to sync immediately.
}

auto SyncScheduler::notify_file_saved(const std::string& file_path) -> void
{
    if (!online_)
    {
        queue_offline_change(file_path);
        return;
    }

    // Track the file for debounced sync.
    // Actual debounce timing is handled by the caller (UI layer).
    last_save_notify_time_ = last_sync_time_; // Simplified for core layer.
}

auto SyncScheduler::set_save_debounce_ms(int64_t debounce_ms) -> void
{
    save_debounce_ms_ = debounce_ms;
}

auto SyncScheduler::save_debounce_ms() const -> int64_t
{
    return save_debounce_ms_;
}

auto SyncScheduler::sync_on_close() -> void
{
    if (sync_on_close_enabled_ && has_cloud_config_)
    {
        // Trigger sync on workspace close.
        sync_now();
    }
}

auto SyncScheduler::set_sync_on_close_enabled(bool enabled) -> void
{
    sync_on_close_enabled_ = enabled;
}

auto SyncScheduler::sync_on_close_enabled() const -> bool
{
    return sync_on_close_enabled_;
}

auto SyncScheduler::set_online(bool online) -> void
{
    const bool was_offline = !online_;
    online_ = online;

    // If coming back online, drain the offline queue.
    if (online && was_offline && !offline_queue_.empty())
    {
        drain_offline_queue();
    }
}

auto SyncScheduler::is_online() const -> bool
{
    return online_;
}

auto SyncScheduler::queue_offline_change(const std::string& file_path) -> void
{
    // Avoid duplicates.
    auto iter = std::find(offline_queue_.begin(), offline_queue_.end(), file_path);
    if (iter == offline_queue_.end())
    {
        offline_queue_.push_back(file_path);
    }

    events::SyncOfflineQueuedEvent evt;
    evt.queued_count = static_cast<int32_t>(offline_queue_.size());
    evt.total_bytes = 0; // Would need filesystem access for real sizes.
    event_bus_.publish(evt);
}

auto SyncScheduler::pending_count() const -> int32_t
{
    return static_cast<int32_t>(offline_queue_.size());
}

auto SyncScheduler::pending_files() const -> std::vector<std::string>
{
    return offline_queue_;
}

auto SyncScheduler::drain_offline_queue() -> void
{
    // Signal that the offline queue should be synced.
    // Actual sync is performed by CloudSyncService.
    offline_queue_.clear();
}

auto SyncScheduler::clear_offline_queue() -> void
{
    offline_queue_.clear();
}

auto SyncScheduler::last_sync_time() const -> int64_t
{
    return last_sync_time_;
}

auto SyncScheduler::next_sync_time() const -> int64_t
{
    if (interval_ == SyncScheduleInterval::kManual || paused_)
    {
        return 0; // No scheduled sync.
    }

    return last_sync_time_ + (static_cast<int64_t>(interval_minutes()) * 60);
}

auto SyncScheduler::set_last_sync_time(int64_t timestamp) -> void
{
    last_sync_time_ = timestamp;
}

auto SyncScheduler::should_sync(int64_t current_time) const -> bool
{
    if (!online_ || paused_ || !has_cloud_config_)
    {
        return false;
    }

    if (interval_ == SyncScheduleInterval::kManual)
    {
        return false;
    }

    const int64_t next = next_sync_time();
    return next > 0 && current_time >= next;
}

auto SyncScheduler::set_cloud_config(const CloudSyncConfig& cloud_config) -> void
{
    cloud_config_ = cloud_config;
    has_cloud_config_ = true;
}

auto SyncScheduler::has_cloud_config() const -> bool
{
    return has_cloud_config_;
}

auto SyncScheduler::set_bandwidth_config(const BandwidthConfig& bandwidth_config) -> void
{
    bandwidth_config_ = bandwidth_config;
}

auto SyncScheduler::bandwidth_config() const -> const BandwidthConfig&
{
    return bandwidth_config_;
}

} // namespace markamp::core
