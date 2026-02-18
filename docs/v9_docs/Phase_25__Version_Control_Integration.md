# Phase 25: Version Control Integration

## Overview
GitService, RepositoryService, HistoryPanel, DiffPanel, FileSnapshotService, and BlockDiffEngine exist. DiffRenderer exists in rendering. However, the git integration is not wired to a functional user experience: users cannot view git status, stage files, commit, or view history from within MarkAmp. This phase builds a complete version control experience.

## Prerequisites
- Phase 20 (File management and workspace)
- Phase 18 (Tool window system for panels)

## Tasks

### Task 1: Wire GitService to Workspace Detection
**Files:** `src/core/GitService.cpp`, `src/core/GitService.h`
**Description:** GitService exists. Wire it to detect git repositories in the workspace on open. If `.git/` exists, initialize git service with repository state.
**Acceptance Criteria:**
- Git repository detected on workspace open
- Current branch name available
- Dirty file count available
- Remote tracking branch detected
- `GitRepositoryDetectedEvent` emitted
- Non-git workspaces: git features hidden

### Task 2: Wire Source Control Panel
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/GitService.cpp`
**Description:** Add a Source Control panel to the activity bar sidebar. Shows: current branch, changed files (staged, unstaged, untracked), and commit message input.
**Acceptance Criteria:**
- Source Control panel in activity bar
- Branch name with branch switcher
- Changed files list: staged (green), modified (yellow), untracked (gray)
- Commit message input at top
- "Commit" button (requires message)
- File count badge on activity bar icon

### Task 3: Wire Git File Status in Explorer
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/GitService.cpp`
**Description:** File explorer shows git status indicators: M (modified), A (added), D (deleted), U (untracked), C (conflict). Status updates on file operations.
**Acceptance Criteria:**
- Status letter next to file name
- Color: modified (yellow), added (green), deleted (red), untracked (gray)
- Folder shows aggregate status if children have changes
- Status updates on git operations
- `.gitignore`d files dimmed

### Task 4: Wire Stage/Unstage Operations
**Files:** `src/core/GitService.cpp`
**Description:** Stage and unstage files from the Source Control panel and file explorer. Support individual file staging and "Stage All".
**Acceptance Criteria:**
- Click "+" on file stages it
- Click "-" on staged file unstages it
- "Stage All" stages all changes
- "Unstage All" unstages all staged changes
- Right-click file: "Stage Changes", "Discard Changes"
- `GitFilesStagedEvent` emitted

### Task 5: Wire Commit Operations
**Files:** `src/core/GitService.cpp`
**Description:** Commit staged changes with a message. Amend last commit option. Commit message input with character count and conventional commit prefix suggestions.
**Acceptance Criteria:**
- Commit requires non-empty message
- Commit with Cmd+Enter in message input
- Amend checkbox appends to last commit
- Conventional commit prefixes: feat:, fix:, docs:, refactor:
- `GitCommitEvent` emitted with hash and message
- Commit count shows in status bar

### Task 6: Wire DiffPanel for File Diffs
**Files:** `src/ui/DiffPanel.cpp`, `src/rendering/DiffRenderer.cpp`
**Description:** DiffPanel and DiffRenderer exist. Wire them to show file diffs: clicking a changed file in Source Control opens side-by-side diff view.
**Acceptance Criteria:**
- Click changed file opens diff view
- Side-by-side: old (left) and new (right)
- Unified diff mode toggle
- Additions highlighted green, deletions highlighted red
- Line numbers shown
- Navigate between hunks with keyboard

### Task 7: Wire HistoryPanel for Commit Log
**Files:** `src/ui/HistoryPanel.cpp`, `src/ui/HistoryPanel.h`
**Description:** HistoryPanel exists. Wire it to show git log: commit list with hash, message, author, date. Click commit shows its diff.
**Acceptance Criteria:**
- Commit list with hash (short), message, author, relative date
- Click commit shows files changed
- Click file in commit shows diff for that file at that commit
- Graph visualization for merge commits
- Search commits by message
- Infinite scroll or pagination

### Task 8: Wire Branch Operations
**Files:** `src/core/GitService.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Branch management: create branch, switch branch, merge branch, delete branch. Branch name shown in status bar.
**Acceptance Criteria:**
- Status bar shows current branch name
- Click branch name opens branch picker
- "Create Branch" from current HEAD
- "Switch Branch" shows local and remote branches
- Branch switch preserves workspace state
- `GitBranchSwitchedEvent` emitted

### Task 9: Wire FileSnapshotService for File History
**Files:** `src/core/FileSnapshotService.cpp`
**Description:** FileSnapshotService exists. Wire it to show file-level history: all commits that modified the current file, with ability to view file at any commit.
**Acceptance Criteria:**
- "File: Show History" command shows commit list for current file
- Each entry: commit hash, message, date
- Click entry shows file at that commit
- Compare: file at commit vs current
- Restore: revert file to specific commit version
- Timeline view of file changes

### Task 10: Wire Inline Blame (Git Annotate)
**Files:** `src/ui/EditorPanel.cpp`, `src/core/GitService.cpp`
**Description:** Toggle inline blame: show commit info (author, date, message) at the end of each line, dimmed color.
**Acceptance Criteria:**
- "Toggle Git Blame" command
- Each line shows: author, relative date, commit message (truncated)
- Blame info dimmed to not interfere with content
- Hover blame shows full commit details
- Click blame navigates to commit in history
- Blame updates on file save

### Task 11: Wire Gutter Change Indicators
**Files:** `src/ui/EditorPanel.cpp`, `src/core/GitService.cpp`
**Description:** Editor gutter shows change indicators: green bar for added lines, blue bar for modified lines, red triangle for deleted lines.
**Acceptance Criteria:**
- Green gutter bar: added lines (not in last commit)
- Blue gutter bar: modified lines (different from last commit)
- Red triangle: deleted lines (in last commit, not current)
- Click indicator shows inline diff popup
- Indicators update on edit
- Based on diff vs HEAD

### Task 12: Wire Merge Conflict Resolution
**Files:** `src/ui/EditorPanel.cpp`, `src/core/GitService.cpp`
**Description:** When merge conflicts occur, show conflict markers with action buttons: Accept Current, Accept Incoming, Accept Both, Compare.
**Acceptance Criteria:**
- Conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`) highlighted
- Inline buttons: Accept Current, Accept Incoming, Accept Both
- "Compare" opens side-by-side diff
- After resolving all conflicts: "Mark as Resolved"
- Conflict file count in Source Control panel
- `GitConflictResolvedEvent` emitted

### Task 13: Wire Stash Operations
**Files:** `src/core/GitService.cpp`
**Description:** Git stash operations: stash changes, apply stash, pop stash, list stashes, drop stash.
**Acceptance Criteria:**
- "Git: Stash" stashes all uncommitted changes
- "Git: Stash with Message" includes description
- "Git: Apply Stash" applies without removing
- "Git: Pop Stash" applies and removes
- Stash list shown in Source Control panel
- `GitStashAppliedEvent` emitted

### Task 14: Wire Remote Operations
**Files:** `src/core/GitService.cpp`, `src/core/RepositoryService.cpp`
**Description:** Push, pull, and fetch operations. Status bar shows sync status: ahead/behind count relative to tracking branch.
**Acceptance Criteria:**
- "Git: Push" pushes current branch
- "Git: Pull" pulls with rebase or merge (configurable)
- "Git: Fetch" fetches all remotes
- Status bar: "2 up, 1 down" indicators
- Progress indicator during remote operations
- Authentication: credential helper integration

### Task 15: Wire Git Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register git commands: "Git: Commit", "Git: Push", "Git: Pull", "Git: Stash", "Git: Switch Branch", "Git: Create Branch", "Git: Show Log", "Git: Toggle Blame", "Git: Stage File", "Git: Discard Changes".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Git:" prefix
- Commands only active when workspace has git
- Keyboard shortcuts for common operations
- Commands ranked by frequency

### Task 16: Wire Git Status Bar Integration
**Files:** `src/ui/StatusBarPanel.cpp`, `src/core/GitService.cpp`
**Description:** Status bar shows: branch name, sync status (ahead/behind), dirty file count. Click each section for relevant action.
**Acceptance Criteria:**
- Branch name with icon (click: switch branch)
- Sync status: arrows with count (click: sync)
- Dirty count: file icon with count (click: open Source Control)
- All elements update on git operations
- Elements hidden when not in git workspace

### Task 17: Wire BlockDiffEngine for Semantic Diffs
**Files:** `src/core/BlockDiffEngine.cpp`
**Description:** BlockDiffEngine exists. Wire it for semantic Markdown diffs: show changes at the block level (paragraph, heading, list item) rather than line level.
**Acceptance Criteria:**
- Paragraph-level diff: moved paragraphs detected
- Heading-level diff: renamed headings detected
- List item changes shown individually
- Semantic diff mode toggleable (vs line diff)
- Better readability for Markdown changes

### Task 18: Wire Auto-Commit for Workspace Snapshots
**Files:** `src/core/GitService.cpp`, `src/core/Config.h`
**Description:** Optional auto-commit: snapshot workspace state at configurable intervals (e.g., every 30 minutes). Uses a separate branch to avoid polluting main history.
**Acceptance Criteria:**
- Auto-commit on configurable interval (default: disabled)
- Commits to `.markamp-snapshots` branch
- Commit message: "Auto-snapshot: YYYY-MM-DD HH:MM"
- Does not affect working branch
- Configurable: enable/disable, interval, max snapshots
- Snapshot branch pruned: keep last 100

### Task 19: Wire Git Ignore Management
**Files:** `src/core/GitService.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Right-click file: "Add to .gitignore". Show .gitignore patterns in settings. Suggest gitignore patterns for common file types.
**Acceptance Criteria:**
- "Add to .gitignore" in file context menu
- Adds file pattern to .gitignore
- .gitignore editor in settings
- Suggested patterns: .markamp/, *.tmp, .DS_Store
- Ignored files visually distinct in explorer

### Task 20: Add Version Control Tests
**Files:** `tests/unit/test_git_service.cpp`, `tests/unit/test_file_snapshots.cpp`, `tests/unit/test_notebook_diff.cpp`
**Description:** Test version control: git operations, diff rendering, merge conflict detection, and file history.
**Acceptance Criteria:**
- Git status detection and file listing
- Stage/unstage operations
- Commit with message
- Diff generation: additions, deletions, modifications
- Merge conflict detection and resolution
- File history navigation

## Testing Requirements
- Git operations: status, stage, commit, branch
- Diff rendering: additions, deletions, moves
- Merge conflict: detection and resolution
- File history: commit navigation and file-at-commit

## Phase Completion Criteria
- Source Control panel with file status
- Stage, commit, push, pull operations
- Diff viewer with side-by-side and unified modes
- Commit history with file-level navigation
- Merge conflict resolution with inline actions
- Git blame and gutter indicators
- All tests pass
