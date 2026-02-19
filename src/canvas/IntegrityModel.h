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
};

} // namespace markamp::canvas
