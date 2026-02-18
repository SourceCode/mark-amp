// ============================================================================
// File: src/core/WorkspaceSharing.h
// Phase 27: Cloud Sync & Collaboration — Workspace sharing
// ============================================================================
#pragma once

#include "CloudSyncTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;

// Share activity record — who did what in a shared workspace.
struct ShareActivity
{
    std::string participant;
    std::string action; // "modified", "added", "deleted"
    std::string file_path;
    int64_t timestamp{0};
};

// WorkspaceSharing — manages workspace sharing and collaboration.
class WorkspaceSharing
{
public:
    explicit WorkspaceSharing(EventBus& event_bus);

    // Create a new share for a workspace.
    auto create_share(const std::string& workspace_path, SharePermission permission) -> std::string;

    // Revoke an existing share.
    auto revoke_share(const std::string& share_id) -> bool;

    // Get a specific share by ID.
    [[nodiscard]] auto get_share(const std::string& share_id) const
        -> std::optional<WorkspaceShareInfo>;

    // List all active shares.
    [[nodiscard]] auto list_shares() const -> std::vector<WorkspaceShareInfo>;
    [[nodiscard]] auto share_count() const -> int32_t;

    // Update share permission.
    auto update_permission(const std::string& share_id, SharePermission permission) -> bool;

    // Add/remove participants.
    auto add_participant(const std::string& share_id, const std::string& participant) -> bool;
    auto remove_participant(const std::string& share_id, const std::string& participant) -> bool;

    // Check if a workspace is shared.
    [[nodiscard]] auto is_shared(const std::string& workspace_path) const -> bool;

    // Get shares for a specific workspace.
    [[nodiscard]] auto shares_for_workspace(const std::string& workspace_path) const
        -> std::vector<WorkspaceShareInfo>;

    // Activity tracking.
    auto record_activity(const std::string& share_id, const ShareActivity& activity) -> void;
    [[nodiscard]] auto get_activities(const std::string& share_id) const
        -> std::vector<ShareActivity>;
    [[nodiscard]] auto recent_activities(int32_t limit = 10) const -> std::vector<ShareActivity>;

private:
    EventBus& event_bus_;

    std::vector<WorkspaceShareInfo> shares_;
    std::vector<ShareActivity> activities_;
    int32_t next_share_id_{1};

    // Generate a unique share ID.
    auto generate_share_id() -> std::string;
};

} // namespace markamp::core
