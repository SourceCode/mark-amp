// ============================================================================
// File: src/canvas/CollabUndoRedoManager.cpp
// Phase 13: Canvas Collaboration — per-user collaborative undo/redo
// ============================================================================
#include "canvas/CollabUndoRedoManager.h"

#include "core/Events.h"

#include <fmt/format.h>

namespace markamp::canvas
{

CollabUndoRedoManager::CollabUndoRedoManager(Board& board, core::EventBus& event_bus)
    : board_(board)
    , event_bus_(event_bus)
{
}

// ── Record Operations ─────────────────────────────────────────────

auto CollabUndoRedoManager::record_operation(const std::string& participant_id,
                                             const std::string& operation_type,
                                             ObjectId object_id,
                                             const std::string& patch_data,
                                             const std::string& reverse_patch_data) -> std::string
{
    CollabOperation operation;
    operation.operation_id = fmt::format("op_{}", next_operation_id_++);
    operation.participant_id = participant_id;
    operation.operation_type = operation_type;
    operation.target_object_id = object_id;
    operation.patch_data = patch_data;
    operation.reverse_patch_data = reverse_patch_data;

    undo_stacks_[participant_id].push_back(operation);
    trim_stack(undo_stacks_[participant_id]);

    // Clear redo stack on new operation
    redo_stacks_[participant_id].clear();

    // Update ownership tracking
    object_owners_[object_id] = participant_id;

    return operation.operation_id;
}

// ── Undo/Redo ─────────────────────────────────────────────────────

auto CollabUndoRedoManager::undo_for_user(const std::string& participant_id) -> CollabUndoResult
{
    auto iter = undo_stacks_.find(participant_id);
    if (iter == undo_stacks_.end() || iter->second.empty())
    {
        return {false, false, "Nothing to undo", {}};
    }

    auto& stack = iter->second;
    auto operation = stack.back();
    stack.pop_back();

    // Check for conflicts
    auto conflicts = check_conflicts(operation);

    operation.is_undone = true;
    redo_stacks_[participant_id].push_back(operation);

    core::events::CanvasCollabUndoRedoEvent evt;
    evt.operation_id = operation.operation_id;
    evt.participant_id = participant_id;
    evt.is_undo = true;
    event_bus_.publish(evt);

    return {true, !conflicts.empty(), "", std::move(conflicts)};
}

auto CollabUndoRedoManager::redo_for_user(const std::string& participant_id) -> CollabUndoResult
{
    auto iter = redo_stacks_.find(participant_id);
    if (iter == redo_stacks_.end() || iter->second.empty())
    {
        return {false, false, "Nothing to redo", {}};
    }

    auto& stack = iter->second;
    auto operation = stack.back();
    stack.pop_back();

    // Check for conflicts
    auto conflicts = check_conflicts(operation);

    operation.is_undone = false;
    undo_stacks_[participant_id].push_back(operation);

    // Update ownership tracking
    object_owners_[operation.target_object_id] = participant_id;

    core::events::CanvasCollabUndoRedoEvent evt;
    evt.operation_id = operation.operation_id;
    evt.participant_id = participant_id;
    evt.is_undo = false;
    event_bus_.publish(evt);

    return {true, !conflicts.empty(), "", std::move(conflicts)};
}

auto CollabUndoRedoManager::can_undo(const std::string& participant_id) const -> bool
{
    auto iter = undo_stacks_.find(participant_id);
    return iter != undo_stacks_.end() && !iter->second.empty();
}

auto CollabUndoRedoManager::can_redo(const std::string& participant_id) const -> bool
{
    auto iter = redo_stacks_.find(participant_id);
    return iter != redo_stacks_.end() && !iter->second.empty();
}

// ── Query ─────────────────────────────────────────────────────────

auto CollabUndoRedoManager::undo_depth(const std::string& participant_id) const -> size_t
{
    auto iter = undo_stacks_.find(participant_id);
    return iter != undo_stacks_.end() ? iter->second.size() : 0;
}

auto CollabUndoRedoManager::redo_depth(const std::string& participant_id) const -> size_t
{
    auto iter = redo_stacks_.find(participant_id);
    return iter != redo_stacks_.end() ? iter->second.size() : 0;
}

auto CollabUndoRedoManager::last_operation(const std::string& participant_id) const
    -> const CollabOperation*
{
    auto iter = undo_stacks_.find(participant_id);
    if (iter != undo_stacks_.end() && !iter->second.empty())
    {
        return &iter->second.back();
    }
    return nullptr;
}

auto CollabUndoRedoManager::object_last_modifier(ObjectId object_id) const -> std::string
{
    auto iter = object_owners_.find(object_id);
    return iter != object_owners_.end() ? iter->second : "";
}

auto CollabUndoRedoManager::total_operations() const -> size_t
{
    size_t total = 0;
    for (const auto& [pid, stack] : undo_stacks_)
    {
        total += stack.size();
    }
    for (const auto& [pid, stack] : redo_stacks_)
    {
        total += stack.size();
    }
    return total;
}

// ── Cleanup ───────────────────────────────────────────────────────

auto CollabUndoRedoManager::clear_participant(const std::string& participant_id) -> void
{
    undo_stacks_.erase(participant_id);
    redo_stacks_.erase(participant_id);
}

auto CollabUndoRedoManager::clear_all() -> void
{
    undo_stacks_.clear();
    redo_stacks_.clear();
    object_owners_.clear();
}

auto CollabUndoRedoManager::set_max_undo_depth(size_t depth) -> void
{
    max_undo_depth_ = depth;
}

auto CollabUndoRedoManager::max_undo_depth() const -> size_t
{
    return max_undo_depth_;
}

// ── Private ───────────────────────────────────────────────────────

auto CollabUndoRedoManager::check_conflicts(const CollabOperation& operation) const
    -> std::vector<UndoConflict>
{
    std::vector<UndoConflict> conflicts;

    auto owner_iter = object_owners_.find(operation.target_object_id);
    if (owner_iter != object_owners_.end() && owner_iter->second != operation.participant_id)
    {
        UndoConflict conflict;
        conflict.operation_id = operation.operation_id;
        conflict.original_participant_id = operation.participant_id;
        conflict.conflicting_participant_id = owner_iter->second;
        conflict.object_id = operation.target_object_id;
        conflict.conflict_description =
            fmt::format("Object {} was modified by {} after this operation",
                        operation.target_object_id,
                        owner_iter->second);
        conflicts.push_back(std::move(conflict));
    }

    return conflicts;
}

auto CollabUndoRedoManager::trim_stack(std::vector<CollabOperation>& stack) -> void
{
    while (stack.size() > max_undo_depth_)
    {
        stack.erase(stack.begin());
    }
}

} // namespace markamp::canvas
