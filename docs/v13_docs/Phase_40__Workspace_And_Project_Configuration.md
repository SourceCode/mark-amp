# Phase 40: Workspace & Project Configuration

## Overview

Build a comprehensive workspace and project configuration system that supports multi-root
workspaces, per-workspace settings, workspace trust, recent workspace management, and
workspace-specific customization. The existing codebase has foundational pieces:
`WorkspaceService` (document tracking, workspace folders, find_files, multi-folder support),
`WorkspaceSettings` (cascading scopes: Default -> User -> Workspace -> Folder),
`WorkspaceProfile` and `WorkspaceSessionRestore` (session save/restore), various workspace
helper classes (`WorkspaceSharing`, `WorkspaceHookManager`, `WorkspaceLayoutEngine`,
`WorkspaceCustomizationCommandProvider`). This phase wires these into a complete user-facing
workspace management system with UI for configuration, trust, switching, and project
settings files.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| WorkspaceService | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceService.h` | Document tracking, folders, find_files |
| WorkspaceSettings | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.h` | Cascading scopes, persistence |
| WorkspaceProfile | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceProfile.h` | Workspace-specific profile |
| WorkspaceSessionRestore | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.h` | Save/restore session state |
| WorkspaceSharing | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSharing.h` | Workspace sharing capabilities |
| WorkspaceHookManager | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceHookManager.h` | Lifecycle hooks |
| WorkspaceLayoutEngine | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceLayoutEngine.h` | Layout persistence |
| WorkspaceCustomizationCommandProvider | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceCustomizationCommandProvider.h` | Workspace commands |
| Config | `/Users/ryanrentfro/code/markamp/src/core/Config.h` | User-level settings |
| SettingsCatalog | `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.h` | Settings registry |
| EventBus | `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` | Communication backbone |

## Dependencies

- Phase 36 (Settings UI Overhaul) provides the settings scope tabs that workspace settings feed into.
- Phase 37 (Compiler Configuration) stores per-workspace compiler settings.
- Phase 38 (Build System Integration) stores per-workspace build settings.

---

## Tasks

### Task 1: Multi-Root Workspace Data Model

**Title:** Define the workspace configuration file format and multi-root support

**Description:** Define the `.markamp-workspace` file format that stores workspace
configuration: root folders, settings overrides, extension recommendations, and tasks.

**Implementation Details:**
- Workspace file: `<name>.markamp-workspace` (YAML format with Markdown body for notes).
- Structure:
```yaml
---
name: "My Project"
folders:
  - path: "./src"
    name: "Source"
  - path: "./docs"
    name: "Documentation"
  - path: "../shared-lib"
    name: "Shared Library"
settings:
  editor.fontSize: 14
  editor.tabSize: 2
extensions:
  recommendations:
    - markdown-lint
    - cpp-tools
tasks:
  - label: "Build"
    command: "cmake --build build/debug"
launch:
  - label: "Run Tests"
    command: "ctest --test-dir build/debug"
---
# Workspace Notes
This workspace contains the main project and shared libraries.
```
- Parse with YAML frontmatter parser (reuse Config's parser).
- Relative paths are resolved against the workspace file location.
- Store in `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfig.h`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfig.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfig.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Workspace file format is defined and parseable.
- Multi-root folders are supported.
- Relative paths resolve correctly.
- Settings overrides are applied.
- Invalid workspace files produce descriptive errors.

**Dependencies:** None (foundational).

---

### Task 2: Workspace Settings File (.markamp/settings.yaml)

**Title:** Create workspace-level settings persistence

**Description:** Implement the `.markamp/settings.yaml` file that stores per-workspace
settings overrides.

**Implementation Details:**
- When a workspace is opened, look for `.markamp/settings.yaml` in the workspace root.
- Parse the file and load settings into `WorkspaceSettings` at `SettingScope::kWorkspace`.
- Settings in this file override user-level settings.
- When a setting is changed at workspace scope via the Settings UI, write to this file.
- Auto-create the `.markamp/` directory on first workspace setting change.
- File format: YAML frontmatter (matching Config format for consistency).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` (file persistence)
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettingsFile.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettingsFile.cpp`

**Acceptance Criteria:**
- `.markamp/settings.yaml` is loaded on workspace open.
- Workspace settings override user settings.
- Changes at workspace scope are written to the file.
- `.markamp/` directory is auto-created.
- File is human-readable YAML.

**Dependencies:** Task 1.

---

### Task 3: Project-Level Settings Override

**Title:** Implement folder-level settings that override workspace settings

**Description:** In a multi-root workspace, each folder can have its own settings
that override the workspace-level settings for files within that folder.

**Implementation Details:**
- Each root folder can contain a `.markamp/settings.yaml` file.
- Settings from folder files are loaded into `WorkspaceSettings` at `SettingScope::kFolder`.
- Resolution order: Folder > Workspace > User > Default.
- When editing a file, the effective settings are determined by which folder contains it.
- Show the active scope in the status bar: "Settings: Workspace" or "Settings: Folder (src)".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` (folder scope)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (scope indicator)

**Acceptance Criteria:**
- Folder-level settings override workspace settings.
- Each folder can have independent settings.
- Status bar shows the active scope.
- Settings resolve correctly for files in different folders.

**Dependencies:** Task 2.

---

### Task 4: Workspace Trust System

**Title:** Implement workspace trust prompting for untrusted workspaces

**Description:** When opening a workspace for the first time (or one with executable
tasks), prompt the user to trust it. Untrusted workspaces run in restricted mode.

**Implementation Details:**
- Create `WorkspaceTrustService` in `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTrustService.h`.
- Trust states: Trusted, Untrusted, Unknown.
- Trusted workspaces: store in `~/.markamp/trusted_workspaces.json`.
- On workspace open, check trust status.
- If Unknown, show trust dialog:
  "Do you trust the authors of this workspace?"
  "Trusted workspaces can run tasks and extensions."
  [Trust] [Don't Trust] [Inspect]
- Untrusted restrictions:
  - Tasks cannot be executed.
  - Extensions are not loaded.
  - Workspace settings that affect execution are ignored.
- Trust can be revoked later via command.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTrustService.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTrustService.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceTrustDialog.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceTrustDialog.cpp`

**Acceptance Criteria:**
- Unknown workspaces show trust dialog on first open.
- Trusted workspaces are remembered.
- Untrusted workspaces have restricted functionality.
- Trust can be granted and revoked.
- Trust state is visible in the status bar.

**Dependencies:** Task 1.

---

### Task 5: Recently Opened Workspaces

**Title:** Track and display recently opened workspaces

**Description:** Maintain a list of recently opened workspaces and display them in
the Welcome screen and File menu.

**Implementation Details:**
- Store recent workspaces in `~/.markamp/recent_workspaces.json`.
- Each entry: `{name, path, last_opened, folder_count, trust_status}`.
- Maximum: 20 recent workspaces.
- Display in:
  - File menu: "Open Recent" submenu.
  - Welcome/Start screen: recent workspace list.
  - Command palette: `ws:` prefix (Phase 35 Task 15).
- Remove entries for workspaces that no longer exist.
- Show workspace metadata: name, path, last opened date.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/RecentWorkspacesManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/RecentWorkspacesManager.cpp`
- Modified: File menu construction code.

**Acceptance Criteria:**
- Recent workspaces appear in File > Open Recent.
- Maximum 20 workspaces tracked.
- Deleted workspaces are cleaned up.
- Metadata (name, date) is displayed.
- Selecting a recent workspace opens it.

**Dependencies:** Task 1.

---

### Task 6: Workspace Switching

**Title:** Implement workspace switching with session save/restore

**Description:** Allow users to switch between workspaces, saving the current session
state and restoring the target workspace's session.

**Implementation Details:**
- On workspace switch:
  1. Save current session via `WorkspaceSessionRestore::save_session`.
  2. Close all open files.
  3. Load target workspace configuration.
  4. Restore target session via `WorkspaceSessionRestore::restore_session`.
- Session state includes: open files, active file, cursor positions, panel visibility,
  sidebar width, split layout.
- Prompt to save unsaved files before switching.
- "Switch Workspace" command: `workspace.switch`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSessionRestore.cpp` (wire)
- Modified: app workspace management code.

**Acceptance Criteria:**
- Session state is saved on workspace switch.
- Target workspace's session is restored.
- Unsaved files prompt for save.
- Open tabs, cursor positions, and layout are preserved.
- Switching is smooth (< 1s).

**Dependencies:** Task 1, Task 5.

---

### Task 7: Workspace-Specific Extensions

**Title:** Support workspace-recommended and workspace-disabled extensions

**Description:** Allow workspaces to recommend specific extensions and disable others.

**Implementation Details:**
- Workspace file includes `extensions.recommendations` and `extensions.unwanted` arrays.
- On workspace open:
  - Check if recommended extensions are installed. If not, show a notification:
    "This workspace recommends extension 'X'. [Install] [Ignore]".
  - Disable extensions listed in `unwanted` (they are loaded but deactivated).
- Show workspace extension status in the Extensions panel.
- "Add to workspace recommendations" right-click option in Extensions panel.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfig.cpp` (extensions)
- Modified: extension management UI.

**Acceptance Criteria:**
- Recommended extensions trigger install notification.
- Unwanted extensions are deactivated.
- Extension status shows workspace context.
- Users can add/remove recommendations.

**Dependencies:** Task 1.

---

### Task 8: Workspace File Associations

**Title:** Configure file type associations per workspace

**Description:** Allow workspaces to define custom file associations (e.g., treat
`.mdx` files as Markdown).

**Implementation Details:**
- Workspace settings: `"files.associations"` as a map:
  `{"*.mdx": "markdown", "*.cmake": "cmake", "Dockerfile*": "dockerfile"}`.
- On file open, check workspace associations before global associations.
- Show active association in the status bar language selector.
- Changing the association in the status bar updates workspace settings.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` (associations)
- Modified: file open/language detection code.

**Acceptance Criteria:**
- Workspace associations override global ones.
- Custom extensions are mapped correctly.
- Status bar shows the active association.
- Changes are persisted to workspace settings.

**Dependencies:** Task 2.

---

### Task 9: Workspace Exclude Patterns

**Title:** Configure file/folder exclusion patterns per workspace

**Description:** Allow workspaces to define patterns for files and folders that should
be hidden from the file tree and search.

**Implementation Details:**
- Workspace settings: `"files.exclude"` as an array of glob patterns.
- Default excludes: `[".git", "node_modules", "build", ".markamp"]`.
- Excluded files/folders:
  - Hidden from FileTreeCtrl.
  - Excluded from search results.
  - Excluded from Quick Open.
- "Show Excluded Files" toggle in the file tree.
- Per-folder exclude patterns are supported.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.cpp` (excludes)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (filter)
- Modified: search and quick open code (filter).

**Acceptance Criteria:**
- Excluded files are hidden from the file tree.
- Excluded files do not appear in search.
- Excluded files do not appear in Quick Open.
- "Show Excluded" toggle reveals hidden files.
- Default excludes are sensible.

**Dependencies:** Task 2.

---

### Task 10: Workspace Environment Variables

**Title:** Configure environment variables per workspace

**Description:** Allow workspaces to define environment variables that are available
to build tasks and terminal sessions.

**Implementation Details:**
- Workspace settings: `"workspace.env"` as a key-value map.
- Variables are set in the environment of:
  - Build processes (Phase 38).
  - Terminal sessions (if terminal is available).
  - Extension host processes.
- Variables can reference other variables: `PATH=/custom/bin:${PATH}`.
- Platform-specific variables: `"workspace.env.mac"`, `"workspace.env.linux"`.
- Show active environment variables in a debug panel.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceEnvironment.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceEnvironment.cpp`
- Modified: build and process spawning code.

**Acceptance Criteria:**
- Workspace environment variables are set for child processes.
- Variable reference expansion works.
- Platform-specific variables are resolved.
- Variables are visible in a debug panel.

**Dependencies:** Task 2.

---

### Task 11: Workspace Tasks Configuration

**Title:** Define workspace-level tasks in .markamp/tasks.json

**Description:** Allow workspaces to define task configurations (build, test, run)
in a dedicated tasks file.

**Implementation Details:**
- File: `.markamp/tasks.json` (JSON format).
- Structure:
```json
{
  "version": "1.0",
  "tasks": [
    {
      "label": "Build Debug",
      "type": "shell",
      "command": "cmake --build build/debug -j8",
      "group": "build",
      "isDefault": true
    },
    {
      "label": "Run Tests",
      "type": "shell",
      "command": "ctest --test-dir build/debug --output-on-failure",
      "group": "test"
    }
  ]
}
```
- Integration with `BuildTaskManager` from Phase 38.
- Tasks are available in the "Run Task" quick-pick.
- Default task runs on Cmd+B.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/BuildTaskManager.cpp` (load workspace tasks)

**Acceptance Criteria:**
- Tasks are loaded from `.markamp/tasks.json`.
- Tasks appear in the task picker.
- Default task runs on Cmd+B.
- Task format is documented and validated.

**Dependencies:** Phase 38 Task 6 (BuildTaskManager).

---

### Task 12: Workspace Launch Configuration

**Title:** Define workspace-level launch configurations in .markamp/launch.json

**Description:** Allow workspaces to define run/debug configurations.

**Implementation Details:**
- File: `.markamp/launch.json` (JSON format).
- Structure:
```json
{
  "version": "1.0",
  "configurations": [
    {
      "name": "Run Main",
      "type": "cppdbg",
      "program": "${workspaceFolder}/build/debug/markamp",
      "args": ["--config", "test.yaml"],
      "cwd": "${workspaceFolder}",
      "environment": [{"name": "DEBUG", "value": "1"}]
    }
  ]
}
```
- Variable substitution: `${workspaceFolder}`, `${file}`, etc.
- "Run" command uses the active launch configuration.
- Configuration selector in the toolbar.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/LaunchConfigManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/LaunchConfigManager.cpp`

**Acceptance Criteria:**
- Launch configurations are loaded from `.markamp/launch.json`.
- Variable substitution works.
- Active configuration is selectable.
- "Run" command uses the active configuration.

**Dependencies:** Task 1.

---

### Task 13: Workspace Recommendations

**Title:** Show recommendations for workspace configuration

**Description:** When a workspace is opened without configuration, suggest relevant
settings and configurations based on project analysis.

**Implementation Details:**
- Analyze workspace on first open:
  - Detect language: C++ (CMakeLists.txt), Markdown (*.md files), etc.
  - Detect build system: CMake, Make, Ninja.
  - Detect version control: .git, .hg.
- Show recommendations panel:
  - "This project uses CMake. [Configure CMake preset]".
  - "Found .clang-format. [Enable format on save]".
  - "No workspace settings found. [Create .markamp/settings.yaml]".
- Recommendations can be dismissed per workspace.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceRecommendations.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceRecommendations.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/RecommendationsPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/RecommendationsPanel.cpp`

**Acceptance Criteria:**
- Recommendations are generated based on project analysis.
- Language, build system, and VCS are detected.
- Each recommendation has an action button.
- Dismissed recommendations do not reappear.

**Dependencies:** Task 1.

---

### Task 14: Multi-Root Workspace UI

**Title:** Build the UI for managing multi-root workspace folders

**Description:** Allow users to add, remove, and reorder root folders in a multi-root
workspace through the file tree and a management dialog.

**Implementation Details:**
- File tree shows root folders as top-level expandable items with folder name labels.
- Right-click root folder > "Remove from Workspace".
- "Add Folder to Workspace" via File menu, command palette, or drag-and-drop.
- Workspace management dialog: reorder folders, rename folder labels, change paths.
- When folders are added/removed, update the `.markamp-workspace` file.
- Wire to `WorkspaceService::add_workspace_folder` and `remove_workspace_folder`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceManagerDialog.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceManagerDialog.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (multi-root display)

**Acceptance Criteria:**
- Multiple root folders appear in the file tree.
- Folders can be added and removed.
- Folder order can be changed.
- Workspace file is updated on changes.
- Drag-and-drop adds folders.

**Dependencies:** Task 1.

---

### Task 15: Workspace Settings UI Integration

**Title:** Integrate workspace settings into the Settings UI (Phase 36)

**Description:** Ensure the Settings UI's "Workspace" scope tab correctly reads from
and writes to the workspace settings file.

**Implementation Details:**
- When the "Workspace" scope tab is active in SettingsPanel:
  - Show settings from `.markamp/settings.yaml`.
  - Edits write to `.markamp/settings.yaml`.
  - Settings not overridden at workspace level show inherited values (dimmed).
- When the "Folder" scope tab is active:
  - Show a folder selector (for multi-root workspaces).
  - Show settings from the selected folder's `.markamp/settings.yaml`.
- Show which settings are overridden at each level.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (workspace scope integration)

**Acceptance Criteria:**
- "Workspace" tab reads/writes workspace settings.
- "Folder" tab shows per-folder settings.
- Inherited values are shown dimmed.
- Override indicators are correct.

**Dependencies:** Task 2, Phase 36 Task 3.

---

### Task 16: Workspace Events

**Title:** Define and emit all workspace lifecycle events

**Description:** Ensure all workspace lifecycle events are declared and emitted
at the appropriate times.

**Implementation Details:**
- Events (add to Events.h):
  - `WorkspaceOpenedEvent{ string workspace_path; vector<string> folders; }`
  - `WorkspaceClosedEvent{ string workspace_path; }`
  - `WorkspaceFolderAddedEvent{ string folder_path; string folder_name; }`
  - `WorkspaceFolderRemovedEvent{ string folder_path; }`
  - `WorkspaceSettingsChangedEvent{ string setting_id; string new_value; }`
  - `WorkspaceTrustChangedEvent{ string workspace_path; bool trusted; }`
- All events emitted via EventBus.
- Components subscribe to relevant events.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (workspace events)
- Modified: workspace management code (emit events).

**Acceptance Criteria:**
- All 6 workspace events are declared.
- Events are emitted at correct lifecycle points.
- Components can subscribe and receive events.
- No events are emitted for unchanged state.

**Dependencies:** Task 1.

---

### Task 17: Workspace Configuration Panel

**Title:** Build a workspace configuration panel accessible from the sidebar

**Description:** A dedicated panel showing the current workspace configuration:
folders, settings overrides, tasks, extensions, and trust status.

**Implementation Details:**
- Create `WorkspaceConfigPanel` as a sidebar panel.
- Sections:
  1. Folders: list of root folders with add/remove buttons.
  2. Settings: count of overrides with "Open Settings" link.
  3. Tasks: list of defined tasks with "Run" buttons.
  4. Extensions: recommended extensions with install status.
  5. Trust: trust status with toggle.
- Access via "View > Workspace Configuration" or command.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceConfigPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceConfigPanel.cpp`

**Acceptance Criteria:**
- Panel shows all workspace configuration sections.
- Folder management works inline.
- Task run buttons work.
- Trust status is toggleable.
- Panel is themed.

**Dependencies:** Tasks 1, 2, 4, 11.

---

### Task 18: Workspace Theme Integration

**Title:** Theme all workspace UI components

**Description:** Apply theme tokens to workspace-related UI elements.

**Implementation Details:**
- Theme tokens: `workspace.panelBackground`, `workspace.folderLabel`,
  `workspace.trustBadgeGreen`, `workspace.trustBadgeRed`, `workspace.sectionHeader`.
- Apply to: WorkspaceConfigPanel, WorkspaceManagerDialog, WorkspaceTrustDialog,
  RecommendationsPanel.

**Files Affected:**
- Modified: All workspace UI files (theme application).

**Acceptance Criteria:**
- All workspace UI uses theme tokens.
- Theme switch updates immediately.

**Dependencies:** Task 17.

---

### Task 19: Workspace Configuration Persistence

**Title:** Ensure all workspace configuration persists correctly

**Description:** Validate that all workspace configuration (folders, settings, tasks,
launch configs, trust) persists and loads correctly.

**Implementation Details:**
- On workspace open: load `.markamp-workspace`, `.markamp/settings.yaml`,
  `.markamp/tasks.json`, `.markamp/launch.json`.
- On workspace close: save session state, flush pending settings.
- Handle concurrent modification (external edits to config files):
  use `ConfigFileWatcher` to detect changes and reload.
- Handle missing files: create with defaults on first access.
- Handle corrupt files: show error, offer to reset.

**Files Affected:**
- Modified: workspace loading/saving code.

**Acceptance Criteria:**
- All config files are loaded on workspace open.
- External changes trigger reload.
- Missing files are created on demand.
- Corrupt files show error with reset option.

**Dependencies:** Tasks 1, 2, 11, 12.

---

### Task 20: Workspace Command Registration

**Title:** Register all workspace commands

**Description:** Register commands for workspace management.

**Implementation Details:**
- Commands:
  - `workspace.open`: Open workspace folder
  - `workspace.openFile`: Open workspace file
  - `workspace.switch`: Switch workspace
  - `workspace.addFolder`: Add folder to workspace
  - `workspace.removeFolder`: Remove folder from workspace
  - `workspace.saveAs`: Save workspace as new file
  - `workspace.openSettings`: Open workspace settings
  - `workspace.trust`: Trust/untrust workspace
  - `workspace.showConfig`: Show workspace configuration panel
  - `workspace.closeAll`: Close workspace
- All in "Workspace" category.

**Files Affected:**
- Modified: command registration code (or use WorkspaceCustomizationCommandProvider).

**Acceptance Criteria:**
- All commands are registered.
- Commands appear in the palette.
- Each command works correctly.

**Dependencies:** Tasks 1-17.

---

### Task 21: Workspace Welcome Screen

**Title:** Show a welcome screen with recent workspaces and quick actions

**Description:** When MarkAmp opens with no workspace, show a welcome screen.

**Implementation Details:**
- Welcome screen content:
  - "Recent Workspaces" list (from RecentWorkspacesManager).
  - "Open Folder" button.
  - "Clone Repository" button (if git is available).
  - "New File" button.
  - "Getting Started" link to documentation.
- Recent workspaces show: name, path, last opened date, folder count.
- Clicking a workspace opens it.
- The welcome screen closes when a workspace is opened.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/WelcomeScreen.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WelcomeScreen.cpp`

**Acceptance Criteria:**
- Welcome screen shows on app launch without workspace.
- Recent workspaces are listed.
- Quick action buttons work.
- Welcome screen closes on workspace open.

**Dependencies:** Task 5.

---

### Task 22-25: Additional Tasks

**Task 22: Workspace Keyboard Shortcuts** - Register shortcuts: Cmd+O for Open Folder, Cmd+Shift+O for Open Workspace, Ctrl+R for Recent Workspaces (in palette). All in "Workspace" category.

**Task 23: Workspace Accessibility** - Screen reader labels for workspace panels and dialogs. Trust dialog is announced. Folder list is navigable. All workspace UI is keyboard-accessible.

**Task 24: Workspace Session Restore Improvements** - Enhance WorkspaceSessionRestore to save: scroll positions per file, find bar state, panel visibility, sidebar width, split layout proportions. Restore all state on workspace open.

**Task 25: Unit Tests for Workspace Configuration** - Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_workspace_config.cpp`. Sections: WorkspaceConfig parsing, WorkspaceSettings cascade, WorkspaceTrust (trust/revoke/persist), RecentWorkspacesManager (add/remove/cleanup), Multi-root folder management, Environment variable expansion, Tasks file parsing. At least 30 test cases.

---

### Task 26: Workspace Template System

**Title:** Create new projects from workspace templates

**Description:** Provide a template system that allows users to create new workspaces
from predefined or custom templates, including folder structure, default settings,
tasks, and starter files.

**Implementation Details:**
- Create `WorkspaceTemplateManager` in `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTemplateManager.h`:
  - Template format: a directory containing:
    - `template.yaml`: metadata (name, description, author, tags, variables).
    - `template/`: the directory tree to copy as the workspace skeleton.
    - Supports variable placeholders in file contents and file names: `{{project_name}}`,
      `{{author}}`, `{{date}}`, `{{cpp_standard}}`.
  - Built-in templates stored in `resources/workspace_templates/`:
    - "C++ CMake Project": CMakeLists.txt, src/main.cpp, include/, tests/, .markamp/ config.
    - "C++ Library": CMakeLists.txt, src/, include/{{project_name}}/, tests/.
    - "Markdown Notes": docs/, README.md, .markamp/settings.yaml with markdown defaults.
    - "Empty Workspace": just .markamp-workspace and .markamp/settings.yaml.
  - Custom templates: users can save any workspace as a template via
    `workspace.saveAsTemplate`. Stored in `~/.markamp/workspace_templates/`.
- Create `NewFromTemplateDialog` in `/Users/ryanrentfro/code/markamp/src/ui/NewFromTemplateDialog.h`:
  - Template gallery: grid of template cards showing name, description, and preview icon.
  - Search/filter by name and tags.
  - After selecting a template:
    - Prompt for project name and location (directory picker).
    - Show variable input form for template variables (project_name, author, etc.).
    - Preview: show the file tree that will be generated.
  - "Create" button: copies the template, replaces variables, opens the new workspace.
- Variable expansion:
  - Replace `{{variable}}` in all text files (.cpp, .h, .md, .yaml, .json, CMakeLists.txt).
  - Replace `{{variable}}` in file and directory names.
  - Skip binary files during variable replacement.
- Commands:
  - `workspace.newFromTemplate`: opens the template gallery.
  - `workspace.saveAsTemplate`: saves current workspace as a reusable template.
- File menu: "File > New Project from Template...".

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTemplateManager.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceTemplateManager.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/NewFromTemplateDialog.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/NewFromTemplateDialog.cpp`
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/cpp_cmake/template.yaml`
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/cpp_cmake/template/` (tree)
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/cpp_library/template.yaml`
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/cpp_library/template/` (tree)
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/markdown_notes/template.yaml`
- New: `/Users/ryanrentfro/code/markamp/resources/workspace_templates/empty/template.yaml`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: File menu construction code.

**Acceptance Criteria:**
- Template gallery shows built-in and custom templates.
- Selecting a template prompts for project name and location.
- Variable placeholders are replaced in file contents and file names.
- Generated workspace opens automatically after creation.
- "Save as Template" captures the current workspace structure.
- Custom templates persist in `~/.markamp/workspace_templates/`.
- File tree preview shows the template structure before creation.
- Binary files are skipped during variable replacement.

**Dependencies:** Task 1, Task 5.

---

### Task 27: Workspace Configuration Comparison

**Title:** Diff two workspace configurations side-by-side

**Description:** Allow users to compare the configuration of two workspaces side-by-side,
highlighting differences in settings, tasks, extensions, and folder structure to assist
with standardization or troubleshooting.

**Implementation Details:**
- Create `WorkspaceComparisonPanel` in `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceComparisonPanel.h`:
  - Two-pane layout: "Workspace A" on the left, "Workspace B" on the right.
  - Input: select two workspaces to compare:
    - Current workspace vs a recent workspace.
    - Current workspace vs a workspace file (browse for .markamp-workspace).
    - Two workspace files.
  - Comparison sections:
    1. **Settings Diff**: show all settings that differ between the two workspaces.
       Each row shows: setting key, value in A, value in B. Differences highlighted.
       Settings only in one workspace are marked as "not set" in the other.
    2. **Tasks Diff**: compare defined tasks. Show added, removed, and modified tasks.
       For modified tasks, highlight the changed fields (command, args, etc.).
    3. **Extensions Diff**: compare recommended and unwanted extensions.
       Show which extensions are recommended in one but not the other.
    4. **Folders Diff**: compare root folder lists.
    5. **Environment Diff**: compare workspace environment variables.
  - Unified diff summary at the top: "12 settings differ, 2 tasks differ, 1 extension differs".
  - "Copy to Current" button per-setting: apply a setting from the other workspace
    to the current one.
  - "Merge All" button: apply all differences from workspace B to workspace A (with confirmation).
- Load workspace configurations without opening the workspace:
  - Parse the `.markamp-workspace`, `.markamp/settings.yaml`, `.markamp/tasks.json` files
    from the target directory.
- Register command: `workspace.compare`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceComparisonPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceComparisonPanel.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfigLoader.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceConfigLoader.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- Two workspaces can be selected for comparison.
- Settings differences are shown with values side-by-side.
- Tasks, extensions, folders, and environment differences are compared.
- Summary shows total difference count per category.
- "Copy to Current" applies a single setting from the compared workspace.
- Workspace configurations load without opening the workspace.
- Differences are color-highlighted (added = green, removed = red, modified = yellow).

**Dependencies:** Task 1, Task 2.

---

### Task 28: Workspace Health Check

**Title:** Validate workspace configuration integrity and report issues

**Description:** Run a comprehensive health check on the current workspace, validating
all paths, settings, tool availability, and configuration consistency, then present
a report with actionable fix suggestions.

**Implementation Details:**
- Create `WorkspaceHealthChecker` in `/Users/ryanrentfro/code/markamp/src/core/WorkspaceHealthChecker.h`:
  - Health check categories:
    1. **Path Validation**: verify all root folder paths exist and are accessible.
       Check include paths, library paths, and sysroot paths from compiler config.
       Check task working directories. Flag broken symlinks.
    2. **Settings Consistency**: check for conflicting settings across scopes
       (e.g., folder setting contradicts workspace setting without intent).
       Detect deprecated or unknown settings keys.
       Validate setting values against their registered types and constraints.
    3. **Tool Availability**: verify compiler path, cmake, make, ccache/sccache,
       git, and other tools referenced in tasks or compiler config are installed
       and accessible.
    4. **Configuration Files**: validate `.markamp/settings.yaml`, `.markamp/tasks.json`,
       `.markamp/launch.json` for parse errors and schema violations.
       Check `.markamp-workspace` file for integrity.
    5. **Extension Health**: check that recommended extensions are installed.
       Detect extensions that have been removed but are still referenced.
    6. **Trust and Security**: warn about untrusted workspace settings that affect execution.
       Flag tasks with potentially dangerous commands.
  - Each check produces a `HealthCheckResult`:
    - `severity`: Info, Warning, Error.
    - `category`: the check category.
    - `message`: human-readable description.
    - `fix_suggestion`: actionable text (e.g., "Run 'brew install cmake' to install CMake").
    - `auto_fix`: optional lambda that applies the fix programmatically.
- Create `WorkspaceHealthPanel` in `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceHealthPanel.h`:
  - Show results grouped by category.
  - Each result shows severity icon, message, and "Fix" button (if auto-fixable).
  - Summary header: "3 errors, 5 warnings, 12 info" with colored badges.
  - "Re-run Check" button refreshes all checks.
  - "Fix All" button applies all available auto-fixes (with confirmation).
  - "Export Report" saves the health check results to a markdown file.
- Auto-run on workspace open (if `workspace.autoHealthCheck` setting is enabled).
- Register command: `workspace.healthCheck`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceHealthChecker.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/WorkspaceHealthChecker.cpp`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceHealthPanel.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WorkspaceHealthPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (WorkspaceHealthCheckEvent)

**Acceptance Criteria:**
- All six health check categories run and produce results.
- Path validation catches missing directories and broken symlinks.
- Tool availability checks detect missing compilers, cmake, git, etc.
- Configuration file validation catches YAML/JSON parse errors.
- Results are displayed grouped by category with severity icons.
- Auto-fixable issues have a working "Fix" button.
- Summary shows error/warning/info counts.
- "Export Report" produces a readable markdown file.
- Health check can be configured to run automatically on workspace open.

**Dependencies:** Task 1, Task 2, Task 4, Phase 37 Task 1, Phase 38 Task 16.
