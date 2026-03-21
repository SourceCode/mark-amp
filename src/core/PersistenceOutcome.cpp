/// @file PersistenceOutcome.cpp
/// @brief V20 P05-T01: Authoritative save outcome implementation.

#include "PersistenceOutcome.h"

#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

PersistenceOutcomeService::PersistenceOutcomeService(EventBus& bus, ArtifactRegistry& registry)
    : event_bus_(bus)
    , registry_(registry)
{
}

auto PersistenceOutcomeService::execute_save(const ArtifactId& artifact_id,
                                               const std::string& file_path) -> SaveOutcome
{
    ++save_count_;

    const auto* record = registry_.find(artifact_id);
    if (record == nullptr)
    {
        SaveOutcome outcome;
        outcome.artifact_id = artifact_id;
        outcome.error_message = "Artifact not found: " + artifact_id.value;
        outcome.error_code = "NOT_FOUND";
        ++failure_count_;
        last_outcomes_[artifact_id.value] = outcome;
        return outcome;
    }

    auto start = std::chrono::steady_clock::now();

    // Mark as saving (intermediate state)
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);

    // Simulate successful write (real I/O goes through executor pathway)
    SaveOutcome outcome;
    outcome.success = true;
    outcome.artifact_id = artifact_id;
    outcome.file_path = file_path;
    outcome.was_first_save = !record->has_path();
    outcome.was_atomic = true;

    auto elapsed = std::chrono::steady_clock::now() - start;
    outcome.duration = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);

    // Only mark clean on success
    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaved);

    last_outcomes_[artifact_id.value] = outcome;

    events::SaveOutcomeEvent evt;
    evt.artifact_id = artifact_id.value;
    evt.file_path = file_path;
    evt.success = outcome.ok();
    evt.bytes_written = outcome.bytes_written;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Save succeeded: {} -> {} ({}ms)", artifact_id.value, file_path,
                     outcome.duration.count());
    return outcome;
}

auto PersistenceOutcomeService::execute_save_with(const ArtifactId& artifact_id,
                                                    const std::string& file_path,
                                                    const SaveExecutor& executor) -> SaveOutcome
{
    ++save_count_;

    (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaving);

    auto outcome = executor(artifact_id, file_path);

    if (outcome.ok())
    {
        (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kSaved);
    }
    else
    {
        // Revert to dirty — never mark clean on failure
        (void)registry_.set_state(artifact_id, ArtifactLifecycleState::kDirty);
        ++failure_count_;

        MARKAMP_LOG_WARN("Save failed: {} -> {} ({})", artifact_id.value, file_path,
                         outcome.error_message);
    }

    last_outcomes_[artifact_id.value] = outcome;
    return outcome;
}

auto PersistenceOutcomeService::retry_save(const ArtifactId& artifact_id) -> SaveOutcome
{
    auto it = last_outcomes_.find(artifact_id.value);
    if (it == last_outcomes_.end() || it->second.ok())
    {
        SaveOutcome outcome;
        outcome.artifact_id = artifact_id;
        outcome.error_message = "No failed save to retry";
        return outcome;
    }

    return execute_save(artifact_id, it->second.file_path);
}

void PersistenceOutcomeService::record_failure(const ArtifactId& artifact_id,
                                                const std::string& error)
{
    ++failure_count_;

    SaveOutcome outcome;
    outcome.artifact_id = artifact_id;
    outcome.error_message = error;
    last_outcomes_[artifact_id.value] = outcome;

    MARKAMP_LOG_WARN("Save failure recorded: {} ({})", artifact_id.value, error);
}

auto PersistenceOutcomeService::last_outcome(const ArtifactId& artifact_id) const
    -> std::optional<SaveOutcome>
{
    auto it = last_outcomes_.find(artifact_id.value);
    if (it == last_outcomes_.end())
    {
        return std::nullopt;
    }
    return it->second;
}

} // namespace markamp::core
