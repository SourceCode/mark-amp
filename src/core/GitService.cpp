/// @file GitService.cpp
/// @brief V4 Phase 35 – Git Integration implementation (stubbed for testability).

#include "core/GitService.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <algorithm>
#include <iomanip>
#include <ranges>
#include <sstream>

namespace markamp::core
{

// ============================================================================
// Constructor / Destructor
// ============================================================================

GitService::GitService(EventBus& event_bus, Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    // Create default main branch.
    GitBranch main_branch;
    main_branch.name = "main";
    main_branch.is_current = true;
    branches_.push_back(std::move(main_branch));
}

GitService::~GitService() = default;

// ============================================================================
// Repository management
// ============================================================================

auto GitService::open_repository(const std::string& path) -> std::expected<void, std::string>
{
    if (path.empty())
    {
        return std::unexpected("Repository path cannot be empty");
    }

    repo_path_ = path;
    is_open_ = true;

    events::GitRepoOpenedEvent event;
    event.path = path;
    event.branch = current_branch_;
    event_bus_.publish(event);

    return {};
}

auto GitService::is_repository() const -> bool
{
    return is_open_;
}

auto GitService::repository_path() const -> const std::string&
{
    return repo_path_;
}

// ============================================================================
// Commit hash generation
// ============================================================================

auto GitService::generate_commit_hash() -> std::string
{
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    // Generate a deterministic fake hash from the commit number.
    const uint64_t seed = static_cast<uint64_t>(next_commit_num_) * 0xDEADBEEFULL;
    oss << std::setw(16) << seed;
    oss << std::setw(16) << (seed ^ 0xCAFEBABEULL);
    oss << std::setw(8) << (static_cast<uint32_t>(next_commit_num_) & 0xFFFFFFFFU);
    ++next_commit_num_;
    return oss.str();
}

// ============================================================================
// Status
// ============================================================================

auto GitService::status() const -> std::vector<GitFileEntry>
{
    std::vector<GitFileEntry> result;

    for (const auto& [path, state] : working_tree_)
    {
        GitFileEntry entry;
        entry.file_path = path;
        entry.status = state.status;
        entry.is_staged = state.is_staged;
        if (state.is_staged)
        {
            entry.staged_status = state.status;
        }
        result.push_back(std::move(entry));
    }

    // Sort by path for deterministic output.
    std::sort(result.begin(),
              result.end(),
              [](const GitFileEntry& lhs, const GitFileEntry& rhs)
              { return lhs.file_path < rhs.file_path; });

    return result;
}

auto GitService::file_status(const std::string& path) const -> GitFileStatus
{
    auto iter = working_tree_.find(path);
    if (iter == working_tree_.end())
    {
        return GitFileStatus::kUnmodified;
    }
    return iter->second.status;
}

// ============================================================================
// Diff
// ============================================================================

auto GitService::diff_file(const std::string& path) const -> FileDiff
{
    FileDiff diff;
    diff.file_path = path;

    auto iter = working_tree_.find(path);
    if (iter == working_tree_.end())
    {
        return diff;
    }

    const auto& state = iter->second;
    if (state.status == GitFileStatus::kUnmodified)
    {
        return diff;
    }

    DiffHunk hunk;
    hunk.old_start = 1;
    hunk.new_start = 1;

    if (state.status == GitFileStatus::kAdded || state.status == GitFileStatus::kUntracked)
    {
        // All lines are additions.
        std::istringstream stream(state.content);
        std::string line;
        while (std::getline(stream, line))
        {
            hunk.lines.push_back("+" + line);
            ++diff.additions;
        }
        hunk.new_count = diff.additions;
        hunk.header = "@@ -0,0 +1," + std::to_string(diff.additions) + " @@";
    }
    else if (state.status == GitFileStatus::kModified)
    {
        // Show original as deletions, new as additions.
        std::istringstream orig_stream(state.original_content);
        std::string line;
        while (std::getline(orig_stream, line))
        {
            hunk.lines.push_back("-" + line);
            ++diff.deletions;
        }
        hunk.old_count = diff.deletions;

        std::istringstream new_stream(state.content);
        while (std::getline(new_stream, line))
        {
            hunk.lines.push_back("+" + line);
            ++diff.additions;
        }
        hunk.new_count = diff.additions;
        hunk.header = "@@ -1," + std::to_string(diff.deletions) + " +1," +
                      std::to_string(diff.additions) + " @@";
    }
    else if (state.status == GitFileStatus::kDeleted)
    {
        // All lines are deletions.
        std::istringstream stream(state.original_content);
        std::string line;
        while (std::getline(stream, line))
        {
            hunk.lines.push_back("-" + line);
            ++diff.deletions;
        }
        hunk.old_count = diff.deletions;
        hunk.header = "@@ -1," + std::to_string(diff.deletions) + " +0,0 @@";
    }

    diff.hunks.push_back(std::move(hunk));
    return diff;
}

auto GitService::diff_staged() const -> std::vector<FileDiff>
{
    std::vector<FileDiff> diffs;

    for (const auto& [path, state] : working_tree_)
    {
        if (state.is_staged)
        {
            diffs.push_back(diff_file(path));
        }
    }

    return diffs;
}

// ============================================================================
// Staging
// ============================================================================

auto GitService::stage_file(const std::string& path) -> std::expected<void, std::string>
{
    auto iter = working_tree_.find(path);
    if (iter == working_tree_.end())
    {
        return std::unexpected("File not found: " + path);
    }

    iter->second.is_staged = true;

    events::GitStatusChangedEvent event;
    const auto all_status = status();
    event.modified = static_cast<int>(std::count_if(
        all_status.begin(),
        all_status.end(),
        [](const GitFileEntry& entry) { return entry.status == GitFileStatus::kModified; }));
    event.staged =
        static_cast<int>(std::count_if(all_status.begin(),
                                       all_status.end(),
                                       [](const GitFileEntry& entry) { return entry.is_staged; }));
    event.untracked = static_cast<int>(std::count_if(
        all_status.begin(),
        all_status.end(),
        [](const GitFileEntry& entry) { return entry.status == GitFileStatus::kUntracked; }));
    event_bus_.publish(event);

    return {};
}

auto GitService::unstage_file(const std::string& path) -> std::expected<void, std::string>
{
    auto iter = working_tree_.find(path);
    if (iter == working_tree_.end())
    {
        return std::unexpected("File not found: " + path);
    }

    iter->second.is_staged = false;
    return {};
}

auto GitService::stage_all() -> std::expected<void, std::string>
{
    for (auto& [path, state] : working_tree_)
    {
        if (state.status != GitFileStatus::kUnmodified)
        {
            state.is_staged = true;
        }
    }
    return {};
}

// ============================================================================
// Commit
// ============================================================================

auto GitService::commit(const std::string& message) -> std::expected<std::string, std::string>
{
    if (message.empty())
    {
        return std::unexpected("Commit message cannot be empty");
    }

    // Collect staged files.
    std::vector<std::string> staged_files;
    for (auto& [path, state] : working_tree_)
    {
        if (state.is_staged)
        {
            staged_files.push_back(path);
        }
    }

    if (staged_files.empty())
    {
        return std::unexpected("Nothing staged to commit");
    }

    const auto hash = generate_commit_hash();

    GitCommit commit_obj;
    commit_obj.hash = hash;
    commit_obj.short_hash = hash.substr(0, 7);
    commit_obj.author = "Test User";
    commit_obj.email = "test@example.com";
    commit_obj.message = message;
    commit_obj.timestamp = std::chrono::system_clock::now();
    commit_obj.changed_files = staged_files;

    commits_.push_back(std::move(commit_obj));

    // Commit staged files: mark as unmodified and store content as original.
    for (const auto& path : staged_files)
    {
        auto iter = working_tree_.find(path);
        if (iter != working_tree_.end())
        {
            if (iter->second.status == GitFileStatus::kDeleted)
            {
                working_tree_.erase(iter);
            }
            else
            {
                iter->second.original_content = iter->second.content;
                iter->second.status = GitFileStatus::kUnmodified;
                iter->second.is_staged = false;
            }
        }
    }

    events::GitCommitCreatedEvent event;
    event.hash = hash;
    event.message = message;
    event_bus_.publish(event);

    return hash;
}

// ============================================================================
// Log
// ============================================================================

auto GitService::log(int limit) const -> std::vector<GitCommit>
{
    // Return most recent commits, up to limit.
    std::vector<GitCommit> result;
    const int count = std::min(limit, static_cast<int>(commits_.size()));
    for (int idx = static_cast<int>(commits_.size()) - 1;
         idx >= 0 && static_cast<int>(result.size()) < count;
         --idx)
    {
        result.push_back(commits_[static_cast<size_t>(idx)]);
    }
    return result;
}

auto GitService::file_log(const std::string& path, int limit) const -> std::vector<GitCommit>
{
    std::vector<GitCommit> result;

    for (const auto& commit_entry : commits_ | std::views::reverse)
    {
        if (static_cast<int>(result.size()) >= limit)
        {
            break;
        }

        const auto& changed = commit_entry.changed_files;
        if (std::find(changed.begin(), changed.end(), path) != changed.end())
        {
            result.push_back(commit_entry);
        }
    }

    return result;
}

// ============================================================================
// Branch
// ============================================================================

auto GitService::branches() const -> std::vector<GitBranch>
{
    return branches_;
}

auto GitService::current_branch() const -> std::string
{
    return current_branch_;
}

auto GitService::create_branch(const std::string& name) -> std::expected<void, std::string>
{
    // Check if branch already exists.
    for (const auto& branch : branches_)
    {
        if (branch.name == name)
        {
            return std::unexpected("Branch already exists: " + name);
        }
    }

    GitBranch new_branch;
    new_branch.name = name;
    new_branch.is_current = false;
    branches_.push_back(std::move(new_branch));

    return {};
}

auto GitService::checkout_branch(const std::string& name) -> std::expected<void, std::string>
{
    bool found = false;
    for (auto& branch : branches_)
    {
        if (branch.name == name)
        {
            branch.is_current = true;
            found = true;
        }
        else
        {
            branch.is_current = false;
        }
    }

    if (!found)
    {
        return std::unexpected("Branch not found: " + name);
    }

    current_branch_ = name;
    return {};
}

// ============================================================================
// Discard
// ============================================================================

auto GitService::discard_file(const std::string& path) -> std::expected<void, std::string>
{
    auto iter = working_tree_.find(path);
    if (iter == working_tree_.end())
    {
        return std::unexpected("File not found: " + path);
    }

    if (iter->second.status == GitFileStatus::kUntracked ||
        iter->second.status == GitFileStatus::kAdded)
    {
        // Remove untracked/added files.
        working_tree_.erase(iter);
    }
    else
    {
        // Restore original content.
        iter->second.content = iter->second.original_content;
        iter->second.status = GitFileStatus::kUnmodified;
        iter->second.is_staged = false;
    }

    return {};
}

// ============================================================================
// Test helpers
// ============================================================================

auto GitService::test_add_file(const std::string& path, const std::string& content) -> void
{
    FileState state;
    state.content = content;
    state.original_content = "";
    state.status = GitFileStatus::kUntracked;
    state.is_staged = false;
    working_tree_[path] = std::move(state);
}

auto GitService::test_modify_file(const std::string& path, const std::string& new_content) -> void
{
    auto iter = working_tree_.find(path);
    if (iter != working_tree_.end())
    {
        // File already in the tree – mark as modified.
        if (iter->second.status == GitFileStatus::kUnmodified)
        {
            iter->second.original_content = iter->second.content;
        }
        iter->second.content = new_content;
        iter->second.status = GitFileStatus::kModified;
    }
    else
    {
        // New file.
        test_add_file(path, new_content);
    }
}

} // namespace markamp::core
