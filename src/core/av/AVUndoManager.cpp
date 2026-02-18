#include "AVUndoManager.h"

#include <chrono>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Constructor
// ════════════════════════════════════════════════════════════

AVUndoManager::AVUndoManager(size_t max_depth)
    : max_depth_(max_depth)
{
}

// ════════════════════════════════════════════════════════════
// Time utility
// ════════════════════════════════════════════════════════════

auto AVUndoManager::now_ms() -> int64_t
{
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
}

// ════════════════════════════════════════════════════════════
// Wrap single → composite
// ════════════════════════════════════════════════════════════

auto AVUndoManager::wrap_single(AVOperation operation) -> AVCompositeOperation
{
    AVCompositeOperation composite;
    composite.description = operation.description;
    composite.timestamp_ms = operation.timestamp_ms;
    composite.operations.push_back(std::move(operation));
    return composite;
}

// ════════════════════════════════════════════════════════════
// Coalescing
// ════════════════════════════════════════════════════════════

void AVUndoManager::begin_coalescing(const std::string& key_id, const std::string& block_id)
{
    coalescing_ = true;
    coalesce_key_id_ = key_id;
    coalesce_block_id_ = block_id;
}

void AVUndoManager::end_coalescing()
{
    coalescing_ = false;
    coalesce_key_id_.clear();
    coalesce_block_id_.clear();
}

auto AVUndoManager::try_coalesce(const AVOperation& operation) -> bool
{
    if (!coalescing_)
    {
        return false;
    }

    if (operation.type != AVOperationType::SetValue)
    {
        return false;
    }

    if (operation.key_id != coalesce_key_id_ || operation.block_id != coalesce_block_id_)
    {
        return false;
    }

    if (undo_stack_.empty())
    {
        return false;
    }

    auto& top = undo_stack_.back();
    if (top.operations.size() != 1)
    {
        return false;
    }

    auto& top_op = top.operations[0];
    if (top_op.type != AVOperationType::SetValue)
    {
        return false;
    }

    if (top_op.key_id != operation.key_id || top_op.block_id != operation.block_id)
    {
        return false;
    }

    // Coalesce: keep the original old_value, update to the new new_value
    constexpr int64_t kCoalesceWindowMs = 1000;
    if (operation.timestamp_ms - top.timestamp_ms > kCoalesceWindowMs)
    {
        return false;
    }

    top_op.new_value = operation.new_value;
    top.timestamp_ms = operation.timestamp_ms;
    return true;
}

// ════════════════════════════════════════════════════════════
// Stack trimming
// ════════════════════════════════════════════════════════════

void AVUndoManager::trim_stack()
{
    while (undo_stack_.size() > max_depth_)
    {
        undo_stack_.erase(undo_stack_.begin());
    }
}

// ════════════════════════════════════════════════════════════
// Record
// ════════════════════════════════════════════════════════════

void AVUndoManager::record(AVOperation operation)
{
    operation.timestamp_ms = now_ms();

    // Try coalescing
    if (try_coalesce(operation))
    {
        return;
    }

    undo_stack_.push_back(wrap_single(std::move(operation)));
    redo_stack_.clear();
    trim_stack();
}

void AVUndoManager::record_composite(AVCompositeOperation composite)
{
    composite.timestamp_ms = now_ms();
    undo_stack_.push_back(std::move(composite));
    redo_stack_.clear();
    trim_stack();
}

// ════════════════════════════════════════════════════════════
// Undo / Redo
// ════════════════════════════════════════════════════════════

auto AVUndoManager::undo() -> AVCompositeOperation
{
    if (undo_stack_.empty())
    {
        return {};
    }

    auto undo_op = std::move(undo_stack_.back());
    undo_stack_.pop_back();

    // Create the inverse for redo
    AVCompositeOperation inverse;
    inverse.description = undo_op.description;
    inverse.timestamp_ms = now_ms();
    for (auto& single_op : undo_op.operations)
    {
        AVOperation inv_op = single_op;
        std::swap(inv_op.old_value, inv_op.new_value);
        inverse.operations.push_back(std::move(inv_op));
    }
    redo_stack_.push_back(std::move(inverse));

    return undo_op;
}

auto AVUndoManager::redo() -> AVCompositeOperation
{
    if (redo_stack_.empty())
    {
        return {};
    }

    auto redo_op = std::move(redo_stack_.back());
    redo_stack_.pop_back();

    // Create the inverse for undo
    AVCompositeOperation inverse;
    inverse.description = redo_op.description;
    inverse.timestamp_ms = now_ms();
    for (auto& single_op : redo_op.operations)
    {
        AVOperation inv_op = single_op;
        std::swap(inv_op.old_value, inv_op.new_value);
        inverse.operations.push_back(std::move(inv_op));
    }
    undo_stack_.push_back(std::move(inverse));

    return redo_op;
}

// ════════════════════════════════════════════════════════════
// Queries
// ════════════════════════════════════════════════════════════

auto AVUndoManager::can_undo() const -> bool
{
    return !undo_stack_.empty();
}

auto AVUndoManager::can_redo() const -> bool
{
    return !redo_stack_.empty();
}

auto AVUndoManager::undo_description() const -> std::string
{
    if (undo_stack_.empty())
    {
        return "";
    }
    return undo_stack_.back().description;
}

auto AVUndoManager::redo_description() const -> std::string
{
    if (redo_stack_.empty())
    {
        return "";
    }
    return redo_stack_.back().description;
}

auto AVUndoManager::undo_count() const -> size_t
{
    return undo_stack_.size();
}

auto AVUndoManager::redo_count() const -> size_t
{
    return redo_stack_.size();
}

void AVUndoManager::clear()
{
    undo_stack_.clear();
    redo_stack_.clear();
}

auto AVUndoManager::max_history() const -> size_t
{
    return max_depth_;
}

void AVUndoManager::set_max_history(size_t depth)
{
    max_depth_ = depth;
    trim_stack();
}

} // namespace markamp::core::av
