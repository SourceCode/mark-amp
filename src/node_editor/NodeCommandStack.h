#pragma once

// V11 Phase 07: Command Stack & Undo/Redo Transactions
// Undo/redo stack for node editor commands.

#include "node_editor/NodeCommand.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::node_editor
{

/// Manages an undo/redo stack of INodeCommand instances.
///
/// Commands are executed through the stack so their effects can be reversed.
/// The stack has a configurable maximum depth; older commands are discarded
/// when the limit is reached.
class NodeCommandStack
{
public:
    /// Default max undo depth.
    static constexpr std::size_t kDefaultMaxDepth = 256;

    explicit NodeCommandStack(std::size_t max_depth = kDefaultMaxDepth);

    /// Execute a command and push it onto the undo stack.
    /// Clears the redo stack.
    void execute(std::unique_ptr<INodeCommand> command, NodeGraph& graph);

    /// Undo the most recent command.
    /// Returns false if nothing to undo.
    auto undo(NodeGraph& graph) -> bool;

    /// Redo the most recently undone command.
    /// Returns false if nothing to redo.
    auto redo(NodeGraph& graph) -> bool;

    /// Whether undo is available.
    [[nodiscard]] auto can_undo() const -> bool;

    /// Whether redo is available.
    [[nodiscard]] auto can_redo() const -> bool;

    /// Description of the command that would be undone.
    [[nodiscard]] auto undo_description() const -> std::string;

    /// Description of the command that would be redone.
    [[nodiscard]] auto redo_description() const -> std::string;

    /// Number of commands in the undo stack.
    [[nodiscard]] auto undo_count() const -> std::size_t;

    /// Number of commands in the redo stack.
    [[nodiscard]] auto redo_count() const -> std::size_t;

    /// Clear both undo and redo stacks.
    void clear();

private:
    std::size_t max_depth_;
    std::vector<std::unique_ptr<INodeCommand>> undo_stack_;
    std::vector<std::unique_ptr<INodeCommand>> redo_stack_;
};

} // namespace markamp::node_editor
