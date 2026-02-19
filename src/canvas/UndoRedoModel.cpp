#include "UndoRedoModel.h"

#include <algorithm>
#include <numeric>

namespace markamp::canvas
{

void UndoRedoModel::push(UndoAction action)
{
    if (in_compound_)
    {
        compound_buffer_.push_back(std::move(action));
        return;
    }

    undo_stack_.push_back(std::move(action));
    redo_stack_.clear();

    // enforce step limit
    while (static_cast<int>(undo_stack_.size()) > max_steps_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }

    // enforce byte limit
    while (total_bytes() > max_bytes_ && !undo_stack_.empty())
    {
        undo_stack_.erase(undo_stack_.begin());
    }
}

void UndoRedoModel::undo()
{
    if (!undo_stack_.empty())
    {
        redo_stack_.push_back(std::move(undo_stack_.back()));
        undo_stack_.pop_back();
    }
}

void UndoRedoModel::redo()
{
    if (!redo_stack_.empty())
    {
        undo_stack_.push_back(std::move(redo_stack_.back()));
        redo_stack_.pop_back();
    }
}

auto UndoRedoModel::can_undo() const -> bool
{
    return !undo_stack_.empty();
}
auto UndoRedoModel::can_redo() const -> bool
{
    return !redo_stack_.empty();
}
auto UndoRedoModel::undo_count() const -> int
{
    return static_cast<int>(undo_stack_.size());
}
auto UndoRedoModel::redo_count() const -> int
{
    return static_cast<int>(redo_stack_.size());
}

auto UndoRedoModel::next_undo_label() const -> std::string
{
    return undo_stack_.empty() ? "" : undo_stack_.back().label;
}

auto UndoRedoModel::next_redo_label() const -> std::string
{
    return redo_stack_.empty() ? "" : redo_stack_.back().label;
}

void UndoRedoModel::begin_compound(const std::string& label)
{
    in_compound_ = true;
    compound_label_ = label;
    compound_buffer_.clear();
}

void UndoRedoModel::end_compound()
{
    if (!in_compound_)
    {
        return;
    }
    in_compound_ = false;
    if (compound_buffer_.empty())
    {
        return;
    }

    int total_estimate = 0;
    for (const auto& act : compound_buffer_)
    {
        total_estimate += act.byte_estimate;
    }

    UndoAction compound{compound_label_, "compound", total_estimate};
    undo_stack_.push_back(std::move(compound));
    redo_stack_.clear();
    compound_buffer_.clear();
}

auto UndoRedoModel::is_in_compound() const -> bool
{
    return in_compound_;
}

void UndoRedoModel::set_max_steps(int max_steps)
{
    max_steps_ = std::max(1, max_steps);
}
auto UndoRedoModel::max_steps() const -> int
{
    return max_steps_;
}

void UndoRedoModel::set_max_bytes(int max_bytes)
{
    max_bytes_ = std::max(1024, max_bytes);
}

auto UndoRedoModel::total_bytes() const -> int
{
    int total = 0;
    for (const auto& act : undo_stack_)
    {
        total += act.byte_estimate;
    }
    return total;
}

void UndoRedoModel::clear()
{
    undo_stack_.clear();
    redo_stack_.clear();
    compound_buffer_.clear();
    in_compound_ = false;
}

} // namespace markamp::canvas
