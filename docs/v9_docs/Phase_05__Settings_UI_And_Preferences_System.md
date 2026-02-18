# Phase 05: Settings UI and Preferences System

## Overview
SettingsPanel exists (1550 lines) and SettingsDialog is a thin wrapper (242 lines), but settings are not consistently accessible from menus, command palette, or the workbench. The settings UI needs a JetBrains-style category tree, staged edits, and deep-linking. This phase makes settings a first-class product surface.

## Prerequisites
- Phase 02 (Config system modernization with scopes, validation, profiles)
- Phase 03 (Theme engine for settings UI theming)

## Tasks

### Task 1: Implement JetBrains-Style Category Tree Navigation
**Files:** `src/ui/SettingsPanel.cpp`, `src/ui/SettingsPanel.h`
**Description:** Replace the current flat settings layout with a split view: left panel has a tree of categories (Appearance > Themes > Colors, Editor > Font > Size, etc.), right panel shows settings for the selected category.
**Acceptance Criteria:**
- Left panel: collapsible tree matching SettingsCatalog group/subgroup hierarchy
- Right panel: scrollable form for selected category's settings
- Tree highlights current category
- Keyboard navigation: arrow keys in tree, Tab to settings form

### Task 2: Add Scoped Configuration Tabs
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Add tab bar at the top of settings panel with three tabs: Application, Workspace, Project. Switching tabs shows settings for that scope, indicating which values override parent scope.
**Acceptance Criteria:**
- Three tabs: Application, Workspace, Project
- Settings that override parent scope shown with "overridden" indicator
- Clear visual distinction between set values and inherited values
- "Reset to inherited" action per overridden setting

### Task 3: Implement Staged Edit Buffer
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Settings changes should not take effect until the user clicks Apply or OK. Implement a pending-change buffer. Cancel discards all pending changes.
**Acceptance Criteria:**
- Changes are staged, not immediate
- Apply button commits all changes (emits SettingsBatchChangedEvent)
- OK button commits and closes
- Cancel button discards all pending changes
- Pending changes count shown in footer

### Task 4: Add Settings Search Bar
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/SettingsCatalog.cpp`
**Description:** Add a search bar at the top of the settings panel that filters settings in real-time using the SettingsCatalog search index (Phase 02 Task 17). Search matches on name, description, and keywords.
**Acceptance Criteria:**
- Search bar filters settings as user types
- Matching settings shown with highlighted match text
- Category tree collapses to show only categories with matches
- Clear button resets search

### Task 5: Add Per-Setting Type Renderers
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Render each setting type with appropriate UI control: boolean (toggle/checkbox), string (text input), integer (spinner), float (slider), enum (dropdown), color (color picker), keybinding (key capture), font (font picker).
**Acceptance Criteria:**
- Each setting type has a dedicated renderer
- Renderers validate input in real-time
- Invalid input shown with error indicator
- Default value shown as placeholder

### Task 6: Add Settings Deep-Link Navigation
**Files:** `src/ui/SettingsPanel.cpp`, `src/ui/SettingsDialog.cpp`
**Description:** Wire `SettingsOpenRequestEvent` handling so that opening settings with a `setting_id` scrolls to and highlights that specific setting. Opening with a `query` pre-fills the search bar.
**Acceptance Criteria:**
- Deep-link by setting_id scrolls to and highlights the setting
- Deep-link by query pre-fills search and filters
- Deep-link by scope selects the scope tab
- Command palette commands use deep-linking: "Open Settings: Editor", "Open Settings: Appearance"

### Task 7: Add Menu Access (Platform-Correct)
**Files:** `src/ui/MainFrame.cpp`
**Description:** Add platform-correct menu access: macOS uses "Preferences..." under app menu with Cmd+comma, Windows/Linux uses "Settings..." under Edit menu with Ctrl+comma.
**Acceptance Criteria:**
- macOS: Preferences... in app menu, Cmd+, shortcut
- Windows/Linux: Settings... in Edit menu, Ctrl+, shortcut
- Menu item opens SettingsDialog
- wxID_PREFERENCES used on macOS for correct menu placement

### Task 8: Add Command Palette Settings Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register command palette commands for direct settings access: "Open Settings", "Open Settings: Editor", "Open Settings: Appearance", "Open Settings: Plugins", "Open Settings: Keyboard Shortcuts".
**Acceptance Criteria:**
- 5+ settings-related commands in command palette
- Each command deep-links to the appropriate category
- Commands are ranked higher when settings-related query is typed

### Task 9: Fix Toolbar Settings Button
**Files:** `src/ui/Toolbar.cpp`
**Description:** The toolbar settings button currently emits `ActivityBarSelectionEvent` instead of `SettingsOpenRequestEvent`. Fix it to open the settings dialog directly.
**Acceptance Criteria:**
- Toolbar settings button opens SettingsDialog
- No longer routes through activity bar
- Tooltip says "Settings (Cmd+,)" / "Settings (Ctrl+,)"

### Task 10: Add Modified Settings Filter
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Add a toggle filter "Show Modified Only" that shows only settings that differ from their default values. Uses `Config::modified_settings()` from Phase 02.
**Acceptance Criteria:**
- Toggle button in settings toolbar
- When active, only modified settings shown
- Modified count shown on the toggle button badge
- Works with search filter simultaneously

### Task 11: Add Settings JSON Editor Mode
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Add a "JSON" tab that shows the raw settings JSON for the selected scope. Advanced users can edit JSON directly. Validate on save with clear error messages.
**Acceptance Criteria:**
- JSON tab shows formatted JSON for selected scope
- Editable with syntax highlighting
- Validation on save: schema errors shown with line numbers
- Apply/Cancel work the same as form mode

### Task 12: Add Keyboard Shortcut Editor
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/ShortcutManager.h`, `src/core/ShortcutManager.cpp`
**Description:** Add a dedicated keyboard shortcuts page within settings. Show all commands with their current shortcuts, allow editing (key capture), conflict detection, and reset to defaults.
**Acceptance Criteria:**
- All registered commands listed with current shortcuts
- Click a shortcut to enter key capture mode
- Conflict detection: warn if shortcut is already assigned
- "Reset" reverts to default shortcut
- Search to filter commands

### Task 13: Add Settings Import/Export UI
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Add Import/Export buttons to the settings panel that use Config::import_from/export_to (Phase 02 Task 6). Show file picker dialogs.
**Acceptance Criteria:**
- Export button opens Save dialog, exports to JSON
- Import button opens Open dialog, imports from JSON
- Import shows preview of changes before applying
- Success/failure notification shown

### Task 14: Add Profile Switcher in Settings
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Add a profile dropdown in the settings toolbar. Selecting a profile applies it (using Config::apply_profile from Phase 02 Task 5) and shows which settings changed.
**Acceptance Criteria:**
- Dropdown lists all available profiles
- Selecting a profile shows preview of changes
- Apply applies the profile settings
- Current profile shown in dropdown

### Task 15: Add Settings Undo
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** After applying settings changes, show a "Undo last change" action for 5 seconds. Uses Config::restore_snapshot (Phase 02 Task 15).
**Acceptance Criteria:**
- After Apply, undo notification shown for 5 seconds
- Clicking undo reverts all changes from last Apply
- Multiple sequential Applies maintain separate undo points

### Task 16: Add Settings Accessibility
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Ensure full keyboard navigation: Tab between tree and form, arrow keys in tree, Enter to edit, Escape to cancel edit. Add screen reader labels for all controls.
**Acceptance Criteria:**
- Full keyboard navigation without mouse
- All controls have accessible names
- Focus visible with theme-aware focus ring
- Tab order is logical (tree, search, form, actions)

### Task 17: Add Settings Panel Theme Fidelity
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** Remove all hard-coded colors from SettingsPanel. Use theme tokens for all visual elements: backgrounds, text, borders, selection, hover states, focus rings, error indicators.
**Acceptance Criteria:**
- Zero hard-coded RGB values in SettingsPanel
- All visual states use theme semantic tokens
- Theme switching updates settings panel immediately
- Works correctly with all 64 built-in themes

### Task 18: Add Settings Panel Tests
**Files:** `tests/unit/test_settings_panel.cpp`, `tests/unit/test_settings_catalog.cpp`
**Description:** Comprehensive tests for settings UI: category tree population, scope switching, staged edits, search, deep-linking, profile switching, and accessibility.
**Acceptance Criteria:**
- Category tree correctly reflects SettingsCatalog hierarchy
- Scope switching shows correct values
- Staged edits: changes not committed until Apply
- Search filters correctly
- Deep-link navigation works

### Task 19: Add Settings Panel Performance
**Files:** `src/ui/SettingsPanel.cpp`
**Description:** With 200+ settings, the panel must be responsive. Implement virtual scrolling for the settings form, lazy rendering of category contents, and debounced search.
**Acceptance Criteria:**
- Initial render < 50ms even with 500+ settings
- Scrolling is smooth (no jank)
- Search is debounced (250ms) with immediate visual feedback
- Category switching is instant

### Task 20: Add First-Run Settings Wizard
**Files:** `src/ui/StartupPanel.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** On first launch (no config file exists), show a compact setup wizard: choose profile (Developer/Research/Whiteboard/Notebook), pick a theme, and set font size. Uses the profile system from Phase 02.
**Acceptance Criteria:**
- Wizard shown on first launch only (detected by absence of config file)
- Three steps: profile, theme, font size
- Skip button available at each step
- Selected options applied via Config::apply_profile

## Testing Requirements
- Settings panel renders correctly with all built-in themes
- Staged edit buffer: changes, apply, cancel, undo all work correctly
- Search covers all settings
- Deep-linking navigates to correct settings
- Keyboard-only workflow verified

## Phase Completion Criteria
- Settings accessible from menu, command palette, toolbar, and keyboard shortcut
- JetBrains-style category tree with search and scope tabs
- Staged edits with Apply/Cancel/Undo workflow
- Profiles enable one-click configuration switching
- Full keyboard and screen reader accessibility
- All tests pass
