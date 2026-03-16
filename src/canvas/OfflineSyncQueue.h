// ============================================================================
// File: src/canvas/OfflineSyncQueue.h
// Phase 13: Canvas Collaboration — offline operation queuing and sync
// ============================================================================
#pragma once

#include "canvas/CanvasTypes.h"

#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Type of queued operation.
enum class QueuedOperationType : uint8_t
{
    kCreate,
    kModify,
    kDelete,
    kMove,
    kResize,
    kStyleChange,
    kConnectorRoute
};

/// A single operation queued while offline.
struct QueuedOperation
{
    std::string operation_id;
    QueuedOperationType type{QueuedOperationType::kModify};
    std::string participant_id;
    ObjectId target_object_id{kInvalidObjectId};
    std::string patch_data;    ///< JSON patch data
    std::string timestamp;     ///< ISO-8601 when queued
    size_t sequence_number{0}; ///< Ordering within queue
    bool is_applied{false};    ///< Has been applied locally

    /// Whether this is a create operation.
    [[nodiscard]] auto is_create() const noexcept -> bool
    {
        return type == QueuedOperationType::kCreate;
    }

    /// Whether this is a delete operation.
    [[nodiscard]] auto is_delete() const noexcept -> bool
    {
        return type == QueuedOperationType::kDelete;
    }

    /// Whether this operation has been applied locally.
    [[nodiscard]] auto is_applied_op() const noexcept -> bool
    {
        return is_applied;
    }

    // ── Round 3 Batch 4-5 (#39-43) ──────────────────────────────

    /// (#39) Whether this is a move operation.
    [[nodiscard]] auto is_move() const noexcept -> bool
    {
        return type == QueuedOperationType::kMove;
    }

    /// (#40) Whether this is a modify operation.
    [[nodiscard]] auto is_modify() const noexcept -> bool
    {
        return type == QueuedOperationType::kModify;
    }

    /// (#41) Whether patch data is present.
    [[nodiscard]] auto has_patch() const noexcept -> bool
    {
        return !patch_data.empty();
    }

    /// (#42) Whether this is a style change operation.
    [[nodiscard]] auto is_style_change() const noexcept -> bool
    {
        return type == QueuedOperationType::kStyleChange;
    }

    /// (#43) Whether this is a resize operation.
    [[nodiscard]] auto is_resize() const noexcept -> bool
    {
        return type == QueuedOperationType::kResize;
    }
};

/// Result of replaying an operation on reconnect.
enum class SyncResult : uint8_t
{
    kApplied,  ///< Applied successfully
    kConflict, ///< Conflict detected, needs resolution
    kSkipped,  ///< Skipped (object no longer exists, etc.)
    kFailed    ///< Failed to apply
};

/// Report for a single operation replay.
struct SyncReport
{
    std::string operation_id;
    SyncResult result{SyncResult::kApplied};
    std::string conflict_description;

    /// Whether the replay resulted in a conflict.
    [[nodiscard]] auto is_conflict() const noexcept -> bool
    {
        return result == SyncResult::kConflict;
    }

    /// Whether the operation was applied successfully.
    [[nodiscard]] auto was_applied() const noexcept -> bool
    {
        return result == SyncResult::kApplied;
    }

    // ── Round 3 Batch 5 (#44-46) ────────────────────────────────

    /// (#44) Whether the operation was skipped.
    [[nodiscard]] auto was_skipped() const noexcept -> bool
    {
        return result == SyncResult::kSkipped;
    }

    /// (#45) Whether the operation failed.
    [[nodiscard]] auto was_failed() const noexcept -> bool
    {
        return result == SyncResult::kFailed;
    }

    /// (#46) Whether a conflict description is present.
    [[nodiscard]] auto has_conflict_desc() const noexcept -> bool
    {
        return !conflict_description.empty();
    }
};

/// Configuration for offline sync behavior.
struct OfflineSyncConfig
{
    size_t max_queue_size{1000};         ///< Maximum queued operations
    bool auto_replay_on_reconnect{true}; ///< Automatically replay on reconnect
    bool persist_queue{true};            ///< Persist queue to disk

    /// Whether queue persistence is enabled.
    [[nodiscard]] auto is_persistent() const noexcept -> bool
    {
        return persist_queue;
    }

    // ── Round 3 Batch 5 (#47) ───────────────────────────────────

    /// (#47) Whether auto-replay on reconnect is enabled.
    [[nodiscard]] auto has_auto_replay() const noexcept -> bool
    {
        return auto_replay_on_reconnect;
    }
};

/// Queues local operations while disconnected and replays them on reconnect.
///
/// Features:
///   - Enqueues create/modify/delete/move operations while offline
///   - Replays queued operations on reconnect with conflict detection
///   - Configurable max queue size and persistence
///   - Merge strategy for conflicting operations
class OfflineSyncQueue
{
public:
    OfflineSyncQueue();
    explicit OfflineSyncQueue(OfflineSyncConfig config);

    // ── Queue Operations ──────────────────────────────────────────

    /// Enqueue an operation for offline sync.
    auto enqueue_operation(QueuedOperationType type,
                           const std::string& participant_id,
                           ObjectId object_id,
                           const std::string& patch_data) -> std::string;

    /// Get all queued operations in order.
    [[nodiscard]] auto queue() const -> const std::vector<QueuedOperation>&;

    /// Number of queued operations.
    [[nodiscard]] auto queue_size() const -> size_t;

    /// Is the queue empty?
    [[nodiscard]] auto is_empty() const -> bool;

    // ── Replay ────────────────────────────────────────────────────

    /// Replay all queued operations (on reconnect).
    /// Returns a report for each operation.
    auto replay_queue() -> std::vector<SyncReport>;

    /// Replay a single operation by ID.
    auto replay_operation(const std::string& operation_id) -> SyncReport;

    // ── Conflict Detection ────────────────────────────────────────

    /// Check a single operation for conflicts against remote state.
    [[nodiscard]] auto detect_conflict(const QueuedOperation& operation) const -> bool;

    /// Set the conflict checker callback (queries remote state).
    using ConflictChecker = std::function<bool(ObjectId object_id, const std::string& patch_data)>;
    auto set_conflict_checker(ConflictChecker checker) -> void;

    // ── Connection State ──────────────────────────────────────────

    /// Mark as offline (start queuing).
    auto go_offline() -> void;

    /// Mark as online (trigger replay if auto-replay is enabled).
    auto go_online() -> std::vector<SyncReport>;

    /// Is currently offline?
    [[nodiscard]] auto is_offline() const -> bool;

    // ── Management ────────────────────────────────────────────────

    /// Clear the queue.
    auto clear() -> void;

    /// Remove a specific operation from the queue.
    auto remove_operation(const std::string& operation_id) -> bool;

    [[nodiscard]] auto config() const -> const OfflineSyncConfig&;
    auto set_config(const OfflineSyncConfig& config) -> void;

    // ── Utility ───────────────────────────────────────────────────

    /// Human-readable operation type name.
    [[nodiscard]] static auto operation_type_name(QueuedOperationType type) -> std::string;

    /// Whether the queue is currently online.
    [[nodiscard]] auto is_online() const noexcept -> bool
    {
        return !offline_;
    }

    /// Whether the queue is at maximum capacity.
    [[nodiscard]] auto is_full() const noexcept -> bool
    {
        return queue_.size() >= config_.max_queue_size;
    }

private:
    OfflineSyncConfig config_;
    std::vector<QueuedOperation> queue_;
    bool offline_{false};
    size_t next_sequence_{1};
    int next_operation_id_{1};
    ConflictChecker conflict_checker_;

    /// Trim queue to max size.
    auto trim_queue() -> void;

    /// Generate current timestamp.
    [[nodiscard]] static auto current_timestamp() -> std::string;
};

} // namespace markamp::canvas
