#pragma once

/// @file WorkspaceTrustService.h
/// @brief Phase 40 Task 4 — Workspace trust management.

#include "WorkspaceConfig.h"

#include <string>
#include <unordered_set>

namespace markamp::core
{

/// Manages workspace trust state and restrictions.
class WorkspaceTrustService
{
public:
    WorkspaceTrustService() = default;

    /// Check if a workspace path is trusted.
    [[nodiscard]] auto is_trusted(const std::string& workspace_path) const -> bool;

    /// Get the trust level for a workspace.
    [[nodiscard]] auto trust_level(const std::string& workspace_path) const -> WorkspaceTrustLevel;

    /// Grant trust to a workspace.
    void grant_trust(const std::string& workspace_path);

    /// Revoke trust from a workspace.
    void revoke_trust(const std::string& workspace_path);

    /// Get all trusted workspace paths.
    [[nodiscard]] auto trusted_workspaces() const -> std::vector<std::string>;

    /// Check if a specific feature is allowed in the current trust level.
    [[nodiscard]] auto is_feature_allowed(const std::string& feature,
                                          WorkspaceTrustLevel level) const -> bool;

    /// Restricted features: task execution, terminal commands, extension activation.
    [[nodiscard]] auto restricted_features() const -> std::vector<std::string>;

    /// Persist trust settings.
    void save(const std::string& config_dir) const;

    /// Load trust settings.
    void load(const std::string& config_dir);

private:
    std::unordered_set<std::string> trusted_paths_;
};

} // namespace markamp::core
