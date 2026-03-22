/// @file ArtifactLifecycleValidator.h
/// @brief V24 P02-T04: Validates artifact lifecycle consistency.
///
/// Ensures every artifact has consistent metadata, a single owner,
/// correct state transitions, and no orphaned references.  Used by
/// smoke tests and runtime diagnostics to catch lifecycle violations.
#pragma once

#include "ArtifactRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Types of lifecycle violations.
enum class LifecycleViolation
{
    kMissingDisplayName,      ///< Artifact has no display name
    kMissingLanguageId,       ///< Text file has no language ID
    kUnsavedWithPath,         ///< kUnsaved state but has a file path
    kSavedWithoutPath,        ///< kSaved state but no file path
    kInvalidStateTransition,  ///< Illegal state transition detected
    kDuplicatePath,           ///< Two artifacts share the same file path
    kOrphanedUnsaved,         ///< Unsaved with no buffer backing
    kMissingSource            ///< No creation source recorded
};

/// Returns a human-readable label for a LifecycleViolation.
[[nodiscard]] constexpr auto violation_label(LifecycleViolation v) noexcept -> const char*
{
    switch (v) {
        case LifecycleViolation::kMissingDisplayName:     return "MissingDisplayName";
        case LifecycleViolation::kMissingLanguageId:      return "MissingLanguageId";
        case LifecycleViolation::kUnsavedWithPath:        return "UnsavedWithPath";
        case LifecycleViolation::kSavedWithoutPath:       return "SavedWithoutPath";
        case LifecycleViolation::kInvalidStateTransition: return "InvalidStateTransition";
        case LifecycleViolation::kDuplicatePath:          return "DuplicatePath";
        case LifecycleViolation::kOrphanedUnsaved:        return "OrphanedUnsaved";
        case LifecycleViolation::kMissingSource:          return "MissingSource";
    }
    return "Unknown";
}

/// A single violation found during validation.
struct LifecycleViolationEntry
{
    ArtifactId artifact_id;
    LifecycleViolation violation{LifecycleViolation::kMissingDisplayName};
    std::string detail;

    [[nodiscard]] auto label() const noexcept -> const char*
    {
        return violation_label(violation);
    }
};

/// Validation result.
struct LifecycleValidationResult
{
    int total_artifacts{0};
    int valid_artifacts{0};
    int violated_artifacts{0};
    std::vector<LifecycleViolationEntry> violations;

    [[nodiscard]] auto is_clean() const noexcept -> bool
    {
        return violations.empty();
    }

    [[nodiscard]] auto violation_count() const noexcept -> int
    {
        return static_cast<int>(violations.size());
    }

    [[nodiscard]] auto validity_ratio() const noexcept -> double
    {
        return total_artifacts > 0
            ? static_cast<double>(valid_artifacts) / static_cast<double>(total_artifacts)
            : 1.0;
    }
};

/// Validates artifact lifecycle consistency across the registry.
class ArtifactLifecycleValidator
{
public:
    ArtifactLifecycleValidator() = default;

    /// Validate all artifacts in a registry.
    [[nodiscard]] auto validate(const ArtifactRegistry& registry) const
        -> LifecycleValidationResult;

    /// Validate a single artifact record.
    [[nodiscard]] auto validate_record(const ArtifactRecord& record) const
        -> std::vector<LifecycleViolationEntry>;

    /// Check if a state transition is valid.
    [[nodiscard]] static auto is_valid_transition(ArtifactLifecycleState from,
                                                    ArtifactLifecycleState to) -> bool;

    /// Export validation result as JSON.
    [[nodiscard]] static auto export_json(const LifecycleValidationResult& result)
        -> std::string;

    /// Export validation result as Markdown.
    [[nodiscard]] static auto export_markdown(const LifecycleValidationResult& result)
        -> std::string;
};

} // namespace markamp::core
