// ============================================================================
// File: src/canvas/CollabEventLogger.h
// Phase 13: Canvas Collaboration — structured event logging
// ============================================================================
#pragma once

#include "core/EventBus.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Category of collaboration event.
enum class CollabEventCategory : uint8_t
{
    kSession,  ///< Join / leave
    kPresence, ///< Cursor / selection updates
    kEdit,     ///< Object create / modify / delete
    kVote,     ///< Vote start / cast / close
    kTimer,    ///< Timer start / stop / expire
    kLock,     ///< Board / region lock
    kReveal,   ///< Private-reveal round events
    kUndo,     ///< Undo / redo operations
    kSystem    ///< System-level events (errors, disconnects)
};

/// A single structured collaboration event log entry.
struct CollabLogEntry
{
    size_t sequence_number{0}; ///< Monotonic sequence
    std::string timestamp;     ///< ISO-8601
    CollabEventCategory category{CollabEventCategory::kSession};
    std::string event_type; ///< e.g. "participant_joined"
    std::string participant_id;
    std::string details; ///< JSON-formatted detail string
    std::string session_id;

    /// Whether this is a session event.
    [[nodiscard]] auto is_session() const noexcept -> bool
    {
        return category == CollabEventCategory::kSession;
    }

    /// Whether this is an edit event.
    [[nodiscard]] auto is_edit() const noexcept -> bool
    {
        return category == CollabEventCategory::kEdit;
    }

    /// Whether this is a system event.
    [[nodiscard]] auto is_system() const noexcept -> bool
    {
        return category == CollabEventCategory::kSystem;
    }

    // ── Round 2 Batch 10 (#99) ──────────────────────────────────

    /// (#99) Whether this is a presence event.
    [[nodiscard]] auto is_presence() const noexcept -> bool
    {
        return category == CollabEventCategory::kPresence;
    }

    /// Whether this is a lock event.
    [[nodiscard]] auto is_lock() const noexcept -> bool
    {
        return category == CollabEventCategory::kLock;
    }
};

/// Configuration for the event logger.
struct CollabLoggerConfig
{
    size_t max_entries{10000};       ///< Maximum log size (ring buffer)
    bool log_presence_events{false}; ///< Presence can be very noisy
    bool include_timestamps{true};

    /// Whether presence events will be logged.
    [[nodiscard]] auto is_presence_enabled() const noexcept -> bool
    {
        return log_presence_events;
    }
};

/// Structured logger for all collaboration events.
///
/// Features:
///   - Logs join/leave, edits, votes, timers, locks, reveals, undo/redo
///   - Configurable ring buffer with max entry limit
///   - Export to JSON for audit/replay
///   - Optional presence event logging (off by default — too noisy)
class CollabEventLogger
{
public:
    CollabEventLogger();
    explicit CollabEventLogger(CollabLoggerConfig config);

    // ── Logging ───────────────────────────────────────────────────

    /// Log an event.
    auto log_event(CollabEventCategory category,
                   const std::string& event_type,
                   const std::string& participant_id,
                   const std::string& details) -> void;

    /// Log a session-level event.
    auto log_session(const std::string& event_type, const std::string& participant_id) -> void;

    /// Log an edit event.
    auto log_edit(const std::string& participant_id,
                  const std::string& object_id,
                  const std::string& edit_type) -> void;

    // ── Query ─────────────────────────────────────────────────────

    /// Get all log entries.
    [[nodiscard]] auto entries() const -> const std::vector<CollabLogEntry>&;

    /// Get entries by category.
    [[nodiscard]] auto entries_by_category(CollabEventCategory category) const
        -> std::vector<const CollabLogEntry*>;

    /// Get entries by participant.
    [[nodiscard]] auto entries_by_participant(const std::string& participant_id) const
        -> std::vector<const CollabLogEntry*>;

    /// Total entry count.
    [[nodiscard]] auto entry_count() const -> size_t;

    /// Last N entries.
    [[nodiscard]] auto recent_entries(size_t count) const -> std::vector<const CollabLogEntry*>;

    // ── Export ────────────────────────────────────────────────────

    /// Export the entire log as a JSON string.
    [[nodiscard]] auto export_json() const -> std::string;

    /// Export entries for replay (chronological order with sequence numbers).
    [[nodiscard]] auto export_for_replay() const -> std::vector<CollabLogEntry>;

    // ── Management ────────────────────────────────────────────────

    /// Clear the log.
    auto clear() -> void;

    /// Set the session ID for all subsequent entries.
    auto set_session_id(const std::string& session_id) -> void;

    [[nodiscard]] auto config() const -> const CollabLoggerConfig&;
    auto set_config(const CollabLoggerConfig& config) -> void;

    // ── Utility ───────────────────────────────────────────────────

    /// Human-readable category name.
    [[nodiscard]] static auto category_name(CollabEventCategory category) -> std::string;

    /// Whether the log is empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return entries_.empty();
    }

    /// Whether the log is at capacity.
    [[nodiscard]] auto is_at_capacity() const noexcept -> bool
    {
        return entries_.size() >= config_.max_entries;
    }

    // ── Round 2 Batch 10 (#100) ─────────────────────────────────

    /// (#100) Whether a session ID has been set.
    [[nodiscard]] auto has_session() const noexcept -> bool
    {
        return !session_id_.empty();
    }

    /// Remaining capacity before the log wraps.
    [[nodiscard]] auto remaining_capacity() const noexcept -> size_t
    {
        return entries_.size() < config_.max_entries
                   ? config_.max_entries - entries_.size()
                   : 0;
    }

private:
    CollabLoggerConfig config_;
    std::vector<CollabLogEntry> entries_;
    std::string session_id_;
    size_t next_sequence_{1};

    /// Trim log to max_entries (ring buffer behavior).
    auto trim_log() -> void;

    /// Generate current ISO-8601 timestamp string.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::canvas
