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
};

/// Conflict when undoing an operation that was modified by another user.
struct UndoConflict
{
    std::string operation_id;
    std::string original_participant_id;
    std::string conflicting_participant_id;
    ObjectId object_id{kInvalidObjectId};
    std::string conflict_description;
};

/// Result of an undo/redo operation.
struct CollabUndoResult
{
    bool success{false};
    bool had_conflict{false};
    std::string error_message;
    std::vector<UndoConflict> conflicts;
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
