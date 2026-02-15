#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Abstract command interface for undo/redo support.
class ICanvasCommand
{
public:
    virtual ~ICanvasCommand() = default;

    ICanvasCommand(const ICanvasCommand&) = delete;
    auto operator=(const ICanvasCommand&) -> ICanvasCommand& = delete;
    ICanvasCommand(ICanvasCommand&&) = delete;
    auto operator=(ICanvasCommand&&) -> ICanvasCommand& = delete;

    /// Execute or re-execute the command.
    virtual auto execute() -> void = 0;

    /// Undo the command.
    virtual auto undo() -> void = 0;

    /// Human-readable description.
    [[nodiscard]] virtual auto description() const -> std::string = 0;

    /// Can this command merge with another of the same type?
    [[nodiscard]] virtual auto can_merge_with(const ICanvasCommand& other) const -> bool;

    /// Merge another command into this one (returns true if merged).
    virtual auto merge_with(const ICanvasCommand& other) -> bool;

protected:
    ICanvasCommand() = default;
};

/// Undo/redo stack with compound command support and max history limit.
class UndoRedoStack
{
public:
    explicit UndoRedoStack(size_t max_history = 100);

    /// Execute a command and push it onto the undo stack.
    auto execute(std::unique_ptr<ICanvasCommand> cmd) -> void;

    /// Execute a command, attempting to merge with the previous command.
    auto execute_or_merge(std::unique_ptr<ICanvasCommand> cmd) -> void;

    /// Undo the most recent command.
    auto undo() -> bool;

    /// Redo the most recently undone command.
    auto redo() -> bool;

    /// State queries.
    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    [[nodiscard]] auto undo_count() const -> size_t;
    [[nodiscard]] auto redo_count() const -> size_t;

    /// Get the description of the next undo/redo action.
    [[nodiscard]] auto undo_description() const -> std::string;
    [[nodiscard]] auto redo_description() const -> std::string;

    /// Clear all history.
    auto clear() -> void;

private:
    std::vector<std::unique_ptr<ICanvasCommand>> undo_stack_;
    std::vector<std::unique_ptr<ICanvasCommand>> redo_stack_;
    size_t max_history_;
};

} // namespace markamp::canvas
