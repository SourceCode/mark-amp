# Phase 35 -- Git Integration and Diff Tools

## Objective

Implement Git integration for version-controlled vaults: display file status (modified, added, untracked), view diffs inline, commit changes, view log history, and structured notebook diff tools. Uses libgit2 for Git operations without requiring a system git binary. Provides a Git panel in the sidebar showing repository state.

## Prerequisites

- Phase 02 (VaultService -- vault file access)
- Existing EditorPanel, MainFrame sidebar
- libgit2 (new dependency)

## Feature References (PRD)

- PRD #19: Version History / Revisions
- PRD Notebook #24: Notebook Diff Tools
- PRD Notebook #25: Git Integration

## Data Structures to Implement

### File: `src/core/GitService.h`

```cpp
#pragma once

#include <chrono>
#include <expected>
#include <string>
#include <vector>

namespace markamp::core
{

class EventBus;
class Config;

enum class GitFileStatus : uint8_t
{
    Unmodified, Modified, Added, Deleted, Renamed, Untracked, Ignored, Conflicted
};

struct GitFileEntry
{
    std::string file_path;
    GitFileStatus status{GitFileStatus::Unmodified};
    GitFileStatus staged_status{GitFileStatus::Unmodified};
    bool is_staged{false};
};

struct DiffHunk
{
    int old_start{0};
    int old_count{0};
    int new_start{0};
    int new_count{0};
    std::string header;
    std::vector<std::string> lines;  // Lines prefixed with +, -, or space
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

class GitService
{
public:
    GitService(EventBus& event_bus, Config& config);
    ~GitService();

    /// Open a git repository in the vault directory.
    [[nodiscard]] auto open_repository(const std::string& path)
        -> std::expected<void, std::string>;

    /// Check if the vault is a git repository.
    [[nodiscard]] auto is_repository() const -> bool;

    // --- Status ---
    [[nodiscard]] auto status() const -> std::vector<GitFileEntry>;
    [[nodiscard]] auto file_status(const std::string& path) const -> GitFileStatus;

    // --- Diff ---
    [[nodiscard]] auto diff_file(const std::string& path) const -> FileDiff;
    [[nodiscard]] auto diff_staged() const -> std::vector<FileDiff>;
    [[nodiscard]] auto diff_commits(const std::string& from, const std::string& to) const
        -> std::vector<FileDiff>;

    // --- Staging ---
    [[nodiscard]] auto stage_file(const std::string& path) -> std::expected<void, std::string>;
    [[nodiscard]] auto unstage_file(const std::string& path) -> std::expected<void, std::string>;
    [[nodiscard]] auto stage_all() -> std::expected<void, std::string>;

    // --- Commit ---
    [[nodiscard]] auto commit(const std::string& message) -> std::expected<std::string, std::string>;

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

private:
    EventBus& event_bus_;
    Config& config_;

    struct GitRepo;
    std::unique_ptr<GitRepo> repo_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`open_repository(path)`** -- Use libgit2 to open the git repo. Initialize internal state. Publish GitRepoOpenedEvent.

2. **`status()`** -- Call libgit2 status. Map each entry to GitFileEntry with status flags.

3. **`diff_file(path)`** -- Generate diff between working tree and HEAD for the file. Parse into DiffHunk/FileDiff.

4. **`stage_file(path)`** -- Add file to git index.

5. **`commit(message)`** -- Create a commit from the current index. Return commit hash.

6. **`log(limit)`** -- Walk commit history. Build GitCommit for each. Return most recent `limit` commits.

7. **`diff_commits(from, to)`** -- Diff between two commit hashes. Return file-level diffs.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitRepoOpenedEvent)
std::string path;
std::string branch;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitStatusChangedEvent)
int modified{0};
int staged{0};
int untracked{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(GitCommitCreatedEvent)
std::string hash;
std::string message;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_git_service.cpp`

1. **Open repository** -- Init temp git repo. Open. Verify is_repository()=true.
2. **Status clean** -- Clean repo. Verify no modified files.
3. **Status modified** -- Modify a file. Verify status shows Modified.
4. **Stage file** -- Stage a file. Verify staged status.
5. **Commit** -- Stage and commit. Verify commit hash returned.
6. **Diff file** -- Modify file. diff_file() returns hunks with + and - lines.
7. **Log** -- Create 3 commits. log(3) returns all 3.
8. **File log** -- Create commits touching different files. file_log("a.md") returns only relevant commits.
9. **Branch listing** -- Create branch. Verify in branches().
10. **Discard changes** -- Modify file. discard_file(). Verify file restored.

## Acceptance Criteria

- [ ] Git repository detection and opening via libgit2
- [ ] File status shows modified/added/deleted/untracked
- [ ] Diff shows line-level changes with hunks
- [ ] Stage/unstage individual files
- [ ] Commit creates a valid git commit
- [ ] Log shows commit history with metadata
- [ ] Branch operations work
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/GitService.h` | GitService, all git types |
| CREATE | `src/core/GitService.cpp` | libgit2 implementation |
| CREATE | `src/ui/GitPanel.h` | Git sidebar panel |
| CREATE | `src/ui/GitPanel.cpp` | Status/diff/commit UI |
| MODIFY | `src/core/Events.h` | Add 3 git events |
| MODIFY | `src/core/PluginContext.h` | Add `GitService* git_service{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add files, link libgit2 |
| MODIFY | `CMakeLists.txt` | Add libgit2 dependency |
| CREATE | `tests/unit/test_git_service.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_git_service target |

## Estimated Complexity

**XL** -- libgit2 integration, diff parsing, staging, commit, branch operations, UI panel, 10 tests.
