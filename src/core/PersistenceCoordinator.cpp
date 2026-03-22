/// @file PersistenceCoordinator.cpp
/// @brief V25 P03: Persistence coordinator implementation.
#include "core/PersistenceCoordinator.h"

namespace markamp::core
{

PersistenceCoordinator::PersistenceCoordinator(EventBus& bus, ArtifactRegistry& registry)
    : event_bus_(bus), registry_(registry)
{
}

auto PersistenceCoordinator::save(const ArtifactId& artifact_id) -> SaveDispatchResult
{
    SaveDispatchResult result;
    result.artifact_id = artifact_id.value;

    const auto* record = registry_.find(artifact_id);
    if (!record) {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    result.kind = record->kind;
    result.success = true;
    ++save_count_;
    return result;
}

auto PersistenceCoordinator::save_as(const ArtifactId& artifact_id,
                                      const std::string& /*new_path*/) -> SaveDispatchResult
{
    SaveDispatchResult result;
    result.artifact_id = artifact_id.value;

    const auto* record = registry_.find(artifact_id);
    if (!record) {
        result.error_message = "Artifact not found: " + artifact_id.value;
        return result;
    }

    result.kind = record->kind;
    result.success = true;
    ++save_count_;
    return result;
}

auto PersistenceCoordinator::create_recovery_metadata(const ArtifactId& artifact_id)
    -> RecoveryDraftMetadata
{
    RecoveryDraftMetadata meta;
    meta.artifact_id = artifact_id.value;

    const auto* record = registry_.find(artifact_id);
    if (record) {
        meta.kind = record->kind;
        meta.draft_path = "/tmp/markamp_recovery/" + artifact_id.value + ".draft";
        meta.is_valid = true;
        ++draft_count_;
    }
    return meta;
}

auto PersistenceCoordinator::validate_restore_entry(const ArtifactId& artifact_id)
    -> RestoreEntryValidation
{
    RestoreEntryValidation validation;
    validation.entry_id = artifact_id.value;

    const auto* record = registry_.find(artifact_id);
    if (record) {
        validation.artifact_exists = true;
        validation.is_valid = true;
    } else {
        validation.reason = "Artifact no longer exists";
    }
    return validation;
}

} // namespace markamp::core
