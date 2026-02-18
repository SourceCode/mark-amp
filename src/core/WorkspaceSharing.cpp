// ============================================================================
// File: src/core/WorkspaceSharing.cpp
// Phase 27: Cloud Sync & Collaboration — Workspace sharing
// ============================================================================

#include "WorkspaceSharing.h"

#include "EventBus.h"
#include "Events.h"

#include <algorithm>
#include <chrono>

namespace markamp::core
{

WorkspaceSharing::WorkspaceSharing(EventBus& event_bus)
    : event_bus_(event_bus)
{
}

auto WorkspaceSharing::create_share(const std::string& workspace_path, SharePermission permission)
    -> std::string
{
    auto share_id = generate_share_id();

    const auto now = std::chrono::system_clock::now().time_since_epoch();
    const auto timestamp = std::chrono::duration_cast<std::chrono::seconds>(now).count();

    WorkspaceShareInfo share;
    share.share_id = share_id;
    share.workspace_path = workspace_path;
    share.permission = permission;
    share.created_at = timestamp;
    share.is_active = true;

    shares_.push_back(share);

    events::WorkspaceShareCreatedEvent evt;
    evt.share_id = share_id;
    evt.permission = static_cast<int>(permission);
    event_bus_.publish(evt);

    return share_id;
}

auto WorkspaceSharing::revoke_share(const std::string& share_id) -> bool
{
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter == shares_.end())
    {
        return false;
    }

    iter->is_active = false;
    shares_.erase(iter);
    return true;
}

auto WorkspaceSharing::get_share(const std::string& share_id) const
    -> std::optional<WorkspaceShareInfo>
{
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter != shares_.end())
    {
        return *iter;
    }
    return std::nullopt;
}

auto WorkspaceSharing::list_shares() const -> std::vector<WorkspaceShareInfo>
{
    return shares_;
}

auto WorkspaceSharing::share_count() const -> int32_t
{
    return static_cast<int32_t>(shares_.size());
}

auto WorkspaceSharing::update_permission(const std::string& share_id, SharePermission permission)
    -> bool
{
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter == shares_.end())
    {
        return false;
    }

    iter->permission = permission;
    return true;
}

auto WorkspaceSharing::add_participant(const std::string& share_id, const std::string& participant)
    -> bool
{
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter == shares_.end())
    {
        return false;
    }

    // Avoid duplicate participants.
    auto part_iter = std::find(iter->shared_with.begin(), iter->shared_with.end(), participant);
    if (part_iter == iter->shared_with.end())
    {
        iter->shared_with.push_back(participant);
    }
    return true;
}

auto WorkspaceSharing::remove_participant(const std::string& share_id,
                                          const std::string& participant) -> bool
{
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter == shares_.end())
    {
        return false;
    }

    auto part_iter = std::find(iter->shared_with.begin(), iter->shared_with.end(), participant);
    if (part_iter != iter->shared_with.end())
    {
        iter->shared_with.erase(part_iter);
        return true;
    }
    return false;
}

auto WorkspaceSharing::is_shared(const std::string& workspace_path) const -> bool
{
    return std::any_of(shares_.begin(),
                       shares_.end(),
                       [&workspace_path](const WorkspaceShareInfo& share)
                       { return share.workspace_path == workspace_path && share.is_active; });
}

auto WorkspaceSharing::shares_for_workspace(const std::string& workspace_path) const
    -> std::vector<WorkspaceShareInfo>
{
    std::vector<WorkspaceShareInfo> results;
    for (const auto& share : shares_)
    {
        if (share.workspace_path == workspace_path && share.is_active)
        {
            results.push_back(share);
        }
    }
    return results;
}

auto WorkspaceSharing::record_activity(const std::string& share_id, const ShareActivity& activity)
    -> void
{
    // Verify the share exists.
    auto iter = std::find_if(shares_.begin(),
                             shares_.end(),
                             [&share_id](const WorkspaceShareInfo& share)
                             { return share.share_id == share_id; });

    if (iter == shares_.end())
    {
        return;
    }

    activities_.push_back(activity);

    // Cap activities at 1000.
    if (activities_.size() > 1000)
    {
        activities_.erase(activities_.begin());
    }
}

auto WorkspaceSharing::get_activities(const std::string& share_id) const
    -> std::vector<ShareActivity>
{
    // Return activities for the workspace associated with this share.
    auto share = get_share(share_id);
    if (!share)
    {
        return {};
    }

    return activities_;
}

auto WorkspaceSharing::recent_activities(int32_t limit) const -> std::vector<ShareActivity>
{
    if (activities_.empty())
    {
        return {};
    }

    const auto count = std::min(static_cast<int32_t>(activities_.size()), limit);
    return {activities_.end() - count, activities_.end()};
}

auto WorkspaceSharing::generate_share_id() -> std::string
{
    return "share_" + std::to_string(next_share_id_++);
}

} // namespace markamp::core
