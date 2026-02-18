// ============================================================================
// File: src/canvas/OfflineSyncQueue.cpp
// Phase 13: Canvas Collaboration — offline operation queuing and sync
// ============================================================================
#include "canvas/OfflineSyncQueue.h"

#include <algorithm>
#include <chrono>
#include <fmt/format.h>
#include <iomanip>
#include <sstream>

namespace markamp::canvas
{

OfflineSyncQueue::OfflineSyncQueue() = default;

OfflineSyncQueue::OfflineSyncQueue(OfflineSyncConfig config)
    : config_(config)
{
}

// ── Queue Operations ──────────────────────────────────────────────

auto OfflineSyncQueue::enqueue_operation(QueuedOperationType type,
                                         const std::string& participant_id,
                                         ObjectId object_id,
                                         const std::string& patch_data) -> std::string
{
    QueuedOperation queued_op;
    queued_op.operation_id = fmt::format("offline_op_{}", next_operation_id_++);
    queued_op.type = type;
    queued_op.participant_id = participant_id;
    queued_op.target_object_id = object_id;
    queued_op.patch_data = patch_data;
    queued_op.timestamp = current_timestamp();
    queued_op.sequence_number = next_sequence_++;
    queued_op.is_applied = false;

    queue_.push_back(std::move(queued_op));
    trim_queue();

    return queue_.back().operation_id;
}

auto OfflineSyncQueue::queue() const -> const std::vector<QueuedOperation>&
{
    return queue_;
}

auto OfflineSyncQueue::queue_size() const -> size_t
{
    return queue_.size();
}

auto OfflineSyncQueue::is_empty() const -> bool
{
    return queue_.empty();
}

// ── Replay ────────────────────────────────────────────────────────

auto OfflineSyncQueue::replay_queue() -> std::vector<SyncReport>
{
    std::vector<SyncReport> reports;
    reports.reserve(queue_.size());

    for (auto& queued_op : queue_)
    {
        SyncReport report;
        report.operation_id = queued_op.operation_id;

        if (detect_conflict(queued_op))
        {
            report.result = SyncResult::kConflict;
            report.conflict_description = fmt::format("Conflict on object {} for {} operation",
                                                      queued_op.target_object_id,
                                                      operation_type_name(queued_op.type));
        }
        else
        {
            queued_op.is_applied = true;
            report.result = SyncResult::kApplied;
        }

        reports.push_back(std::move(report));
    }

    // Remove applied operations
    std::erase_if(queue_, [](const QueuedOperation& queued_op) { return queued_op.is_applied; });

    return reports;
}

auto OfflineSyncQueue::replay_operation(const std::string& operation_id) -> SyncReport
{
    auto iter = std::ranges::find_if(queue_,
                                     [&](const QueuedOperation& queued_op)
                                     { return queued_op.operation_id == operation_id; });

    if (iter == queue_.end())
    {
        return {operation_id, SyncResult::kSkipped, "Operation not found"};
    }

    SyncReport report;
    report.operation_id = operation_id;

    if (detect_conflict(*iter))
    {
        report.result = SyncResult::kConflict;
        report.conflict_description = "Conflict detected";
    }
    else
    {
        iter->is_applied = true;
        report.result = SyncResult::kApplied;
        queue_.erase(iter);
    }

    return report;
}

// ── Conflict Detection ────────────────────────────────────────────

auto OfflineSyncQueue::detect_conflict(const QueuedOperation& operation) const -> bool
{
    if (conflict_checker_)
    {
        return conflict_checker_(operation.target_object_id, operation.patch_data);
    }
    return false; // No conflict checker — assume no conflicts
}

auto OfflineSyncQueue::set_conflict_checker(ConflictChecker checker) -> void
{
    conflict_checker_ = std::move(checker);
}

// ── Connection State ──────────────────────────────────────────────

auto OfflineSyncQueue::go_offline() -> void
{
    offline_ = true;
}

auto OfflineSyncQueue::go_online() -> std::vector<SyncReport>
{
    offline_ = false;

    if (config_.auto_replay_on_reconnect && !queue_.empty())
    {
        return replay_queue();
    }

    return {};
}

auto OfflineSyncQueue::is_offline() const -> bool
{
    return offline_;
}

// ── Management ────────────────────────────────────────────────────

auto OfflineSyncQueue::clear() -> void
{
    queue_.clear();
}

auto OfflineSyncQueue::remove_operation(const std::string& operation_id) -> bool
{
    auto iter = std::ranges::find_if(queue_,
                                     [&](const QueuedOperation& queued_op)
                                     { return queued_op.operation_id == operation_id; });

    if (iter != queue_.end())
    {
        queue_.erase(iter);
        return true;
    }
    return false;
}

auto OfflineSyncQueue::config() const -> const OfflineSyncConfig&
{
    return config_;
}

auto OfflineSyncQueue::set_config(const OfflineSyncConfig& config) -> void
{
    config_ = config;
}

// ── Utility ───────────────────────────────────────────────────────

auto OfflineSyncQueue::operation_type_name(QueuedOperationType type) -> std::string
{
    switch (type)
    {
        case QueuedOperationType::kCreate:
            return "Create";
        case QueuedOperationType::kModify:
            return "Modify";
        case QueuedOperationType::kDelete:
            return "Delete";
        case QueuedOperationType::kMove:
            return "Move";
        case QueuedOperationType::kResize:
            return "Resize";
        case QueuedOperationType::kStyleChange:
            return "Style Change";
        case QueuedOperationType::kConnectorRoute:
            return "Connector Route";
    }
    return "Unknown";
}

// ── Private ───────────────────────────────────────────────────────

auto OfflineSyncQueue::trim_queue() -> void
{
    while (queue_.size() > config_.max_queue_size)
    {
        queue_.erase(queue_.begin());
    }
}

auto OfflineSyncQueue::current_timestamp() -> std::string
{
    const auto now_time = std::chrono::system_clock::now();
    const auto time_value = std::chrono::system_clock::to_time_t(now_time);
    std::ostringstream oss;
    oss << std::put_time(std::gmtime(&time_value), "%Y-%m-%dT%H:%M:%SZ");
    return oss.str();
}

} // namespace markamp::canvas
