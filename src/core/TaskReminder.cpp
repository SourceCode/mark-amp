/// @file TaskReminder.cpp
/// @brief V9 Phase 23 – Reminder scheduling implementation.

#include "core/TaskReminder.h"

#include <algorithm>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Public API
// ============================================================================

auto TaskReminder::schedule(const std::string& task_id,
                            std::chrono::system_clock::time_point due_time,
                            ReminderTiming timing) -> std::string
{
    TaskReminderEntry entry;
    entry.reminder_id = generate_id();
    entry.task_id = task_id;
    entry.timing = timing;
    entry.trigger_time = compute_trigger_time(due_time, timing);
    entry.dismissed = false;

    reminders_.push_back(std::move(entry));
    return reminders_.back().reminder_id;
}

auto TaskReminder::schedule_custom(const std::string& task_id,
                                   std::chrono::system_clock::time_point due_time,
                                   int minutes_before) -> std::string
{
    TaskReminderEntry entry;
    entry.reminder_id = generate_id();
    entry.task_id = task_id;
    entry.timing = ReminderTiming::kCustom;
    entry.custom_minutes = minutes_before;
    entry.trigger_time = compute_trigger_time(due_time, ReminderTiming::kCustom, minutes_before);
    entry.dismissed = false;

    reminders_.push_back(std::move(entry));
    return reminders_.back().reminder_id;
}

auto TaskReminder::check_due(std::chrono::system_clock::time_point now) const
    -> std::vector<TaskReminderEntry>
{
    std::vector<TaskReminderEntry> due_reminders;

    for (const auto& entry : reminders_)
    {
        if (entry.dismissed)
        {
            continue;
        }

        // If snoozed, check snoozed_until instead
        if (entry.snoozed_until.has_value())
        {
            if (now >= entry.snoozed_until.value())
            {
                due_reminders.push_back(entry);
            }
            continue;
        }

        if (now >= entry.trigger_time)
        {
            due_reminders.push_back(entry);
        }
    }

    return due_reminders;
}

auto TaskReminder::dismiss(const std::string& reminder_id) -> std::expected<void, std::string>
{
    for (auto& entry : reminders_)
    {
        if (entry.reminder_id == reminder_id)
        {
            entry.dismissed = true;
            return {};
        }
    }
    return std::unexpected("Reminder not found: " + reminder_id);
}

auto TaskReminder::snooze(const std::string& reminder_id, std::chrono::minutes duration)
    -> std::expected<void, std::string>
{
    for (auto& entry : reminders_)
    {
        if (entry.reminder_id == reminder_id)
        {
            auto base_time = entry.snoozed_until.value_or(entry.trigger_time);
            auto now = std::chrono::system_clock::now();
            // Snooze from whichever is later: now or the existing trigger
            auto snooze_from = (now > base_time) ? now : base_time;
            entry.snoozed_until = snooze_from + duration;
            return {};
        }
    }
    return std::unexpected("Reminder not found: " + reminder_id);
}

auto TaskReminder::clear_for_task(const std::string& task_id) -> int
{
    int removed = 0;
    auto iter = std::remove_if(reminders_.begin(),
                               reminders_.end(),
                               [&task_id, &removed](const TaskReminderEntry& entry)
                               {
                                   if (entry.task_id == task_id)
                                   {
                                       ++removed;
                                       return true;
                                   }
                                   return false;
                               });
    reminders_.erase(iter, reminders_.end());
    return removed;
}

auto TaskReminder::active_reminders() const -> std::vector<TaskReminderEntry>
{
    std::vector<TaskReminderEntry> result;
    for (const auto& entry : reminders_)
    {
        if (!entry.dismissed)
        {
            result.push_back(entry);
        }
    }
    return result;
}

auto TaskReminder::count() const -> int
{
    return static_cast<int>(reminders_.size());
}

auto TaskReminder::serialize() const -> std::string
{
    std::ostringstream oss;
    oss << R"({"reminders":[)";

    bool first = true;
    for (const auto& entry : reminders_)
    {
        if (!first)
        {
            oss << ",";
        }
        first = false;

        auto trigger_epoch =
            std::chrono::duration_cast<std::chrono::seconds>(entry.trigger_time.time_since_epoch())
                .count();

        oss << R"({"id":")" << entry.reminder_id << R"(","task_id":")" << entry.task_id
            << R"(","timing":)" << static_cast<int>(entry.timing) << R"(,"trigger_time":)"
            << trigger_epoch << R"(,"dismissed":)" << (entry.dismissed ? "true" : "false")
            << R"(,"custom_minutes":)" << entry.custom_minutes;

        if (entry.snoozed_until.has_value())
        {
            auto snooze_epoch = std::chrono::duration_cast<std::chrono::seconds>(
                                    entry.snoozed_until.value().time_since_epoch())
                                    .count();
            oss << R"(,"snoozed_until":)" << snooze_epoch;
        }

        oss << "}";
    }

    oss << R"(],"next_id":)" << next_id_ << "}";
    return oss.str();
}

auto TaskReminder::deserialize(const std::string& json) -> std::expected<void, std::string>
{
    if (json.empty())
    {
        return std::unexpected("Empty JSON input");
    }

    // Simple JSON array parsing (matches pattern from Phase 22 StudyStreakTracker)
    reminders_.clear();

    // Find next_id
    auto next_id_pos = json.find(R"("next_id":)");
    if (next_id_pos != std::string::npos)
    {
        auto val_start = next_id_pos + 10;
        auto val_end = json.find_first_of(",}", val_start);
        if (val_end != std::string::npos)
        {
            next_id_ = std::stoi(json.substr(val_start, val_end - val_start));
        }
    }

    // Parse individual reminder objects
    auto pos = json.find(R"("reminders":[)");
    if (pos == std::string::npos)
    {
        return std::unexpected("Missing reminders array");
    }

    pos = json.find('[', pos);
    if (pos == std::string::npos)
    {
        return std::unexpected("Malformed JSON");
    }

    // Find each {...} object within the array
    size_t obj_start = json.find('{', pos + 1);
    while (obj_start != std::string::npos)
    {
        size_t obj_end = json.find('}', obj_start);
        if (obj_end == std::string::npos)
        {
            break;
        }

        auto obj = json.substr(obj_start, obj_end - obj_start + 1);
        TaskReminderEntry entry;

        // Extract fields using simple find
        auto extract_string = [&obj](const std::string& key) -> std::string
        {
            auto key_pos = obj.find("\"" + key + "\":\"");
            if (key_pos == std::string::npos)
            {
                return "";
            }
            auto val_s = key_pos + key.size() + 4;
            auto val_e = obj.find('"', val_s);
            return (val_e != std::string::npos) ? obj.substr(val_s, val_e - val_s) : "";
        };

        auto extract_int = [&obj](const std::string& key) -> int
        {
            auto key_pos = obj.find("\"" + key + "\":");
            if (key_pos == std::string::npos)
            {
                return 0;
            }
            auto val_s = key_pos + key.size() + 3;
            auto val_e = obj.find_first_of(",}", val_s);
            return (val_e != std::string::npos) ? std::stoi(obj.substr(val_s, val_e - val_s)) : 0;
        };

        auto extract_long = [&obj](const std::string& key) -> long long
        {
            auto key_pos = obj.find("\"" + key + "\":");
            if (key_pos == std::string::npos)
            {
                return 0;
            }
            auto val_s = key_pos + key.size() + 3;
            auto val_e = obj.find_first_of(",}", val_s);
            return (val_e != std::string::npos) ? std::stoll(obj.substr(val_s, val_e - val_s)) : 0;
        };

        auto extract_bool = [&obj](const std::string& key) -> bool
        {
            auto key_pos = obj.find("\"" + key + "\":");
            if (key_pos == std::string::npos)
            {
                return false;
            }
            auto val_s = key_pos + key.size() + 3;
            return obj.substr(val_s, 4) == "true";
        };

        entry.reminder_id = extract_string("id");
        entry.task_id = extract_string("task_id");
        entry.timing = static_cast<ReminderTiming>(extract_int("timing"));
        entry.custom_minutes = extract_int("custom_minutes");
        entry.dismissed = extract_bool("dismissed");

        auto trigger_epoch = extract_long("trigger_time");
        entry.trigger_time =
            std::chrono::system_clock::time_point{std::chrono::seconds{trigger_epoch}};

        auto snooze_pos = obj.find(R"("snoozed_until":)");
        if (snooze_pos != std::string::npos)
        {
            auto snooze_epoch = extract_long("snoozed_until");
            entry.snoozed_until =
                std::chrono::system_clock::time_point{std::chrono::seconds{snooze_epoch}};
        }

        reminders_.push_back(std::move(entry));

        obj_start = json.find('{', obj_end + 1);
    }

    return {};
}

// ============================================================================
// Private helpers
// ============================================================================

auto TaskReminder::compute_trigger_time(std::chrono::system_clock::time_point due_time,
                                        ReminderTiming timing,
                                        int custom_minutes) -> std::chrono::system_clock::time_point
{
    switch (timing)
    {
        case ReminderTiming::kAtDue:
            return due_time;
        case ReminderTiming::kMinutes15:
            return due_time - std::chrono::minutes{15};
        case ReminderTiming::kHour1:
            return due_time - std::chrono::hours{1};
        case ReminderTiming::kDay1:
            return due_time - std::chrono::hours{24};
        case ReminderTiming::kWeek1:
            return due_time - std::chrono::hours{168}; // 7 * 24
        case ReminderTiming::kCustom:
            return due_time - std::chrono::minutes{custom_minutes};
    }
    return due_time;
}

auto TaskReminder::generate_id() -> std::string
{
    return "reminder_" + std::to_string(next_id_++);
}

} // namespace markamp::core
