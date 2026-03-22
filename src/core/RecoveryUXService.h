/// @file RecoveryUXService.h
/// @brief V24 P03-T03: Recovery dialog model for crash/restore workflows.
///
/// Provides the data model for a recovery dialog that lists recoverable
/// artifacts and lets the user accept or discard each one.  Integrates
/// with AutosaveService to discover and restore stale autosaves.
#pragma once

#include "ArtifactRegistry.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// A single recoverable artifact entry.
struct RecoverableArtifact
{
    ArtifactId artifact_id;
    ArtifactKind kind{ArtifactKind::kTextFile};
    std::string display_name;
    std::string autosave_path;          ///< Path to autosave data
    std::chrono::steady_clock::time_point last_modified;
    bool is_accepted{false};            ///< User chose to recover
    bool is_discarded{false};           ///< User chose to discard

    [[nodiscard]] auto is_pending() const noexcept -> bool
    {
        return !is_accepted && !is_discarded;
    }
};

/// Recovery dialog action taken by user.
enum class RecoveryAction
{
    kAcceptAll,
    kDiscardAll,
    kSelective,
    kDismissed
};

/// Result of recovery dialog interaction.
struct RecoveryResult
{
    RecoveryAction action{RecoveryAction::kDismissed};
    int accepted{0};
    int discarded{0};
    int total{0};

    [[nodiscard]] auto was_acted_upon() const noexcept -> bool
    {
        return action != RecoveryAction::kDismissed;
    }

    [[nodiscard]] auto any_accepted() const noexcept -> bool
    {
        return accepted > 0;
    }
};

/// Manages the recovery UX model: discovery, presentation, and user decisions.
class RecoveryUXService
{
public:
    RecoveryUXService() = default;

    /// Add a recoverable artifact.
    void add_recoverable(const RecoverableArtifact& artifact);

    /// Accept a specific artifact for recovery.
    [[nodiscard]] auto accept(const ArtifactId& id) -> bool;

    /// Discard a specific artifact.
    [[nodiscard]] auto discard(const ArtifactId& id) -> bool;

    /// Accept all pending artifacts.
    void accept_all();

    /// Discard all pending artifacts.
    void discard_all();

    /// Get all recoverable artifacts.
    [[nodiscard]] auto all_recoverables() const -> const std::vector<RecoverableArtifact>&
    {
        return recoverables_;
    }

    /// Get pending (unresolved) artifacts.
    [[nodiscard]] auto pending_count() const noexcept -> int;

    /// Get accepted artifacts.
    [[nodiscard]] auto accepted_count() const noexcept -> int;

    /// Get discarded artifacts.
    [[nodiscard]] auto discarded_count() const noexcept -> int;

    /// Total recoverable count.
    [[nodiscard]] auto total_count() const noexcept -> int
    {
        return static_cast<int>(recoverables_.size());
    }

    /// Has any pending recoveries?
    [[nodiscard]] auto has_pending() const noexcept -> bool
    {
        return pending_count() > 0;
    }

    /// Build result from current state.
    [[nodiscard]] auto result() const -> RecoveryResult;

    /// Clear all entries.
    void clear();

private:
    std::vector<RecoverableArtifact> recoverables_;
};

} // namespace markamp::core
