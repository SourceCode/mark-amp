# Phase 18: Source Control Panel

## Objective

Build a complete git integration panel in the sidebar, providing a visual interface for common git operations: viewing changed files, staging/unstaging, committing, branching, push/pull, blame annotations, and merge conflict resolution. The codebase currently has `SidebarMode::kGit` registered and the `StatusBarPanel` already supports displaying `git_branch_` via `set_git_branch()`. This phase delivers a full source control panel rivaling VSCode's git integration.

## Prerequisites

- `SidebarMode::kGit` in `src/ui/SidebarMode.h`
- `StatusBarPanel::set_git_branch()` already wired
- `SidebarPanelRegistry` for panel registration
- `ActivityBar` with git icon slot
- Phase 14 Task 6 (git gutter change indicators, `GitGutterProvider`)
- Phase 16 Task 4 (git status file decorations, `GitStatusProvider`)
- `EventBus` and `ThemeEngine`

## Deliverables

A complete Source Control sidebar panel with 27 tasks covering file status view, staging, commits, branching, push/pull, blame, and timeline.

---

## Task 1: SourceControlPanel Shell

**Title:** Create the main SourceControlPanel widget

**Description:** Create the wxWidgets panel that serves as the git sidebar. It contains a commit message area at the top, action buttons, and a scrollable changed files list below.

**Implementation Details:**
- Create `src/ui/SourceControlPanel.h` / `.cpp`:
  ```cpp
  class SourceControlPanel : public wxPanel {
  public:
      SourceControlPanel(wxWindow* parent, core::ThemeEngine& theme_engine,
                         core::EventBus& event_bus);
      void Refresh(); // Rescan git status
  private:
      void CreateLayout();
      wxPanel* header_area_;         // Branch, actions
      wxTextCtrl* commit_input_;     // Commit message
      wxPanel* action_bar_;          // Commit, stage all buttons
      wxScrolledWindow* changes_area_; // Changed files list
  };
  ```
- Register with `SidebarPanelRegistry` for `SidebarMode::kGit`
- Auto-refresh on panel show and on file save events

**Files Affected:**
- `src/ui/SourceControlPanel.h` (new)
- `src/ui/SourceControlPanel.cpp` (new)
- `src/ui/LayoutManager.cpp` (register panel)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Panel visible when git sidebar mode selected
- Basic layout with header, commit input, and changes area
- Registered in sidebar panel registry
- Auto-refresh on panel show

**Dependencies:** None

---

## Task 2: Git Status Provider Enhancement

**Title:** Enhance GitStatusProvider with full change tracking

**Description:** Extend the `GitStatusProvider` (from Phase 16) to provide detailed change information: index status vs working tree status, renamed file tracking, and unmerged files.

**Implementation Details:**
- Enhance `GitStatusProvider::GetFileStatuses()` to return:
  ```cpp
  struct GitChangeEntry {
      std::string path;
      std::string original_path;   // for renames
      GitChangeStatus index_status;  // staged status
      GitChangeStatus working_status; // unstaged status
  };
  enum class GitChangeStatus { None, Modified, Added, Deleted, Renamed, Copied, Untracked, Unmerged };
  ```
- Parse `git status --porcelain=v2` for richer data
- Categorize into: Staged Changes, Changes (unstaged), Untracked Files, Merge Changes

**Files Affected:**
- `src/ui/GitStatusProvider.h` (enhance data structures)
- `src/ui/GitStatusProvider.cpp` (parse porcelain v2 output)

**Acceptance Criteria:**
- All git status types correctly parsed
- Staged vs unstaged distinguished
- Renamed files tracked with original path
- Unmerged files identified
- Performance: status query < 200ms for repos with 1000+ files

**Dependencies:** Phase 16 Task 4

---

## Task 3: Changed Files List with Status Icons

**Title:** Render changed files with status icons and color coding

**Description:** Display the list of changed files in the source control panel, grouped into sections: "Staged Changes", "Changes" (unstaged), and "Untracked Files". Each file shows a status icon and color-coded badge.

**Implementation Details:**
- Custom-rendered scrollable panel:
  ```
  STAGED CHANGES (3)
    M  src/ui/TabBar.cpp
    A  src/ui/NewFile.h
    D  src/ui/OldFile.cpp

  CHANGES (5)
    M  src/ui/EditorPanel.cpp
    M  src/ui/BreadcrumbBar.cpp
    ...

  UNTRACKED FILES (2)
    U  docs/notes.md
    U  temp/scratch.txt
  ```
- Status badges: "M" blue, "A" green, "D" red, "R" purple, "U" gray, "C" yellow (conflict)
- File-type icons before filenames (reuse `FileTypeIconRegistry`)
- Section headers collapsible
- Click file: show inline diff preview (Task 4)
- Right-click: context menu for staging operations

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (implement file list rendering)

**Acceptance Criteria:**
- Files grouped by status category
- Correct status icons and colors
- File-type icons shown
- Sections collapsible
- Click shows diff preview
- Updated on git status change

**Dependencies:** Task 2

---

## Task 4: Inline Diff Preview on Hover/Click

**Title:** Show diff preview when clicking a changed file

**Description:** Clicking a changed file in the source control panel shows a quick diff preview: either inline in the panel (compact mode) or by opening the diff editor (Phase 12 Task 15).

**Implementation Details:**
- Compact inline preview (in the panel):
  - Show first 10 changed lines below the file entry
  - Green background for additions, red for deletions
  - "Show Full Diff" link to open in editor
- Full diff (open in editor):
  - Open the file in a diff editor group (Phase 12 Task 15)
  - Left: HEAD version, Right: working copy
- Get diff via `git diff HEAD -- <file>` (for unstaged) or `git diff --cached -- <file>` (for staged)

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add inline diff preview)
- `src/core/GitCommandRunner.h` (new -- encapsulate git command execution)
- `src/core/GitCommandRunner.cpp` (new)

**Acceptance Criteria:**
- Click shows compact diff preview
- Additions in green, deletions in red
- "Show Full Diff" opens in editor
- Both staged and unstaged diffs work
- Preview updates on file changes

**Dependencies:** Tasks 2, 3

---

## Task 5: Staging Area (Stage/Unstage Individual Files)

**Title:** Implement staging and unstaging individual files

**Description:** Add "+" button on each changed file to stage it, and "-" button on each staged file to unstage it. Also provide "Stage All" and "Unstage All" bulk actions.

**Implementation Details:**
- Each file row has a "+" or "-" button on the right side:
  - Unstaged file: "+" stages it (`git add <file>`)
  - Staged file: "-" unstages it (`git restore --staged <file>`)
- Bulk actions in section headers:
  - "Stage All Changes" button
  - "Unstage All" button
  - "Discard All Changes" button (with confirmation)
- After staging/unstaging, refresh the file list
- Stage/unstage via `GitCommandRunner`:
  ```cpp
  void StageFile(const std::string& path);
  void UnstageFile(const std::string& path);
  void StageAllFiles();
  void UnstageAllFiles();
  void DiscardChanges(const std::string& path);
  ```

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add stage/unstage buttons and logic)
- `src/core/GitCommandRunner.cpp` (implement staging commands)

**Acceptance Criteria:**
- Individual file staging/unstaging works
- Bulk stage/unstage works
- Discard changes with confirmation
- File list refreshes after operations
- Visual feedback during operation

**Dependencies:** Tasks 3, 4

---

## Task 6: Commit Message Input

**Title:** Implement the commit message text area with character count

**Description:** Create a multi-line text input for the commit message with character count, subject line guidance, and commit button.

**Implementation Details:**
- Multi-line `wxTextCtrl` with:
  - Placeholder text: "Message (Cmd+Enter to commit)"
  - Character count: "72/72" for subject line (first line), unlimited for body
  - Subject line color warning when >72 characters (yellow >72, red >100)
  - Auto-resize: grows from 1 line to max 5 lines as text is entered
- Commit button below the input:
  - Primary button: "Commit" (enabled only when message is non-empty and staged files exist)
  - Dropdown arrow for: "Commit & Push", "Commit & Sync", "Amend Last Commit"
- Cmd+Enter: commit shortcut

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (implement commit input area)

**Acceptance Criteria:**
- Multi-line commit message input
- Character count for subject line
- Warning colors for long subjects
- Commit button enabled conditionally
- Dropdown with additional commit options
- Cmd+Enter shortcut

**Dependencies:** Task 5

---

## Task 7: Commit Execution

**Title:** Execute git commit from the panel

**Description:** Wire the commit button to execute `git commit` with the entered message. Handle success and error states.

**Implementation Details:**
- On commit:
  1. Validate: non-empty message, files are staged
  2. Execute: `git commit -m "message"`
  3. On success: clear message input, refresh file list, show success notification
  4. On failure: show error message in panel
- "Commit & Push": commit then `git push`
- "Amend": `git commit --amend -m "message"`
- Publish `GitCommitEvent(commit_hash, message)` on EventBus

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (wire commit button)
- `src/core/GitCommandRunner.cpp` (implement commit command)
- `src/core/Events.h` (add git events)

**Acceptance Criteria:**
- Commit creates a git commit
- Message input cleared after success
- File list refreshes
- Error messages shown on failure
- Commit & Push works
- Amend works
- Event published

**Dependencies:** Task 6

---

## Task 8: Branch Indicator with Switch Dropdown

**Title:** Show current branch with switch/create branch dropdown

**Description:** Display the current git branch name in the panel header. Clicking it shows a dropdown to switch branches, create new branches, or view recent branches.

**Implementation Details:**
- Header shows: branch icon + "main" (current branch name)
- Click opens a popup:
  ```
  Current: main
  ---
  Recent branches:
    feature/tabs
    bugfix/scroll
    develop
  ---
  All branches:
    main
    develop
    feature/tabs
    bugfix/scroll
    release/v2.6
  ---
  + Create New Branch...
  ```
- Switch branch: `git checkout <branch>` (or `git switch <branch>`)
- Create branch: dialog for name, `git checkout -b <name>`
- After switch: refresh everything (file list, editor content, tree)

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (implement branch header and dropdown)
- `src/core/GitCommandRunner.cpp` (implement branch operations)

**Acceptance Criteria:**
- Current branch displayed in header
- Dropdown shows all branches
- Switch branch works
- Create new branch works
- UI refreshes after branch switch

**Dependencies:** Task 1

---

## Task 9: Pull/Push Actions in Header

**Title:** Add pull and push buttons to the panel header

**Description:** Add icon buttons in the source control panel header for pulling and pushing changes. Show indicators for incoming/outgoing commit counts.

**Implementation Details:**
- Header buttons:
  - Pull (down arrow): `git pull`
  - Push (up arrow): `git push`
  - Sync (circular arrows): pull then push
  - Fetch (cloud download): `git fetch`
- Commit count indicators:
  - Incoming: "2 incoming" badge on pull button (from `git rev-list HEAD..@{u} --count`)
  - Outgoing: "3 outgoing" badge on push button (from `git rev-list @{u}..HEAD --count`)
- Operations run on background thread with progress
- Error handling: push rejected, merge conflicts, auth failures

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add push/pull buttons)
- `src/core/GitCommandRunner.cpp` (implement push/pull/fetch)

**Acceptance Criteria:**
- Pull, push, sync, fetch buttons functional
- Incoming/outgoing counts displayed
- Background execution with progress
- Error messages for failures
- Auth prompt if needed

**Dependencies:** Task 1

---

## Task 10: Merge Conflict Resolution UI

**Title:** Show merge conflicts with resolution actions

**Description:** When merge conflicts exist, show them in a dedicated "Merge Changes" section with resolution actions: "Accept Current", "Accept Incoming", "Accept Both", and "Open Merge Editor".

**Implementation Details:**
- Merge conflicts section (shown when unmerged files exist):
  ```
  MERGE CHANGES (2)
    C  src/ui/TabBar.cpp      [Accept Current] [Accept Incoming] [Open Editor]
    C  src/ui/Config.cpp      [Accept Current] [Accept Incoming] [Open Editor]
  ```
- "Accept Current": `git checkout --ours <file> && git add <file>`
- "Accept Incoming": `git checkout --theirs <file> && git add <file>`
- "Open Editor": opens file with merge conflict decorators (Phase 14 Task 12)
- After all conflicts resolved, show "All conflicts resolved - commit to complete merge"

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add merge conflicts section)
- `src/core/GitCommandRunner.cpp` (implement conflict resolution commands)

**Acceptance Criteria:**
- Merge conflicts displayed in dedicated section
- Resolution actions functional
- "Accept" operations run git commands
- Open Editor shows conflict decorators
- Prompt to commit after all conflicts resolved

**Dependencies:** Tasks 3, 5

---

## Task 11: Stash/Unstash Actions

**Title:** Implement git stash operations

**Description:** Add stash management: "Stash All Changes", "Pop Latest Stash", and a stash list viewer.

**Implementation Details:**
- Add stash button (drawer icon) to panel header
- Dropdown menu:
  ```
  Stash All Changes
  Stash Staged Changes
  ---
  Pop Latest Stash
  Apply Latest Stash
  ---
  View Stash List...
  ```
- Stash list viewer: popup showing all stashes with description, date, and actions (apply, pop, drop)
- Stash commands: `git stash push -m "description"`, `git stash pop`, `git stash list`
- After stash/pop: refresh file list

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add stash UI)
- `src/core/GitCommandRunner.cpp` (implement stash commands)

**Acceptance Criteria:**
- Stash all changes works
- Pop/apply stash works
- Stash list viewable
- Individual stash drop supported
- Descriptions shown
- File list refreshes after operations

**Dependencies:** Tasks 1, 5

---

## Task 12: Git Blame Gutter Annotations

**Title:** Show git blame information in editor gutter

**Description:** Add an optional git blame display in the editor gutter showing the author and date of each line. Toggle via command palette or status bar.

**Implementation Details:**
- Run `git blame --porcelain <file>` to get per-line blame data:
  ```cpp
  struct BlameLine {
      std::string commit_hash;
      std::string author;
      std::string date;
      int original_line;
  };
  ```
- In the editor, show blame info in a dedicated annotation area (left of line numbers):
  - Author name (truncated to 15 chars) + relative date ("3 days ago")
  - Color: `TextMuted` at 50% opacity
  - Hover: full tooltip with commit hash, author, date, and commit message
- Toggle via: "Git: Toggle Blame" command
- Performance: cache blame data, invalidate on file save/commit

**Files Affected:**
- `src/ui/EditorPanel.h` (add blame display state)
- `src/ui/EditorPanel.cpp` (implement blame rendering)
- `src/core/GitCommandRunner.cpp` (implement blame command)
- `src/ui/CommandPalette.cpp` (register toggle command)

**Acceptance Criteria:**
- Blame info shown in gutter for each line
- Author and relative date displayed
- Hover shows full commit details
- Toggle via command
- Cached for performance
- Clears on file close

**Dependencies:** Task 4

---

## Task 13: Timeline View

**Title:** Show file commit history timeline

**Description:** Add a "Timeline" section at the bottom of the source control panel (or as a collapsible section) showing the commit history for the active file.

**Implementation Details:**
- Run `git log --follow --oneline -20 -- <file>` for recent commits
- Display as a vertical timeline:
  ```
  TIMELINE
  * abc1234 Fix tab overflow (2 hours ago) - Author
  * def5678 Add close animation (yesterday) - Author
  * ghi9012 Initial tab bar impl (3 days ago) - Author
  ```
- Each entry clickable to show that version's diff
- Load more with "Show More..." link
- Auto-update when file changes or commit occurs

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add timeline section)
- `src/core/GitCommandRunner.cpp` (implement log command)

**Acceptance Criteria:**
- Last 20 commits for active file shown
- Each entry shows hash, message, date, author
- Click opens diff for that commit
- "Show More" loads additional history
- Updates on file change/commit

**Dependencies:** Task 1

---

## Task 14: Discard Changes per File

**Title:** Discard unstaged changes for individual files

**Description:** Add a "Discard" button (revert icon) on each changed file that reverts the working copy to the HEAD version.

**Implementation Details:**
- Discard button appears on hover over unstaged changed files
- Confirmation dialog: "Discard changes to {filename}? This cannot be undone."
- Execute: `git checkout -- <file>` (or `git restore <file>`)
- Reload file in editor after discard
- Bulk: "Discard All Changes" button in Changes section header

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add discard buttons)
- `src/core/GitCommandRunner.cpp` (implement discard command)

**Acceptance Criteria:**
- Discard button on each unstaged file
- Confirmation before discarding
- File reverted to HEAD version
- Editor reloaded with reverted content
- Bulk discard available

**Dependencies:** Tasks 3, 5

---

## Task 15: Git Command Runner

**Title:** Create a robust git command execution wrapper

**Description:** Build a centralized `GitCommandRunner` class that handles all git command execution with proper error handling, background threading, and output parsing.

**Implementation Details:**
- Create `src/core/GitCommandRunner.h` / `.cpp`:
  ```cpp
  class GitCommandRunner {
  public:
      GitCommandRunner(const std::string& workspace_root, core::EventBus& event_bus);

      struct CommandResult {
          int exit_code;
          std::string stdout_text;
          std::string stderr_text;
          bool success() const { return exit_code == 0; }
      };

      auto RunSync(const std::string& command) -> CommandResult;
      void RunAsync(const std::string& command, std::function<void(CommandResult)> callback);

      // High-level operations
      auto GetStatus() -> std::vector<GitChangeEntry>;
      auto GetBranch() -> std::string;
      auto GetBranches() -> std::vector<std::string>;
      auto GetLog(const std::string& file, int count) -> std::vector<GitLogEntry>;
      auto GetBlame(const std::string& file) -> std::vector<BlameLine>;
      auto GetDiff(const std::string& file, bool staged) -> std::string;

      void Stage(const std::string& path);
      void Unstage(const std::string& path);
      void Commit(const std::string& message);
      void Push();
      void Pull();
      void Fetch();
      void SwitchBranch(const std::string& branch);
      void CreateBranch(const std::string& name);
      void Stash(const std::string& message);
      void StashPop();
  };
  ```
- All commands run via `wxExecute` or `std::system` with captured output
- Async commands run on `std::thread` with callback on main thread
- Error handling: parse stderr for common git errors

**Files Affected:**
- `src/core/GitCommandRunner.h` (new)
- `src/core/GitCommandRunner.cpp` (new)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Sync and async command execution
- Output captured and parsed
- Error messages extracted from stderr
- Background thread for long operations
- Callback on main thread for async

**Dependencies:** None

---

## Task 16: Source Control Badge on Activity Bar

**Title:** Show pending changes count on the git activity bar icon

**Description:** Display a badge on the git activity bar icon showing the total number of changed files (staged + unstaged + untracked).

**Implementation Details:**
- After each status refresh, calculate total changed files
- Call `activity_bar_->SetBadge(ActivityBarItem::Git, total_count)`
- Badge color: accent color
- Badge text: number (e.g., "12")
- Clear badge when no changes

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (publish badge count)
- `src/ui/ActivityBar.cpp` (display badge)

**Acceptance Criteria:**
- Badge shows total changed file count
- Updates on status refresh
- Clears when no changes
- Badge visible and properly themed

**Dependencies:** Task 3

---

## Task 17: Diff Navigation in Editor

**Title:** Navigate between changed regions in the editor

**Description:** Add keyboard shortcuts and gutter buttons to navigate between git change regions (hunks) in the editor: "Go to Next Change" and "Go to Previous Change".

**Implementation Details:**
- Shortcuts: Alt+F5 (next change), Alt+Shift+F5 (previous change)
- In the git change gutter margin, clicking a change indicator scrolls to center that hunk
- `void GoToNextGitChange()` / `void GoToPreviousGitChange()` in EditorPanel
- Commands registered in command palette

**Files Affected:**
- `src/ui/EditorPanel.h` (add navigation methods)
- `src/ui/EditorPanel.cpp` (implement hunk navigation)
- `src/ui/CommandPalette.cpp` (register commands)

**Acceptance Criteria:**
- Navigate between change hunks
- Editor scrolls to center the change
- Wraps around at document boundaries
- Commands in palette and shortcuts

**Dependencies:** Phase 14 Task 6

---

## Task 18: Commit History Graph

**Title:** Show a visual commit graph in the timeline

**Description:** In the timeline view, render a simple branch/merge graph similar to `git log --graph`.

**Implementation Details:**
- Parse `git log --oneline --graph -30` output
- Render graph characters as colored lines:
  - `*` = commit dot (accent color)
  - `|` = vertical line (graph color)
  - `/`, `\` = branch/merge lines
  - Parallel branches in different colors
- Each commit: dot + abbreviated hash + message + relative time
- Clickable to view diff

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add commit graph rendering)

**Acceptance Criteria:**
- Visual graph with branches/merges
- Multiple branches shown in different colors
- Commits clickable
- Graph drawn with wxGraphicsContext lines
- Scrollable for long history

**Dependencies:** Task 13

---

## Task 19: Status Bar Git Integration

**Title:** Enhance status bar with git information

**Description:** Show additional git information in the status bar: branch name (already implemented), sync status (ahead/behind), and click to open branch picker.

**Implementation Details:**
- Status bar git segment:
  ```
  [branch icon] main  ↑2 ↓1
  ```
- "2" ahead (local commits not pushed), "1" behind (remote commits not pulled)
- Click: open branch picker popup (same as Task 8 dropdown)
- Color: normal for clean, accent for dirty (uncommitted changes)

**Files Affected:**
- `src/ui/StatusBarPanel.cpp` (enhance git display)
- `src/ui/SourceControlPanel.cpp` (publish status to status bar)

**Acceptance Criteria:**
- Branch name with ahead/behind counts
- Click opens branch picker
- Color indicates clean/dirty state
- Updates on status change

**Dependencies:** Task 8

---

## Task 20: Git Commit Message Templates

**Title:** Support commit message templates

**Description:** Allow configuring commit message templates that pre-populate the commit input with a standard format (e.g., type prefix, ticket reference).

**Implementation Details:**
- Templates in config:
  ```yaml
  git:
    commit_templates:
      - name: "Conventional"
        template: "feat: \n\nDescription:\n\nBreaking changes: none"
      - name: "Ticket"
        template: "[TICKET-XXX] "
  ```
- Template selector dropdown above commit input
- Selecting a template pre-fills the commit message
- Cursor positioned at the first blank/placeholder

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add template selector)
- `src/core/Config.h` (add template settings)

**Acceptance Criteria:**
- Templates configurable in settings
- Dropdown to select template
- Pre-fills commit input
- Cursor positioned at first placeholder
- Templates persist in config

**Dependencies:** Task 6

---

## Task 21: File Diff Stats

**Title:** Show line addition/deletion counts per file

**Description:** Next to each changed file, show the number of lines added and deleted (e.g., "+12 -3") with green/red coloring.

**Implementation Details:**
- Run `git diff --stat` or `git diff --numstat` to get per-file stats:
  ```
  12    3    src/ui/TabBar.cpp
  5     0    src/ui/NewFile.h
  ```
- Display after filename: "+12 -3" with green for additions, red for deletions
- Also show total stats at the bottom of each section

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add stat display per file)
- `src/core/GitCommandRunner.cpp` (implement numstat query)

**Acceptance Criteria:**
- Line counts shown per file
- Green for additions, red for deletions
- Total stats at section bottom
- Updates on status refresh

**Dependencies:** Task 3

---

## Task 22: Source Control Panel Context Menu

**Title:** Add context menu for source control file entries

**Description:** Right-click on a file in the source control panel shows a context menu with git operations.

**Implementation Details:**
- Context menu items:
  ```
  Open File
  Open Changes (Diff)
  ---
  Stage Changes       (for unstaged files)
  Unstage Changes     (for staged files)
  ---
  Discard Changes
  ---
  Stash File
  ---
  Copy Path
  Copy Relative Path
  Reveal in Explorer
  ```

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add context menu)

**Acceptance Criteria:**
- Context menu shows appropriate items per file status
- All actions functional
- Stage/unstage items conditional on file status

**Dependencies:** Tasks 5, 14

---

## Task 23: Auto-Refresh on Focus

**Title:** Automatically refresh git status when panel gains focus

**Description:** When the application regains focus (e.g., user switches from terminal after committing), automatically refresh the git status.

**Implementation Details:**
- Subscribe to `wxActivateEvent` in MainFrame
- On activate: trigger git status refresh if >2 seconds since last refresh
- Debounce: don't refresh more than once per 2 seconds
- Also refresh on file save events
- Show "Refreshing..." indicator during refresh

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add auto-refresh logic)
- `src/ui/MainFrame.cpp` (forward activate events)

**Acceptance Criteria:**
- Status refreshes on application focus
- Debounced to avoid excessive git calls
- "Refreshing..." indicator shown
- Refreshes on file save

**Dependencies:** Task 2

---

## Task 24: Git Log Integration

**Title:** View full commit log from the panel

**Description:** Add a "View Log" button that opens a commit log viewer showing the full repository history with filtering by branch, author, and date range.

**Implementation Details:**
- "View Log" button in panel header
- Opens in the bottom panel (or a dedicated panel):
  - List of commits: hash, message, author, date
  - Filter by: branch dropdown, author input, date range
  - Click commit to see full diff
  - Graph view (from Task 18) integrated
- Pagination: show 50 commits, "Load More" button

**Files Affected:**
- `src/ui/GitLogPanel.h` (new)
- `src/ui/GitLogPanel.cpp` (new)
- `src/ui/SourceControlPanel.cpp` (add "View Log" button)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Full commit history viewable
- Filterable by branch, author, date
- Click shows commit diff
- Pagination with load more
- Graph view integrated

**Dependencies:** Task 18

---

## Task 25: Source Control Panel Theme

**Title:** Full theme integration for source control panel

**Description:** Ensure all source control elements use theme colors correctly.

**Implementation Details:**
- Color mappings:
  - Panel background: `BgPanel`
  - Section headers: `TextMuted` semibold
  - File names: `TextMain`
  - Status badges: Modified=blue, Added=green, Deleted=red, Untracked=gray
  - Commit input background: `BgApp`
  - Commit button: `AccentPrimary`
  - Branch indicator: `AccentSecondary`
  - Diff additions: `SuccessColor`
  - Diff deletions: `ErrorColor`
- All elements update on theme change

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (theme all elements)

**Acceptance Criteria:**
- All elements themed
- Dark and light themes work
- No hardcoded colors
- Updates on theme change

**Dependencies:** Tasks 1-24

---

## Task 26: Source Control Panel Accessibility

**Title:** Accessibility support for source control panel

**Description:** Ensure the source control panel is fully keyboard navigable and screen reader compatible.

**Implementation Details:**
- Tab order: commit input -> commit button -> staged files -> changed files -> untracked files
- Arrow keys navigate within file lists
- Space toggles stage/unstage
- Enter opens file
- Screen reader: "3 staged changes, 5 unstaged changes, 2 untracked files"
- File entries: "{status} {filename}" (e.g., "Modified TabBar.cpp")

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add accessibility)

**Acceptance Criteria:**
- Full keyboard navigation
- Screen reader support
- File status announced
- Logical tab order

**Dependencies:** Tasks 1-7

---

## Task 27: Source Control Initialization Check

**Title:** Handle non-git workspaces gracefully

**Description:** When the workspace is not a git repository, show an appropriate message with an "Initialize Repository" button.

**Implementation Details:**
- On panel show, check if `.git` exists in workspace root
- If not: show centered message "No source control providers registered" with "Initialize Repository" button
- "Initialize Repository": runs `git init`, creates `.gitignore` with common patterns, refreshes
- Also detect: git not installed (show "Git not found" with install instructions)

**Files Affected:**
- `src/ui/SourceControlPanel.cpp` (add initialization check)

**Acceptance Criteria:**
- Non-git workspaces show appropriate message
- Initialize button creates git repo
- Git not installed detection
- Transitions to normal view after initialization

**Dependencies:** Task 1

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Panel Shell (Task 1) | Medium |
| Status Provider (Task 2) | Medium |
| Changed Files List (Task 3) | High |
| Diff Preview (Task 4) | High |
| Staging (Task 5) | Medium |
| Commit Input (Task 6) | Medium |
| Commit Execution (Task 7) | Medium |
| Branch Switching (Task 8) | High |
| Push/Pull (Task 9) | High |
| Merge Conflicts (Task 10) | High |
| Stash (Task 11) | Medium |
| Git Blame (Task 12) | High |
| Timeline (Task 13) | Medium |
| Discard (Task 14) | Low |
| Git Command Runner (Task 15) | High |
| Activity Badge (Task 16) | Low |
| Diff Navigation (Task 17) | Low |
| Commit Graph (Task 18) | High |
| Status Bar (Task 19) | Low |
| Templates (Task 20) | Low |
| Diff Stats (Task 21) | Low |
| Context Menu (Task 22) | Low |
| Auto-Refresh (Task 23) | Low |
| Git Log (Task 24) | High |
| Theme (Task 25) | Low |
| Accessibility (Task 26) | Medium |
| Init Check (Task 27) | Low |

## Files Created

- `src/ui/SourceControlPanel.h`
- `src/ui/SourceControlPanel.cpp`
- `src/core/GitCommandRunner.h`
- `src/core/GitCommandRunner.cpp`
- `src/ui/GitLogPanel.h`
- `src/ui/GitLogPanel.cpp`

## Files Modified

- `src/ui/GitStatusProvider.h` / `.cpp`
- `src/ui/LayoutManager.cpp`
- `src/ui/StatusBarPanel.cpp`
- `src/ui/ActivityBar.cpp`
- `src/ui/EditorPanel.h` / `.cpp`
- `src/ui/CommandPalette.cpp`
- `src/core/Events.h`
- `src/core/Config.h`
- `src/ui/MainFrame.cpp`
- `CMakeLists.txt`
