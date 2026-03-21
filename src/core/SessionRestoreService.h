/// @file SessionRestoreService.h
/// @brief V20 P05-T03/T04: Artifact-aware session restore and autosave policy.
///
/// Rebuilds session restore around artifact records instead of loose path
/// snapshots. Provides unified autosave policy across all artifact types.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class Config;

/// Snapshot of an artifact's state for session persistence.
struct ArtifactSessionRecord
{
    ArtifactId artifact_id;
    ArtifactKind kind{ArtifactKind::kTextFile};
    std::string display_name;
    std::optional<std::string> file_path;
    std::string language_id;
    bool was_active{false};
    bool was_dirty{false};
    int cursor_line{0};
    int cursor_column{0};
};

/// Autosave policy configuration.
struct AutosavePolicy
{
    bool enabled{true};
    int interval_seconds{30};
    bool save_on_focus_loss{true};
    bool save_on_idle{true};
    int idle_threshold_seconds{5};

    [[nodiscard]] auto is_enabled() const noexcept -> bool { return enabled; }
};

/// Orchestrates artifact-aware session restore and autosave policy.
class SessionRestoreService
{
public:
    SessionRestoreService(EventBus& bus, ArtifactRegistry& registry, Config& config);

    // ── Session Snapshot ──

    /// Capture current session state as artifact records.
    [[nodiscard]] auto capture_session() const -> std::vector<ArtifactSessionRecord>;

    /// Apply a session snapshot to restore the workspace.
    [[nodiscard]] auto apply_session(const std::vector<ArtifactSessionRecord>& records) -> int;

    // ── Autosave Policy ──

    /// Get current autosave policy.
    [[nodiscard]] auto autosave_policy() const -> const AutosavePolicy& { return policy_; }

    /// Update autosave policy.
    void set_autosave_policy(const AutosavePolicy& policy);

    /// Check if autosave should trigger now.
    [[nodiscard]] auto should_autosave() const -> bool;

    /// Trigger autosave for all dirty artifacts.
    [[nodiscard]] auto trigger_autosave() -> int;

    // ── Queries ──

    [[nodiscard]] auto restore_count() const noexcept -> int { return restore_count_; }
    [[nodiscard]] auto autosave_trigger_count() const noexcept -> int { return autosave_count_; }

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;
    Config& config_;
    AutosavePolicy policy_;
    int restore_count_{0};
    int autosave_count_{0};
};

} // namespace markamp::core
