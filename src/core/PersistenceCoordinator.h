/// @file PersistenceCoordinator.h
/// @brief V25 P03: Shell-owned persistence coordinator.
///
/// Dispatches save/save-as by artifact kind, replacing LayoutManager-local
/// save logic. Manages autosave recovery metadata and session restore validation.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Save dispatch result.
struct SaveDispatchResult
{
    bool success{false};
    ArtifactKind kind{ArtifactKind::kTextFile};
    std::string artifact_id;
    std::string error_message;

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
};

/// Recovery metadata for autosave drafts.
struct RecoveryDraftMetadata
{
    std::string artifact_id;
    std::string draft_path;
    std::string original_path;
    ArtifactKind kind{ArtifactKind::kTextFile};
    bool is_valid{false};

    [[nodiscard]] auto has_draft() const noexcept -> bool { return !draft_path.empty(); }
};

/// Session restore entry validation result.
struct RestoreEntryValidation
{
    std::string entry_id;
    bool is_valid{false};
    bool artifact_exists{false};
    std::string reason;
};

/// Shell-owned persistence coordinator.
class PersistenceCoordinator
{
public:
    PersistenceCoordinator(EventBus& bus, ArtifactRegistry& registry);

    /// Dispatch save by artifact kind.
    [[nodiscard]] auto save(const ArtifactId& artifact_id) -> SaveDispatchResult;

    /// Dispatch save-as by artifact kind.
    [[nodiscard]] auto save_as(const ArtifactId& artifact_id,
                                const std::string& new_path) -> SaveDispatchResult;

    /// Create recovery metadata for autosave.
    [[nodiscard]] auto create_recovery_metadata(const ArtifactId& artifact_id)
        -> RecoveryDraftMetadata;

    /// Validate a session restore entry.
    [[nodiscard]] auto validate_restore_entry(const ArtifactId& artifact_id)
        -> RestoreEntryValidation;

    /// Total saves dispatched.
    [[nodiscard]] auto save_count() const noexcept -> int { return save_count_; }

    /// Total recovery drafts created.
    [[nodiscard]] auto recovery_draft_count() const noexcept -> int { return draft_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    int save_count_{0};
    int draft_count_{0};
};

} // namespace markamp::core
