/// @file ArtifactLifecycleValidator.cpp
/// @brief V24 P02-T04: Artifact lifecycle validation implementation.
#include "ArtifactLifecycleValidator.h"

#include <sstream>
#include <unordered_set>

namespace markamp::core
{

auto ArtifactLifecycleValidator::validate(const ArtifactRegistry& registry) const
    -> LifecycleValidationResult
{
    LifecycleValidationResult result;
    auto all = registry.all_artifacts();
    result.total_artifacts = static_cast<int>(all.size());

    // Track paths for duplicate detection
    std::unordered_set<std::string> seen_paths;

    for (const auto& record : all) {
        auto violations = validate_record(record);

        // Check for duplicate paths
        if (record.has_path()) {
            if (seen_paths.count(*record.file_path)) {
                violations.push_back({
                    .artifact_id = record.id,
                    .violation = LifecycleViolation::kDuplicatePath,
                    .detail = "Path: " + *record.file_path
                });
            }
            seen_paths.insert(*record.file_path);
        }

        if (violations.empty()) {
            ++result.valid_artifacts;
        } else {
            ++result.violated_artifacts;
            for (auto& v : violations) {
                result.violations.push_back(std::move(v));
            }
        }
    }
    return result;
}

auto ArtifactLifecycleValidator::validate_record(const ArtifactRecord& record) const
    -> std::vector<LifecycleViolationEntry>
{
    std::vector<LifecycleViolationEntry> violations;

    // P02-T02: Ensure display name is set
    if (record.display_name.empty()) {
        violations.push_back({
            .artifact_id = record.id,
            .violation = LifecycleViolation::kMissingDisplayName,
            .detail = "Artifact has no display name"
        });
    }

    // P02-T03: Ensure language ID is set for text files
    if (record.is_text_file() && record.language_id.empty()) {
        violations.push_back({
            .artifact_id = record.id,
            .violation = LifecycleViolation::kMissingLanguageId,
            .detail = "Text file artifact has no language_id"
        });
    }

    // P02-T01: Unsaved artifact should not have a path
    if (record.is_unsaved() && record.has_path()) {
        violations.push_back({
            .artifact_id = record.id,
            .violation = LifecycleViolation::kUnsavedWithPath,
            .detail = "Unsaved artifact should not have file_path"
        });
    }

    // Saved artifact must have a path
    if (record.is_saved() && !record.has_path()) {
        violations.push_back({
            .artifact_id = record.id,
            .violation = LifecycleViolation::kSavedWithoutPath,
            .detail = "Saved artifact must have file_path"
        });
    }

    // P02-T02: Source should be recorded
    if (record.source.empty()) {
        violations.push_back({
            .artifact_id = record.id,
            .violation = LifecycleViolation::kMissingSource,
            .detail = "No creation source recorded"
        });
    }

    return violations;
}

auto ArtifactLifecycleValidator::is_valid_transition(ArtifactLifecycleState from,
                                                      ArtifactLifecycleState to) -> bool
{
    // Valid transitions:
    // kUnsaved -> kSaving, kDeleted
    // kSaving  -> kSaved, kSaveError
    // kSaved   -> kDirty, kDeleted
    // kDirty   -> kSaving, kDeleted
    // kSaveError -> kSaving, kDirty, kDeleted
    // kDeleted -> (none - terminal)
    switch (from) {
        case ArtifactLifecycleState::kUnsaved:
            return to == ArtifactLifecycleState::kSaving ||
                   to == ArtifactLifecycleState::kDeleted;
        case ArtifactLifecycleState::kSaving:
            return to == ArtifactLifecycleState::kSaved ||
                   to == ArtifactLifecycleState::kSaveError;
        case ArtifactLifecycleState::kSaved:
            return to == ArtifactLifecycleState::kDirty ||
                   to == ArtifactLifecycleState::kDeleted;
        case ArtifactLifecycleState::kDirty:
            return to == ArtifactLifecycleState::kSaving ||
                   to == ArtifactLifecycleState::kDeleted;
        case ArtifactLifecycleState::kSaveError:
            return to == ArtifactLifecycleState::kSaving ||
                   to == ArtifactLifecycleState::kDirty ||
                   to == ArtifactLifecycleState::kDeleted;
        case ArtifactLifecycleState::kDeleted:
            return false; // terminal
    }
    return false;
}

auto ArtifactLifecycleValidator::export_json(const LifecycleValidationResult& result)
    -> std::string
{
    std::ostringstream ss;
    ss << "{\n  \"total\": " << result.total_artifacts
       << ",\n  \"valid\": " << result.valid_artifacts
       << ",\n  \"violated\": " << result.violated_artifacts
       << ",\n  \"violations\": " << result.violation_count()
       << "\n}";
    return ss.str();
}

auto ArtifactLifecycleValidator::export_markdown(const LifecycleValidationResult& result)
    -> std::string
{
    std::ostringstream ss;
    ss << "# Artifact Lifecycle Validation\n\n";
    ss << "| Metric | Value |\n";
    ss << "|--------|-------|\n";
    ss << "| Total | " << result.total_artifacts << " |\n";
    ss << "| Valid | " << result.valid_artifacts << " |\n";
    ss << "| Violated | " << result.violated_artifacts << " |\n";
    if (!result.violations.empty()) {
        ss << "\n## Violations\n\n";
        for (const auto& v : result.violations) {
            ss << "- **" << v.label() << "**: " << v.detail << "\n";
        }
    }
    return ss.str();
}

} // namespace markamp::core
