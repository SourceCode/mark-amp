#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// A single undo/redo action.
struct UndoAction
{
    std::string label;
    std::string action_type;
    int byte_estimate{0};

    // ── Round 5 Batch 7 (#61-62) ────────────────────────────────

    /// (#61) Whether a label is set.
    [[nodiscard]] auto has_label() const noexcept -> bool
    {
        return !label.empty();
    }

    /// (#62) Whether an action type is set.
    [[nodiscard]] auto has_type() const noexcept -> bool
    {
        return !action_type.empty();
    }
};

/// Testable model for Undo/Redo Transaction System (Phase 67).
///
/// Encapsulates:
/// - Undo/redo stack with push/pop
/// - Transaction boundaries (begin/end compound)
/// - History metadata labels
/// - Memory budget controls (step limit + byte limit)
class UndoRedoModel
{
public:
    // ── Stack operations ────────────────────────────────────────────

    void push(UndoAction action);
    void undo();
    void redo();

    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    [[nodiscard]] auto undo_count() const -> int;
    [[nodiscard]] auto redo_count() const -> int;

    // ── History metadata ────────────────────────────────────────────

    [[nodiscard]] auto next_undo_label() const -> std::string;
    [[nodiscard]] auto next_redo_label() const -> std::string;

    // ── Compound transactions ───────────────────────────────────────

    void begin_compound(const std::string& label);
    void end_compound();
    [[nodiscard]] auto is_in_compound() const -> bool;

    // ── Memory budget ───────────────────────────────────────────────

    void set_max_steps(int max_steps);
    [[nodiscard]] auto max_steps() const -> int;

    void set_max_bytes(int max_bytes);
    [[nodiscard]] auto total_bytes() const -> int;

    void clear();

private:
    std::vector<UndoAction> undo_stack_;
    std::vector<UndoAction> redo_stack_;
    int max_steps_{200};
    int max_bytes_{50 * 1024 * 1024}; // 50 MB
    bool in_compound_{false};
    std::string compound_label_;
    std::vector<UndoAction> compound_buffer_;

    // ── Round 5 Batch 7 (#63-65) ────────────────────────────────

    /// (#63) Whether both stacks are empty.
    [[nodiscard]] auto is_empty() const noexcept -> bool
    {
        return undo_stack_.empty() && redo_stack_.empty();
    }

    /// (#64) Whether a compound label is set.
    [[nodiscard]] auto has_compound_label() const noexcept -> bool
    {
        return !compound_label_.empty();
    }

    /// (#65) Whether undo stack is near the limit.
    [[nodiscard]] auto is_near_limit() const noexcept -> bool
    {
        return static_cast<int>(undo_stack_.size()) >= max_steps_ - 10;
    }
};

} // namespace markamp::canvas
