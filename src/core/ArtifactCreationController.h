/// @file ArtifactCreationController.h
/// @brief V25 P02-T01: Shell-level artifact creation controller.
///
/// Provides the single entry point for all UI surfaces to create artifacts,
/// replacing direct Untitled-* generation in TabBar, FileTreeCtrl, MainFrame.
#pragma once

#include "ArtifactCreationService.h"

#include <string>
#include <vector>

namespace markamp::core
{

/// Tracks creation routing for audit purposes.
struct CreationRouteRecord
{
    std::string source_surface;   ///< e.g. "TabBar", "FileTreeCtrl", "MainFrame"
    ArtifactKind kind{ArtifactKind::kTextFile};
    ArtifactId artifact_id;
    bool success{false};
};

/// Shell-level controller routing all artifact creation through canonical service.
class ArtifactCreationController
{
public:
    explicit ArtifactCreationController(ArtifactCreationService& service);

    /// Create from any surface. Routes through ArtifactCreationService.
    [[nodiscard]] auto create_from_surface(const std::string& surface,
                                            ArtifactKind kind,
                                            const std::string& name = {},
                                            const std::string& language = "markdown")
        -> ArtifactCreationResult;

    /// Creation history for audit.
    [[nodiscard]] auto route_history() const -> const std::vector<CreationRouteRecord>&
    {
        return history_;
    }

    /// Total routed creations.
    [[nodiscard]] auto routed_count() const noexcept -> int
    {
        return static_cast<int>(history_.size());
    }

    /// Count by surface.
    [[nodiscard]] auto count_by_surface(const std::string& surface) const -> int;

    /// Verify no direct creation bypassed this controller.
    [[nodiscard]] auto all_routed() const noexcept -> bool { return bypass_count_ == 0; }

    /// Record a bypass for audit.
    void record_bypass(const std::string& surface);

    /// Bypass count.
    [[nodiscard]] auto bypass_count() const noexcept -> int { return bypass_count_; }

private:
    ArtifactCreationService& service_;
    std::vector<CreationRouteRecord> history_;
    int bypass_count_{0};
};

} // namespace markamp::core
