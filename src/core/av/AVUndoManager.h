#pragma once

#include "AVValue.h"

#include <cstddef>
#include <string>
#include <vector>

namespace markamp::core::av
{

// ════════════════════════════════════════════════════════════
// Operation Types
// ════════════════════════════════════════════════════════════

enum class AVOperationType : std::uint8_t
{
    SetValue,
    AddRow,
    RemoveRow,
    AddKey,
    RemoveKey,
    RenameKey,
    ReorderKeys,
    AddView,
    RemoveView,
    Composite // A group of operations (e.g. paste)
};

// ════════════════════════════════════════════════════════════
// Single Operation Record
// ════════════════════════════════════════════════════════════

struct AVOperation
{
    AVOperationType type{AVOperationType::SetValue};
    std::string av_id;
    std::string key_id;
    std::string block_id;
    AVValueVariant old_value;
    AVValueVariant new_value;
    std::string description;
    int64_t timestamp_ms{0};
};

// ════════════════════════════════════════════════════════════
// Composite Operation (batch operations)
// ════════════════════════════════════════════════════════════

struct AVCompositeOperation
{
    std::vector<AVOperation> operations;
    std::string description;
    int64_t timestamp_ms{0};
};

// ════════════════════════════════════════════════════════════
// Undo Manager
// ════════════════════════════════════════════════════════════

class AVUndoManager
{
public:
    explicit AVUndoManager(size_t max_depth = 100);

    /// Record a single operation on the undo stack.
    void record(AVOperation operation);

    /// Record a batch of operations as a single undoable action.
    void record_composite(AVCompositeOperation composite);

    /// Undo the last operation. Returns the operation that was undone.
    [[nodiscard]] auto undo() -> AVCompositeOperation;

    /// Redo the last undone operation. Returns the operation that was redone.
    [[nodiscard]] auto redo() -> AVCompositeOperation;

    /// Check if undo is available.
    [[nodiscard]] auto can_undo() const -> bool;

    /// Check if redo is available.
    [[nodiscard]] auto can_redo() const -> bool;

    /// Get the description of the next undo operation.
    [[nodiscard]] auto undo_description() const -> std::string;

    /// Get the description of the next redo operation.
    [[nodiscard]] auto redo_description() const -> std::string;

    /// Get the number of operations in the undo stack.
    [[nodiscard]] auto undo_count() const -> size_t;

    /// Get the number of operations in the redo stack.
    [[nodiscard]] auto redo_count() const -> size_t;

    /// Clear both undo and redo stacks.
    void clear();

    /// Get/set max history depth.
    [[nodiscard]] auto max_history() const -> size_t;
    void set_max_history(size_t depth);

    /// Begin/end a coalescing group (rapid edits to the same cell coalesce).
    void begin_coalescing(const std::string& key_id, const std::string& block_id);
    void end_coalescing();

private:
    size_t max_depth_;
    std::vector<AVCompositeOperation> undo_stack_;
    std::vector<AVCompositeOperation> redo_stack_;

    // Coalescing state
    bool coalescing_{false};
    std::string coalesce_key_id_;
    std::string coalesce_block_id_;

    /// Wrap a single operation into a composite.
    [[nodiscard]] static auto wrap_single(AVOperation operation) -> AVCompositeOperation;

    /// Try to coalesce a new operation into the top of the undo stack.
    [[nodiscard]] auto try_coalesce(const AVOperation& operation) -> bool;

    /// Trim the undo stack to max_depth_.
    void trim_stack();

    /// Get current time in milliseconds.
    [[nodiscard]] static auto now_ms() -> int64_t;
};

} // namespace markamp::core::av
