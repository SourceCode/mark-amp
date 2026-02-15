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

} // namespace markamp::canvas
