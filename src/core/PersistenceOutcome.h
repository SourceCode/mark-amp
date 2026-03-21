/// @file PersistenceOutcome.h
/// @brief V20 P05-T01: Authoritative save outcomes.
///
/// Replaces optimistic save orchestration with verified write results.
/// No artifact is marked clean after a failed save. Provides save result
/// objects, per-artifact save executors, and transactional dirty-state updates.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <chrono>
#include <optional>
#include <string>

namespace markamp::core
{

/// Detailed result of a save operation.
struct SaveOutcome
{
    bool success{false};
    ArtifactId artifact_id;
    std::string file_path;
    std::string error_message;
    std::optional<std::string> error_code;      ///< e.g. "PERMISSION_DENIED", "DISK_FULL"
    size_t bytes_written{0};
    std::chrono::milliseconds duration{0};
    bool was_first_save{false};
    bool was_atomic{false};                      ///< Whether atomic write was used

    [[nodiscard]] auto ok() const noexcept -> bool { return success; }
    [[nodiscard]] auto is_retriable() const noexcept -> bool
    {
        return !success && error_code != "PERMISSION_DENIED";
    }
};

/// Save executor callback type — performs actual I/O and returns outcome.
using SaveExecutor = std::function<SaveOutcome(const ArtifactId&, const std::string& path)>;

/// Coordinates authoritative persistence with verified outcomes.
///
/// Key invariant: dirty state is only cleared after confirmed write success.
class PersistenceOutcomeService
{
public:
    PersistenceOutcomeService(EventBus& bus, ArtifactRegistry& registry);

    /// Execute a save with verified outcome.
    [[nodiscard]] auto execute_save(const ArtifactId& artifact_id,
                                     const std::string& file_path) -> SaveOutcome;

    /// Execute a save using a custom executor.
    [[nodiscard]] auto execute_save_with(const ArtifactId& artifact_id,
                                          const std::string& file_path,
                                          const SaveExecutor& executor) -> SaveOutcome;

    /// Retry a previously failed save.
    [[nodiscard]] auto retry_save(const ArtifactId& artifact_id) -> SaveOutcome;

    /// Record a save failure without clearing dirty state.
    void record_failure(const ArtifactId& artifact_id, const std::string& error);

    /// Get the last save outcome for an artifact.
    [[nodiscard]] auto last_outcome(const ArtifactId& artifact_id) const
        -> std::optional<SaveOutcome>;

    /// Total save operations.
    [[nodiscard]] auto save_count() const noexcept -> int { return save_count_; }

    /// Total failed saves.
    [[nodiscard]] auto failure_count() const noexcept -> int { return failure_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    std::unordered_map<std::string, SaveOutcome> last_outcomes_;
    int save_count_{0};
    int failure_count_{0};
};

} // namespace markamp::core
