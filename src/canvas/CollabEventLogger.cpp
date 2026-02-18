// ============================================================================
// File: src/canvas/CollabEventLogger.cpp
// Phase 13: Canvas Collaboration — structured event logging
// ============================================================================
#include "canvas/CollabEventLogger.h"

#include <algorithm>
#include <chrono>
#include <fmt/format.h>
#include <iomanip>
#include <sstream>

namespace markamp::canvas
{

CollabEventLogger::CollabEventLogger() = default;

CollabEventLogger::CollabEventLogger(CollabLoggerConfig config)
    : config_(config)
{
}

// ── Logging ───────────────────────────────────────────────────────

auto CollabEventLogger::log_event(CollabEventCategory category,
                                  const std::string& event_type,
                                  const std::string& participant_id,
                                  const std::string& details) -> void
{
    // Skip presence events if not configured to log them
    if (category == CollabEventCategory::kPresence && !config_.log_presence_events)
    {
        return;
    }

    CollabLogEntry entry;
    entry.sequence_number = next_sequence_++;
    entry.timestamp = config_.include_timestamps ? current_timestamp() : "";
    entry.category = category;
    entry.event_type = event_type;
    entry.participant_id = participant_id;
    entry.details = details;
    entry.session_id = session_id_;

    entries_.push_back(std::move(entry));
    trim_log();
}

auto CollabEventLogger::log_session(const std::string& event_type,
                                    const std::string& participant_id) -> void
{
    log_event(CollabEventCategory::kSession, event_type, participant_id, "");
}

auto CollabEventLogger::log_edit(const std::string& participant_id,
                                 const std::string& object_id,
                                 const std::string& edit_type) -> void
{
    const auto detail_json =
        fmt::format(R"({{"object_id":"{}","edit_type":"{}"}})", object_id, edit_type);
    log_event(CollabEventCategory::kEdit, "object_edit", participant_id, detail_json);
}

// ── Query ─────────────────────────────────────────────────────────

auto CollabEventLogger::entries() const -> const std::vector<CollabLogEntry>&
{
    return entries_;
}

auto CollabEventLogger::entries_by_category(CollabEventCategory category) const
    -> std::vector<const CollabLogEntry*>
{
    std::vector<const CollabLogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.category == category)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto CollabEventLogger::entries_by_participant(const std::string& participant_id) const
    -> std::vector<const CollabLogEntry*>
{
    std::vector<const CollabLogEntry*> result;
    for (const auto& entry : entries_)
    {
        if (entry.participant_id == participant_id)
        {
            result.push_back(&entry);
        }
    }
    return result;
}

auto CollabEventLogger::entry_count() const -> size_t
{
    return entries_.size();
}

auto CollabEventLogger::recent_entries(size_t count) const -> std::vector<const CollabLogEntry*>
{
    std::vector<const CollabLogEntry*> result;
    const size_t start_idx = entries_.size() > count ? entries_.size() - count : 0;
    for (size_t idx = start_idx; idx < entries_.size(); ++idx)
    {
        result.push_back(&entries_[idx]);
    }
    return result;
}

// ── Export ─────────────────────────────────────────────────────────

auto CollabEventLogger::export_json() const -> std::string
{
    std::string json = "[";
    for (size_t idx = 0; idx < entries_.size(); ++idx)
    {
        const auto& entry = entries_[idx];
        if (idx > 0)
        {
            json += ",";
        }
        json += fmt::format(
            R"({{"seq":{},"ts":"{}","cat":"{}","type":"{}","pid":"{}","details":"{}"}})",
            entry.sequence_number,
            entry.timestamp,
            category_name(entry.category),
            entry.event_type,
            entry.participant_id,
            entry.details);
    }
    json += "]";
    return json;
}

auto CollabEventLogger::export_for_replay() const -> std::vector<CollabLogEntry>
{
    auto sorted = entries_;
    std::ranges::sort(sorted,
                      [](const CollabLogEntry& lhs, const CollabLogEntry& rhs)
                      { return lhs.sequence_number < rhs.sequence_number; });
    return sorted;
}

// ── Management ────────────────────────────────────────────────────

auto CollabEventLogger::clear() -> void
{
    entries_.clear();
}

auto CollabEventLogger::set_session_id(const std::string& session_id) -> void
{
    session_id_ = session_id;
}

auto CollabEventLogger::config() const -> const CollabLoggerConfig&
{
    return config_;
}

auto CollabEventLogger::set_config(const CollabLoggerConfig& config) -> void
{
    config_ = config;
}

// ── Utility ───────────────────────────────────────────────────────

auto CollabEventLogger::category_name(CollabEventCategory category) -> std::string
{
    switch (category)
    {
        case CollabEventCategory::kSession:
            return "session";
        case CollabEventCategory::kPresence:
            return "presence";
        case CollabEventCategory::kEdit:
            return "edit";
        case CollabEventCategory::kVote:
            return "vote";
        case CollabEventCategory::kTimer:
            return "timer";
        case CollabEventCategory::kLock:
            return "lock";
        case CollabEventCategory::kReveal:
            return "reveal";
        case CollabEventCategory::kUndo:
            return "undo";
        case CollabEventCategory::kSystem:
            return "system";
    }
    return "unknown";
}

// ── Private ───────────────────────────────────────────────────────

auto CollabEventLogger::trim_log() -> void
{
    while (entries_.size() > config_.max_entries)
    {
        entries_.erase(entries_.begin());
    }
}

auto CollabEventLogger::current_timestamp() -> std::string
{
    const auto now_time = std::chrono::system_clock::now();
    const auto time_value = std::chrono::system_clock::to_time_t(now_time);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_value), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

} // namespace markamp::canvas
