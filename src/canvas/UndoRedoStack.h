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

    /// Whether this command originated from a remote participant.
    [[nodiscard]] auto is_remote() const -> bool
    {
        return is_remote_;
    }
    auto set_remote(bool remote) -> void
    {
        is_remote_ = remote;
    }

protected:
    ICanvasCommand() = default;

private:
    bool is_remote_{false};
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

    // ── V8 Phase 7: Collaborative undo support ─────────────────────

    /// Apply a remote command without pushing to local undo stack.
    auto apply_remote(std::unique_ptr<ICanvasCommand> cmd) -> void;

    /// Undo only the most recent local (non-remote) command.
    auto undo_local_only() -> bool;

    /// Number of local (non-remote) undo entries.
    [[nodiscard]] auto local_undo_count() const -> size_t;

    // ── Batch 9 (#52-54) ──────────────────────────────────────────

    /// Return all undo descriptions in order (oldest first).
    [[nodiscard]] auto all_descriptions() const -> std::vector<std::string>;

    /// Current index in the undo stack (== undo_count()).
    [[nodiscard]] auto current_index() const -> size_t;

    /// Trim undo history to keep at most N entries.
    auto trim_to(size_t max_entries) -> void;

private:
    std::vector<std::unique_ptr<ICanvasCommand>> undo_stack_;
    std::vector<std::unique_ptr<ICanvasCommand>> redo_stack_;
    size_t max_history_;
};

} // namespace markamp::canvas
