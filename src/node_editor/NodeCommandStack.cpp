// V11 Phase 07: Command Stack & Undo/Redo Transactions
// NodeCommandStack implementation.

#include "node_editor/NodeCommandStack.h"

namespace markamp::node_editor
{

NodeCommandStack::NodeCommandStack(std::size_t max_depth)
    : max_depth_(max_depth)
{
}

void NodeCommandStack::execute(std::unique_ptr<INodeCommand> command, NodeGraph& graph)
{
    command->execute(graph);

    // Clear redo stack on new command
    redo_stack_.clear();

    undo_stack_.push_back(std::move(command));

    // Enforce max depth
    if (undo_stack_.size() > max_depth_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
}

auto NodeCommandStack::undo(NodeGraph& graph) -> bool
{
    if (undo_stack_.empty())
    {
        return false;
    }

    auto command = std::move(undo_stack_.back());
    undo_stack_.pop_back();

    command->undo(graph);

    redo_stack_.push_back(std::move(command));
    return true;
}

auto NodeCommandStack::redo(NodeGraph& graph) -> bool
{
    if (redo_stack_.empty())
    {
        return false;
    }

    auto command = std::move(redo_stack_.back());
    redo_stack_.pop_back();

    command->execute(graph);

    undo_stack_.push_back(std::move(command));
    return true;
}

auto NodeCommandStack::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

auto NodeCommandStack::can_redo() const -> bool
{
    return !redo_stack_.empty();
}

auto NodeCommandStack::undo_description() const -> std::string
{
    if (undo_stack_.empty())
    {
        return {};
    }
    return undo_stack_.back()->description();
}

auto NodeCommandStack::redo_description() const -> std::string
{
    if (redo_stack_.empty())
    {
        return {};
    }
    return redo_stack_.back()->description();
}

auto NodeCommandStack::undo_count() const -> std::size_t
{
    return undo_stack_.size();
}

auto NodeCommandStack::redo_count() const -> std::size_t
{
    return redo_stack_.size();
}

void NodeCommandStack::clear()
{
    undo_stack_.clear();
    redo_stack_.clear();
}

} // namespace markamp::node_editor
