/// @file TaskReminder.h
/// @brief V9 Phase 23 – Reminder scheduling for task due dates.
/// Schedules reminders at configurable offsets before due dates,
/// supports dismiss, snooze, and JSON persistence.

#pragma once

#include <chrono>
#include <cstdint>
#include <expected>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Reminder types
// ============================================================================

enum class ReminderTiming : uint8_t
{
    kAtDue,     ///< At the due time
    kMinutes15, ///< 15 minutes before
    kHour1,     ///< 1 hour before
    kDay1,      ///< 1 day before
    kWeek1,     ///< 1 week before
    kCustom     ///< Custom offset
};

/// A single scheduled reminder entry.
struct TaskReminderEntry
{
    std::string reminder_id;
    std::string task_id;
    std::chrono::system_clock::time_point trigger_time;
    ReminderTiming timing{ReminderTiming::kAtDue};
    bool dismissed{false};
    std::optional<std::chrono::system_clock::time_point> snoozed_until;
    int custom_minutes{0}; ///< For kCustom timing
};

// ============================================================================
// TaskReminder
// ============================================================================

class TaskReminder
{
public:
    TaskReminder() = default;

    /// Schedule a reminder for a task.
    /// Returns the generated reminder ID.
    auto schedule(const std::string& task_id,
                  std::chrono::system_clock::time_point due_time,
                  ReminderTiming timing) -> std::string;

    /// Schedule with custom minutes offset.
    auto schedule_custom(const std::string& task_id,
                         std::chrono::system_clock::time_point due_time,
                         int minutes_before) -> std::string;

    /// Check for reminders due at the given time.
    [[nodiscard]] auto check_due(std::chrono::system_clock::time_point now) const
        -> std::vector<TaskReminderEntry>;

    /// Dismiss a reminder.
    auto dismiss(const std::string& reminder_id) -> std::expected<void, std::string>;

    /// Snooze a reminder for a duration.
    auto snooze(const std::string& reminder_id, std::chrono::minutes duration)
        -> std::expected<void, std::string>;

    /// Clear all reminders for a task.
    auto clear_for_task(const std::string& task_id) -> int;

    /// Get all active (non-dismissed) reminders.
    [[nodiscard]] auto active_reminders() const -> std::vector<TaskReminderEntry>;

    /// Get total reminder count.
    [[nodiscard]] auto count() const -> int;

    /// Serialize to JSON string.
    [[nodiscard]] auto serialize() const -> std::string;

    /// Deserialize from JSON string.
    auto deserialize(const std::string& json) -> std::expected<void, std::string>;

private:
    std::vector<TaskReminderEntry> reminders_;
    int next_id_{1};

    /// Compute trigger time from due time and timing offset.
    [[nodiscard]] static auto compute_trigger_time(std::chrono::system_clock::time_point due_time,
                                                   ReminderTiming timing,
                                                   int custom_minutes = 0)
        -> std::chrono::system_clock::time_point;

    /// Generate a unique reminder ID.
    auto generate_id() -> std::string;
};

} // namespace markamp::core
