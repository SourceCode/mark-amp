#pragma once

/**
 * @file UxAuditRunner.h
 * @brief Phase 40 Task 1-2: Audit checks for control surfaces.
 *
 * Defines audit checks (interaction, style, accessibility, cross-theme,
 * cross-DPI), runs against control surfaces, and reports pass/fail.
 */

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Audit check category.
enum class AuditCategory : uint8_t
{
    kInteraction,
    kStyle,
    kAccessibility,
    kCrossTheme,
    kCrossDpi,
    kPlatform,
};

/// Result of a single audit check.
struct AuditCheckResult
{
    std::string check_id;
    std::string description;
    AuditCategory category{AuditCategory::kInteraction};
    bool passed{false};
    std::string failure_reason;

    /// Get category as string.
    [[nodiscard]] auto category_name() const -> std::string;
};

/**
 * @brief Runs UX audit checks against control surfaces.
 */
class UxAuditRunner
{
public:
    UxAuditRunner() = default;

    // ── Check registration ─────────────────────────────────────────

    /// Register an audit check.
    void add_check(const std::string& check_id,
                   const std::string& description,
                   AuditCategory category,
                   std::function<bool()> check);

    /// Get check count.
    [[nodiscard]] auto check_count() const -> int;

    /// Get checks by category.
    [[nodiscard]] auto checks_in_category(AuditCategory category) const -> int;

    // ── Execution ──────────────────────────────────────────────────

    /// Run all checks and return results.
    [[nodiscard]] auto run() -> std::vector<AuditCheckResult>;

    /// Run checks in a specific category.
    [[nodiscard]] auto run_category(AuditCategory category) -> std::vector<AuditCheckResult>;

    /// Get overall pass rate (0.0-1.0).
    [[nodiscard]] auto pass_rate(const std::vector<AuditCheckResult>& results) const -> double;

    // ── Summary ────────────────────────────────────────────────────

    /// Get failure descriptions.
    [[nodiscard]] auto failure_descriptions(const std::vector<AuditCheckResult>& results) const
        -> std::vector<std::string>;

private:
    struct AuditCheck
    {
        std::string check_id;
        std::string description;
        AuditCategory category{AuditCategory::kInteraction};
        std::function<bool()> check;
    };

    std::vector<AuditCheck> checks_;
};

} // namespace markamp::ui
