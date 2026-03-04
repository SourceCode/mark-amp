#pragma once

/**
 * @file ScreenReaderAnnouncer.h
 * @brief Phase 35 Task 2: Screen reader state announcement queue.
 *
 * Announces meaningful state changes and action outcomes with
 * polite/assertive priority, deduplication, and throttling.
 */

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Announcement priority.
enum class AnnouncementPriority : uint8_t
{
    kPolite,    ///< Queued, spoken when idle
    kAssertive, ///< Interrupts current speech
};

/// A single announcement.
struct Announcement
{
    std::string message;
    AnnouncementPriority priority{AnnouncementPriority::kPolite};
    int timestamp_ms{0};

    /// Get priority as string.
    [[nodiscard]] auto priority_name() const -> std::string;
};

/**
 * @brief Manages screen reader announcements with dedup and throttle.
 */
class ScreenReaderAnnouncer
{
public:
    ScreenReaderAnnouncer() = default;

    // ── Announce ───────────────────────────────────────────────────

    /// Queue an announcement.
    void announce(const std::string& message,
                  AnnouncementPriority priority = AnnouncementPriority::kPolite);

    /// Queue an assertive announcement (interrupts).
    void announce_assertive(const std::string& message);

    // ── Queue management ───────────────────────────────────────────

    /// Get all pending announcements.
    [[nodiscard]] auto pending() const -> const std::vector<Announcement>&;

    /// Get the number of pending announcements.
    [[nodiscard]] auto pending_count() const -> int;

    /// Pop the next announcement to process.
    auto pop_next() -> Announcement;

    /// Clear all pending announcements.
    void clear();

    // ── Throttling ─────────────────────────────────────────────────

    /// Set the minimum interval between announcements (ms).
    void set_throttle_ms(int interval_ms);

    /// Get the throttle interval.
    [[nodiscard]] auto throttle_ms() const -> int;

    // ── Dedup ──────────────────────────────────────────────────────

    /// Enable/disable deduplication of consecutive identical messages.
    void set_dedup_enabled(bool enabled);

    /// Check if dedup is enabled.
    [[nodiscard]] auto is_dedup_enabled() const -> bool;

    // ── History ────────────────────────────────────────────────────

    /// Get the last N announcements that were processed.
    [[nodiscard]] auto history(int max_count) const -> std::vector<Announcement>;

    /// Get total announcements ever made.
    [[nodiscard]] auto total_announced() const -> int;

private:
    std::vector<Announcement> queue_;
    std::vector<Announcement> history_;
    int throttle_ms_{0};
    bool dedup_enabled_{true};
    int current_time_ms_{0};
    int total_announced_{0};
};

} // namespace markamp::ui
