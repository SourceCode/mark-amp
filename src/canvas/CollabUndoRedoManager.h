// ============================================================================
// File: src/canvas/CollabUndoRedoManager.h
// Phase 13: Canvas Collaboration — per-user collaborative undo/redo
// ============================================================================
#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// A single undoable operation in collaborative context.
struct CollabOperation
{
    std::string operation_id;
    std::string participant_id; ///< Who performed this operation
    std::string operation_type; ///< "create", "delete", "move", "modify"
    ObjectId target_object_id{kInvalidObjectId};
    std::string patch_data;         ///< JSON patch for undo/redo
    std::string reverse_patch_data; ///< JSON patch for reverse (undo)
    bool is_undone{false};

    /// Whether this operation has been undone.
    [[nodiscard]] auto is_undone_op() const noexcept -> bool
    {
        return is_undone;
    }

    /// Whether this is a create operation.
    [[nodiscard]] auto is_create() const noexcept -> bool
    {
        return operation_type == "create";
    }

    // ── Round 3 Batch 1 (#1-5) ──────────────────────────────────

    /// (#1) Whether this is a delete operation.
    [[nodiscard]] auto is_delete() const noexcept -> bool
    {
        return operation_type == "delete";
    }

    /// (#2) Whether this is a modify operation.
    [[nodiscard]] auto is_modify() const noexcept -> bool
    {
        return operation_type == "modify";
    }

    /// (#3) Whether this is a move operation.
    [[nodiscard]] auto is_move() const noexcept -> bool
    {
        return operation_type == "move";
    }

    /// (#4) Whether patch data is present.
    [[nodiscard]] auto has_patch() const noexcept -> bool
    {
        return !patch_data.empty();
    }

    /// (#5) Whether reverse patch data is present.
    [[nodiscard]] auto has_reverse_patch() const noexcept -> bool
    {
        return !reverse_patch_data.empty();
    }
};

/// Conflict when undoing an operation that was modified by another user.
struct UndoConflict
{
    std::string operation_id;
    std::string original_participant_id;
    std::string conflicting_participant_id;
    ObjectId object_id{kInvalidObjectId};
    std::string conflict_description;

    /// Whether the conflict involves a specific object.
    [[nodiscard]] auto involves_object(ObjectId obj_id) const noexcept -> bool
    {
        return object_id == obj_id;
    }

    // ── Round 3 Batch 1 (#6-7) ──────────────────────────────────

    /// (#6) Whether a conflict description is provided.
    [[nodiscard]] auto has_description() const noexcept -> bool
    {
        return !conflict_description.empty();
    }

    /// (#7) Whether the conflict is between the same user.
    [[nodiscard]] auto is_same_user() const noexcept -> bool
    {
        return original_participant_id == conflicting_participant_id;
    }
};

/// Result of an undo/redo operation.
struct CollabUndoResult
{
    bool success{false};
    bool had_conflict{false};
    std::string error_message;
    std::vector<UndoConflict> conflicts;

    /// Whether the undo/redo had conflicts.
    [[nodiscard]] auto had_conflicts() const noexcept -> bool
    {
        return had_conflict;
    }

    /// Whether the result was clean (success, no conflicts).
    [[nodiscard]] auto is_clean() const noexcept -> bool
    {
        return success && !had_conflict;
    }

    // ── Round 3 Batch 1 (#8-10) ─────────────────────────────────

    /// (#8) Whether the undo/redo failed.
    [[nodiscard]] auto failed() const noexcept -> bool
    {
        return !success;
    }

    /// (#9) Whether there's an error message.
    [[nodiscard]] auto has_error() const noexcept -> bool
    {
        return !error_message.empty();
    }

    /// (#10) Number of conflicts encountered.
    [[nodiscard]] auto conflict_count() const noexcept -> size_t
    {
        return conflicts.size();
    }
};

/// Per-user collaborative undo/redo management.
///
/// Features:
///   - Per-user undo/redo stacks (each user only undoes their own actions)
///   - Conflict detection when undoing operations on objects modified by others
///   - Broadcasts undo/redo as patches for remote sync
///   - Object ownership tracking for conflict resolution
class CollabUndoRedoManager
{
public:
    CollabUndoRedoManager(Board& board, core::EventBus& event_bus);

    // ── Record Operations ─────────────────────────────────────────

    /// Record a new operation performed by a participant.
    auto record_operation(const std::string& participant_id,
                          const std::string& operation_type,
                          ObjectId object_id,
                          const std::string& patch_data,
                          const std::string& reverse_patch_data) -> std::string;

    // ── Undo/Redo ─────────────────────────────────────────────────

    /// Undo the last operation by the given participant.
    auto undo_for_user(const std::string& participant_id) -> CollabUndoResult;

    /// Redo the last undone operation by the given participant.
    auto redo_for_user(const std::string& participant_id) -> CollabUndoResult;

    /// Can the participant undo?
    [[nodiscard]] auto can_undo(const std::string& participant_id) const -> bool;

    /// Can the participant redo?
    [[nodiscard]] auto can_redo(const std::string& participant_id) const -> bool;

    // ── Query ─────────────────────────────────────────────────────

    /// Get the undo stack depth for a participant.
    [[nodiscard]] auto undo_depth(const std::string& participant_id) const -> size_t;

    /// Get the redo stack depth for a participant.
    [[nodiscard]] auto redo_depth(const std::string& participant_id) const -> size_t;

    /// Get the last operation by a participant (for display).
    [[nodiscard]] auto last_operation(const std::string& participant_id) const
        -> const CollabOperation*;

    /// Get the participant who last modified an object (ownership tracking).
    [[nodiscard]] auto object_last_modifier(ObjectId object_id) const -> std::string;

    /// Total operations across all participants.
    [[nodiscard]] auto total_operations() const -> size_t;

    // ── Cleanup ───────────────────────────────────────────────────

    /// Clear all stacks for a participant (when they leave).
    auto clear_participant(const std::string& participant_id) -> void;

    /// Clear all operation history.
    auto clear_all() -> void;

    /// Set maximum undo stack depth per user.
    auto set_max_undo_depth(size_t depth) -> void;
    [[nodiscard]] auto max_undo_depth() const -> size_t;

    /// Number of participants with active undo stacks.
    [[nodiscard]] auto participant_count() const noexcept -> std::size_t
    {
        return undo_stacks_.size();
    }

    /// Whether any operations have been recorded.
    [[nodiscard]] auto has_history() const noexcept -> bool
    {
        return !undo_stacks_.empty();
    }

private:
    [[maybe_unused]] Board& board_;
    core::EventBus& event_bus_;
    size_t max_undo_depth_{50};
    int next_operation_id_{1};

    /// Per-participant undo stacks (operations not yet undone).
    std::unordered_map<std::string, std::vector<CollabOperation>> undo_stacks_;

    /// Per-participant redo stacks (operations that were undone).
    std::unordered_map<std::string, std::vector<CollabOperation>> redo_stacks_;

    /// Object → last modifier mapping for conflict detection.
    std::unordered_map<ObjectId, std::string> object_owners_;

    /// Check for conflicts before undoing.
    [[nodiscard]] auto check_conflicts(const CollabOperation& operation) const
        -> std::vector<UndoConflict>;

    /// Trim undo stack to max depth.
    auto trim_stack(std::vector<CollabOperation>& stack) -> void;
};

} // namespace markamp::canvas
