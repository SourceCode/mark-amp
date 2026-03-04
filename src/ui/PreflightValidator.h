#pragma once

/**
 * @file PreflightValidator.h
 * @brief Phase 34 Task 1: Preflight checks for risky actions.
 *
 * Validates constraints before executing risky operations, producing
 * actionable messages with severity (info/warning/error/blocker).
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Severity of a preflight check result.
enum class PreflightSeverity : uint8_t
{
    kInfo,    ///< Informational, proceed allowed
    kWarning, ///< Cautionary, user should acknowledge
    kError,   ///< Problem, should be fixed first
    kBlocker, ///< Fatal, cannot proceed
};

/// A single preflight check result.
struct PreflightResult
{
    std::string check_id;
    PreflightSeverity severity{PreflightSeverity::kInfo};
    std::string what_happened;
    std::string why;
    std::string what_to_do;

    /// Check if this result blocks the action.
    [[nodiscard]] auto is_blocking() const -> bool;

    /// Get a human-readable summary.
    [[nodiscard]] auto message() const -> std::string;

    /// Get severity as a string.
    [[nodiscard]] auto severity_name() const -> std::string;
};

/// Callback type for a preflight check.
using PreflightCheck = std::function<PreflightResult(const std::string& action_id)>;

/**
 * @brief Validates constraints before executing risky operations.
 *
 * Register named checks per action. Running validation returns all
 * results, and the caller can decide whether to proceed based on
 * the highest severity.
 */
class PreflightValidator
{
public:
    PreflightValidator() = default;

    // ── Check registration ─────────────────────────────────────────

    /// Register a preflight check for an action.
    void
    register_check(const std::string& action_id, const std::string& check_id, PreflightCheck check);

    /// Remove all checks for an action.
    void clear_checks(const std::string& action_id);

    /// Get the number of registered checks for an action.
    [[nodiscard]] auto check_count(const std::string& action_id) const -> int;

    // ── Validation ─────────────────────────────────────────────────

    /// Run all preflight checks for an action.
    [[nodiscard]] auto validate(const std::string& action_id) const -> std::vector<PreflightResult>;

    /// Check if an action can proceed (no blockers/errors).
    [[nodiscard]] auto can_proceed(const std::string& action_id) const -> bool;

    /// Get the highest severity from validation results.
    [[nodiscard]] auto highest_severity(const std::string& action_id) const -> PreflightSeverity;

private:
    struct RegisteredCheck
    {
        std::string action_id;
        std::string check_id;
        PreflightCheck check;
    };

    std::vector<RegisteredCheck> checks_;
};

} // namespace markamp::ui
