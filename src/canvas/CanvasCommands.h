#pragma once

#include "canvas/Board.h"
#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/UndoRedoStack.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Add an object to a board.
class AddObjectCommand : public ICanvasCommand
{
public:
    AddObjectCommand(Board& board, std::unique_ptr<CanvasObject> obj);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    std::unique_ptr<CanvasObject> object_;
    ObjectId added_id_{kInvalidObjectId};
    bool executed_{false};
};

/// Remove an object from a board.
class RemoveObjectCommand : public ICanvasCommand
{
public:
    RemoveObjectCommand(Board& board, ObjectId obj_id);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    ObjectId target_id_;
    std::unique_ptr<CanvasObject> removed_object_;
};

/// Move objects by a delta.
class MoveObjectsCommand : public ICanvasCommand
{
public:
    MoveObjectsCommand(Board& board, std::vector<ObjectId> ids, double delta_x, double delta_y);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;
    [[nodiscard]] auto can_merge_with(const ICanvasCommand& other) const -> bool override;
    auto merge_with(const ICanvasCommand& other) -> bool override;

private:
    Board& board_;
    std::vector<ObjectId> target_ids_;
    double delta_x_;
    double delta_y_;

public:
    // ── Batch 9 (#89-90) ──────────────────────────────────────────

    /// (#89) Number of target objects in this move.
    [[nodiscard]] auto target_count() const noexcept -> size_t
    {
        return target_ids_.size();
    }

    /// (#90) Movement delta as a Point2D.
    [[nodiscard]] auto delta() const noexcept -> Point2D
    {
        return {delta_x_, delta_y_};
    }
};

/// Compound command: groups multiple commands into one undo/redo unit.
class CompoundCommand : public ICanvasCommand
{
public:
    explicit CompoundCommand(std::string desc);

    auto add(std::unique_ptr<ICanvasCommand> cmd) -> void;
    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;
    [[nodiscard]] auto command_count() const -> size_t;

private:
    std::string description_;
    std::vector<std::unique_ptr<ICanvasCommand>> commands_;

public:
    // ── Batch 9 (#86-87) ──────────────────────────────────────────

    /// (#86) Whether the compound command has no sub-commands.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return commands_.empty();
    }

    /// (#87) Whether the compound command has any sub-commands.
    [[nodiscard]] auto has_commands() const noexcept -> bool
    {
        return !commands_.empty();
    }
};

/// Change the z-index of an object.
class ReorderZCommand : public ICanvasCommand
{
public:
    enum class Direction : uint8_t
    {
        BringToFront,
        SendToBack,
        BringForward,
        SendBackward
    };

    ReorderZCommand(Board& board, ObjectId obj_id, Direction dir);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    ObjectId target_id_;
    Direction direction_;
    int previous_z_{0};
};

// ── V8 Phase 7: Collaborative remote patch command ─────────────────

/// Applies a serialized remote patch received from another participant.
/// Marked as remote so the UndoRedoStack separates it from local history.
class RemotePatchCommand : public ICanvasCommand
{
public:
    RemotePatchCommand(Board& board,
                       ObjectId obj_id,
                       std::string patch_type,
                       std::string patch_json,
                       std::string participant_id);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

    [[nodiscard]] auto patch_type() const -> const std::string&;
    [[nodiscard]] auto patch_json() const -> const std::string&;
    [[nodiscard]] auto source_participant() const -> const std::string&;

private:
    Board& board_;
    ObjectId target_id_;
    std::string patch_type_;
    std::string patch_json_;
    std::string participant_id_;
    std::string previous_state_; ///< Snapshot for undo
};

// ── Group Command (#30) ────────────────────────────────────────────

/// Groups selected objects under a parent, or ungroups them on undo.
class GroupObjectsCommand : public ICanvasCommand
{
public:
    GroupObjectsCommand(Board& board, std::vector<ObjectId> child_ids);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

    [[nodiscard]] auto group_id() const -> ObjectId;

private:
    Board& board_;
    std::vector<ObjectId> child_ids_;
    ObjectId group_id_{kInvalidObjectId};
    std::vector<ObjectId> previous_parents_;
};

// ── Batch 10 (#58-60) ──────────────────────────────────────────

/// Resize an object to a new width/height and undo back to original.
class ResizeObjectCommand : public ICanvasCommand
{
public:
    ResizeObjectCommand(Board& board, ObjectId obj_id, double new_width, double new_height);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    ObjectId target_id_;
    double new_width_;
    double new_height_;
    double old_width_{0.0};
    double old_height_{0.0};
};

/// Change an object's style (fill/stroke color) and undo to previous.
class SetStyleCommand : public ICanvasCommand
{
public:
    SetStyleCommand(Board& board, ObjectId obj_id, CanvasColor new_stroke, CanvasColor new_fill);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

private:
    Board& board_;
    ObjectId target_id_;
    CanvasColor new_stroke_;
    CanvasColor new_fill_;
    CanvasColor old_stroke_{0, 0, 0, 255};
    CanvasColor old_fill_{255, 255, 255, 255};
};

/// Duplicate a set of objects and undo by removing the duplicates.
class DuplicateObjectsCommand : public ICanvasCommand
{
public:
    DuplicateObjectsCommand(Board& board, std::vector<ObjectId> source_ids, Point2D offset);

    auto execute() -> void override;
    auto undo() -> void override;
    [[nodiscard]] auto description() const -> std::string override;

    [[nodiscard]] auto duplicated_ids() const -> const std::vector<ObjectId>&;

private:
    Board& board_;
    std::vector<ObjectId> source_ids_;
    Point2D offset_;
    std::vector<ObjectId> created_ids_;

public:
    // ── Batch 9 (#88) ───────────────────────────────────────────

    /// (#88) Number of duplicated objects created.
    [[nodiscard]] auto duplicated_count() const noexcept -> size_t
    {
        return created_ids_.size();
    }
};

} // namespace markamp::canvas
