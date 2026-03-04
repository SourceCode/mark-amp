#pragma once

/**
 * @file BulkActionController.h
 * @brief Phase 33 Task 2: Contextual bulk action controller.
 *
 * Shows contextual bulk action strip when multiple items selected.
 * Supports safe destructive flows with preview and confirmation.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Severity of a bulk action.
enum class ActionSeverity : uint8_t
{
    kNormal,       ///< No special treatment
    kDestructive,  ///< Requires confirmation
    kIrreversible, ///< Requires explicit confirmation with warning
};

/// A single bulk action definition.
struct BulkAction
{
    std::string action_id;
    std::string label;
    std::string icon_name;
    ActionSeverity severity{ActionSeverity::kNormal};
    int min_selection{1}; ///< Minimum selected items to enable

    /// Check if this action requires confirmation.
    [[nodiscard]] auto requires_confirmation() const -> bool;
};

/// Result of a bulk operation.
struct BulkOperationResult
{
    int affected_count{0};
    int failed_count{0};
    std::string status_message;

    /// Check if operation fully succeeded.
    [[nodiscard]] auto is_success() const -> bool;
};

/**
 * @brief Controller for contextual bulk action strips.
 *
 * Manages available bulk actions based on selection count, provides
 * confirmation flows for destructive actions, and tracks operation results.
 */
class BulkActionController
{
public:
    BulkActionController() = default;

    // ── Action registration ────────────────────────────────────────

    /// Register a bulk action.
    void register_action(const BulkAction& action);

    /// Get all registered actions.
    [[nodiscard]] auto all_actions() const -> const std::vector<BulkAction>&;

    /// Get actions enabled for the current selection count.
    [[nodiscard]] auto enabled_actions(int selection_count) const -> std::vector<BulkAction>;

    /// Get a specific action by ID.
    [[nodiscard]] auto find_action(const std::string& action_id) const -> const BulkAction*;

    /// Get the number of registered actions.
    [[nodiscard]] auto action_count() const -> int;

    // ── Execution ──────────────────────────────────────────────────

    /// Execute a bulk action. Returns false if confirmation needed first.
    auto execute(const std::string& action_id, int selection_count) -> BulkOperationResult;

    /// Confirm and execute a destructive action.
    auto confirm_and_execute(const std::string& action_id, int selection_count)
        -> BulkOperationResult;

    /// Get the last operation result.
    [[nodiscard]] auto last_result() const -> const BulkOperationResult&;

    // ── Confirmation state ─────────────────────────────────────────

    /// Check if an action needs confirmation before execution.
    [[nodiscard]] auto needs_confirmation(const std::string& action_id) const -> bool;

    /// Get a confirmation message for a destructive action.
    [[nodiscard]] auto confirmation_message(const std::string& action_id, int selection_count) const
        -> std::string;

    // ── Standard actions factory ───────────────────────────────────

    /// Register standard bulk actions (delete, move, copy, rename, export).
    void register_standard_actions();

private:
    std::vector<BulkAction> actions_;
    BulkOperationResult last_result_;
};

} // namespace markamp::ui
