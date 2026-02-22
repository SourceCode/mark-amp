# Phase 36: Settings UI Overhaul

## Overview

Overhaul the settings experience into a best-in-class, searchable, categorized settings
editor that rivals VSCode's settings UI. The current codebase has a rich foundation:
`SettingsPanel` with 9 widget types (Boolean, Integer, String, Choice, Double, FilePath,
Color, KeyBinding, StringList), staged-edit lifecycle (pending changes, apply/discard),
undo/redo stack, export/import, `SettingsDialog` with scope selector and profile support,
`SettingsJsonEditor` for raw JSON editing, `SettingsNavModel` with category tree and search,
`SettingsControlModel` with validation and staged changes, `SettingsCatalog` as the single
source of truth, and `ConfigProfileManager` for named profiles. This phase focuses on
the visual redesign: search with highlighted matches, improved category navigation, scope
tabs, modified indicator, default value display, commonly used section, and polish.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| SettingsPanel | `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.h` | 9 widget types, staged edits, undo/redo, export/import |
| SettingsDialog | `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.h` | Modal dialog, scope selector, profile selector |
| SettingsJsonEditor | `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.h` | Raw JSON editing with syntax highlighting |
| SettingsNavModel | `/Users/ryanrentfro/code/markamp/src/ui/SettingsNavModel.h` | Category tree, search, breadcrumb, badges |
| SettingsControlModel | `/Users/ryanrentfro/code/markamp/src/ui/SettingsControlModel.h` | Validation, staged changes, per-setting revert |
| SettingsCatalog | `/Users/ryanrentfro/code/markamp/src/core/SettingsCatalog.h` | Central registry, 60+ settings, search |
| Config | `/Users/ryanrentfro/code/markamp/src/core/Config.h` | YAML-backed config, batch, snapshot, cascade |
| ConfigProfileManager | `/Users/ryanrentfro/code/markamp/src/core/ConfigProfile.h` | Named profiles (Developer, Research, etc.) |
| WorkspaceSettings | `/Users/ryanrentfro/code/markamp/src/core/WorkspaceSettings.h` | Cascading scopes (Default, User, Workspace, Folder) |

## Dependencies

- Phase 37 (Compiler Configuration) adds settings entries to the catalog.
- Phase 39 (Keyboard Shortcuts Editor) shares the settings UI patterns.

---

## Tasks

### Task 1: Search with Highlighted Matches

**Title:** Enhance settings search to highlight matched text within results

**Description:** When the user searches settings, highlight the matched substring within
the setting label, description, and setting ID. Currently `SettingsPanel` searches and
filters but does not highlight the matching portion.

**Implementation Details:**
- When `RebuildSettingsList` is called with an active search query, pass the query
  to each setting row widget.
- In each setting row, parse the label and description for the query substring.
- Render matched substrings with bold font + accent color.
- Use the `FuzzyScorer` (Phase 31 Task 1) match_positions for fuzzy highlight.
- Also highlight matches in the setting ID (shown in dimmed text below the label).
- Highlight updates in real-time as the user types.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (highlight rendering)
- Modified: Widget creation methods (CreateBooleanSetting, etc.) to accept query context.

**Acceptance Criteria:**
- Search "font" highlights "font" in "Font Size", "Font Family", and descriptions.
- Highlights use bold font and accent color.
- Fuzzy search "ftsz" highlights "f", "t", "S", "z" in "Font Size".
- Clearing the search removes all highlights.
- Highlights update in real-time.

**Dependencies:** Phase 31 Task 1 (FuzzyScorer).

---

### Task 2: Improved Category Tree Navigation

**Title:** Redesign the category tree with group/subgroup hierarchy and item counts

**Description:** Enhance the `wxTreeCtrl` category tree with proper group/subgroup
nesting, item count badges, and icons per category.

**Implementation Details:**
- Build the tree using `SettingsNavModel::category_tree()` which returns `SettingsCategoryNode`.
- Top-level groups: "Editor", "Workbench", "Extensions", "Terminal", "Markdown", "Advanced".
- Each group has subgroups (e.g., Editor > Font, Editor > Cursor, Editor > Minimap).
- Show item count badge: "(12)" next to each group name.
- Add category icons: Editor = code icon, Workbench = window icon, Extensions = puzzle.
- Clicking a group/subgroup filters the settings list.
- "All Settings" node at the top shows all (no filter).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (BuildCategoryTree)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsNavModel.cpp` (ensure hierarchy)

**Acceptance Criteria:**
- Category tree shows group > subgroup hierarchy.
- Each group shows item count badge.
- Icons differentiate categories.
- Clicking a group filters settings to that group.
- "All Settings" shows all settings.

**Dependencies:** None.

---

### Task 3: Setting Scope Tabs (User, Workspace, Folder)

**Title:** Redesign scope tabs with proper cascade indication

**Description:** The existing `scope_tabs_` (`wxNotebook`) provides scope selection.
Redesign as custom-drawn tabs showing: "User Settings", "Workspace Settings", "Folder Settings"
with visual indication of which scope a value is coming from.

**Implementation Details:**
- Replace `wxNotebook` with custom-drawn tab buttons.
- Three tabs: User (global), Workspace (.markamp/settings.yaml), Folder (per-folder).
- Each setting row shows a scope indicator: tiny colored dot (blue = User, green = Workspace,
  orange = Folder) next to settings that have overrides at that scope.
- Wire to `WorkspaceSettings::effective_value` and scope-specific getters.
- When on the Workspace tab, settings without workspace overrides show the effective
  (inherited) value in dimmed text with "Inherited from User" tooltip.
- Editing on the Workspace tab writes to `WorkspaceSettings` with `SettingScope::kWorkspace`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.h` (custom scope tabs)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (scope tab redesign)

**Acceptance Criteria:**
- Three scope tabs are visible.
- Switching tabs shows settings for that scope.
- Scope indicator dots show which scope defines each value.
- Inherited values show as dimmed with tooltip.
- Editing writes to the correct scope.

**Dependencies:** WorkspaceSettings integration.

---

### Task 4: Modified Settings Indicator

**Title:** Show a blue bar and badge for settings that differ from defaults

**Description:** Settings whose current value differs from the default should show a
blue left-border bar and appear in a "Modified" filter.

**Implementation Details:**
- For each setting row, call `IsSettingModified(def)` which compares current value
  to `def.default_value`.
- If modified, render a 3px blue bar on the left edge of the setting row.
- Add a "Modified" filter checkbox in the toolbar area (already exists as
  `show_modified_only_` in the existing code).
- Wire the checkbox to filter `definitions_` to only modified settings.
- Show a count badge: "5 modified" in the toolbar.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (blue bar rendering)

**Acceptance Criteria:**
- Modified settings show a blue left-border bar.
- "Modified" checkbox filters to only modified settings.
- Modified count badge updates in real-time.
- Resetting a setting to default removes the blue bar.
- The bar is themed (uses `settings.modifiedIndicator` token).

**Dependencies:** None.

---

### Task 5: Reset to Default Action

**Title:** Per-setting reset button with default value display

**Description:** Each setting row should show: the default value as reference text, and
a "Reset to Default" button (gear icon with undo) when the value differs from default.

**Implementation Details:**
- Below the setting control, show "Default: [value]" in dimmed text.
- When the setting is modified, show a small reset button (the existing
  `CreateResetButton` method).
- Clicking reset calls `ResetSettingToDefault` and removes the modification.
- Add a confirmation for destructive resets (e.g., file paths).
- After reset, briefly flash the setting row green to confirm.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (default display, reset)

**Acceptance Criteria:**
- Default value is displayed below each setting.
- Reset button appears for modified settings.
- Clicking reset restores the default value.
- The blue modified indicator disappears after reset.
- A brief green flash confirms the reset.

**Dependencies:** Task 4.

---

### Task 6: Settings JSON Toggle

**Title:** Seamless toggle between GUI and raw JSON settings view

**Description:** Add a toggle button that switches between the GUI settings editor and
the raw JSON editor within the same dialog.

**Implementation Details:**
- Add a toggle button in the SettingsDialog toolbar: "GUI" / "JSON".
- When switching to JSON, serialize current settings via `SettingsJsonEditor::ConfigToJson`.
- When switching back to GUI, apply any JSON changes via `ApplyJsonToConfig`.
- Preserve the scroll position and active category when toggling.
- If JSON is invalid when switching to GUI, show an error and stay in JSON mode.
- The toggle should be a smooth transition (not a dialog replacement).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.h` (toggle state)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (toggle logic)

**Acceptance Criteria:**
- Toggle button switches between GUI and JSON views.
- Changes in JSON are reflected in GUI when switching back.
- Invalid JSON shows an error without losing data.
- Scroll position is preserved across toggles.
- Both views show the same data.

**Dependencies:** SettingsJsonEditor.

---

### Task 7: Extension-Contributed Settings Integration

**Title:** Display extension-contributed settings alongside built-in settings

**Description:** Settings registered by extensions (via `SettingsCatalog::ingest_plugin_settings`)
should appear in the settings UI under an "Extensions" category with the extension name
as a subgroup.

**Implementation Details:**
- Extension-contributed settings have `source` set to the extension ID.
- In the category tree, group them under "Extensions > [Extension Name]".
- Show an extension icon next to extension settings.
- Extension settings support all the same widget types as built-in settings.
- If an extension is uninstalled, its settings are grayed out with a
  "Extension not installed" badge.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (extension grouping)

**Acceptance Criteria:**
- Extension settings appear under "Extensions > [Name]".
- Extension icon distinguishes them from built-in settings.
- Uninstalled extension settings are grayed out.
- All widget types work for extension settings.

**Dependencies:** SettingsCatalog.

---

### Task 8: Setting Descriptions with Links

**Title:** Render setting descriptions as rich text with clickable links

**Description:** Setting descriptions may contain Markdown-style links
`[text](url)` or references to other settings `{setting.id}`. Render these
as interactive elements.

**Implementation Details:**
- Parse description text for `[link text](url)` patterns.
- Render link text in accent color with underline; clicking opens the URL.
- Parse `{editor.fontSize}` references and render them as clickable links that
  scroll to that setting.
- Parse `\`code\`` backtick patterns and render them in a monospace font.
- Use a custom wxPanel for description rendering (not wxStaticText).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/RichDescriptionRenderer.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/RichDescriptionRenderer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`

**Acceptance Criteria:**
- Links in descriptions are clickable and open in the browser.
- Setting references scroll to the referenced setting.
- Code spans render in monospace.
- Plain descriptions render normally.
- Links use theme accent color.

**Dependencies:** None.

---

### Task 9: Commonly Used Settings Section

**Title:** Show a "Commonly Used" section at the top of the settings panel

**Description:** When no search query is active and "All Settings" is selected in the
category tree, show a "Commonly Used" section at the top with the most frequently
accessed settings.

**Implementation Details:**
- Track setting access: each time a user views or modifies a setting, record it.
- Compute "commonly used" as the top 10 settings by access frequency.
- Default commonly used (before tracking data exists): Font Size, Font Family, Theme,
  Tab Size, Word Wrap, Auto Save, Line Numbers, Minimap, Bracket Matching.
- Show as a collapsible section header: "Commonly Used (9)" at the top.
- The section is skippable: user can collapse it and the preference persists.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (commonly used section)

**Acceptance Criteria:**
- "Commonly Used" section appears at the top.
- Contains the most accessed settings.
- Default list shown before tracking data exists.
- Section is collapsible.
- Collapse state persists.

**Dependencies:** None.

---

### Task 10: Settings Breadcrumb Navigation

**Title:** Show breadcrumb trail in the settings dialog header

**Description:** As the user navigates categories, show a breadcrumb: "Settings > Editor > Font"
in the dialog header area.

**Implementation Details:**
- Use the existing `breadcrumb_label_` in `SettingsPanel`.
- On category selection, build breadcrumb: "All Settings > [Group] > [Subgroup]".
- Each breadcrumb segment is clickable (navigates to that level).
- On search, breadcrumb shows: "Search results for 'font'".
- Use `SettingsNavModel::breadcrumb_for` for setting-specific breadcrumbs.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (breadcrumb updates)

**Acceptance Criteria:**
- Breadcrumb shows the current navigation path.
- Each segment is clickable.
- Search state shows "Search results for 'query'".
- Deep-linking to a setting shows full breadcrumb path.

**Dependencies:** Task 2.

---

### Task 11: Setting Type-Specific Widget Improvements

**Title:** Polish all 9 setting widget types for visual consistency

**Description:** Review and improve all widget types for visual consistency, spacing,
and interaction quality.

**Implementation Details:**
- Boolean: Use a custom toggle switch instead of `wxCheckBox`. Show "On" / "Off" text.
- Integer: Show a slider + number input side by side. Display min/max bounds.
- Double: Same as integer with decimal precision.
- String: Auto-growing text input. Show character count if relevant.
- Choice: Use a styled dropdown with icons per option if applicable.
- Color: Show a color swatch preview alongside the color picker button.
- FilePath: Show a truncated path with a browse button. Validate path exists.
- KeyBinding: Show a key capture button that records the next keystroke.
- StringList: Show items as removable chips/tags. Add button for new items.
- All widgets: consistent 44px row height, 12px category padding.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (all Create*Setting methods)

**Acceptance Criteria:**
- All 9 widget types render with consistent spacing and height.
- Boolean uses a toggle switch.
- Integer/Double shows slider + number input.
- Color shows a swatch preview.
- StringList uses chip/tag display.
- All widgets respect the theme.

**Dependencies:** None.

---

### Task 12: Inline Validation Errors

**Title:** Show inline validation errors below setting controls

**Description:** Use `SettingsControlModel::validate` to check values as the user types
and show inline error messages.

**Implementation Details:**
- After each keystroke in a setting control, call `SettingsControlModel::validate`.
- If invalid, show a red error message below the control with the `error_message`.
- If a corrective hint exists, show it in orange below the error.
- Prevent applying invalid values (disable Apply button).
- Show a red border on the control input.
- Validate on setting change, not on every keystroke (debounce 300ms).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (validation display)

**Acceptance Criteria:**
- Invalid values show a red error message below the control.
- Corrective hints show in orange.
- Red border appears on invalid inputs.
- Apply button is disabled while invalid values exist.
- Validation is debounced.

**Dependencies:** SettingsControlModel.

---

### Task 13: Settings Status Bar

**Title:** Show settings summary in a status bar at the bottom of the settings panel

**Description:** Display a status bar showing: total settings count, visible count,
modified count, and active scope.

**Implementation Details:**
- Use the existing `status_bar_label_` and `search_count_label_`.
- Format: "120 settings | 47 shown | 5 modified | Scope: User".
- Update on filter change, modification, and scope switch.
- Right side: "Last saved: 2 minutes ago" (if applicable).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (status bar)

**Acceptance Criteria:**
- Status bar shows counts.
- Counts update in real-time.
- Active scope is displayed.
- Status bar is themed.

**Dependencies:** None.

---

### Task 14: Settings Import/Export Polish

**Title:** Polish the import/export workflow with format options

**Description:** Enhance the existing `ExportSettings` and `ImportSettings` methods
with format selection and conflict resolution.

**Implementation Details:**
- Export dialog offers: JSON, YAML formats.
- Export scope: "All settings", "Modified only", "Current category only".
- Import dialog: file chooser with format auto-detection.
- On import, show a diff preview: "5 settings will be changed, 3 settings will be added."
- Conflict resolution: "Keep existing" / "Use imported" / "Ask for each" radio buttons.
- After import, show a summary toast.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (import/export polish)

**Acceptance Criteria:**
- Export offers format and scope selection.
- Import shows a diff preview before applying.
- Conflict resolution options are provided.
- Summary toast appears after import.
- Exported files are human-readable.

**Dependencies:** None.

---

### Task 15: Settings Profile Quick Switcher

**Title:** Add a profile quick-switcher dropdown in the settings toolbar

**Description:** Enhance the existing `profile_selector_` in `SettingsDialog` with
a quick-switch dropdown that previews profile effects before applying.

**Implementation Details:**
- Show available profiles from `ConfigProfileManager::profile_names()`.
- On profile hover (before selection), preview which settings would change
  by highlighting affected setting rows.
- On profile selection, apply via `ConfigProfileManager::apply_profile`.
- Show a confirmation: "Apply 'Developer' profile? 12 settings will change. [Apply] [Cancel]"
- After applying, show which settings changed.
- "Create Custom Profile" option at the bottom saves current modifications as a new profile.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (profile switcher)

**Acceptance Criteria:**
- Profile dropdown shows all available profiles.
- Hovering a profile previews affected settings.
- Applying shows a confirmation dialog.
- "Create Custom Profile" saves current state.
- Profile changes are reflected immediately.

**Dependencies:** ConfigProfileManager.

---

### Task 16: Settings Badge Rendering

**Title:** Render restart-required, experimental, and deprecated badges

**Description:** Use `SettingsNavModel::badge_for` to display visual badges next to
settings that require special attention.

**Implementation Details:**
- Restart-required: orange "Restart" badge. After changing, show a warning bar at
  the top: "Some settings require a restart to take effect. [Restart Now]".
- Experimental: blue "Experimental" badge with italic description.
- Deprecated: gray "Deprecated" badge with strikethrough label.
- Badges are rendered to the right of the setting label.
- Badges are searchable (searching "restart" finds all restart-required settings).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (badge rendering)

**Acceptance Criteria:**
- Restart-required settings show an orange badge.
- Experimental settings show a blue badge.
- Deprecated settings show a gray badge with strikethrough.
- Restart warning bar appears when relevant settings change.
- Badges are included in search results.

**Dependencies:** SettingsNavModel.

---

### Task 17: Settings Deep-Link from Command Palette

**Title:** Allow opening settings to a specific setting from the command palette

**Description:** When a user selects a setting from the palette's `?` mode (Phase 31
Task 6), open the settings dialog scrolled to that specific setting with it highlighted.

**Implementation Details:**
- `SettingsDialog::OpenToSetting(setting_id)` already exists.
- Enhance it to: scroll the settings list so the target setting is visible,
  flash the target setting row (brief yellow highlight), and set the category
  tree selection to the setting's category.
- Also wire to `SettingsPanel::SetSearchText(query)` for search-based deep linking.
- If the setting_id is not found, show a message: "Setting not found."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (deep-link polish)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (scroll-to, flash)

**Acceptance Criteria:**
- Selecting a setting from the palette opens the dialog scrolled to it.
- The target setting flashes briefly for visual identification.
- Category tree shows the correct category.
- Non-existent setting shows an error.

**Dependencies:** Phase 31 Task 6.

---

### Task 18: Settings Comparison View

**Title:** Add a comparison view showing differences between scopes

**Description:** Allow users to compare settings between User and Workspace scopes
to see what the workspace overrides.

**Implementation Details:**
- Add a "Compare Scopes" button in the settings dialog toolbar.
- Opens a split view: left = User settings, right = Workspace settings.
- Settings that differ between scopes are highlighted.
- Settings only in one scope are marked "Only in User" / "Only in Workspace".
- Allow copying values between scopes via drag or button.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/SettingsCompareView.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/SettingsCompareView.cpp`

**Acceptance Criteria:**
- Compare view shows side-by-side scope settings.
- Differing settings are highlighted.
- Scope-specific settings are marked.
- Values can be copied between scopes.

**Dependencies:** Task 3.

---

### Task 19: Settings Search Debounce and Performance

**Title:** Optimize settings search for large catalogs with 500+ settings

**Description:** Ensure search remains responsive with extension-contributed settings
expanding the catalog to 500+ entries.

**Implementation Details:**
- The existing `search_debounce_timer_` (300ms) is a good start.
- Reduce debounce to 150ms for better responsiveness.
- Compute search results in a background thread for catalogs > 200 entries.
- Cache the last 10 search results for instant back-navigation.
- Virtual scrolling for the settings list (only render visible setting rows).
- Lazy widget creation: create setting widgets on demand as they scroll into view.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (performance optimizations)

**Acceptance Criteria:**
- Search with 500+ settings responds within 100ms.
- Scrolling is smooth with 200+ visible settings.
- Debounce is 150ms.
- Background search does not cause UI freezes.

**Dependencies:** None.

---

### Task 20: Settings Table of Contents

**Title:** Add a floating table of contents for long settings categories

**Description:** When a category has 20+ settings, show a floating mini-TOC on the
right side that lists subgroups as quick-jump links.

**Implementation Details:**
- When the settings list for a category exceeds 20 items, show a mini-TOC.
- Mini-TOC lists subgroup names as clickable anchors.
- Clicking a subgroup scrolls the settings list to that section.
- The currently visible subgroup is highlighted in the TOC.
- TOC is positioned as a fixed sidebar (does not scroll with settings).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.h` (mini-TOC widget)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp`

**Acceptance Criteria:**
- Mini-TOC appears for categories with 20+ settings.
- Clicking a TOC entry scrolls to that subgroup.
- Current subgroup is highlighted in the TOC.
- TOC is fixed and does not scroll.
- TOC hides for small categories.

**Dependencies:** Task 2.

---

### Task 21: Settings Undo/Redo Polish

**Title:** Enhance undo/redo with visual feedback and keyboard shortcuts

**Description:** The existing undo/redo stack in SettingsPanel works but lacks visual
feedback. Add Cmd+Z / Cmd+Shift+Z shortcuts and a brief change animation.

**Implementation Details:**
- Register Cmd+Z and Cmd+Shift+Z in the SettingsDialog context.
- On undo, briefly flash the reverted setting row (amber highlight).
- Show undo/redo count in the toolbar: "Undo (3)" / "Redo (1)".
- Disable undo/redo buttons when stacks are empty.
- On multi-step undo (holding Cmd+Z), undo rapidly with a 200ms delay between each.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (undo/redo polish)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (flash animation)

**Acceptance Criteria:**
- Cmd+Z undoes the last setting change with a visual flash.
- Cmd+Shift+Z redoes.
- Toolbar shows undo/redo counts.
- Buttons are disabled when stacks are empty.
- Multi-step undo works with rapid key presses.

**Dependencies:** None.

---

### Task 22: Settings Theme Integration

**Title:** Full theme support for all settings UI components

**Description:** Apply theme tokens to every settings UI element.

**Implementation Details:**
- Theme tokens: `settings.background`, `settings.headerBackground`, `settings.headerForeground`,
  `settings.modifiedIndicator`, `settings.searchHighlight`, `settings.controlBackground`,
  `settings.controlBorder`, `settings.resetButton`, `settings.badgeBackground`.
- Apply to: SettingsPanel, SettingsDialog, SettingsJsonEditor.
- Subscribe to `ThemeChangedEvent` in all components.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (theme)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (theme)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsJsonEditor.cpp` (theme)

**Acceptance Criteria:**
- All settings UI elements use theme tokens.
- Theme switch updates all colors immediately.
- High contrast theme maintains readability.

**Dependencies:** None.

---

### Task 23: Settings Accessibility

**Title:** Full accessibility for the settings UI

**Description:** Ensure all settings controls are accessible to screen readers and
keyboard-only users.

**Implementation Details:**
- Each setting control has an accessible label: "Font Size: 14 (default: 14)".
- Category tree is navigable with arrow keys.
- Tab key moves between setting controls.
- Search input is labeled "Search Settings".
- Scope tabs are accessible with proper role.
- Modified indicator is announced: "Font Size: modified".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (accessibility)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsDialog.cpp` (accessibility)

**Acceptance Criteria:**
- VoiceOver reads setting labels and values.
- Tab navigation moves between controls.
- Modified state is announced.
- All interactive elements are keyboard-accessible.

**Dependencies:** None.

---

### Task 24: Settings Command Registration

**Title:** Register all settings commands in CommandRegistry

**Description:** Ensure all settings-related commands are registered.

**Implementation Details:**
- Commands:
  - `settings.open` (Cmd+,): Open settings
  - `settings.openJSON`: Open JSON settings
  - `settings.openWorkspace`: Open workspace settings
  - `settings.resetAll`: Reset all settings to defaults
  - `settings.exportSettings`: Export settings
  - `settings.importSettings`: Import settings
  - `settings.switchProfile`: Switch settings profile
- All in "Settings" category.

**Files Affected:**
- Modified: command registration code.

**Acceptance Criteria:**
- All commands are registered.
- Shortcuts are correct.
- Commands appear in the palette.

**Dependencies:** None.

---

### Task 25: Unit Tests for Settings UI Overhaul

**Title:** Comprehensive test suite for settings UI enhancements

**Description:** Write Catch2 tests covering search highlighting, category tree,
scope cascade, validation, and profile switching.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_settings_ui_v2.cpp`
- Sections:
  - SettingsNavModel: category tree, search with ranking, breadcrumb, badges.
  - SettingsControlModel: validate all types, clamp, staged changes, revert.
  - Search: fuzzy match ranking, highlight position computation.
  - Scope cascade: effective_value resolution, override detection.
  - Profile: apply profile, create from diff, export.
  - Modified detection: modified_count, reset clears modified.
  - Import/export: round-trip, conflict detection.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_settings_ui_v2.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt`

**Acceptance Criteria:**
- All test sections pass.
- Validation tests cover all setting types.
- Scope cascade tests cover all 4 scopes.
- At least 30 test cases.

**Dependencies:** Tasks 1, 3, 12.
