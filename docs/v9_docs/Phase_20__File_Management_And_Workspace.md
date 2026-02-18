# Phase 20: File Management and Workspace

## Overview
VaultService, WorkspaceService, AsyncFileLoader, FileTreeCtrl, RecentWorkspaces, and SampleFiles exist but workspace management is fragmented. VaultService manages vault operations, WorkspaceService handles workspace state, but the file explorer tree, recent files, file operations (rename, move, delete), and workspace initialization are not fully integrated. This phase completes the file management experience.

## Prerequisites
- Phase 06 (Workbench navigation with Explorer sidebar)
- Phase 02 (Config system for workspace settings)

## Tasks

### Task 1: Wire FileTreeCtrl to VaultService
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/VaultService.cpp`
**Description:** FileTreeCtrl exists. Wire it to VaultService for workspace-aware file browsing: files loaded from vault root, file watching for external changes, virtual folders for tags and recent files.
**Acceptance Criteria:**
- File tree shows workspace directory structure
- File watcher detects external changes (create, delete, rename)
- Tree updates automatically on file system changes
- Virtual folders: "Recent Files", "Tagged Files" at top level
- Hidden files/folders configurable (default: hide .git, .markamp)

### Task 2: Wire File Operations in Explorer
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/VaultService.cpp`
**Description:** Complete file operations in the file explorer: New File, New Folder, Rename, Delete, Move (drag-and-drop), Copy, Duplicate.
**Acceptance Criteria:**
- New File: creates file with name input inline
- New Folder: creates folder with name input inline
- Rename: inline rename on F2
- Delete: confirmation dialog, moves to trash
- Drag-and-drop: move files between folders
- Copy: with Cmd+C / Cmd+V
- Duplicate: creates "filename (copy).md"

### Task 3: Wire File Rename with Link Update
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/BacklinkIndex.cpp`, `src/core/WikiLinkParser.cpp`
**Description:** When a file is renamed in the explorer, all wiki-links referencing it are updated across the workspace. Uses BacklinkIndex to find references.
**Acceptance Criteria:**
- Rename triggers backlink scan
- All `[[old-name]]` references updated to `[[new-name]]`
- Confirmation dialog shows affected files
- Undo support for the rename + link updates
- `FileRenamedEvent` emitted with old and new paths

### Task 4: Wire AsyncFileLoader for Fast File Opening
**Files:** `src/core/AsyncFileLoader.cpp`, `src/core/AsyncFileLoader.h`
**Description:** AsyncFileLoader exists. Wire it to load files asynchronously: file content loaded in background thread, editor populated on completion, progress indicator for large files.
**Acceptance Criteria:**
- Files > 100KB loaded asynchronously
- Loading indicator shown in editor tab
- Editor populated on load completion
- Cancellable: switching tabs cancels pending load
- `FileLoadedEvent` emitted with timing

### Task 5: Wire RecentWorkspaces for Quick Access
**Files:** `src/core/RecentWorkspaces.cpp`, `src/ui/StartupPanel.cpp`
**Description:** RecentWorkspaces tracks recently opened workspaces. Wire to startup panel and File menu for quick access.
**Acceptance Criteria:**
- Recent workspaces shown on startup panel
- File > Recent Workspaces menu
- Last 10 workspaces remembered
- Invalid paths (deleted) removed automatically
- "Clear Recent" command available

### Task 6: Wire Workspace Initialization
**Files:** `src/core/WorkspaceService.cpp`, `src/core/VaultService.cpp`
**Description:** When opening a workspace for the first time, initialize `.markamp/` directory with default config, search index, and workspace state.
**Acceptance Criteria:**
- `.markamp/` directory created on first open
- Default config.yaml created with workspace settings
- `.markamp/boards/` directory for canvas boards
- `.markamp/notebooks/` directory for notebooks
- `.markamp/state.json` for workspace state (open tabs, layout)
- `WorkspaceInitializedEvent` emitted

### Task 7: Wire Workspace State Persistence
**Files:** `src/core/WorkspaceService.cpp`, `src/ui/LayoutManager.cpp`
**Description:** Persist workspace state on close, restore on open: open tabs, active tab, split view configuration, panel layout, sidebar width, scroll positions.
**Acceptance Criteria:**
- Open tabs saved with file paths and order
- Active tab remembered
- Split view configuration saved
- Panel layout (which panels, positions, sizes) saved
- Scroll position per file saved
- State restored on workspace reopen

### Task 8: Wire File Template System
**Files:** `src/core/TemplateEngine.cpp`, `src/core/TemplateFunctions.cpp`
**Description:** TemplateEngine and TemplateFunctions exist. Wire file templates: "New from Template" command shows template picker. Templates define frontmatter, content structure, and variables.
**Acceptance Criteria:**
- "New from Template" command in file explorer and command palette
- Template picker shows available templates
- Templates support variables: `{{date}}`, `{{title}}`, `{{author}}`
- Custom templates in `.markamp/templates/`
- Built-in templates: blank, daily note, meeting notes, project, journal

### Task 9: Wire Daily Note Service
**Files:** `src/core/DailyNoteService.cpp`, `src/ui/MainFrame.cpp`
**Description:** DailyNoteService exists. Wire "Open Daily Note" command: creates or opens today's daily note from template. Configurable path pattern and template.
**Acceptance Criteria:**
- "Open Daily Note" command (Cmd+Shift+D)
- Creates file from daily note template if not exists
- Path configurable: `daily/YYYY-MM-DD.md` (default)
- Template configurable in settings
- Previous/Next daily note navigation

### Task 10: Wire Folder-Level Operations
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/VaultService.cpp`
**Description:** Folder context menu: New File in Folder, New Subfolder, Collapse All, Expand All, Reveal in Finder/Explorer, Copy Path, Open in Terminal.
**Acceptance Criteria:**
- Right-click folder shows context menu
- "New File" creates file inside clicked folder
- "Reveal in Finder" opens OS file manager
- "Copy Path" copies absolute path
- "Open in Terminal" opens terminal at folder path
- "Collapse All" collapses all children

### Task 11: Wire File Search in Explorer
**Files:** `src/ui/FileTreeCtrl.cpp`
**Description:** Type to search/filter in file explorer: as user types, tree filters to show matching files. Filter persists until cleared.
**Acceptance Criteria:**
- Typing in explorer focuses search filter
- Matching files shown, non-matching hidden
- Parent folders of matches stay visible
- Match highlighting in file names
- Escape clears filter
- Filter indicator shown when active

### Task 12: Wire File Sorting Options
**Files:** `src/ui/FileTreeCtrl.cpp`
**Description:** File explorer sort options: Name (A-Z, Z-A), Modified Date (Newest, Oldest), Type (extension), Size. Folders always sorted before files.
**Acceptance Criteria:**
- Sort selector in explorer toolbar
- Four sort modes with ascending/descending
- Folders always appear before files
- Sort persisted per workspace
- Default: Name A-Z

### Task 13: Wire File Badges
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/DiagnosticsService.cpp`
**Description:** File tree shows badges: modified (dot), errors (red), warnings (yellow), git status (M/A/D). Badges update in real-time.
**Acceptance Criteria:**
- Modified file: colored dot indicator
- File with errors: red dot
- File with warnings: yellow dot
- Git status letters if git workspace
- Badges update on file save and diagnostic change

### Task 14: Wire Drag-and-Drop File Import
**Files:** `src/ui/MainFrame.cpp`, `src/core/VaultService.cpp`
**Description:** Dragging files from OS file manager into MarkAmp imports them: Markdown files opened, images copied to assets folder, other files copied to workspace.
**Acceptance Criteria:**
- Drag .md file: opens in editor
- Drag image: copies to `.markamp/assets/`, inserts link in active editor
- Drag other file: copies to workspace root
- Drag folder: shows "Import Folder" dialog
- Multiple file drag supported

### Task 15: Wire Clipboard Integration for Files
**Files:** `src/core/ClipboardService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** ClipboardService exists. Wire image paste: pasting an image from clipboard saves to `.markamp/assets/` and inserts Markdown image link.
**Acceptance Criteria:**
- Cmd+V with image in clipboard: save and insert link
- Image saved to `.markamp/assets/pasted-{timestamp}.png`
- Markdown link inserted: `![](assets/pasted-{timestamp}.png)`
- Configurable asset directory
- `ImagePastedEvent` emitted

### Task 16: Wire File Move Across Workspaces
**Files:** `src/core/VaultService.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Support moving files between workspaces with link updates: "Move to Workspace" command moves file and updates references in both source and target workspaces.
**Acceptance Criteria:**
- "Move to Workspace" shows workspace picker
- File moved to target workspace
- References in source workspace updated to cross-workspace link
- References in target workspace created
- Undo support

### Task 17: Wire Workspace Quick Switcher
**Files:** `src/ui/CommandPalette.cpp`, `src/core/RecentWorkspaces.cpp`
**Description:** Quick workspace switcher (Cmd+Shift+O): shows recent workspaces with fuzzy search. Switching workspaces saves current state and restores target state.
**Acceptance Criteria:**
- Cmd+Shift+O opens workspace switcher
- Recent workspaces listed with paths
- Fuzzy search to filter
- Current workspace state saved before switch
- Target workspace state restored after switch

### Task 18: Wire SampleFiles for Onboarding
**Files:** `src/core/SampleFiles.cpp`, `src/core/SampleFiles.h`
**Description:** SampleFiles exists. Wire it to create sample content on first workspace open: welcome document, getting started guide, sample notebook, sample board.
**Acceptance Criteria:**
- Sample files created on first workspace initialization
- Welcome.md with product overview
- Getting Started.md with feature walkthrough
- Sample Notebook with code examples
- Sample Board with canvas demonstration
- Skip sample files option in settings

### Task 19: Wire File Conflict Resolution
**Files:** `src/core/VaultService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** When a file is modified externally while open in the editor, show conflict notification: "File changed on disk. Reload or keep editor version?"
**Acceptance Criteria:**
- File watcher detects external changes to open files
- Notification shown: "File changed on disk"
- Actions: "Reload" (discard editor changes), "Keep" (ignore disk change), "Compare" (show diff)
- Auto-reload configurable for unmodified files
- `FileConflictDetectedEvent` emitted

### Task 20: Add File Management Tests
**Files:** `tests/unit/test_vault_service.cpp`, `tests/unit/test_file_tree.cpp`, `tests/unit/test_file_system.cpp`
**Description:** Test file management: CRUD operations, rename with link updates, workspace state, and file watching.
**Acceptance Criteria:**
- File CRUD operations work correctly
- Rename updates all wiki-link references
- Workspace state save/restore round-trip
- File watcher detects all change types
- Template creation produces valid files

## Testing Requirements
- File operations: create, rename, delete, move with link updates
- Workspace state: save/restore preserves all state
- File watching: detects external changes
- Template system: variable substitution and creation

## Phase Completion Criteria
- Full file explorer with CRUD operations
- Rename propagates link updates across workspace
- Workspace state persists and restores
- File templates with variable substitution
- Daily note service functional
- All tests pass
