#include "UndoRedoStack.h"

namespace markamp::canvas
{

// ── ICanvasCommand defaults ────────────────────────────────────

auto ICanvasCommand::can_merge_with(const ICanvasCommand& /*other*/) const -> bool
{
    return false;
}

auto ICanvasCommand::merge_with(const ICanvasCommand& /*other*/) -> bool
{
    return false;
}

// ── UndoRedoStack ──────────────────────────────────────────────

UndoRedoStack::UndoRedoStack(size_t max_history)
    : max_history_(max_history)
{
}

auto UndoRedoStack::execute(std::unique_ptr<ICanvasCommand> cmd) -> void
{
    cmd->execute();
    undo_stack_.push_back(std::move(cmd));
    redo_stack_.clear();

    // Trim history if over limit.
    if (undo_stack_.size() > max_history_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
}

auto UndoRedoStack::execute_or_merge(std::unique_ptr<ICanvasCommand> cmd) -> void
{
    if (!undo_stack_.empty() && undo_stack_.back()->can_merge_with(*cmd))
    {
        undo_stack_.back()->merge_with(*cmd);
        cmd->execute(); // Still execute the action.
    }
    else
    {
        execute(std::move(cmd));
    }
}

auto UndoRedoStack::undo() -> bool
{
    if (undo_stack_.empty())
    {
        return false;
    }

    auto cmd = std::move(undo_stack_.back());
    undo_stack_.pop_back();
    cmd->undo();
    redo_stack_.push_back(std::move(cmd));
    return true;
}

auto UndoRedoStack::redo() -> bool
{
    if (redo_stack_.empty())
    {
        return false;
    }

    auto cmd = std::move(redo_stack_.back());
    redo_stack_.pop_back();
    cmd->execute();
    undo_stack_.push_back(std::move(cmd));
    return true;
}

auto UndoRedoStack::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

auto UndoRedoStack::can_redo() const -> bool
{
    return !redo_stack_.empty();
}

auto UndoRedoStack::undo_count() const -> size_t
{
    return undo_stack_.size();
}

auto UndoRedoStack::redo_count() const -> size_t
{
    return redo_stack_.size();
}

auto UndoRedoStack::undo_description() const -> std::string
{
    if (undo_stack_.empty())
    {
        return {};
    }
    return undo_stack_.back()->description();
}

auto UndoRedoStack::redo_description() const -> std::string
{
    if (redo_stack_.empty())
    {
        return {};
    }
    return redo_stack_.back()->description();
}

auto UndoRedoStack::clear() -> void
{
    undo_stack_.clear();
    redo_stack_.clear();
}

// ── V8 Phase 7: Collaborative undo support ─────────────────────

auto UndoRedoStack::apply_remote(std::unique_ptr<ICanvasCommand> cmd) -> void
{
    cmd->set_remote(true);
    cmd->execute();
    undo_stack_.push_back(std::move(cmd));
}

auto UndoRedoStack::undo_local_only() -> bool
{
    for (auto iter = undo_stack_.rbegin(); iter != undo_stack_.rend(); ++iter)
    {
        if (!(*iter)->is_remote())
        {
            (*iter)->undo();
            redo_stack_.push_back(std::move(*iter));
            undo_stack_.erase(std::next(iter).base());
            return true;
        }
    }
    return false;
}

auto UndoRedoStack::local_undo_count() const -> size_t
{
    size_t count = 0;
    for (const auto& cmd : undo_stack_)
    {
        if (!cmd->is_remote())
        {
            ++count;
        }
    }
    return count;
}

// --- Batch 9 (#52-54) ---

auto UndoRedoStack::all_descriptions() const -> std::vector<std::string>
{
    std::vector<std::string> result;
    result.reserve(undo_stack_.size());
    for (const auto& cmd : undo_stack_)
    {
        result.push_back(cmd->description());
    }
    return result;
}

auto UndoRedoStack::current_index() const -> size_t
{
    return undo_stack_.size();
}

auto UndoRedoStack::trim_to(size_t max_entries) -> void
{
    while (undo_stack_.size() > max_entries)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
}

// (#106) Return total entries across both stacks.
auto UndoRedoStack::total_count() const -> size_t
{
    return undo_stack_.size() + redo_stack_.size();
}

// (#107) Check if both stacks are empty.
auto UndoRedoStack::is_empty() const -> bool
{
    return undo_stack_.empty() && redo_stack_.empty();
}

// (#108) Return the configured maximum history limit.
auto UndoRedoStack::max_history() const -> size_t
{
    return max_history_;
}

} // namespace markamp::canvas
