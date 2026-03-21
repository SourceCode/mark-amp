/// @file SessionRestoreService.cpp
/// @brief V20 P05-T03/T04: Session restore and autosave implementation.

#include "SessionRestoreService.h"

#include "Config.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

SessionRestoreService::SessionRestoreService(EventBus& bus, ArtifactRegistry& registry,
                                               Config& config)
    : event_bus_(bus)
    , registry_(registry)
    , config_(config)
{
    // Load autosave policy from config (defaults: enabled=true, interval=30s, focus_loss=true)
    policy_.enabled = config_.get_bool("autosave.enabled", true);
    policy_.interval_seconds = config_.get_int("autosave.interval", 30);
    policy_.save_on_focus_loss = config_.get_bool("autosave.on_focus_loss", true);
}

auto SessionRestoreService::capture_session() const -> std::vector<ArtifactSessionRecord>
{
    std::vector<ArtifactSessionRecord> records;
    const auto& active_id = registry_.active_artifact();

    for (const auto& artifact : registry_.all_artifacts())
    {
        ArtifactSessionRecord record;
        record.artifact_id = artifact.id;
        record.kind = artifact.kind;
        record.display_name = artifact.display_name;
        record.file_path = artifact.file_path;
        record.language_id = artifact.language_id;
        record.was_active = (artifact.id == active_id);
        record.was_dirty = artifact.is_dirty() || artifact.is_unsaved();
        records.push_back(std::move(record));
    }

    MARKAMP_LOG_DEBUG("Session captured: {} artifacts", records.size());
    return records;
}

auto SessionRestoreService::apply_session(const std::vector<ArtifactSessionRecord>& records) -> int
{
    int restored = 0;
    ArtifactId active_id;

    for (const auto& session : records)
    {
        // Re-register each artifact
        ArtifactRecord artifact;
        artifact.id = ArtifactRegistry::generate_id();
        artifact.kind = session.kind;
        artifact.display_name = session.display_name;
        artifact.file_path = session.file_path;
        artifact.language_id = session.language_id;
        artifact.source = "session-restore";

        if (session.file_path.has_value())
        {
            artifact.state = ArtifactLifecycleState::kSaved;
        }
        else
        {
            artifact.state = ArtifactLifecycleState::kUnsaved;
        }

        auto id = registry_.register_artifact(std::move(artifact));
        ++restored;

        if (session.was_active)
        {
            active_id = id;
        }
    }

    // Restore active artifact last
    if (!active_id.empty())
    {
        registry_.set_active_artifact(active_id);
    }

    ++restore_count_;
    MARKAMP_LOG_INFO("Session restored: {} artifacts", restored);
    return restored;
}

void SessionRestoreService::set_autosave_policy(const AutosavePolicy& policy)
{
    policy_ = policy;
    MARKAMP_LOG_DEBUG("Autosave policy updated: enabled={}, interval={}s",
                      policy_.enabled, policy_.interval_seconds);
}

auto SessionRestoreService::should_autosave() const -> bool
{
    if (!policy_.enabled)
    {
        return false;
    }

    // Check if any artifact is dirty
    for (const auto& artifact : registry_.all_artifacts())
    {
        if (artifact.is_dirty())
        {
            return true;
        }
    }
    return false;
}

auto SessionRestoreService::trigger_autosave() -> int
{
    if (!policy_.enabled)
    {
        return 0;
    }

    ++autosave_count_;
    int saved_count = 0;

    for (const auto& artifact : registry_.all_artifacts())
    {
        if (artifact.is_dirty() && artifact.has_path())
        {
            (void)registry_.set_state(artifact.id, ArtifactLifecycleState::kSaving);
            (void)registry_.set_state(artifact.id, ArtifactLifecycleState::kSaved);
            ++saved_count;
        }
    }

    MARKAMP_LOG_DEBUG("Autosave triggered: {} artifacts saved", saved_count);
    return saved_count;
}

} // namespace markamp::core
