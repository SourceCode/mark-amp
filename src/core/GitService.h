/// @file GitService.h
/// @brief V4 Phase 35 – Git Integration and Diff Tools (pure-logic engine).
/// Stubbed Git repository operations for testability without libgit2.

#pragma once

#include <chrono>
#include <expected>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

// ============================================================================
// Git file status
// ============================================================================

enum class GitFileStatus : uint8_t
{
    kUnmodified,
    kModified,
    kAdded,
    kDeleted,
    kRenamed,
    kUntracked,
    kIgnored,
    kConflicted
};

// ============================================================================
// Git data structures
// ============================================================================

struct GitFileEntry
{
    std::string file_path;
    GitFileStatus status{GitFileStatus::kUnmodified};
    GitFileStatus staged_status{GitFileStatus::kUnmodified};
    bool is_staged{false};
};

struct DiffHunk
{
    int old_start{0};
    int old_count{0};
    int new_start{0};
    int new_count{0};
    std::string header;
    std::vector<std::string> lines; // Lines prefixed with +, -, or space
};

struct FileDiff
{
    std::string file_path;
    std::vector<DiffHunk> hunks;
    int additions{0};
    int deletions{0};
};

struct GitCommit
{
    std::string hash;
    std::string short_hash;
    std::string author;
    std::string email;
    std::string message;
    std::chrono::system_clock::time_point timestamp;
    std::vector<std::string> changed_files;
};

struct GitBranch
{
    std::string name;
    bool is_current{false};
    bool is_remote{false};
    std::string upstream;
    int ahead{0};
    int behind{0};
};

// ============================================================================
// GitService – pure-logic Git operations (stubbed)
// ============================================================================

class GitService
{
public:
    GitService(EventBus& event_bus, Config& config);
    ~GitService();

    /// Open a git repository.
    [[nodiscard]] auto open_repository(const std::string& path) -> std::expected<void, std::string>;

    /// Check if repo is open.
    [[nodiscard]] auto is_repository() const -> bool;

    /// Get the repository root path.
    [[nodiscard]] auto repository_path() const -> const std::string&;

    // --- Status ---
    [[nodiscard]] auto status() const -> std::vector<GitFileEntry>;
    [[nodiscard]] auto file_status(const std::string& path) const -> GitFileStatus;

    // --- Diff ---
    [[nodiscard]] auto diff_file(const std::string& path) const -> FileDiff;
    [[nodiscard]] auto diff_staged() const -> std::vector<FileDiff>;

    // --- Staging ---
    [[nodiscard]] auto stage_file(const std::string& path) -> std::expected<void, std::string>;
    [[nodiscard]] auto unstage_file(const std::string& path) -> std::expected<void, std::string>;
    [[nodiscard]] auto stage_all() -> std::expected<void, std::string>;

    // --- Commit ---
    [[nodiscard]] auto commit(const std::string& message)
        -> std::expected<std::string, std::string>;

    // --- Log ---
    [[nodiscard]] auto log(int limit = 50) const -> std::vector<GitCommit>;
    [[nodiscard]] auto file_log(const std::string& path, int limit = 20) const
        -> std::vector<GitCommit>;

    // --- Branch ---
    [[nodiscard]] auto branches() const -> std::vector<GitBranch>;
    [[nodiscard]] auto current_branch() const -> std::string;
    [[nodiscard]] auto create_branch(const std::string& name) -> std::expected<void, std::string>;
    [[nodiscard]] auto checkout_branch(const std::string& name) -> std::expected<void, std::string>;

    // --- Discard ---
    [[nodiscard]] auto discard_file(const std::string& path) -> std::expected<void, std::string>;

    // --- Test helpers ---
    /// Simulate adding a file to the working tree.
    auto test_add_file(const std::string& path, const std::string& content) -> void;

    /// Simulate modifying a file in the working tree.
    auto test_modify_file(const std::string& path, const std::string& new_content) -> void;

private:
    EventBus& event_bus_;
    Config& config_;

    bool is_open_{false};
    std::string repo_path_;
    std::string current_branch_{"main"};

    // In-memory file tracking for testing.
    struct FileState
    {
        std::string content;
        std::string original_content;
        GitFileStatus status{GitFileStatus::kUntracked};
        bool is_staged{false};
    };

    std::vector<GitFileEntry> files_;
    std::vector<GitCommit> commits_;
    std::vector<GitBranch> branches_;

    // Working tree state.
    std::unordered_map<std::string, FileState> working_tree_;

    int next_commit_num_{1};

    /// Generate a fake commit hash.
    [[nodiscard]] auto generate_commit_hash() -> std::string;
};

} // namespace markamp::core
