// ============================================================================
// File: src/core/ExportValidator.h
// Phase 24: Export & Publishing — Pre-export content validation
// ============================================================================
#pragma once

#include "ExportTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Data structures
// ============================================================================

/// Severity of a validation issue.
enum class ValidationSeverity : uint8_t
{
    kError,   ///< Must be resolved before export
    kWarning, ///< May cause issues but export can proceed
    kInfo     ///< Informational suggestion
};

/// Category of validation issue.
enum class ValidationCategory : uint8_t
{
    kBrokenLink,         ///< Internal or external link target not found
    kMissingAsset,       ///< Referenced image/file does not exist
    kInvalidFrontmatter, ///< Malformed or missing YAML frontmatter
    kUnsupportedSyntax,  ///< Markdown syntax not supported by target format
    kLargeFile,          ///< Content exceeds recommended size
    kEmptyContent,       ///< Document has no meaningful content
    kDuplicateHeading    ///< Duplicate heading text (may cause anchor conflicts)
};

/// A single validation issue.
struct ValidationIssue
{
    ValidationSeverity severity{ValidationSeverity::kWarning};
    ValidationCategory category{ValidationCategory::kBrokenLink};
    std::string message;
    int line_number{0}; ///< 0 if not associated with a specific line
};

/// Summary counts from a validation run.
struct ValidationSummary
{
    int32_t errors{0};
    int32_t warnings{0};
    int32_t infos{0};
    [[nodiscard]] auto total() const -> int32_t
    {
        return errors + warnings + infos;
    }
    [[nodiscard]] auto has_errors() const -> bool
    {
        return errors > 0;
    }
};

// ============================================================================
// ExportValidator
// ============================================================================

/// ExportValidator — runs a series of checks on markdown content before
/// export to catch broken links, missing assets, and format compatibility issues.
class ExportValidator
{
public:
    ExportValidator();

    // ----- Core API ---------------------------------------------------------

    /// Run all validation checks against the content.
    [[nodiscard]] auto validate(const std::string& markdown_content,
                                ExportFormat target_format = ExportFormat::HTML) const
        -> std::vector<ValidationIssue>;

    /// Get a summary of the last validation result.
    [[nodiscard]] static auto summary(const std::vector<ValidationIssue>& issues)
        -> ValidationSummary;

    // ----- Individual checks ------------------------------------------------

    /// Check for broken internal links (e.g., [[non-existent]]).
    [[nodiscard]] auto check_links(const std::string& content) const
        -> std::vector<ValidationIssue>;

    /// Check for missing asset references (images, embedded files).
    [[nodiscard]] auto check_assets(const std::string& content) const
        -> std::vector<ValidationIssue>;

    /// Validate YAML frontmatter structure.
    [[nodiscard]] static auto check_frontmatter(const std::string& content)
        -> std::vector<ValidationIssue>;

    /// Warn about syntax not supported by the target format.
    [[nodiscard]] static auto check_format_compatibility(const std::string& content,
                                                         ExportFormat format)
        -> std::vector<ValidationIssue>;

    /// Check for overly large content.
    [[nodiscard]] static auto check_size(const std::string& content)
        -> std::vector<ValidationIssue>;

    /// Check for duplicate headings that could cause anchor conflicts.
    [[nodiscard]] static auto check_duplicate_headings(const std::string& content)
        -> std::vector<ValidationIssue>;

    /// (#100) Return the number of available validation checks.
    [[nodiscard]] static auto check_count() -> std::size_t;

    // ----- Configuration ----------------------------------------------------

    /// Set the base directory for resolving relative asset paths.
    auto set_base_directory(const std::string& dir) -> void;

    /// Set the maximum recommended content size (bytes). Default: 1MB.
    auto set_max_size(int64_t max_bytes) -> void;

private:
    std::string base_directory_;
    int64_t max_size_{1024LL * 1024LL}; // 1 MB

    /// Extract image paths from markdown content.
    [[nodiscard]] static auto extract_image_paths(const std::string& content)
        -> std::vector<std::pair<std::string, int>>; // path, line_number

    /// Extract link targets from markdown content.
    [[nodiscard]] static auto extract_link_targets(const std::string& content)
        -> std::vector<std::pair<std::string, int>>; // target, line_number
};

} // namespace markamp::core
