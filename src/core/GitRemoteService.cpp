/// @file GitRemoteService.cpp
/// @brief Phase 25: Version Control Integration — Git remote implementation.

#include "core/GitRemoteService.h"

#include "core/EventBus.h"
#include "core/Events.h"
#include "core/GitService.h"

#include <algorithm>
#include <fmt/format.h>

namespace markamp::core
{

GitRemoteService::GitRemoteService(EventBus& event_bus, GitService& git_service)
    : event_bus_(event_bus)
    , git_service_(git_service)
{
}

auto GitRemoteService::add_remote(const std::string& name, const std::string& url)
    -> std::expected<void, std::string>
{
    if (name.empty())
    {
        return std::unexpected(std::string("Remote name cannot be empty"));
    }

    if (url.empty())
    {
        return std::unexpected(std::string("Remote URL cannot be empty"));
    }

    if (remotes_.contains(name))
    {
        return std::unexpected(fmt::format("Remote '{}' already exists", name));
    }

    RemoteInfo info;
    info.name = name;
    info.url = url;
    info.fetch_url = url;
    info.push_url = url;

    remotes_[name] = std::move(info);
    return {};
}

auto GitRemoteService::remove_remote(const std::string& name) -> std::expected<void, std::string>
{
    auto iter = remotes_.find(name);
    if (iter == remotes_.end())
    {
        return std::unexpected(fmt::format("Remote '{}' not found", name));
    }

    remotes_.erase(iter);
    remote_commits_.erase(name);
    return {};
}

auto GitRemoteService::list_remotes() const -> std::vector<RemoteInfo>
{
    std::vector<RemoteInfo> result;
    result.reserve(remotes_.size());
    for (const auto& [_name, info] : remotes_)
    {
        result.push_back(info);
    }

    std::ranges::sort(
        result, [](const RemoteInfo& lhs, const RemoteInfo& rhs) { return lhs.name < rhs.name; });
    return result;
}

auto GitRemoteService::get_remote(const std::string& name) const
    -> std::expected<RemoteInfo, std::string>
{
    auto iter = remotes_.find(name);
    if (iter == remotes_.end())
    {
        return std::unexpected(fmt::format("Remote '{}' not found", name));
    }
    return iter->second;
}

auto GitRemoteService::fetch(const std::string& remote_name) -> FetchResult
{
    FetchResult result;
    result.remote_name = remote_name;

    if (!remotes_.contains(remote_name))
    {
        return result; // No remote configured — return empty result.
    }

    // Simulate fetching: check if remote has commits we don't have locally.
    auto& remote_commits = remote_commits_[remote_name];
    auto local_commits = git_service_.log();

    // Count "new" commits as any remote commits not in local log.
    const int new_count = static_cast<int>(remote_commits.size());
    result.new_commits = new_count;

    if (new_count > 0)
    {
        result.updated_refs.push_back(remote_name + "/" + git_service_.current_branch());
    }

    events::GitRemoteFetchedEvent fetch_evt;
    fetch_evt.remote_name = remote_name;
    fetch_evt.new_commits = new_count;
    event_bus_.publish(fetch_evt);

    return result;
}

auto GitRemoteService::push(const std::string& remote_name, const std::string& branch) -> PushResult
{
    PushResult result;
    result.remote_name = remote_name;

    if (!remotes_.contains(remote_name))
    {
        result.rejected_refs.push_back(branch.empty() ? git_service_.current_branch() : branch);
        return result;
    }

    auto local_commits = git_service_.log();
    if (local_commits.empty())
    {
        result.up_to_date = true;
        return result;
    }

    const std::string ref = branch.empty() ? git_service_.current_branch() : branch;
    result.pushed_refs.push_back(ref);

    // Store local commits as "pushed" to the remote.
    auto& commits = remote_commits_[remote_name];
    for (const auto& commit : local_commits)
    {
        commits.push_back(commit.hash);
    }

    events::GitRemotePushedEvent push_evt;
    push_evt.remote_name = remote_name;
    push_evt.pushed_refs = static_cast<int>(result.pushed_refs.size());
    event_bus_.publish(push_evt);

    return result;
}

auto GitRemoteService::pull(const std::string& remote_name, const std::string& branch) -> PullResult
{
    PullResult result;
    result.remote_name = remote_name;

    if (!remotes_.contains(remote_name))
    {
        result.merge_status = PullResult::MergeStatus::kUpToDate;
        return result;
    }

    // First fetch.
    auto fetch_result = fetch(remote_name);

    if (fetch_result.new_commits == 0)
    {
        result.merge_status = PullResult::MergeStatus::kUpToDate;
        return result;
    }

    // Simulate merge: if there are no local uncommitted changes, fast-forward.
    auto status_entries = git_service_.status();
    bool has_local_changes = false;
    for (const auto& entry : status_entries)
    {
        if (entry.status != GitFileStatus::kUnmodified && entry.status != GitFileStatus::kIgnored)
        {
            has_local_changes = true;
            break;
        }
    }

    if (has_local_changes)
    {
        result.merge_status = PullResult::MergeStatus::kConflict;
        result.conflicts = 1;
    }
    else
    {
        result.merge_status = PullResult::MergeStatus::kFastForward;
    }

    const std::string ref = branch.empty() ? git_service_.current_branch() : branch;
    result.updated_files.push_back(ref);

    return result;
}

auto GitRemoteService::remote_count() const -> int
{
    return static_cast<int>(remotes_.size());
}

} // namespace markamp::core
