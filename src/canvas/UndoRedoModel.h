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
};

} // namespace markamp::canvas
