#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Preflight check severity.
enum class PreflightSeverity : uint8_t
{
    kPass,
    kWarning,
    kBlock,
};

/// A preflight check result.
struct PreflightCheck
{
    std::string check_id;
    std::string message;
    PreflightSeverity severity{PreflightSeverity::kPass};
};

/// Recovery action offered after an error.
struct RecoveryAction
{
    std::string action_id;
    std::string label; ///< e.g., "Retry", "Open Settings", "View Log"
    bool is_primary{false};
};

/// An error with recovery context.
struct ErrorContext
{
    std::string error_id;
    std::string what_happened;
    std::string why;
    std::string what_to_do;
    std::vector<RecoveryAction> actions;
};

/// Testable model for Error Prevention & Recovery (Phase 34).
///
/// Encapsulates:
/// - Preflight validation before risky actions
/// - Rich error context (what/why/next)
/// - Recovery actions per error
/// - Undo tracking (number of undoable recent actions)
class ErrorRecoveryModel
{
public:
    // ── Preflight ───────────────────────────────────────────────────

    void set_preflights(std::vector<PreflightCheck> checks);
    [[nodiscard]] auto preflights() const -> const std::vector<PreflightCheck>&;

    /// Returns true if any check blocks the action.
    [[nodiscard]] auto is_blocked() const -> bool;

    /// Returns all blocking checks.
    [[nodiscard]] auto blockers() const -> std::vector<PreflightCheck>;

    /// Returns all warning checks.
    [[nodiscard]] auto warnings() const -> std::vector<PreflightCheck>;

    // ── Errors ──────────────────────────────────────────────────────

    void push_error(ErrorContext error);
    [[nodiscard]] auto errors() const -> const std::vector<ErrorContext>&;
    [[nodiscard]] auto latest_error() const -> const ErrorContext*;
    void clear_errors();

    // ── Undo tracking ───────────────────────────────────────────────

    void push_undoable(const std::string& action_label);
    [[nodiscard]] auto undo_stack_size() const -> int;
    [[nodiscard]] auto last_undoable() const -> const std::string*;
    void pop_undoable();

private:
    std::vector<PreflightCheck> preflights_;
    std::vector<ErrorContext> errors_;
    std::vector<std::string> undo_stack_;
};

} // namespace markamp::ui
