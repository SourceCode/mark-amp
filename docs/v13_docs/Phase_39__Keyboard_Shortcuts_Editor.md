# Phase 39: Keyboard Shortcuts Editor

## Overview

Build a dedicated, visual keyboard shortcuts editor that allows users to view, search,
edit, and customize all keyboard bindings in the application. The current codebase has
`ShortcutEditor` (a basic wxDialog with wxListCtrl, search, and record-binding support),
`ShortcutOverlay` (a modal overlay displaying shortcuts by category with search filter,
export, and click-to-copy), `ShortcutOverlayModel` (search/filter/category/tooltip model),
and `ShortcutManager` (core manager with registration, remapping, persistence, conflict
detection, and export/import). This phase enhances these into a production-grade keybinding
editor with chord support, when-clause conditions, scope management, preset keymaps,
a cheat sheet view, and import/export workflows.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| ShortcutEditor | `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.h` | wxDialog, wxListCtrl, search, record binding |
| ShortcutOverlay | `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.h` | ThemeAwareWindow, categories, filter, export MD/JSON |
| ShortcutOverlayModel | `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlayModel.h` | Entries, search, context filter, categories |
| ShortcutManager | `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` | Register, remap, persist, conflict detect, export |
| CommandRegistry | `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h` | Commands with when-clause, shortcuts |
| ContextKeyService | `/Users/ryanrentfro/code/markamp/src/core/ContextKeyService.h` | Context key evaluation |
| WhenClause | `/Users/ryanrentfro/code/markamp/src/core/WhenClause.h` | When-clause expression evaluator |

## Dependencies

- Phase 36 (Settings UI Overhaul) provides UI patterns reused here.
- Phase 32 (Go-To System) Task 22 provides chord support foundations.

---

## Tasks

### Task 1: Visual Keybinding Editor Redesign

**Title:** Redesign ShortcutEditor as a full-featured keybinding management view

**Description:** Replace the basic `wxListCtrl` in `ShortcutEditor` with a rich, themed
table view showing: command name, keybinding, source (default/user/extension), when-clause,
and action buttons.

**Implementation Details:**
- Redesign `ShortcutEditor` layout:
  - Search bar at top (reuse existing `search_ctrl_`).
  - Column headers: Command | Keybinding | When | Source.
  - Each row shows: command label, current binding (styled as a key cap), when-clause
    (or "always"), source badge.
  - Hover on a row shows an edit icon and a reset icon.
  - Click on the keybinding cell enters record mode.
- Use custom drawing for key cap styling (rounded rect with key text).
- Row height: 36px. Alternating row background for readability.
- Sort by: Command name (default), Keybinding, Source.
- Show total count: "256 keybindings".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.h` (redesign layout)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp`

**Acceptance Criteria:**
- Table shows all registered shortcuts with columns.
- Key bindings render as styled key caps.
- Rows support hover with edit/reset icons.
- Sorting works on all columns.
- Total count is displayed.

**Dependencies:** None.

---

### Task 2: Chord Binding Support

**Title:** Support multi-key chord bindings (e.g., Ctrl+K Ctrl+C)

**Description:** Allow recording and displaying chord (two-step) keybindings.

**Implementation Details:**
- Extend `Shortcut` struct with `int secondary_key_code` and `int secondary_modifiers`.
- In record mode, after the first keypress, show "Press second key..." and wait
  for the second keypress (with 2-second timeout).
- Display chord bindings as: "[Ctrl+K] [Ctrl+C]" (two separate key caps).
- `ShortcutManager::process_key_event` already needs chord support from Phase 32 Task 22;
  this task handles the UI recording side.
- Format: `ShortcutManager::format_shortcut` extended to produce "Ctrl+K Ctrl+C".
- Persistence: save chord as "Ctrl+K Ctrl+C" in keybindings.md.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` (chord fields)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp` (format, persist)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (chord recording)

**Acceptance Criteria:**
- Chord bindings can be recorded (first key + second key).
- Chords display as two key caps.
- Chord format "Ctrl+K Ctrl+C" persists correctly.
- Timeout after 2 seconds cancels the chord recording.
- Single-key and chord bindings coexist.

**Dependencies:** Phase 32 Task 22 (chord processing in ShortcutManager).

---

### Task 3: Conflict Detection with Resolution

**Title:** Detect and resolve keybinding conflicts with visual guidance

**Description:** When the user records a binding that conflicts with an existing one,
show the conflict and offer resolution options.

**Implementation Details:**
- On binding record, call `ShortcutManager::has_conflict(key, mods, context)`.
- If conflict exists, show a conflict dialog:
  - "This binding conflicts with [command name] in [context]."
  - Options: "Override" (remove the existing binding), "Keep Both" (different contexts
    may allow coexistence), "Cancel".
- In the shortcut list, conflicting bindings show a warning icon.
- Use the existing `ShortcutManager::get_conflicts()` to highlight all conflicts.
- Conflicting pairs are shown with a red border.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (conflict UI)

**Acceptance Criteria:**
- Recording a conflicting binding shows a conflict dialog.
- "Override" removes the previous binding.
- "Keep Both" is available when contexts differ.
- "Cancel" aborts the rebinding.
- All conflicts are highlighted in the list.

**Dependencies:** Task 1.

---

### Task 4: When-Clause Conditions Editor

**Title:** Allow setting when-clause conditions for keybindings

**Description:** Each keybinding can have a "when" condition that determines when it is
active (e.g., `editorTextFocus`, `panelVisible:outline`).

**Implementation Details:**
- Add a "When" column in the shortcut editor.
- Clicking the "When" cell opens a when-clause editor:
  - Text input with autocomplete for known context keys.
  - Predefined conditions: `editorTextFocus`, `editorHasSelection`,
    `panelVisible:search`, `panelVisible:outline`, `inputFocus`, `terminalFocus`.
  - Operators: `&&`, `||`, `!`.
- Validate when-clause syntax using `WhenClauseEvaluator`.
- Invalid clauses show an error and are not saved.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/WhenClauseEditor.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/WhenClauseEditor.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp`

**Acceptance Criteria:**
- When-clause can be edited per binding.
- Autocomplete suggests known context keys.
- Invalid syntax shows an error.
- When-clauses are persisted with the binding.
- Bindings without when-clauses are "always active".

**Dependencies:** Task 1, WhenClauseEvaluator.

---

### Task 5: Keybinding Scope (Global, Editor, Terminal, Panel)

**Title:** Visual scope assignment for keybindings

**Description:** Each binding has a scope that determines where it is active. Provide
UI for viewing and changing the scope.

**Implementation Details:**
- Scopes: "Global", "Editor", "Terminal", "Sidebar", "Panel".
- Scope is stored as `Shortcut::context`.
- In the shortcut editor, add a "Scope" dropdown per binding.
- Filter the list by scope: show all, or filter to a specific scope.
- Scope badges: colored dots (Global = gray, Editor = blue, Terminal = green,
  Sidebar = orange, Panel = purple).
- When scope is "Editor", the binding only works when the editor has focus.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (scope UI)

**Acceptance Criteria:**
- Scope is displayed per binding.
- Scope can be changed via dropdown.
- Filter by scope works.
- Scope badges are colored.
- Scope affects when the binding is active.

**Dependencies:** Task 1.

---

### Task 6: Import/Export Keybinding Profiles

**Title:** Import and export keybinding configurations as JSON files

**Description:** Allow users to export their custom keybindings and import keybindings
from files or shared configurations.

**Implementation Details:**
- Export: `ShortcutManager::export_as_json()` already exists. Add a UI button "Export"
  that opens a save-file dialog.
- Import: `ShortcutManager::import_from_json()` exists. Add "Import" button with
  file chooser.
- On import, show a preview: "15 bindings will be added, 3 will be overridden."
- Merge vs Replace: "Merge with current" (adds missing, overrides conflicts) or
  "Replace all" (clears existing custom bindings first).
- Share format: `.json` file compatible with VSCode's keybindings.json structure.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (import/export buttons)

**Acceptance Criteria:**
- Export saves keybindings to a JSON file.
- Import reads and applies keybindings.
- Preview shows changes before import.
- Merge and Replace options are available.
- Format is compatible with standard keybinding JSON.

**Dependencies:** Task 1.

---

### Task 7: Reset Individual Binding

**Title:** Reset a single keybinding to its default

**Description:** Each customized binding shows a "Reset" button that reverts it to the
default binding.

**Implementation Details:**
- In the shortcut list, customized bindings show a reset icon (undo arrow).
- Clicking reset: restores the default key, modifiers, and when-clause from
  `default_shortcuts_` in `ShortcutManager`.
- If the binding was added by the user (no default), reset removes it.
- Show a confirmation: "Reset [command] to default binding [Ctrl+S]?"
- After reset, the row briefly flashes to confirm.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (reset button)

**Acceptance Criteria:**
- Reset icon appears on customized bindings.
- Reset restores the default binding.
- Confirmation dialog is shown.
- Row flashes after reset.
- User-added bindings are removed on reset.

**Dependencies:** Task 1.

---

### Task 8: Reset All to Defaults

**Title:** Reset all keybindings to factory defaults

**Description:** A "Reset All" action that restores all bindings to their defaults.

**Implementation Details:**
- "Reset All" button in the editor toolbar.
- Confirmation dialog: "Reset all N custom bindings to defaults? This cannot be undone."
- Calls `ShortcutManager::reset_to_defaults()`.
- After reset, refresh the editor list.
- Show a toast: "All keybindings reset to defaults."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (reset all)

**Acceptance Criteria:**
- "Reset All" restores all defaults.
- Confirmation dialog is shown.
- List refreshes after reset.
- Toast confirms the action.

**Dependencies:** Task 1.

---

### Task 9: Keymap Presets (VSCode, IntelliJ, Sublime, Vim)

**Title:** Provide importable keymap presets from popular editors

**Description:** Allow users to switch to a keymap preset that mirrors the keybindings
of their preferred editor.

**Implementation Details:**
- Create keymap files: `keymaps/vscode.json`, `keymaps/intellij.json`,
  `keymaps/sublime.json`, `keymaps/vim.json`.
- Each file maps command IDs to key bindings.
- "Keymap Preset" dropdown in the editor: "MarkAmp (default)", "VSCode", "IntelliJ",
  "Sublime Text", "Vim".
- Selecting a preset applies the keymap (with confirmation: "Apply VSCode keymap?
  12 bindings will change.").
- Show a diff of what will change before applying.
- "Vim" preset enables Vim mode emulation (if available).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/resources/keymaps/vscode.json`
- New: `/Users/ryanrentfro/code/markamp/resources/keymaps/intellij.json`
- New: `/Users/ryanrentfro/code/markamp/resources/keymaps/sublime.json`
- New: `/Users/ryanrentfro/code/markamp/resources/keymaps/vim.json`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (preset selector)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp` (apply preset)

**Acceptance Criteria:**
- Four keymap presets are available.
- Selecting a preset shows a diff preview.
- Confirmation is required before applying.
- Applied preset changes keybindings immediately.
- "MarkAmp (default)" restores original bindings.

**Dependencies:** Task 1.

---

### Task 10: Keyboard Shortcut Cheat Sheet

**Title:** Build a printable keyboard shortcut cheat sheet view

**Description:** A formatted, categorized view of all shortcuts optimized for printing
or quick reference.

**Implementation Details:**
- Enhance the existing `ShortcutOverlay` or create a new view.
- Layout: multi-column grid organized by category.
- Each category shows a header with shortcuts listed as: "Label ... Ctrl+S".
- Optimized for printing: clean layout, no background colors, high contrast.
- "Print" button opens the system print dialog.
- "Export as PDF" option (if available) or "Export as Markdown".
- The existing `export_as_markdown()` and `export_as_json()` can be reused.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutCheatSheet.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutCheatSheet.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutOverlay.cpp` (link to cheat sheet)

**Acceptance Criteria:**
- Cheat sheet shows all shortcuts organized by category.
- Layout is print-friendly.
- "Print" opens system print dialog.
- "Export as Markdown" generates a clean markdown file.
- Multi-column layout maximizes space.

**Dependencies:** Task 1.

---

### Task 11: Record Keystrokes Mode

**Title:** Add a "Record" mode that captures keystrokes for binding

**Description:** A dedicated recording state where the user presses keys and the editor
captures them as a binding, showing real-time feedback.

**Implementation Details:**
- In the keybinding cell, clicking "Record" or pressing Enter starts capture mode.
- The cell shows: "Press desired key combination..." with a blinking border.
- Capture the next key event (including modifiers).
- For chords, show "First key captured: Ctrl+K. Press second key..." after first press.
- Display the captured binding in real-time.
- Escape cancels recording.
- Enter (after capturing) confirms the binding.
- If the captured key matches an existing binding, show conflict inline.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (record mode)

**Acceptance Criteria:**
- Clicking record starts capture mode with visual indicator.
- Key presses are captured and displayed in real-time.
- Chord recording shows progressive feedback.
- Escape cancels recording.
- Conflicts are shown inline during recording.

**Dependencies:** Task 2 (chord support).

---

### Task 12: Duplicate Detection

**Title:** Highlight and resolve duplicate keybinding assignments

**Description:** Beyond simple conflict detection, find and highlight all cases where
the same key combination is assigned to multiple commands.

**Implementation Details:**
- Use `ShortcutManager::get_conflicts()` to get all conflict pairs.
- In the shortcut list, duplicate bindings show:
  - A warning icon with a count badge: "2 conflicts".
  - Clicking the icon shows a popup listing all commands with the same binding.
  - The popup offers: "Keep this one" (removes others), "Open other" (navigates to it).
- Add a filter: "Show conflicts only" that displays only conflicting bindings.
- Show a summary in the toolbar: "3 conflicts detected".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (duplicate UI)

**Acceptance Criteria:**
- Duplicate bindings are highlighted.
- Conflict badge shows the count.
- Popup lists all conflicting commands.
- "Show conflicts only" filter works.
- Conflict summary is displayed.

**Dependencies:** Task 3.

---

### Task 13: Keybinding Search Enhancement

**Title:** Enhanced search that matches command names, keybindings, and when-clauses

**Description:** The search bar should match against all columns: command name,
description, key binding text, when-clause, and category.

**Implementation Details:**
- Search logic: query is matched against all fields of each shortcut.
- Special search syntax:
  - `@binding:Ctrl+S` searches by key combination text.
  - `@source:user` searches by source (user/default/extension).
  - `@when:editor` searches by when-clause.
  - Plain text searches command name and description.
- Results are ranked: exact keybinding match > command name match > description match.
- Show match count: "12 matching keybindings".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (search enhancement)

**Acceptance Criteria:**
- Search matches across all columns.
- Special search prefixes work (@binding, @source, @when).
- Results are ranked by relevance.
- Match count is displayed.

**Dependencies:** Task 1.

---

### Task 14: Customized Bindings Indicator

**Title:** Visually distinguish default vs customized keybindings

**Description:** Make it immediately clear which bindings have been customized by the user.

**Implementation Details:**
- Customized bindings show a blue left-border bar (matching settings modified indicator).
- Source column shows "Custom" badge for user-modified bindings.
- Filter: "Show customized only" shows only user-modified bindings.
- Count: "12 customized bindings" in the toolbar.
- Default bindings show "Default" in gray.
- Extension bindings show "Extension: [name]" in purple.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (customization indicator)

**Acceptance Criteria:**
- Customized bindings have a blue left border.
- Source badges distinguish Default/Custom/Extension.
- "Show customized only" filter works.
- Count of customized bindings is displayed.

**Dependencies:** Task 1.

---

### Task 15: Shortcut Editor Integration with Settings

**Title:** Access the shortcut editor from the settings UI

**Description:** Add a "Keyboard Shortcuts" section in the settings dialog that links
to the full shortcut editor.

**Implementation Details:**
- In the SettingsPanel category tree, add "Keyboard Shortcuts" under "Workbench".
- Clicking it shows a summary: "256 keybindings (12 customized)" with a link
  "Open Keyboard Shortcuts Editor".
- Also show the top 5 most-used shortcuts.
- Setting `keybindings.file` points to the keybindings file path (read-only display).
- "Edit Keybindings JSON" button opens the raw keybindings file.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (shortcuts section)

**Acceptance Criteria:**
- "Keyboard Shortcuts" appears in settings.
- Link opens the full editor.
- Summary shows count and customization.
- Top 5 most-used are shown.
- JSON edit link works.

**Dependencies:** Task 1, Phase 36.

---

### Task 16: Shortcut Editor Theme Integration

**Title:** Full theme support for the shortcut editor

**Description:** Apply theme tokens to all shortcut editor visual elements.

**Implementation Details:**
- Theme tokens: `shortcuts.background`, `shortcuts.rowBackground`,
  `shortcuts.rowAlternateBackground`, `shortcuts.keyCapBackground`,
  `shortcuts.keyCapBorder`, `shortcuts.keyCapForeground`, `shortcuts.conflictColor`,
  `shortcuts.customizedIndicator`, `shortcuts.recordingBorder`.
- Key cap rendering: rounded rectangle with subtle shadow.
- Subscribe to ThemeChangedEvent.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (theme)

**Acceptance Criteria:**
- All elements use theme tokens.
- Key caps render with themed styling.
- Theme switch updates immediately.
- Recording mode border is themed.

**Dependencies:** Task 1.

---

### Task 17: Shortcut Editor Accessibility

**Title:** Full accessibility support for the shortcut editor

**Description:** Ensure screen readers can navigate and use the shortcut editor.

**Implementation Details:**
- Table rows announce: "Command: Save, Binding: Ctrl+S, Scope: Global".
- Recording mode announces: "Recording shortcut. Press desired key combination."
- Conflict warnings are announced.
- All toolbar buttons have accessible labels.
- Tab navigation works through the table and toolbar.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (accessibility)

**Acceptance Criteria:**
- VoiceOver reads shortcut entries correctly.
- Recording mode is announced.
- Conflicts are announced.
- All elements are keyboard-navigable.

**Dependencies:** Task 1.

---

### Task 18: Keyboard Shortcuts in Command Palette

**Title:** Display keybinding alongside each command in the command palette

**Description:** Ensure the command palette (Phase 31) shows the correct, current
keybinding for each command, including custom remappings.

**Implementation Details:**
- When building palette command list, query `ShortcutManager::get_shortcut_text(command_id)`.
- Show the binding right-aligned in the palette (already done in Phase 31 Task 9).
- Ensure remapped bindings are reflected (not just default bindings).
- For unbound commands, show no shortcut text.
- Chord bindings display correctly: "Ctrl+K Ctrl+C".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (live binding lookup)

**Acceptance Criteria:**
- Palette shows current (possibly remapped) bindings.
- Chord bindings display correctly.
- Unbound commands show no binding.
- Remapped bindings are reflected immediately.

**Dependencies:** Phase 31 Task 9.

---

### Task 19: Shortcut Quick Record from Palette

**Title:** Allow recording a new shortcut directly from the command palette

**Description:** When hovering a command in the palette, show a "Set Shortcut" action
that enters record mode without opening the full shortcut editor.

**Implementation Details:**
- Right-click a command in the palette > "Set Keyboard Shortcut...".
- Inline record: the palette input transforms to show "Press desired key combination..."
- After recording, the binding is saved.
- Conflicts are shown inline.
- Cancel with Escape.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (quick record)

**Acceptance Criteria:**
- Right-click shows "Set Keyboard Shortcut" option.
- Recording works inline in the palette.
- Binding is saved after recording.
- Conflicts are shown.
- Escape cancels.

**Dependencies:** Task 11.

---

### Task 20: Command Registration for Shortcuts Editor

**Title:** Register all shortcut editor commands

**Description:** Register commands for all shortcut editor operations.

**Implementation Details:**
- Commands:
  - `shortcuts.open` (Cmd+K Cmd+S): Open keyboard shortcuts
  - `shortcuts.resetAll`: Reset all bindings
  - `shortcuts.export`: Export bindings
  - `shortcuts.import`: Import bindings
  - `shortcuts.cheatSheet`: Open cheat sheet
  - `shortcuts.showConflicts`: Show binding conflicts
- All in "Keyboard Shortcuts" category.

**Files Affected:**
- Modified: command registration code.

**Acceptance Criteria:**
- All commands are registered.
- Chord shortcut Cmd+K Cmd+S works.
- Commands appear in the palette.

**Dependencies:** Task 1.

---

### Task 21-25: Additional Tasks

**Task 21: Keybinding File Editor** - Provide a raw keybindings.md editor (similar to SettingsJsonEditor). Show the markdown format with YAML frontmatter. Syntax highlighting for key combinations.

**Task 22: Contextual Shortcut Help** - When a user holds a modifier key for 1 second, show a floating overlay listing all available shortcuts with that modifier in the current context.

**Task 23: Shortcut Conflict Auto-Resolution** - When importing keymaps, automatically resolve conflicts by context-based priority: editor-specific bindings override global for editor context. Log resolutions.

**Task 24: Shortcut Usage Analytics** - Track which shortcuts the user uses most/least. Show usage stats in the editor: "Used 42 times" per binding. Suggest "Did you know?" for unused shortcuts.

**Task 25: Unit Tests for Shortcuts Editor** - Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_shortcuts_editor.cpp`. Sections: Chord recording, conflict detection, when-clause validation, preset application, import/export round-trip, scope filtering, search with special prefixes. At least 30 test cases.

---

### Task 26: Print/Export Keyboard Shortcuts Reference Card

**Title:** Generate a formatted, printable keyboard shortcuts reference card

**Description:** Create a compact, visually polished reference card layout that users can
print or export as PDF/HTML, organized by category with key cap styling, suitable for
desk reference.

**Implementation Details:**
- Create `ShortcutReferenceCard` generator in `/Users/ryanrentfro/code/markamp/src/ui/ShortcutReferenceCard.h`:
  - Accepts the full list of active shortcuts from `ShortcutManager`.
  - Groups shortcuts by category (File, Edit, View, Navigation, Build, etc.).
  - Layout: 2-column or 3-column grid, each category as a card/box.
  - Each entry rendered as: `Command Label ........... Ctrl+Shift+P`.
  - Key combinations rendered with key cap styling (rounded rectangles).
  - Header: "MarkAmp Keyboard Shortcuts" with active keymap preset name.
  - Footer: date generated, custom shortcuts marked with an asterisk.
- Export formats:
  - HTML: self-contained HTML with inline CSS for key cap styling. Opens in default browser.
  - Markdown: clean table-based format (reuse `export_as_markdown()` from ShortcutManager
    but with enhanced formatting and category grouping).
  - Print: render to a `wxPrintout` subclass for native printing.
    - Paper size selection: A4, Letter.
    - Orientation: landscape (fits more columns) or portrait.
    - Font size auto-scales to fit all shortcuts on minimal pages.
- "Export Reference Card" submenu in ShortcutEditor toolbar:
  - "Print..." opens the print dialog.
  - "Export as HTML..." saves an HTML file.
  - "Export as Markdown..." saves a markdown file.
- Register command: `shortcuts.exportReferenceCard`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutReferenceCard.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutReferenceCard.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (export menu)

**Acceptance Criteria:**
- Reference card is generated with all active shortcuts grouped by category.
- HTML export opens in the browser with key cap styling.
- Markdown export produces clean, readable tables.
- Print dialog allows paper size and orientation selection.
- Font size auto-scales to minimize page count.
- Custom shortcuts are marked with an asterisk.
- Active keymap preset name appears in the header.

**Dependencies:** Task 1, Task 10.

---

### Task 27: Keyboard Shortcut Learning Mode

**Title:** Implement an in-context shortcut learning overlay

**Description:** A learning mode that shows a floating overlay displaying available keyboard
shortcuts relevant to the user's current context (editor, file tree, build panel, etc.),
helping users discover shortcuts organically as they work.

**Implementation Details:**
- Create `ShortcutLearningOverlay` in `/Users/ryanrentfro/code/markamp/src/ui/ShortcutLearningOverlay.h`:
  - A semi-transparent floating panel anchored to the bottom-right of the main frame.
  - Shows shortcuts relevant to the current focus context:
    - When editor has focus: show editor shortcuts (Save, Undo, Find, Go to Line, etc.).
    - When file tree has focus: show file tree shortcuts (Rename, Delete, New File, etc.).
    - When build panel has focus: show build shortcuts (Build, Clean, Cancel, etc.).
  - Maximum 8-10 shortcuts shown at a time (the most commonly used for that context).
  - Shortcuts display as: `[Ctrl+S] Save` with key cap styling.
  - Auto-hide after 5 seconds of inactivity; reappear on focus change.
- Context detection:
  - Subscribe to `FocusChangedEvent` (or use `wxWindow::FindFocus()` polling).
  - Map focus windows to context categories using `ContextKeyService`.
  - Query `ShortcutManager` for shortcuts with matching when-clauses.
- Learning progression:
  - Track which shortcuts the user has used (from Task 24 analytics).
  - Prioritize showing shortcuts the user has NOT yet used.
  - After a shortcut is used 5+ times, remove it from the learning overlay
    (the user has "learned" it).
  - Show a subtle "New!" badge on shortcuts the user has never used.
- Toggle: `View > Toggle Shortcut Learning Mode` or `shortcuts.toggleLearningMode`.
- Setting: `shortcuts.learningMode` (default: false).
- Persist learning state in `~/.markamp/shortcut_learning.json`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutLearningOverlay.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutLearningOverlay.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` (context query method)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp` (get_shortcuts_for_context)

**Acceptance Criteria:**
- Overlay shows shortcuts relevant to the currently focused panel.
- Shortcuts are displayed with key cap styling.
- Overlay auto-hides after 5 seconds and reappears on context change.
- Shortcuts the user has mastered (5+ uses) are removed from the overlay.
- "New!" badge appears on never-used shortcuts.
- Learning state persists across sessions.
- Learning mode is toggleable via command and settings.
- Maximum 8-10 shortcuts shown at a time.

**Dependencies:** Task 1, Task 24 (shortcut usage analytics).

---

### Task 28: Command Name Autocomplete in Binding Editor

**Title:** Add command name autocomplete when creating new keybindings

**Description:** When adding a new keybinding or searching for a command to bind, provide
fuzzy autocomplete over all registered command IDs and their display labels.

**Implementation Details:**
- Enhance the ShortcutEditor's "Add Binding" workflow:
  - Add an "Add Custom Binding" button in the toolbar.
  - Clicking opens a command picker: a search input with a dropdown list of all registered
    commands from `CommandRegistry`.
  - Autocomplete features:
    - Fuzzy matching on both command ID and display label (e.g., typing "save" matches
      `file.save`, `file.saveAs`, `file.saveAll`).
    - Results show: command label (bold), command ID (dimmed), current binding (if any),
      category badge.
    - Unbound commands are highlighted to suggest they need bindings.
    - Results sorted: unbound commands first, then by relevance score.
  - After selecting a command, enter record mode for the binding.
  - After recording, optionally set the when-clause (default: "always").
- Reuse the fuzzy matching algorithm from the Command Palette (Phase 31).
- Also enhance the existing search bar (Task 13) with command ID autocomplete:
  - When typing in the search bar, show autocomplete suggestions below.
  - Pressing Tab accepts the suggestion and filters the list.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/CommandPickerPopup.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/CommandPickerPopup.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.h` (add binding button)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (command picker integration)

**Acceptance Criteria:**
- "Add Custom Binding" button opens the command picker.
- Fuzzy search matches both command IDs and display labels.
- Results show command label, ID, current binding, and category.
- Unbound commands are prioritized in results.
- Selecting a command enters record mode for the new binding.
- Tab-completion works in the search bar.
- Matching algorithm is consistent with Command Palette behavior.

**Dependencies:** Task 1, Task 13, Phase 31 (Command Palette fuzzy matching).

---

### Task 29: Multi-Key Sequence Recording and Display

**Title:** Support arbitrary-length key sequences beyond two-key chords

**Description:** Extend chord support from two-key sequences to arbitrary-length key
sequences (e.g., Ctrl+K Ctrl+Shift+C Ctrl+F for three-step sequences), with progressive
recording UI and sequence visualization.

**Implementation Details:**
- Extend `Shortcut` struct to support variable-length sequences:
  - Replace `secondary_key_code`/`secondary_modifiers` with:
    `vector<KeyChord> sequence` where `struct KeyChord { int key_code; int modifiers; }`.
  - Backward compatible: single-key shortcuts have a sequence of length 1,
    two-key chords have length 2.
  - Maximum sequence length: 4 keys (prevent runaway recording).
- Recording UI changes:
  - After each keypress, show: "[Ctrl+K] [?] Press next key or Enter to finish..."
  - Progressive display: recorded keys appear as key caps from left to right.
  - Enter confirms the sequence at current length.
  - Escape cancels the entire recording.
  - Backspace removes the last recorded key in the sequence.
  - Timeout: 3 seconds of inactivity after the first key auto-confirms.
- `ShortcutManager` changes:
  - `process_key_event` tracks the current position in a multi-key sequence.
  - Maintain a "pending sequence" state: after the first key matches, wait for the next.
  - Status bar shows: "Waiting for next key in sequence (Ctrl+K ...)".
  - If no matching sequence continues after the timeout, cancel and process the
    first key as a standalone shortcut (if one exists).
- Display:
  - Multi-key sequences render as: "[Ctrl+K] [Ctrl+Shift+C] [Ctrl+F]" (three key caps).
  - `format_shortcut` produces: "Ctrl+K Ctrl+Shift+C Ctrl+F".
- Persistence:
  - Sequences stored as space-separated chord strings in keybindings.md.
  - JSON format: `{"keys": ["Ctrl+K", "Ctrl+Shift+C", "Ctrl+F"]}`.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.h` (KeyChord struct, sequence vector)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/ShortcutManager.cpp` (sequence processing, format, persist)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/ShortcutEditor.cpp` (multi-step recording UI)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (pending sequence display)

**Acceptance Criteria:**
- Key sequences of length 1, 2, 3, and 4 are supported.
- Recording UI progressively shows each key cap as pressed.
- Enter confirms, Escape cancels, Backspace removes last key.
- Maximum sequence length of 4 is enforced.
- Status bar shows pending sequence state during key processing.
- Timeout auto-confirms after 3 seconds.
- Sequences persist and reload correctly in both markdown and JSON formats.
- Backward compatible with existing single-key and two-key chord bindings.

**Dependencies:** Task 2 (chord support), Task 11 (record mode).
