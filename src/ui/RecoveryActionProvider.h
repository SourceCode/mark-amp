#pragma once

/**
 * @file RecoveryActionProvider.h
 * @brief Phase 34 Task 2: Inline recovery action registry and undo stack.
 *
 * Maps error contexts to recovery actions (retry, open log, settings, undo),
 * and provides a simple undo stack for reversible operations.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Recovery action type.
enum class RecoveryActionType : uint8_t
{
    kRetry,
    kOpenLog,
    kOpenSettings,
    kUndo,
    kCustom,
};

/// A recovery action offered to the user.
struct RecoveryAction
{
    RecoveryActionType type{RecoveryActionType::kRetry};
    std::string label;
    std::string icon_name;

    /// Convenience: get a human-readable type name.
    [[nodiscard]] auto type_name() const -> std::string;
};

/// An undoable operation on the stack.
struct UndoEntry
{
    std::string operation_id;
    std::string description;
    std::function<void()> undo_fn;
    std::function<void()> redo_fn;
};

/**
 * @brief Inline recovery action registry and undo stack.
 *
 * Provides context-to-recovery-action mapping, plus a simple undo/redo
 * stack for reversible operations with consistent messaging.
 */
class RecoveryActionProvider
{
public:
    RecoveryActionProvider() = default;

    // ── Recovery action registry ───────────────────────────────────

    /// Register a recovery action for an error context.
    void register_recovery(const std::string& error_context, const RecoveryAction& action);

    /// Get recovery actions for an error context.
    [[nodiscard]] auto recoveries_for(const std::string& error_context) const
        -> std::vector<RecoveryAction>;

    /// Check if recovery actions exist for a context.
    [[nodiscard]] auto has_recoveries(const std::string& error_context) const -> bool;

    // ── Undo stack ─────────────────────────────────────────────────

    /// Push a reversible operation onto the undo stack.
    void push_undo(const UndoEntry& entry);

    /// Undo the last operation. Returns true if undo occurred.
    auto undo() -> bool;

    /// Redo the last undone operation. Returns true if redo occurred.
    auto redo() -> bool;

    /// Check if undo is available.
    [[nodiscard]] auto can_undo() const -> bool;

    /// Check if redo is available.
    [[nodiscard]] auto can_redo() const -> bool;

    /// Get the description of the next undoable operation.
    [[nodiscard]] auto undo_description() const -> std::string;

    /// Get the description of the next redoable operation.
    [[nodiscard]] auto redo_description() const -> std::string;

    /// Get the undo stack depth.
    [[nodiscard]] auto undo_depth() const -> int;

    /// Clear the undo/redo stack.
    void clear_undo();

private:
    struct RecoveryEntry
    {
        std::string error_context;
        RecoveryAction action;
    };

    std::vector<RecoveryEntry> recoveries_;
    std::vector<UndoEntry> undo_stack_;
    std::vector<UndoEntry> redo_stack_;
};

} // namespace markamp::ui
