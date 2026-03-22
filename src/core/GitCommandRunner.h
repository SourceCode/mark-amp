#pragma once

#include "EventBus.h"

#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace markamp::core
{

// File status entries (Task 2 + 15)
enum class GitChangeStatus
{
    None,
    Modified,
    Added,
    Deleted,
    Renamed,
    Copied,
    Untracked,
    Unmerged
};

struct GitChangeEntry
{
    std::string path;
    std::string original_path;                             // For renames (from -> to)
    GitChangeStatus index_status{GitChangeStatus::None};   // Staged
    GitChangeStatus working_status{GitChangeStatus::None}; // Unstaged
    int staged_additions{0};
    int staged_deletions{0};
    int unstaged_additions{0};
    int unstaged_deletions{0};
};

// Blame annotation line (Task 12)
struct BlameLine
{
    std::string commit_hash;
    std::string author;
    std::string date;
    int original_line{0};
};

// File History Log (Tasks 13, 24)
struct GitLogEntry
{
    std::string graph;
    std::string hash;
    std::string message;
    std::string author;
    std::string date;
};

// Return type wrapping process outputs
struct CommandResult
{
    int exit_code{-1};
    std::string stdout_text;
    std::string stderr_text;
    [[nodiscard]] bool success() const
    {
        return exit_code == 0;
    }
};

/// robust git command execution wrapper with proper error handling,
/// background threading, and output parsing.
class GitCommandRunner
{
public:
    explicit GitCommandRunner(std::string workspace_root);
    ~GitCommandRunner();

    // Non-copyable, non-movable
    GitCommandRunner(const GitCommandRunner&) = delete;
    auto operator=(const GitCommandRunner&) -> GitCommandRunner& = delete;
    GitCommandRunner(GitCommandRunner&&) = delete;
    auto operator=(GitCommandRunner&&) -> GitCommandRunner& = delete;

    // Baseline execution
    auto RunSync(const std::string& command) -> CommandResult;
    void RunAsync(const std::string& command, std::function<void(CommandResult)> callback);

    /// Join all pending async threads. Called automatically in destructor.
    void JoinAsyncThreads();

    // High-level parsing Operations
    auto GetStatus() -> std::vector<GitChangeEntry>;
    auto GetBranch() -> std::string;
    auto GetBranches() -> std::vector<std::string>;
    std::vector<GitLogEntry>
    GetLog(const std::string& file = "", int count = 0, bool with_graph = false);
    auto GetBlame(const std::string& file) -> std::vector<BlameLine>;
    static auto ParseBlameOutput(const std::string& output) -> std::vector<BlameLine>;
    auto GetDiff(const std::string& file, bool staged) -> std::string;
    auto GetFileContentAtHEAD(const std::string& file) -> std::string;
    auto GetFileContentFromIndex(const std::string& file) -> std::string;

    // Standard mutators (void returning - fire and forget sync)
    void Stage(const std::string& path);
    void Unstage(const std::string& path);
    void Commit(const std::string& message);
    void Push();
    void Pull();
    void Fetch();
    void SwitchBranch(const std::string& branch);
    void CreateBranch(const std::string& name);
    void Stash(const std::string& message);
    void StashFile(const std::string& path, const std::string& message = "");
    void StashPop();
    void Discard(const std::string& path);
    void ResolveConflict(const std::string& path, bool accept_current);

private:
    std::string workspace_root_;

    /// Tracks async threads so they can be joined before destruction.
    std::mutex async_threads_mutex_;
    std::vector<std::thread> async_threads_;
};

} // namespace markamp::core
