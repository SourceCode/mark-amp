# Phase 35: Quick Open & File Switching

## Overview

Build a comprehensive quick-open and file switching system that provides instant access to
any file in the workspace with intelligent ranking, preview capabilities, and deep
integration with the tab system. This phase builds on the Command Palette V2's file picker
mode (Phase 31 Task 3) and extends it with file preview on hover, split-open gestures,
pin-to-recent, workspace-grouped recents, and Cmd+Tab-style tab switching. The existing
codebase has `WorkspaceService` with document tracking, `TabBar` with tab management,
`FrecencyTracker` (from Phase 31), and `BreadcrumbBar` for path display.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| CommandPalette (file mode) | `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` | kQuickOpen mode from Phase 31 |
| WorkspaceService | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.h` | Document tracking, workspace folders, find_files |
| TabBar | `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` | Tab rendering and management |
| FrecencyTracker | Phase 31 Task 2 | Frequency + recency scoring |
| BreadcrumbBar | `/Users/ryanrentfro/code/markamp/src/ui/BreadcrumbBar.h` | Path display |

## Dependencies

- Phase 31 (Command Palette V2) provides the file picker mode and FrecencyTracker.
- Phase 32 (Go-To System) provides NavigationService for history tracking.

---

## Tasks

### Task 1: MRU File List Core

**Title:** Build the Most Recently Used file list with frecency scoring

**Description:** Maintain a ranked list of recently opened files using the FrecencyTracker
from Phase 31 Task 2. This list is the primary data source for Cmd+P when no query is typed.

**Implementation Details:**
- Create `RecentFilesManager` in `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.h`.
- Constructor: `RecentFilesManager(FrecencyTracker&, Config&)`.
- Methods: `record_open(file_path)`, `ranked_files(limit) -> vector<RecentFileEntry>`,
  `pin_file(file_path)`, `unpin_file(file_path)`, `remove_file(file_path)`.
- `RecentFileEntry` struct: `string file_path`, `string display_name`, `double frecency_score`,
  `bool is_pinned`, `chrono::system_clock::time_point last_opened`.
- Subscribe to `FileOpenedEvent` to automatically record access.
- Persist pinned files in Config: `"recentFiles.pinned"`.
- Maximum tracked files: 500.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Opening a file records it in the MRU list.
- `ranked_files(10)` returns the top 10 by frecency score.
- Pinned files always appear at the top regardless of score.
- List persists across application restarts.
- Maximum 500 files tracked; oldest are evicted.

**Dependencies:** Phase 31 Task 2 (FrecencyTracker).

---

### Task 2: File Fuzzy Search with Path Matching

**Title:** Enhanced fuzzy search that matches both filename and path segments

**Description:** Extend the file search to match against the full path, not just the
filename. Path segment matching allows queries like "ui/ed" to match "src/ui/EditorPanel.cpp".

**Implementation Details:**
- Use `FuzzyScorer` (Phase 31 Task 1) with a modified scoring function for files.
- Score components:
  - Filename match: weight 3x (most important).
  - Path segment match: weight 1x.
  - Directory proximity: files in the same directory as the active file get a boost.
  - Open-tab boost: currently open files get a +20 bonus.
- For the query "ui/ed", split on "/" and match each segment against path components.
- Display: bold matched characters in both filename and path.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/FileSearchScorer.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/FileSearchScorer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (use FileSearchScorer)

**Acceptance Criteria:**
- "EdPan" matches "EditorPanel.cpp" with high score.
- "ui/ed" matches "src/ui/EditorPanel.cpp".
- "CMake" matches "CMakeLists.txt" at the root.
- Open tabs receive a score boost.
- Files in the active file's directory receive a proximity boost.

**Dependencies:** Phase 31 Task 1 (FuzzyScorer).

---

### Task 3: Preview File on Arrow Key Selection

**Title:** Preview file content when navigating results with arrow keys

**Description:** When the user arrow-keys through file results in Cmd+P, show a preview
of the selected file in the editor area without fully opening it (preview mode).

**Implementation Details:**
- On arrow-key selection change in the file picker, emit `FilePreviewRequestEvent{ file_path }`.
- The editor opens the file in "preview mode": a tab with an italic title that closes
  when another preview is requested or a different file is opened.
- Preview tabs do not count toward the file MRU unless the user explicitly opens them.
- The preview loads only the first 200 lines for performance.
- Cancel preview (Escape) restores the previous editor state.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (preview on arrow)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (preview mode)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` (preview tab support)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (FilePreviewRequestEvent)

**Acceptance Criteria:**
- Arrow-keying through file results previews each file.
- Preview tab has an italic title.
- Only one preview tab exists at a time.
- Pressing Enter converts the preview to a permanent tab.
- Pressing Escape dismisses the preview and restores the previous state.
- Preview does not affect MRU or navigation history.

**Dependencies:** Task 1 (MRU should not record previews).

---

### Task 4: Open File on Enter, Split on Cmd+Enter

**Title:** Wire Enter and Cmd+Enter gestures in the file picker

**Description:** Enter opens the file normally (replacing the preview if one exists).
Cmd+Enter opens the file in a split editor pane alongside the current file.

**Implementation Details:**
- Enter: emit `FileOpenRequestEvent` which opens the file in the current editor group.
- Cmd+Enter: emit `FileSplitOpenRequestEvent{ file_path }` which opens the file in
  a new split pane (right of the current).
- After opening, close the command palette.
- Record the file in FrecencyTracker on open (but not on preview).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (Enter/Cmd+Enter)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (FileSplitOpenRequestEvent)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` (split open handling)

**Acceptance Criteria:**
- Enter opens the file normally.
- Cmd+Enter opens the file in a split pane.
- Palette closes after both actions.
- FrecencyTracker records the access.
- Split pane is positioned to the right of the current editor.

**Dependencies:** Task 1, Phase 31 Task 3.

---

### Task 5: File Icons in Quick Open

**Title:** Show file-type icons next to each file in the quick open results

**Description:** Each file in the quick open list shows an icon based on its extension
using the `FileIconResolver` from Phase 31 Task 3.

**Implementation Details:**
- Use `FileIconResolver` to determine the icon for each file.
- Render the icon (16x16) to the left of the filename.
- Icons: `.md` = document icon, `.cpp/.h` = code icon, `.json/.yaml` = gear icon,
  `.png/.jpg` = image icon, `.txt` = text icon, unknown = generic file icon.
- Icons should be theme-aware (different icon sets for light/dark themes).
- HiDPI support (2x icons).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (icon rendering)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h` (icon set)

**Acceptance Criteria:**
- Each file shows an appropriate type icon.
- Icons differentiate between common file types.
- Icons look correct on both standard and HiDPI displays.
- Light and dark themes show appropriate icon variants.

**Dependencies:** Phase 31 Task 3 (FileIconResolver), Phase 31 Task 8 (IconProvider).

---

### Task 6: Recent Files Grouped by Workspace

**Title:** Group recent files by workspace in the MRU section

**Description:** When showing recent files with no query, group them by the workspace
they belong to (useful when working with multiple workspaces).

**Implementation Details:**
- `RecentFilesManager` tracks which workspace each file belongs to.
- In the Cmd+P empty query state, show files grouped: "Current Workspace" (top),
  "Other Workspaces" (below), with workspace name as section headers.
- Within each group, files are ordered by frecency.
- "Current Workspace" is determined by `WorkspaceService::workspace_folders()`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.h` (workspace tracking)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (grouped display)

**Acceptance Criteria:**
- Empty query shows files grouped by workspace.
- "Current Workspace" section appears first.
- Files from other workspaces appear in a separate section.
- Workspace names are shown as section headers.
- Files within each group are ordered by frecency.

**Dependencies:** Task 1.

---

### Task 7: Pin Recent Files

**Title:** Allow pinning frequently used files to the top of the MRU list

**Description:** Users can pin files so they always appear at the top of the Cmd+P list,
regardless of frecency score.

**Implementation Details:**
- Right-click a file in the quick open results > "Pin to top" / "Unpin".
- Pinned files show a pin icon and appear in a "Pinned" section above recent files.
- Pinned files are stored in `Config::set("recentFiles.pinned", json_array)`.
- Maximum pinned files: 10.
- Keyboard shortcut in quick open: Cmd+Shift+P on a selected file toggles pin.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.h` (pin/unpin)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (pin section, context menu)

**Acceptance Criteria:**
- Right-click shows pin/unpin option.
- Pinned files appear in a "Pinned" section at the top.
- Pin state persists across sessions.
- Maximum 10 pinned files.
- Pin icon is visible.

**Dependencies:** Task 1.

---

### Task 8: Quick Open for Settings/Keybindings/Snippets

**Title:** Extend quick open to search for special files

**Description:** The quick open should also find and open settings, keybinding, and
snippet files with appropriate handling.

**Implementation Details:**
- Add special-file entries to the quick open results:
  - "Preferences: User Settings" -> opens SettingsDialog.
  - "Preferences: Keyboard Shortcuts" -> opens ShortcutEditor.
  - "Preferences: JSON Settings" -> opens SettingsJsonEditor.
- These entries appear when the query matches their names.
- Prefix them with a gear icon.
- Opening these files launches the appropriate dialog instead of the text editor.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (special file entries)

**Acceptance Criteria:**
- Typing "settings" shows the settings entries.
- Typing "keyboard" shows the keybinding entry.
- Selecting "User Settings" opens the SettingsDialog.
- Special entries have a distinct gear icon.
- Special entries appear alongside regular file results.

**Dependencies:** Phase 31 Task 3 (file picker mode).

---

### Task 9: Cmd+Tab Style Tab Switcher

**Title:** Build a Cmd+Tab overlay for cycling between open tabs

**Description:** Ctrl+Tab (or Cmd+Tab on macOS) shows a floating overlay of open tabs
in MRU order. Holding the modifier and pressing Tab cycles through them.

**Implementation Details:**
- Create `TabSwitcherOverlay` as a `wxPopupWindow` positioned center-screen.
- Show open tabs as a vertical list with: file icon, filename, relative path (dimmed).
- Tabs ordered by MRU (most recently active first).
- While Ctrl is held, Tab moves selection down, Shift+Tab moves up.
- Releasing Ctrl switches to the selected tab.
- If only 2 tabs, Ctrl+Tab switches immediately (no overlay).
- Show file modification indicator (dot) for unsaved files.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` (MRU ordering API)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Ctrl+Tab shows the tab switcher overlay.
- Tabs are in MRU order.
- Holding Ctrl and pressing Tab cycles through tabs.
- Releasing Ctrl switches to the selected tab.
- 2-tab case switches immediately without overlay.
- Unsaved files show a modification indicator.

**Dependencies:** None (independent UI component).

---

### Task 10: File Preview Mode (Single Click vs Double Click)

**Title:** Implement preview tabs (single click = preview, double click = keep)

**Description:** Single-clicking a file in the file tree opens it in preview mode
(temporary tab). Double-clicking opens it permanently. This mirrors VSCode behavior.

**Implementation Details:**
- Add `Tab::preview` boolean to the tab data model.
- Single click in FileTreeCtrl: open file in preview mode.
- Double click: convert preview tab to permanent, or open permanent directly.
- Preview tabs have italic titles and a maximum of 1 at a time.
- When a preview tab exists and another file is previewed, replace it.
- Editing a preview file converts it to a permanent tab.
- Config setting: `"editor.enablePreviewMode"` (default: true).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.h` (preview flag, Tab struct)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (preview rendering)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (single/double click)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (promote on edit)

**Acceptance Criteria:**
- Single click opens file in preview mode (italic tab title).
- Double click opens permanently (normal tab title).
- Only one preview tab exists at a time.
- Editing the preview file promotes it to permanent.
- Preview mode can be disabled via settings.

**Dependencies:** None.

---

### Task 11: Open File from Clipboard

**Title:** Detect and offer to open file paths from clipboard content

**Description:** When Cmd+P is opened and the clipboard contains a file path, show it
as the first result with a "Open from clipboard" indicator.

**Implementation Details:**
- On palette open in file picker mode, check clipboard for a plausible file path.
- Path detection: starts with `/`, `~`, or `./`, or contains a file extension.
- If the path exists on disk, show it as the first result with a clipboard icon.
- If the path is relative, resolve it against the workspace root.
- Show "Clipboard: path/to/file.md" as the result label.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (clipboard detection)

**Acceptance Criteria:**
- Clipboard containing a valid file path shows it as the first result.
- Clipboard icon distinguishes it from regular results.
- Relative paths are resolved against workspace root.
- Invalid paths are not shown.
- Non-path clipboard content is ignored.

**Dependencies:** Phase 31 Task 3 (file picker).

---

### Task 12: Quick Open File Size and Modified Date Display

**Title:** Show file metadata (size, modification date) in quick open results

**Description:** In the file picker, show additional metadata below each result: file
size and last modified date.

**Implementation Details:**
- For each file in the quick open results, query `std::filesystem::file_size` and
  `std::filesystem::last_write_time`.
- Display below the path: "4.2 KB - Modified 2 hours ago".
- Format: human-readable size (B, KB, MB) and relative time.
- Cache metadata to avoid filesystem calls on every keystroke.
- Metadata is dimmed (secondary text color).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (metadata display)

**Acceptance Criteria:**
- File size and modification date appear below each result.
- Sizes use human-readable formatting.
- Dates use relative time ("2 hours ago", "yesterday").
- Metadata is cached per palette session.
- Filesystem errors (deleted files) are handled gracefully.

**Dependencies:** Phase 31 Task 3.

---

### Task 13: Quick Open Exclude Patterns

**Title:** Respect exclude patterns to hide irrelevant files from quick open

**Description:** Files matching exclude patterns (like `build/**`, `.git/**`, `*.o`)
should be hidden from quick open results.

**Implementation Details:**
- Read exclude patterns from Config: `"quickOpen.excludePatterns"` (array of globs).
- Default excludes: `["build/**", ".git/**", "*.o", "*.obj", ".DS_Store"]`.
- Apply glob matching to filter files before scoring.
- Allow temporary override: a "Show excluded files" toggle in the quick open UI.
- Patterns are editable in settings.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (exclude filter)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp` (exclude filter)

**Acceptance Criteria:**
- Files matching exclude patterns are hidden from quick open.
- Default excludes filter build artifacts and VCS directories.
- "Show excluded files" toggle temporarily overrides filtering.
- Custom patterns are configurable in settings.

**Dependencies:** Task 2.

---

### Task 14: Recent Files Cleanup

**Title:** Automatically clean up references to deleted files from the MRU list

**Description:** When a file in the MRU list no longer exists on disk, remove it
automatically rather than showing a broken entry.

**Implementation Details:**
- On each `ranked_files()` call, check existence of the top N files.
- Lazily validate: check top 20 immediately, validate the rest in a background thread.
- Remove entries for files that no longer exist.
- Show a subtle "File not found" indicator (gray, strikethrough) for recently deleted
  files before removing them (give the user a chance to see it was removed).
- Log cleanup activity.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp` (cleanup logic)

**Acceptance Criteria:**
- Deleted files are removed from the MRU list.
- Validation does not block the UI (background for non-top files).
- Recently deleted files show briefly with a "not found" indicator.
- Cleanup is logged.
- Large MRU lists do not cause startup delays.

**Dependencies:** Task 1.

---

### Task 15: Quick Open Workspace Switching

**Title:** Add workspace switching to quick open with a special prefix

**Description:** Allow users to switch workspaces from the quick open dialog using a
`ws:` prefix.

**Implementation Details:**
- Detect `ws:` prefix in the quick open input.
- Show recently opened workspaces from Config: `"workspace.recentWorkspaces"`.
- Each workspace shows: workspace name, path, last opened date.
- Selecting a workspace opens it (closing the current one with a save prompt).
- Also accessible via "Open Recent Workspace" command.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (ws: prefix mode)

**Acceptance Criteria:**
- `ws:` prefix shows recent workspaces.
- Selecting a workspace opens it.
- Workspace entries show name, path, and last opened date.
- Save prompt appears if the current workspace has unsaved changes.
- Recent workspaces are persisted.

**Dependencies:** WorkspaceService.

---

### Task 16: Quick Open Status Bar Integration

**Title:** Show quick-open-related information in the status bar

**Description:** Display file count information and search status in the status bar
during quick open interactions.

**Implementation Details:**
- When Cmd+P is active, show "Quick Open: N files indexed" in the status bar.
- During search, show "Searching N files...".
- After selection, briefly show the opened file path.
- Use `StatusBarPanel` to manage these transient messages.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (quick open messages)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (status updates)

**Acceptance Criteria:**
- Status bar shows file count when quick open is active.
- Search progress is shown during file search.
- Opened file path is briefly displayed.
- Messages are transient and do not persist after quick open closes.

**Dependencies:** Phase 31 Task 20 (palette events).

---

### Task 17: Quick Open File Drag and Drop

**Title:** Support dragging files from quick open results to editor split zones

**Description:** Allow users to drag a file from the quick open results list to a split
zone in the editor to open it in that specific pane.

**Implementation Details:**
- Enable drag initiation from the file list items in the palette.
- When dragging starts, show drop zone indicators on the editor area (left, right, bottom).
- Dropping on a zone opens the file in a new split at that position.
- If dropped without hitting a zone, open normally.
- Use `wxDropSource` and `wxFileDropTarget`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (drag initiation)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (drop zones)

**Acceptance Criteria:**
- Files can be dragged from quick open results.
- Drop zones appear on the editor area during drag.
- Dropping on a zone opens the file in a split pane.
- Regular open still works (Enter key).

**Dependencies:** Task 4.

---

### Task 18: Quick Open Animations

**Title:** Smooth transitions when opening and switching files

**Description:** Add subtle animations to file open/switch operations for visual polish.

**Implementation Details:**
- When a file is opened from quick open, fade in the new content.
- When switching between preview files (arrow keys), cross-fade.
- Tab switch animation: brief slide transition.
- All animations are 100-150ms with ease-out.
- Config setting: `"editor.enableAnimations"` (default: true).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (file open animation)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (tab switch animation)

**Acceptance Criteria:**
- File open has a subtle fade-in animation.
- Preview switching cross-fades.
- Tab switches have a brief slide.
- Animations can be disabled via settings.
- Animations do not cause UI jank.

**Dependencies:** None.

---

### Task 19: Quick Open Keyboard Chord: Cmd+P Cmd+P

**Title:** Double Cmd+P toggles between the last two files

**Description:** Pressing Cmd+P twice quickly (within 300ms) toggles between the current
file and the previously active file, without showing the palette.

**Implementation Details:**
- Track the timestamp of the last Cmd+P press.
- If Cmd+P is pressed again within 300ms, switch to the previous file directly.
- Use `RecentFilesManager` or `TabBar` MRU to determine the previous file.
- Show a brief status bar message: "Switched to previous-file.md".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (double-press detection)
- Modified: shortcut handler code.

**Acceptance Criteria:**
- Double Cmd+P within 300ms switches to the previous file.
- Single Cmd+P (held for > 300ms) opens the palette normally.
- Status bar shows the switched file name.
- Works correctly with only 1 file open (no-op).

**Dependencies:** Task 1.

---

### Task 20: Quick Open Performance Optimization

**Title:** Ensure quick open responds within 50ms for large workspaces

**Description:** Optimize the file index and search to handle workspaces with 10,000+ files
without perceptible delay.

**Implementation Details:**
- Build a pre-sorted file index at workspace load time.
- Use a trigram index for O(1) initial filtering.
- Debounce search input by 50ms.
- Limit FuzzyScorer evaluation to the top 500 candidates after trigram filter.
- Cache file metadata (size, modified time) at index build time.
- Background index rebuilds when files are added/removed.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/FileIndex.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/FileIndex.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp`

**Acceptance Criteria:**
- First result appears within 50ms of typing on a 10,000-file workspace.
- Index builds in under 500ms at workspace load.
- Incremental index updates handle file add/remove.
- Memory usage for the index is under 10MB for 10,000 files.

**Dependencies:** Tasks 1, 2.

---

### Task 21: Quick Open Theme Integration

**Title:** Full theme support for quick open components

**Description:** Apply theme tokens to the tab switcher overlay, file preview, and
all quick open visual elements.

**Implementation Details:**
- Theme tokens: `quickOpen.previewTabBackground`, `quickOpen.previewTabForeground`,
  `quickOpen.previewTabBorder`, `tabSwitcher.background`, `tabSwitcher.selectedBackground`,
  `tabSwitcher.foreground`, `tabSwitcher.pathForeground`.
- Apply to: TabSwitcherOverlay, preview tab rendering, file metadata text.
- Subscribe to `ThemeChangedEvent`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp` (theme)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (preview tab theme)

**Acceptance Criteria:**
- All quick open components use theme tokens.
- Theme switch updates colors immediately.
- Preview tabs are visually distinct (italic title, different border).

**Dependencies:** Tasks 3, 9.

---

### Task 22: Quick Open Accessibility

**Title:** Ensure quick open and tab switcher are fully accessible

**Description:** Add proper accessibility labels and screen reader support for the
quick open and tab switcher.

**Implementation Details:**
- Quick open list items: accessible label includes filename and path.
- Tab switcher: accessible label includes tab name and modification status.
- Announce selection changes: "Selected file: EditorPanel.cpp, src/ui/".
- Tab switcher overlay has accessible role "dialog" with "Tab Switcher" label.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (accessibility)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/TabSwitcherOverlay.cpp` (accessibility)

**Acceptance Criteria:**
- VoiceOver reads file results correctly.
- Tab switcher is announced as a dialog.
- Selection changes are announced.
- All interactive elements have accessible labels.

**Dependencies:** Tasks 3, 9.

---

### Task 23: Quick Open Command Registration

**Title:** Register all quick open commands in CommandRegistry

**Description:** Ensure all quick-open-related commands are registered and searchable.

**Implementation Details:**
- Commands to register:
  - `quickOpen.open` (Cmd+P): Open quick open
  - `quickOpen.openInSplit` (Cmd+Enter): Open in split
  - `quickOpen.switchFile` (Ctrl+Tab): Tab switcher
  - `quickOpen.previousFile` (Cmd+P Cmd+P): Toggle previous file
  - `quickOpen.pinFile`: Pin current file
  - `quickOpen.unpinFile`: Unpin current file
  - `quickOpen.clearRecent`: Clear recent files
- All in "Quick Open" category.

**Files Affected:**
- Modified: command registration code.

**Acceptance Criteria:**
- All commands are registered and appear in the palette.
- Shortcuts are displayed alongside commands.
- Commands have descriptions.

**Dependencies:** Task 4.

---

### Task 24: Quick Open File Watcher Integration

**Title:** Update quick open index when files are added or removed from the workspace

**Description:** When files are created, renamed, or deleted in the workspace, update
the quick open file index accordingly.

**Implementation Details:**
- Subscribe to `ConfigFileWatcher` events or filesystem events from the workspace.
- On file creation: add to the index.
- On file deletion: remove from the index and MRU list.
- On file rename: update the index entry and any MRU references.
- Debounce updates by 500ms to batch rapid file operations.
- Log index update activity at DEBUG level.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/FileIndex.cpp` (watcher integration)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/RecentFilesManager.cpp`

**Acceptance Criteria:**
- New files appear in quick open immediately after creation.
- Deleted files disappear from quick open.
- Renamed files update their names in the index.
- Rapid file operations are batched (debounced).

**Dependencies:** Task 20 (FileIndex).

---

### Task 25: Unit Tests for Quick Open

**Title:** Comprehensive test suite for quick open and file switching

**Description:** Write Catch2 tests for the RecentFilesManager, FileSearchScorer,
FileIndex, and tab switching logic.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_quick_open.cpp`
- Sections:
  - RecentFilesManager: record, ranked, pin/unpin, cleanup, max size.
  - FileSearchScorer: filename match, path match, proximity boost, open-tab boost.
  - FileIndex: build, add, remove, search, trigram filter.
  - Tab MRU: MRU ordering, Ctrl+Tab cycle, two-tab shortcut.
  - PreviewMode: single-click preview, double-click permanent, edit promotes.
  - QuickOpen: exclude patterns, special files, workspace grouping.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_quick_open.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All test sections pass.
- RecentFilesManager tests cover frecency ordering and pin behavior.
- FileSearchScorer tests cover all scoring signals.
- At least 30 test cases.

**Dependencies:** Tasks 1, 2, 10, 20.
