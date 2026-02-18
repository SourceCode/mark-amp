/// @file GitRemoteService.h
/// @brief Phase 25: Version Control Integration — Git remote operations.
/// Manages remote registrations and simulates fetch/push/pull against
/// GitService branches and commits.

#pragma once

#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class GitService;

// ============================================================================
// Data structures
// ============================================================================

/// Information about a configured remote.
struct RemoteInfo
{
    std::string name;      ///< Remote name (e.g. "origin").
    std::string url;       ///< Primary URL.
    std::string fetch_url; ///< Fetch URL (defaults to url).
    std::string push_url;  ///< Push URL (defaults to url).
};

/// Result of a fetch operation.
struct FetchResult
{
    std::string remote_name;
    int new_commits{0};
    std::vector<std::string> updated_refs; ///< Ref names that were updated.
};

/// Result of a push operation.
struct PushResult
{
    std::string remote_name;
    std::vector<std::string> pushed_refs;   ///< Successfully pushed refs.
    std::vector<std::string> rejected_refs; ///< Rejected refs (e.g. non-fast-forward).
    bool up_to_date{false};
};

/// Result of a pull operation.
struct PullResult
{
    std::string remote_name;
    enum class MergeStatus : uint8_t
    {
        kFastForward,
        kMerge,
        kConflict,
        kUpToDate
    };
    MergeStatus merge_status{MergeStatus::kUpToDate};
    int conflicts{0};
    std::vector<std::string> updated_files;
};

// ============================================================================
// GitRemoteService
// ============================================================================

/// Manages remote repositories and provides fetch/push/pull operations.
class GitRemoteService
{
public:
    GitRemoteService(EventBus& event_bus, GitService& git_service);

    /// Add a new remote.
    [[nodiscard]] auto add_remote(const std::string& name, const std::string& url)
        -> std::expected<void, std::string>;

    /// Remove an existing remote.
    [[nodiscard]] auto remove_remote(const std::string& name) -> std::expected<void, std::string>;

    /// List all configured remotes.
    [[nodiscard]] auto list_remotes() const -> std::vector<RemoteInfo>;

    /// Get information about a specific remote.
    [[nodiscard]] auto get_remote(const std::string& name) const
        -> std::expected<RemoteInfo, std::string>;

    /// Fetch new commits from a remote.
    [[nodiscard]] auto fetch(const std::string& remote_name = "origin") -> FetchResult;

    /// Push local commits to a remote.
    [[nodiscard]] auto push(const std::string& remote_name = "origin",
                            const std::string& branch = "") -> PushResult;

    /// Pull (fetch + merge) from a remote.
    [[nodiscard]] auto pull(const std::string& remote_name = "origin",
                            const std::string& branch = "") -> PullResult;

    /// Get the number of configured remotes.
    [[nodiscard]] auto remote_count() const -> int;

private:
    EventBus& event_bus_;
    GitService& git_service_;

    std::unordered_map<std::string, RemoteInfo> remotes_;

    /// Simulated remote commit storage.
    std::unordered_map<std::string,
                       std::vector<std::string>>
        remote_commits_; ///< remote -> commit hashes
};

} // namespace markamp::core
