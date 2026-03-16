#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Validation severity.
enum class ValidationSeverity : uint8_t
{
    kInfo,
    kWarning,
    kError,
};

/// A validation issue found during integrity check.
struct ValidationIssue
{
    std::string object_id;
    std::string field;
    std::string message;
    ValidationSeverity severity{ValidationSeverity::kWarning};

    // ── Round 6 Batch 2 (#18-20) ────────────────────────────────

    /// (#18) Whether severity is error.
    [[nodiscard]] auto is_error() const noexcept -> bool
    {
        return severity == ValidationSeverity::kError;
    }

    /// (#19) Whether severity is warning.
    [[nodiscard]] auto is_warning() const noexcept -> bool
    {
        return severity == ValidationSeverity::kWarning;
    }

    /// (#20) Whether a message is set.
    [[nodiscard]] auto has_message() const noexcept -> bool
    {
        return !message.empty();
    }
};

/// Testable model for Canvas Validation / Data Integrity (Phase 76).
///
/// Encapsulates:
/// - Object invariant validation (bounds, transforms, fields)
/// - Command precondition checking
/// - Board consistency scan (orphan refs, invalid links)
/// - Integrity report aggregation
class IntegrityModel
{
public:
    // ── Issues ──────────────────────────────────────────────────────

    void add_issue(ValidationIssue issue);
    void clear_issues();
    [[nodiscard]] auto issues() const -> const std::vector<ValidationIssue>&;
    [[nodiscard]] auto error_count() const -> int;
    [[nodiscard]] auto warning_count() const -> int;
    [[nodiscard]] auto is_valid() const -> bool;

    // ── Precondition check ──────────────────────────────────────────

    void set_precondition_result(bool passed, const std::string& reason);
    [[nodiscard]] auto precondition_passed() const -> bool;
    [[nodiscard]] auto precondition_reason() const -> const std::string&;

    // ── Consistency scan ────────────────────────────────────────────

    void set_orphan_count(int count);
    [[nodiscard]] auto orphan_count() const -> int;

    void set_invalid_link_count(int count);
    [[nodiscard]] auto invalid_link_count() const -> int;

private:
    std::vector<ValidationIssue> issues_;
    bool precondition_passed_{true};
    std::string precondition_reason_;
    int orphan_count_{0};
    int invalid_link_count_{0};

    // ── Round 6 Batch 3 (#21-25) ────────────────────────────────

    /// (#21) Whether issues exist.
    [[nodiscard]] auto has_issues() const noexcept -> bool
    {
        return !issues_.empty();
    }

    /// (#22) Number of issues.
    [[nodiscard]] auto issue_count() const noexcept -> size_t
    {
        return issues_.size();
    }

    /// (#23) Whether orphan references exist.
    [[nodiscard]] auto has_orphans() const noexcept -> bool
    {
        return orphan_count_ > 0;
    }

    /// (#24) Whether invalid links exist.
    [[nodiscard]] auto has_invalid_links() const noexcept -> bool
    {
        return invalid_link_count_ > 0;
    }

    /// (#25) Whether a precondition reason is set.
    [[nodiscard]] auto has_precondition_reason() const noexcept -> bool
    {
        return !precondition_reason_.empty();
    }
};

} // namespace markamp::canvas
