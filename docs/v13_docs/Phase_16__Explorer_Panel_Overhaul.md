# Phase 16: Explorer Panel Overhaul

## Objective

Overhaul the `FileTreeCtrl` and surrounding explorer sidebar into a feature-rich, VSCode-equivalent file explorer panel. The current implementation (`FileTreeCtrl` at `src/ui/FileTreeCtrl.h`, 132 lines header) provides custom-rendered tree nodes with folder/file icons, chevrons, type-ahead search, context menus, keyboard navigation, and filtering. This phase adds language-specific file-type icons, git status decorations, an "Open Editors" section, a workspace section header with actions, drag-to-move files, inline rename, copy/paste, and file watcher integration.

## Prerequisites

- `FileTreeCtrl` at `src/ui/FileTreeCtrl.h` / `.cpp` with `core::FileNode` tree model
- `core::FileNode` struct with `name`, `path`, `is_directory`, `is_expanded`, `children`
- `FileTypeIconRegistry` from Phase 11 Task 1 (file extension to icon mapping)
- `ThemeEngine` and `EventBus`
- Existing context menus for files and empty areas
- `LayoutManager` managing sidebar layout

## Deliverables

A complete file explorer panel with 26 tasks covering file-type icons, git decorations, open editors section, file operations, drag/drop, and file watching.

---

## Task 1: Language-Specific File Icons

**Title:** Integrate FileTypeIconRegistry into FileTreeCtrl rendering

**Description:** Replace the generic `icon_file_` and `icon_file_text_` bitmaps with language-specific icons from the `FileTypeIconRegistry` (Phase 11). Each file in the tree shows an icon matching its extension (C++ = blue, Python = green, Markdown = purple, etc.).

**Implementation Details:**
- Add `FileTypeIconRegistry* icon_registry_` to `FileTreeCtrl`
- In `DrawNode()`, replace file icon drawing:
  ```cpp
  if (!node.is_directory) {
      icon_registry_->DrawFileIcon(dc, node.name, icon_x, icon_y, kIconSize);
  }
  ```
- Pass `FileTypeIconRegistry&` through constructor
- Ensure icons render at 16x16 (`kIconSize`)

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add icon registry dependency)
- `src/ui/FileTreeCtrl.cpp` (modify `DrawNode()` to use registry)
- `src/ui/LayoutManager.cpp` (pass icon registry)

**Acceptance Criteria:**
- Each file shows extension-appropriate icon
- All 20+ supported extensions have distinct icons
- Unknown extensions show generic file icon
- Icons theme-colored and properly sized

**Dependencies:** Phase 11 Task 1

---

## Task 2: Folder Icons (Open/Closed State)

**Title:** Differentiate open and closed folder icons

**Description:** Replace the existing folder icons with distinct open (expanded) and closed (collapsed) folder icons. Open folders show an "open folder" glyph, closed folders show a "closed folder" glyph. Special folders (`.git`, `node_modules`, `build`) show distinct icons.

**Implementation Details:**
- Define special folder icons in the icon registry:
  ```cpp
  enum class FolderIconType { Normal, Git, NodeModules, Build, Src, Docs, Tests, Config };
  auto GetFolderIconType(const std::string& folder_name) -> FolderIconType;
  ```
- In `DrawNode()`:
  ```cpp
  if (node.is_directory) {
      auto folder_type = GetFolderIconType(node.name);
      if (node.is_expanded) {
          DrawOpenFolderIcon(dc, folder_type, icon_x, icon_y, kIconSize);
      } else {
          DrawClosedFolderIcon(dc, folder_type, icon_x, icon_y, kIconSize);
      }
  }
  ```
- Special folders: `.git` = red lock, `src` = blue braces, `tests` = green flask, `build` = gray gear

**Files Affected:**
- `src/ui/FileTypeIconRegistry.h` (add folder icon types)
- `src/ui/FileTypeIconRegistry.cpp` (implement folder icon drawing)
- `src/ui/FileTreeCtrl.cpp` (use folder-specific icons)

**Acceptance Criteria:**
- Open and closed folders have distinct icons
- Special folders (src, tests, build, .git) have unique icons
- Icons are 16x16 and theme-colored
- Transition between open/closed is immediate on expand/collapse

**Dependencies:** Task 1

---

## Task 3: Explorer Action Toolbar

**Title:** Add an action toolbar above the file tree

**Description:** Add a thin (28px) toolbar above the file tree with action buttons: "New File", "New Folder", "Collapse All", "Refresh", "Toggle Filter". Buttons are icon-only with tooltips.

**Implementation Details:**
- Create a `wxPanel` toolbar above the file tree in the explorer panel layout
- Buttons rendered as small icons (16x16) in a horizontal row:
  - New File: "+" with file icon
  - New Folder: "+" with folder icon
  - Collapse All: downward arrows icon
  - Refresh: circular arrow icon
  - Toggle Filter: funnel icon
- Each button has hover state (lighter background) and tooltip
- Wire actions:
  - New File: show input field for filename, create file
  - New Folder: show input field for folder name, create directory
  - Collapse All: call `FileTreeCtrl::CollapseAllNodes()`
  - Refresh: rescan filesystem and rebuild tree
  - Toggle Filter: show/hide filter input

**Files Affected:**
- `src/ui/ExplorerToolbar.h` (new)
- `src/ui/ExplorerToolbar.cpp` (new)
- `src/ui/LayoutManager.cpp` (integrate toolbar above file tree)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Toolbar visible above file tree
- All 5 actions functional
- Icons with tooltips
- Hover states on buttons
- Actions fire correct operations

**Dependencies:** None

---

## Task 4: Git Status File Decorations

**Title:** Show git status indicators on files in the tree

**Description:** Display git status decorations on files and folders: modified files show "M" badge in blue, untracked files show "U" in green, deleted files show "D" in red, renamed files show "R" in purple. Folders show the aggregate status of their contents.

**Implementation Details:**
- Create `GitStatusProvider` that runs `git status --porcelain` and parses output:
  ```cpp
  struct GitFileStatus {
      std::string path;
      char index_status;   // ' ', 'M', 'A', 'D', 'R', '?'
      char working_status; // ' ', 'M', 'A', 'D', 'R', '?'
  };
  auto GetFileStatuses(const std::string& workspace_root) -> std::vector<GitFileStatus>;
  ```
- In `DrawNode()`, after drawing the filename, draw status badge:
  ```cpp
  if (auto status = git_status_provider_->GetStatus(node.path)) {
      wxColour color = StatusToColor(status->working_status);
      std::string badge = StatusToBadge(status->working_status);
      DrawBadge(dc, badge, color, name_x + name_width + 4, y_offset);
  }
  ```
- File name text color also tinted: modified=blue, untracked=green, deleted=red (with strikethrough)
- Folders: scan children for any status, show aggregate badge
- Refresh on file save, focus gain, and manual refresh

**Files Affected:**
- `src/ui/GitStatusProvider.h` (new)
- `src/ui/GitStatusProvider.cpp` (new)
- `src/ui/FileTreeCtrl.h` (add git status integration)
- `src/ui/FileTreeCtrl.cpp` (render status decorations)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Modified files show "M" badge in blue
- Untracked files show "U" badge in green
- Deleted files show "D" badge in red
- Filename text tinted by status color
- Folders show aggregate status
- Status refreshes on save and focus

**Dependencies:** None

---

## Task 5: Open Editors Section

**Title:** Add "Open Editors" section at the top of the explorer

**Description:** Display an "Open Editors" collapsible section at the top of the explorer panel, listing all currently open files. Each entry shows the file-type icon, filename, modified dot, and close button. Clicking an entry switches to that tab.

**Implementation Details:**
- Create `OpenEditorsSection` widget (custom-painted panel):
  - Header: "OPEN EDITORS" label with collapse chevron and count badge
  - List: one row per open file, matching TabBar's file list
  - Each row: file icon + filename + modified dot + close "x" on hover
  - Click row: switch to that tab
  - Click "x": close the tab (with save confirmation if modified)
- Height: `header(24px) + rows(24px each)`, max 200px then scrollable
- Subscribe to tab events to stay synchronized

**Files Affected:**
- `src/ui/OpenEditorsSection.h` (new)
- `src/ui/OpenEditorsSection.cpp` (new)
- `src/ui/LayoutManager.cpp` (add above file tree in explorer)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Shows all open files
- Matches TabBar file list exactly
- Click switches to tab
- Close button closes tab
- Modified indicator shown
- Collapsible section
- Synchronized with tab events

**Dependencies:** None

---

## Task 6: Workspace Section Header

**Title:** Add workspace name header above the file tree

**Description:** Display a "WORKSPACE" section header above the file tree showing the workspace/project name. If multiple workspaces are open (future multi-root workspace support), each gets its own header.

**Implementation Details:**
- Header: 24px tall, "WORKSPACE_NAME" in 9pt uppercase semibold, `TextMuted` color
- Right side: collapse chevron
- Background: slightly lighter than sidebar
- Click header to collapse/expand the workspace tree
- Workspace name derived from the root folder name
- If no workspace open, show "NO FOLDER OPEN" with an "Open Folder" button

**Files Affected:**
- `src/ui/LayoutManager.cpp` (add workspace header)

**Acceptance Criteria:**
- Workspace name displayed in header
- Header collapsible
- "NO FOLDER OPEN" state with action button
- Proper theming and styling

**Dependencies:** None

---

## Task 7: Drag Files to Reorder/Move

**Title:** Enable drag-and-drop to move files within the tree

**Description:** Allow dragging files and folders within the file tree to move them to a different directory. Show a visual drop target indicator and perform the filesystem move on drop.

**Implementation Details:**
- Start drag on mouse down + 5px movement threshold
- Visual indicators during drag:
  - Dragged item follows cursor as semi-transparent ghost
  - Drop target folder highlighted with accent border
  - Insertion line between files for reordering within a folder
- On drop:
  1. Compute source and destination paths
  2. Call `std::filesystem::rename(source, destination / source_name)`
  3. Refresh tree
  4. If file was open in editor, update tab path
- Cancel drag with Escape
- Cannot drop a folder into itself or its children

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add drag state tracking)
- `src/ui/FileTreeCtrl.cpp` (implement drag/drop rendering and file move)

**Acceptance Criteria:**
- Files can be dragged to different folders
- Ghost item follows cursor during drag
- Drop target highlighted
- File actually moved on filesystem
- Open tabs updated after move
- Cannot create circular moves
- Escape cancels drag

**Dependencies:** None

---

## Task 8: Inline Rename (F2)

**Title:** Implement inline file rename in the tree

**Description:** Pressing F2 on a selected file/folder shows an inline text input overlaying the filename, allowing the user to rename it without a dialog.

**Implementation Details:**
- On F2 key press (or context menu "Rename"):
  1. Create a `wxTextCtrl` overlay positioned exactly over the filename text
  2. Pre-fill with the current name, select all text
  3. On Enter: perform rename
  4. On Escape: cancel rename
  5. On focus loss: perform rename
- Rename operation:
  ```cpp
  std::error_code ec;
  std::filesystem::rename(old_path, new_path, ec);
  if (ec) { ShowError("Rename failed: " + ec.message()); }
  ```
- Update all open tabs that reference the old path
- Publish `FileRenamedEvent(old_path, new_path)` on EventBus

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add rename state, text control pointer)
- `src/ui/FileTreeCtrl.cpp` (implement inline rename)
- `src/core/Events.h` (add `FileRenamedEvent`)

**Acceptance Criteria:**
- F2 activates inline rename
- Text control appears over filename
- Enter commits rename
- Escape cancels
- Filesystem rename performed
- Open tabs updated
- Invalid names rejected (empty, /, etc.)

**Dependencies:** None

---

## Task 9: Delete with Confirmation

**Title:** Implement file/folder delete with confirmation dialog

**Description:** Delete key or context menu "Delete" on a selected file/folder shows a confirmation dialog, then moves the item to the system trash (not permanent delete).

**Implementation Details:**
- On Delete key or context menu:
  ```cpp
  wxMessageDialog dlg(this,
      "Are you sure you want to delete '" + node.name + "'?\nThis will move it to the Trash.",
      "Delete", wxYES_NO | wxNO_DEFAULT | wxICON_WARNING);
  if (dlg.ShowModal() == wxID_YES) {
      MoveToTrash(node.path);
  }
  ```
- `MoveToTrash()`: platform-specific:
  - macOS: `NSFileManager trashItemAtURL:`
  - Linux: `gio trash`
  - Windows: `SHFileOperation` with `FOF_ALLOWUNDO`
- Close any open tabs for the deleted file
- Publish `FileDeletedEvent(path)` on EventBus
- Recursive delete for folders (with additional warning)

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (implement delete with confirmation)
- `src/platform/MacPlatform.h` / `.cpp` (add `MoveToTrash`)
- `src/platform/LinuxPlatform.h` / `.cpp` (add `MoveToTrash`)
- `src/core/Events.h` (add `FileDeletedEvent`)

**Acceptance Criteria:**
- Confirmation dialog shown before delete
- File moved to system trash (recoverable)
- Folder delete includes recursive warning
- Open tabs for deleted files closed
- Tree refreshed after delete
- Event published for other consumers

**Dependencies:** None

---

## Task 10: Copy/Paste Files

**Title:** Implement copy/paste for files in the tree

**Description:** Support Cmd+C to copy selected file(s), Cmd+V to paste into the selected folder, Cmd+X to cut (move on paste). Support both keyboard shortcuts and context menu items.

**Implementation Details:**
- Clipboard model:
  ```cpp
  struct FileClipboard {
      std::vector<std::string> paths;
      bool is_cut{false}; // cut = move on paste, copy = duplicate on paste
  };
  FileClipboard file_clipboard_;
  ```
- Copy (Cmd+C): store selected file paths in clipboard
- Cut (Cmd+X): store paths with `is_cut = true`
- Paste (Cmd+V):
  - If copy: `std::filesystem::copy(source, dest / source_name)`
  - If cut: `std::filesystem::rename(source, dest / source_name)`
  - Handle name conflicts: append " (copy)" suffix
- Context menu: "Copy", "Cut", "Paste"
- Visual feedback: cut files shown with dimmed opacity until paste completes

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add clipboard state)
- `src/ui/FileTreeCtrl.cpp` (implement copy/cut/paste)

**Acceptance Criteria:**
- Cmd+C copies selected files
- Cmd+V pastes into selected folder
- Cmd+X cuts (moves on paste)
- Name conflicts handled with suffix
- Cut files appear dimmed
- Context menu items available
- Multi-file selection supported

**Dependencies:** None

---

## Task 11: File Watcher Integration

**Title:** Monitor filesystem for external changes

**Description:** Watch the workspace directory for external filesystem changes (files created, modified, deleted, renamed by other tools) and automatically refresh the affected parts of the file tree.

**Implementation Details:**
- Create `FileWatcher` class using platform-specific APIs:
  - macOS: `FSEvents` (already partially in platform layer)
  - Linux: `inotify`
  - Windows: `ReadDirectoryChangesW`
- `FileWatcher` runs on a background thread, publishes events to EventBus on main thread:
  ```cpp
  MARKAMP_DECLARE_EVENT(FileSystemChangedEvent, std::string path, ChangeType type);
  enum class ChangeType { Created, Modified, Deleted, Renamed };
  ```
- `FileTreeCtrl` subscribes and updates the affected subtree
- Debounce: batch changes within 500ms to avoid excessive refreshes
- If an open file is modified externally, prompt user: "File has changed on disk. Reload?"

**Files Affected:**
- `src/core/FileWatcher.h` (new)
- `src/core/FileWatcher.cpp` (new)
- `src/platform/MacPlatform.h` / `.cpp` (add FSEvents watcher)
- `src/ui/FileTreeCtrl.cpp` (subscribe to file system events)
- `src/ui/LayoutManager.cpp` (handle external file changes)
- `src/core/Events.h` (add file system events)

**Acceptance Criteria:**
- External file creation detected and tree updated
- External deletion detected and tree updated
- External modification triggers reload prompt for open files
- Debounced to avoid excessive UI updates
- Watcher runs on background thread
- Clean shutdown on application exit

**Dependencies:** None

---

## Task 12: File Tree Decorations API

**Title:** Create an extensible decoration API for file tree items

**Description:** Design an API that allows other components (git provider, diagnostics, extensions) to add visual decorations to file tree items: badges, color tints, icons, and tooltips.

**Implementation Details:**
- Create `IFileTreeDecorationProvider`:
  ```cpp
  struct FileDecoration {
      std::string badge_text;   // e.g., "M", "3 errors"
      wxColour badge_color;
      wxColour text_tint;       // tint the filename color
      std::string tooltip_suffix; // appended to default tooltip
      bool strikethrough{false};  // deleted files
  };

  class IFileTreeDecorationProvider {
  public:
      virtual ~IFileTreeDecorationProvider() = default;
      virtual auto GetDecoration(const std::string& path) -> std::optional<FileDecoration> = 0;
  };
  ```
- `FileTreeCtrl` queries all registered providers during rendering
- Multiple providers can decorate the same file (badges concatenated, colors blended)
- Built-in providers: `GitStatusProvider`, `DiagnosticsProvider` (error count per file)

**Files Affected:**
- `src/ui/FileTreeDecorationProvider.h` (new)
- `src/ui/FileTreeCtrl.h` (add provider registration)
- `src/ui/FileTreeCtrl.cpp` (query providers during render)

**Acceptance Criteria:**
- Provider interface is clean and extensible
- Multiple providers can decorate simultaneously
- Git status and diagnostics providers both work
- Decorations render correctly in draw order
- Performance: provider queries cached

**Dependencies:** Task 4

---

## Task 13: Multi-Selection in File Tree

**Title:** Support selecting multiple files in the tree

**Description:** Allow Ctrl+Click to add individual files to selection and Shift+Click to select a range. Multi-selection enables batch operations (delete, copy, move).

**Implementation Details:**
- Add `std::unordered_set<std::string> selected_node_ids_` member
- Ctrl+Click: toggle selection of clicked node
- Shift+Click: select range from last selected to clicked node
- Draw selected nodes with accent background tint
- Context menu adapts for multi-selection:
  - "Delete N files"
  - "Copy N files"
  - "Move to Folder..."
- Cmd+A: select all visible files

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add multi-selection state)
- `src/ui/FileTreeCtrl.cpp` (implement multi-selection logic, update rendering)

**Acceptance Criteria:**
- Ctrl+Click adds/removes from selection
- Shift+Click selects range
- Multiple selected nodes highlighted
- Context menu shows batch operations
- Cmd+A selects all
- Keyboard navigation with Shift extends selection

**Dependencies:** None

---

## Task 14: File Tree Node Badges

**Title:** Show count badges on folder nodes

**Description:** Folders can display count badges showing aggregate information: number of errors in contained files, number of modified files, etc.

**Implementation Details:**
- In `DrawNode()`, for folders, render badge after folder name:
  ```cpp
  int total_errors = GetDescendantErrorCount(node);
  if (total_errors > 0) {
      DrawBadge(dc, std::to_string(total_errors), error_color, badge_x, badge_y);
  }
  ```
- Badge: small rounded rectangle with count text
- Color: red for errors, yellow for warnings, blue for git changes
- Badge from highest-severity decorator takes priority

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (add badge rendering for folders)

**Acceptance Criteria:**
- Folders show error count badges
- Multiple badge types with priority ordering
- Badges update when contained file status changes
- Badge rendering is efficient (cached counts)

**Dependencies:** Task 12

---

## Task 15: File Tree Expand/Collapse Animation

**Title:** Animate folder expand/collapse

**Description:** When a folder is expanded or collapsed, animate the children sliding in/out over 100ms instead of appearing/disappearing instantly.

**Implementation Details:**
- On expand:
  - Set `expanding_node_id_` and `expand_progress_ = 0.0`
  - Animate height of children rows from 0 to `kRowHeight` each
  - Drive via timer at 16ms interval
- On collapse:
  - Animate height from `kRowHeight` to 0
  - Remove children from rendering after animation completes
- Also animate the chevron rotation (90 degrees for open, 0 for closed)

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add animation state)
- `src/ui/FileTreeCtrl.cpp` (implement expand/collapse animation)

**Acceptance Criteria:**
- Children slide in/out over 100ms
- Chevron rotates smoothly
- Animation interruptible (clicking during animation)
- Performance: no lag for folders with 100+ children

**Dependencies:** None

---

## Task 16: New File Inline Creation

**Title:** Create new files via inline input in the tree

**Description:** When "New File" is triggered (toolbar button or context menu), show an inline text input at the appropriate position in the tree (inside the selected folder or at root level) for entering the new filename.

**Implementation Details:**
- Create a `wxTextCtrl` overlay positioned at the insertion point:
  - Inside the selected folder (if a folder is selected)
  - At root level (if no selection or a file is selected -- use file's parent)
  - Pre-filled with "untitled" and extension based on context
- On Enter: create the file, add to tree, open in editor
- On Escape: cancel
- Validate: no duplicate names, valid characters
- Auto-focus the text input
- Show a temporary "new file" icon while input is active

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (implement inline file creation)

**Acceptance Criteria:**
- Inline input appears at correct tree position
- Enter creates file and opens it
- Escape cancels
- Duplicate names rejected with error message
- File created on filesystem
- Tree updated with new file

**Dependencies:** Task 3

---

## Task 17: New Folder Inline Creation

**Title:** Create new folders via inline input in the tree

**Description:** Same as Task 16 but for folders. Shows inline input with folder icon, creates the directory on Enter.

**Implementation Details:**
- Identical UX to Task 16 but:
  - Shows folder icon instead of file icon
  - Creates directory with `std::filesystem::create_directories()`
  - Folder appears expanded (empty) after creation

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (implement inline folder creation)

**Acceptance Criteria:**
- Inline input for new folder name
- Directory created on filesystem
- Folder appears expanded after creation
- Duplicate names rejected

**Dependencies:** Task 3

---

## Task 18: File Tree Sorting

**Title:** Add configurable sort order for file tree

**Description:** Allow sorting file tree entries by name (A-Z, Z-A), type (folders first or mixed), or modified date. Default: folders first, then files, both alphabetical.

**Implementation Details:**
- Add `enum class TreeSortOrder { NameAsc, NameDesc, TypeFirst, ModifiedDate }`
- Add `TreeSortOrder sort_order_{TreeSortOrder::TypeFirst}` member
- `void SortNodes(std::vector<core::FileNode>& nodes)`:
  ```cpp
  std::sort(nodes.begin(), nodes.end(), [&](const FileNode& a, const FileNode& b) {
      if (sort_order_ == TypeFirst) {
          if (a.is_directory != b.is_directory) return a.is_directory > b.is_directory;
      }
      return a.name < b.name;
  });
  ```
- Apply sorting recursively to all levels
- Toggle via context menu or settings

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add sort order)
- `src/ui/FileTreeCtrl.cpp` (implement sorting)

**Acceptance Criteria:**
- Folders first is default
- All sort orders functional
- Sorting applied recursively
- Setting persists in config

**Dependencies:** None

---

## Task 19: File Tree Search/Filter Enhancement

**Title:** Enhance the existing filter with highlight matching

**Description:** Improve the existing `ApplyFilter()` functionality. When filtering, highlight the matching characters in the filename (not just show/hide nodes). Show the filter input above the tree with match count.

**Implementation Details:**
- In `DrawNode()`, when filter is active, highlight matching characters:
  ```cpp
  // Draw filename with highlighted match characters
  for (size_t i = 0; i < display_name.size(); ++i) {
      wxColour char_color = IsMatchChar(i) ? accent_color : normal_color;
      dc.SetTextForeground(char_color);
      dc.DrawText(wxString(display_name[i]), x_pos, y_pos);
      x_pos += dc.GetCharWidth();
  }
  ```
- Show filter input bar (already exists as `search_field_`) with match count label
- Fuzzy matching: "epc" matches "EditorPanel.cpp"
- Clear filter with Escape

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (enhance filter rendering with match highlighting)

**Acceptance Criteria:**
- Matching characters highlighted in accent color
- Fuzzy matching supported
- Match count shown in filter bar
- Escape clears filter
- Non-matching files/folders hidden

**Dependencies:** None

---

## Task 20: File Tree Keyboard Navigation Enhancement

**Title:** Complete keyboard navigation implementation

**Description:** Enhance the existing keyboard navigation with full arrow key support, Enter to open, Space to toggle folder expand, Home/End for first/last node, PageUp/PageDown for page scrolling.

**Implementation Details:**
- Extend `OnKeyDown()`:
  - Up/Down: move focus between visible nodes
  - Left: collapse focused folder, or move to parent if already collapsed/file
  - Right: expand focused folder, or move to first child if already expanded
  - Enter: open focused file in editor
  - Space: toggle expand on focused folder
  - Home: focus first node
  - End: focus last visible node
  - PageUp/PageDown: move focus by visible page height
  - Delete: delete focused file (with confirmation)
  - F2: rename focused file

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (extend keyboard handler)

**Acceptance Criteria:**
- All keyboard shortcuts functional
- Focus visually tracked (highlighted row)
- Navigation wraps at boundaries (Home/End)
- Page navigation scrolls appropriately
- Enter opens files in editor
- Works in combination with filter

**Dependencies:** None

---

## Task 21: File Size and Date Display

**Title:** Optionally show file size and modification date

**Description:** In an expanded detail mode, show file size and last modification date after each filename. Configurable via settings (default: off to keep the tree compact).

**Implementation Details:**
- Add `bool show_file_details_{false}` setting
- When enabled, after drawing the filename:
  ```cpp
  if (show_file_details_ && !node.is_directory) {
      auto size = std::filesystem::file_size(node.path);
      auto time = std::filesystem::last_write_time(node.path);
      std::string details = FormatFileSize(size) + " " + FormatDate(time);
      dc.SetTextForeground(muted_color);
      dc.DrawText(details, detail_x, y_offset);
  }
  ```
- `FormatFileSize()`: "1.2 KB", "3.4 MB", etc.
- Row height increases to accommodate details (from 24px to 36px)

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add detail mode setting)
- `src/ui/FileTreeCtrl.cpp` (implement detail rendering)

**Acceptance Criteria:**
- File size and date shown when enabled
- Formatted nicely (KB/MB units, relative dates)
- Row height adjusts
- Togglable via settings
- Performance: stat calls cached/debounced

**Dependencies:** None

---

## Task 22: Drag Files to Editor

**Title:** Enable dragging files from tree to editor for insertion

**Description:** Allow dragging a file from the tree to the editor to insert a link/reference to that file. For Markdown: inserts `[filename](relative_path)`. For code: inserts `#include "path"` or `import`.

**Implementation Details:**
- Start drag from file tree node
- Drop on editor panel: detect file type and insert appropriate reference:
  - Markdown editor + Markdown file: `[filename](relative_path)`
  - Markdown editor + image file: `![alt](relative_path)`
  - C++ editor: `#include "relative_path"`
- Use `wxDropSource` with `wxTextDataObject` containing the formatted text
- Also support drag to external applications (exports the full path)

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (implement drag source)
- `src/ui/EditorPanel.cpp` (implement drop target)

**Acceptance Criteria:**
- Files can be dragged from tree to editor
- Correct format inserted based on context
- Images insert as image links in Markdown
- External drag exports full path
- Visual feedback during drag

**Dependencies:** None

---

## Task 23: File Tree Context Menu Enhancement

**Title:** Complete the file context menu with all operations

**Description:** Enhance the existing context menus with comprehensive file operations matching VSCode's explorer context menu.

**Implementation Details:**
- File context menu:
  ```
  Open
  Open to the Side
  ---
  Cut                 Cmd+X
  Copy                Cmd+C
  Paste               Cmd+V
  ---
  Copy Path           Cmd+Shift+C
  Copy Relative Path  Cmd+K Cmd+Shift+C
  ---
  Rename              F2
  Delete              Cmd+Backspace
  ---
  Reveal in Finder    Cmd+Shift+R
  Open in Terminal    Cmd+Shift+T
  ```
- Folder context menu (additional items):
  ```
  New File            Cmd+N
  New Folder          Cmd+Shift+N
  ---
  Find in Folder...
  ---
  Collapse Folder
  ```

**Files Affected:**
- `src/ui/FileTreeCtrl.cpp` (enhance `ShowFileContextMenu` and `ShowEmptyAreaContextMenu`)

**Acceptance Criteria:**
- All menu items present and functional
- Keyboard shortcuts shown in menu
- "Open to the Side" opens in split editor group
- "Find in Folder" opens search panel scoped to that folder
- Platform-appropriate labels ("Finder" on macOS, "File Explorer" on Windows)

**Dependencies:** Tasks 8, 9, 10

---

## Task 24: Explorer Panel Configuration

**Title:** Add settings for explorer behavior and appearance

**Description:** Add configuration options for the explorer panel: icon theme, sort order, auto-reveal, compact folders, exclude patterns, and detail display.

**Implementation Details:**
- Settings in config:
  ```yaml
  explorer:
    auto_reveal: true          # auto-reveal active file in tree
    compact_folders: true      # collapse single-child folders
    sort_order: "type_first"   # name_asc, name_desc, type_first, modified
    exclude_patterns:           # patterns to hide
      - "*.pyc"
      - "__pycache__"
      - ".DS_Store"
      - "node_modules"
    show_file_details: false
    file_nesting:              # nest related files
      ".cpp": [".h"]
      ".ts": [".js", ".d.ts", ".js.map"]
  ```
- Compact folders: when a folder has only one child folder, collapse them into "parent/child" display
- Exclude patterns: hide matching files/folders
- Auto-reveal: scroll tree to active file when switching tabs

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add configuration)
- `src/ui/FileTreeCtrl.cpp` (implement configuration)
- `src/core/Config.h` (add explorer settings)

**Acceptance Criteria:**
- All settings load from and save to config
- Compact folders work for single-child chains
- Exclude patterns hide matching entries
- Auto-reveal scrolls to active file
- Settings persist across restarts

**Dependencies:** None

---

## Task 25: File Nesting

**Title:** Nest related files under parent files

**Description:** Automatically nest related files under their parent file. For example, `Component.test.tsx` nests under `Component.tsx`, and `style.module.css` nests under `Component.tsx`. Nesting patterns are configurable.

**Implementation Details:**
- Define nesting rules:
  ```cpp
  struct NestingRule {
      std::string parent_extension; // e.g., ".tsx"
      std::vector<std::string> child_patterns; // e.g., [".test.tsx", ".module.css", ".stories.tsx"]
  };
  ```
- During tree building, identify parent/child relationships
- Nested files appear as children of their parent file with an expand/collapse toggle
- Nested file count badge shown on parent

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add nesting logic)
- `src/ui/FileTreeCtrl.cpp` (implement file nesting)

**Acceptance Criteria:**
- Related files nested under parent
- Nesting rules configurable
- Expand/collapse toggle on parent files
- Child count badge on parent
- Works with multiple nesting levels

**Dependencies:** Task 24

---

## Task 26: Explorer Panel Accessibility

**Title:** Ensure explorer panel is fully accessible

**Description:** Add accessibility support: tree role, item roles, keyboard focus management, screen reader announcements, and high-contrast mode.

**Implementation Details:**
- Tree has role "tree" with aria-label "File Explorer"
- Each node has role "treeitem" with name = filename
- Expanded/collapsed state announced
- Focus management: Tab enters tree, arrows navigate, Enter opens
- File operations announced: "File deleted", "File renamed to..."
- Screen reader text: "filename, folder/file, modified/untracked"

**Files Affected:**
- `src/ui/FileTreeCtrl.h` (add accessibility interface)
- `src/ui/FileTreeCtrl.cpp` (implement accessible names and roles)

**Acceptance Criteria:**
- Tree navigable via screen reader
- Node states (expanded, selected) announced
- File operations announced
- Keyboard navigation complete
- Focus management logical

**Dependencies:** Tasks 13, 20

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| File Icons (Tasks 1-2) | Medium |
| Action Toolbar (Task 3) | Medium |
| Git Decorations (Task 4) | High |
| Open Editors (Task 5) | High |
| Workspace Header (Task 6) | Low |
| Drag/Drop (Task 7) | High |
| Inline Rename (Task 8) | Medium |
| Delete (Task 9) | Medium |
| Copy/Paste (Task 10) | Medium |
| File Watcher (Task 11) | High |
| Decoration API (Task 12) | Medium |
| Multi-Selection (Task 13) | Medium |
| Badges (Task 14) | Low |
| Animations (Task 15) | Medium |
| New File/Folder (Tasks 16-17) | Medium |
| Sorting (Task 18) | Low |
| Filter Enhancement (Task 19) | Medium |
| Keyboard Nav (Task 20) | Medium |
| File Details (Task 21) | Low |
| Drag to Editor (Task 22) | Medium |
| Context Menu (Task 23) | Low |
| Configuration (Task 24) | Medium |
| File Nesting (Task 25) | High |
| Accessibility (Task 26) | Medium |

## Files Created

- `src/ui/ExplorerToolbar.h`
- `src/ui/ExplorerToolbar.cpp`
- `src/ui/GitStatusProvider.h`
- `src/ui/GitStatusProvider.cpp`
- `src/ui/OpenEditorsSection.h`
- `src/ui/OpenEditorsSection.cpp`
- `src/ui/FileTreeDecorationProvider.h`
- `src/core/FileWatcher.h`
- `src/core/FileWatcher.cpp`

## Files Modified

- `src/ui/FileTreeCtrl.h`
- `src/ui/FileTreeCtrl.cpp`
- `src/ui/FileTypeIconRegistry.h`
- `src/ui/FileTypeIconRegistry.cpp`
- `src/ui/LayoutManager.cpp`
- `src/ui/EditorPanel.cpp`
- `src/core/Events.h`
- `src/core/Config.h`
- `src/platform/MacPlatform.h` / `.cpp`
- `src/platform/LinuxPlatform.h` / `.cpp`
- `CMakeLists.txt`
