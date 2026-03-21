/// @file ArtifactMountAdapter.h
/// @brief V20 P01-T04: Canonical artifact-open and artifact-mount adapters.
///
/// Defines the mount interface for presenting artifacts in the appropriate
/// UI surface (editor, notebook host, canvas workbench). Each artifact kind
/// registers a mount adapter that knows how to display it.
#pragma once

#include "ArtifactRegistry.h"
#include "EventBus.h"

#include <string>
#include <unordered_map>

namespace markamp::core
{

/// Request to mount an artifact into a UI surface.
struct ArtifactMountRequest
{
    ArtifactId artifact_id;
    int target_surface{0};  ///< WorkbenchMode as int
    std::string context;    ///< Additional context for the mount
};

/// Result of a mount attempt.
struct ArtifactMountResult
{
    bool success{false};
    std::string error_message;
};

/// Interface for surface-specific mount adapters.
///
/// Each artifact kind (text file, notebook, canvas) provides an
/// implementation that knows how to display that artifact in its
/// corresponding workbench surface.
class IArtifactMountAdapter
{
public:
    virtual ~IArtifactMountAdapter() = default;

protected:
    IArtifactMountAdapter() = default;

public:
    IArtifactMountAdapter(const IArtifactMountAdapter&) = delete;
    auto operator=(const IArtifactMountAdapter&) -> IArtifactMountAdapter& = delete;
    IArtifactMountAdapter(IArtifactMountAdapter&&) = delete;
    auto operator=(IArtifactMountAdapter&&) -> IArtifactMountAdapter& = delete;

    /// Mount the artifact into the appropriate UI surface.
    [[nodiscard]] virtual auto mount(const ArtifactMountRequest& request,
                                      const ArtifactRecord& record) -> ArtifactMountResult = 0;

    /// Unmount a previously mounted artifact.
    [[nodiscard]] virtual auto unmount(const ArtifactId& id) -> bool = 0;
};

/// Central mount service that dispatches to kind-specific adapters.
class ArtifactMountService
{
public:
    ArtifactMountService(EventBus& bus, ArtifactRegistry& registry);

    /// Register a mount adapter for a specific artifact kind.
    void register_adapter(ArtifactKind kind, IArtifactMountAdapter* adapter);

    /// Mount an artifact using its registered adapter.
    [[nodiscard]] auto mount_artifact(const ArtifactId& id) -> ArtifactMountResult;

    /// Unmount a previously mounted artifact.
    [[nodiscard]] auto unmount_artifact(const ArtifactId& id) -> bool;

    /// Check if an artifact is currently mounted.
    [[nodiscard]] auto is_mounted(const ArtifactId& id) const -> bool;

    /// Number of currently mounted artifacts.
    [[nodiscard]] auto mounted_count() const -> int
    {
        return static_cast<int>(mounted_artifacts_.size());
    }

    /// Check if an adapter is registered for a kind.
    [[nodiscard]] auto has_adapter(ArtifactKind kind) const -> bool;

private:
    EventBus& event_bus_;
    ArtifactRegistry& registry_;

    std::unordered_map<int, IArtifactMountAdapter*> adapters_;  ///< ArtifactKind → adapter
    std::unordered_map<std::string, bool> mounted_artifacts_;   ///< artifact ID → mounted
};

} // namespace markamp::core
