# Phase 36: Command System Completion

## Overview
CommandPalette, CommandHistory, and ShortcutManager exist. Commands are registered in MainFrame but the command system lacks: command categorization, context-aware filtering, recently used ranking, command arguments, and unified command registration. This phase completes the command system to VS Code parity.

## Prerequisites
- Phase 06 (Workbench navigation)
- Phase 35 (Documentation for command reference)

## Tasks

### Task 1: Wire Command Registry with Metadata
**Files:** `src/core/Command.cpp`, `src/core/Command.h`, `src/ui/MainFrame.cpp`
**Description:** Centralize all command registration in a CommandRegistry. Each command has: ID, title, category, description, shortcut, when-clause, icon.
**Acceptance Criteria:**
- CommandRegistry: central registration point
- Command metadata: id, title, category, description, icon
- When-clause: condition for command availability
- All existing commands migrated to registry
- Duplicate ID detection and warning
- Command count: 200+ registered commands

### Task 2: Wire Command Palette Fuzzy Search
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Command palette uses fuzzy search: match characters in order but not necessarily consecutive. Highlight matching characters. Rank by match quality and recency.
**Acceptance Criteria:**
- Fuzzy search: "ofl" matches "Open File"
- Matching characters highlighted in results
- Ranking: exact > prefix > fuzzy
- Recently used commands ranked higher
- Category shown: "Editor: Format Document"
- Result limit: 50 visible results

### Task 3: Wire Command Categories and Grouping
**Files:** `src/ui/CommandPalette.cpp`, `src/core/Command.h`
**Description:** Commands grouped by category. Category prefix shown in results. Filter by category: type ">" for commands, "#" for headings, "@" for symbols, ":" for go-to-line.
**Acceptance Criteria:**
- ">" prefix: show all commands (default)
- "#" prefix: show Markdown headings in current file
- "@" prefix: show symbols (code blocks, links)
- ":" prefix: go to line number
- Category badges in results
- Mode switching within palette

### Task 4: Wire Command History and Recently Used
**Files:** `src/core/CommandHistory.cpp`, `src/ui/CommandPalette.cpp`
**Description:** CommandHistory exists. Wire it to show recently used commands at the top of command palette results when no search text entered.
**Acceptance Criteria:**
- Last 20 used commands shown first (no search text)
- History persisted across sessions
- History cleared via command
- Most frequent commands tracked
- History section separator from search results
- History entry shows last-used timestamp

### Task 5: Wire Command Arguments
**Files:** `src/core/Command.h`, `src/ui/CommandPalette.cpp`
**Description:** Commands can accept arguments. After selecting a command, palette shows argument input (e.g., "Go to Line" shows number input, "Insert Snippet" shows snippet picker).
**Acceptance Criteria:**
- Commands declare argument types: string, number, choice, file
- Argument prompt shown after command selection
- Choice type: shows picker with options
- Number type: shows number input with validation
- File type: shows file picker
- Arguments passed to command handler

### Task 6: Wire Context-Aware Command Filtering
**Files:** `src/core/WhenClause.cpp`, `src/ui/CommandPalette.cpp`
**Description:** WhenClause exists. Wire it to filter commands based on current context: active surface, focused panel, selection state, file type.
**Acceptance Criteria:**
- Editor commands hidden when canvas active
- Canvas commands hidden when editor active
- Selection-dependent commands hidden when no selection
- File-type-specific commands filtered
- Context keys updated on focus change
- "Show All Commands" option bypasses filtering

### Task 7: Wire Quick Open (File Picker Mode)
**Files:** `src/ui/CommandPalette.cpp`, `src/core/VaultService.cpp`
**Description:** Cmd+P opens command palette in file picker mode: shows workspace files with fuzzy search, recent files first.
**Acceptance Criteria:**
- Cmd+P: file picker mode (no prefix)
- All workspace files listed
- Fuzzy search on file names
- Recent files ranked higher
- File path shown below name
- Icons for file type (.md, .json, .yaml)

### Task 8: Wire Go to Symbol
**Files:** `src/ui/CommandPalette.cpp`, `src/core/ContentIndexer.cpp`
**Description:** "@" prefix in command palette shows symbols: headings, code blocks, links, frontmatter keys in the current file.
**Acceptance Criteria:**
- "@" shows symbols in current file
- Headings: with level indicator (H1-H6)
- Code blocks: with language
- Links: wiki-links and URLs
- Frontmatter keys
- Navigate to selected symbol

### Task 9: Wire Go to Line
**Files:** `src/ui/CommandPalette.cpp`, `src/ui/EditorPanel.cpp`
**Description:** ":" prefix in command palette goes to a specific line. Also supports ":line:column" format.
**Acceptance Criteria:**
- ":" shows line input
- ":42" navigates to line 42
- ":42:10" navigates to line 42, column 10
- Current line shown as placeholder
- Out-of-range line: go to last line
- Cursor placed at target position

### Task 10: Wire Command Palette Inline Preview
**Files:** `src/ui/CommandPalette.cpp`
**Description:** File picker mode shows inline preview of selected file: first 10 lines shown to the right of the picker.
**Acceptance Criteria:**
- Preview panel to the right of picker
- Shows first 10 lines of selected file
- Preview updates on selection change
- Syntax highlighted preview
- Toggle preview on/off
- Preview respects current theme

### Task 11: Wire Multi-Command Chains
**Files:** `src/core/Command.h`, `src/core/ShortcutManager.cpp`
**Description:** Support chord shortcuts (two key combinations): Cmd+K Cmd+S, Cmd+K Cmd+C, etc. First key enters "waiting" mode, second key completes the command.
**Acceptance Criteria:**
- Chord shortcuts: Cmd+K followed by second key
- "Waiting" indicator in status bar: "Cmd+K was pressed"
- Escape cancels chord
- Timeout: chord canceled after 2 seconds
- Chord sequences registered in ShortcutManager
- At least 10 chord shortcuts defined

### Task 12: Wire Task Runner Commands
**Files:** `src/core/TaskRunnerService.cpp`, `src/ui/CommandPalette.cpp`
**Description:** TaskRunnerService is a 23-line stub. Upgrade it to run simple tasks: Markdown lint, export document, build search index. Tasks shown in command palette.
**Acceptance Criteria:**
- Task definitions in `.markamp/tasks.json`
- Tasks: shell command, label, group
- "Tasks: Run Task" shows task picker
- Task output in Output panel
- Task status in status bar
- Built-in tasks: lint, export, reindex

### Task 13: Wire Quick Pick Input
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Generic quick pick input: commands can show a list of options for user selection. Used by theme picker, kernel selector, branch selector.
**Acceptance Criteria:**
- Quick pick: list of items with icons and descriptions
- Single select and multi-select modes
- Fuzzy search within items
- Checkboxes for multi-select
- Current selection highlighted
- API for extensions to show quick picks

### Task 14: Wire Input Box
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Generic input box: commands can prompt for text input with validation. Used by rename, branch create, search replace.
**Acceptance Criteria:**
- Input box with title and placeholder
- Validation function with real-time feedback
- Input history (up arrow for previous inputs)
- Password mode (masked input)
- Multiline mode option
- Escape cancels, Enter confirms

### Task 15: Wire Command Keybinding Editor
**Files:** `src/core/ShortcutManager.cpp`, `src/ui/SettingsPanel.cpp`
**Description:** Keybinding editor: view all keybindings, search, record new binding, detect conflicts, reset.
**Acceptance Criteria:**
- All keybindings listed with command and context
- Search by command name or key combination
- "Record Keys" mode to capture new binding
- Conflict detection and resolution
- "Reset" to default per binding
- Custom bindings in `.markamp/keybindings.json`

### Task 16: Wire Command Execution Logging
**Files:** `src/core/CommandHistory.cpp`, `src/core/StructuredLogger.h`
**Description:** Log all command executions for diagnostics: command ID, timestamp, source (palette/shortcut/menu), execution time.
**Acceptance Criteria:**
- All command executions logged
- Log: command ID, source, timing
- Commands per minute metric
- Slow command warning: > 500ms
- Command failure logged with error
- Log in "Commands" output channel

### Task 17: Wire Command Palette Accessibility
**Files:** `src/ui/CommandPalette.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Command palette fully accessible: screen reader announces results, keyboard navigation, result count.
**Acceptance Criteria:**
- Screen reader: "X results for 'query'"
- Arrow key navigation with announcement
- Selected item fully described
- Category and shortcut announced
- Result count live region
- Focus trapped in palette

### Task 18: Wire Command Palette Theme
**Files:** `src/ui/CommandPalette.cpp`
**Description:** Command palette uses theme tokens: background, input field, results, selection, match highlight, category badge.
**Acceptance Criteria:**
- Background from `palette_bg` token
- Input field matches editor input styling
- Selected result uses accent background
- Match highlight uses `match_highlight` token
- Category badge styling from theme
- All 64 themes render correctly

### Task 19: Wire Extension Command Contributions
**Files:** `src/core/PluginManager.cpp`, `src/core/Command.h`
**Description:** Extensions can contribute commands via manifest. Extension commands appear in palette with extension name prefix.
**Acceptance Criteria:**
- Extension manifest `commands` contribution point
- Extension commands registered in CommandRegistry
- Prefix: "Extension Name: Command Title"
- Extension commands respect when-clauses
- Extension command icons from extension
- Uninstalling extension removes commands

### Task 20: Add Command System Tests
**Files:** `tests/unit/test_command_system.cpp`
**Description:** Test command system: registration, palette search, when-clause filtering, chord shortcuts, and history.
**Acceptance Criteria:**
- Command registration and lookup
- Fuzzy search correctness
- When-clause evaluation
- Chord shortcut detection
- History persistence
- Quick pick and input box

## Testing Requirements
- Command registration and deduplication
- Fuzzy search ranking accuracy
- When-clause context filtering
- Chord shortcut detection and timeout

## Phase Completion Criteria
- Centralized command registry with metadata
- Fuzzy search with match highlighting
- Context-aware command filtering
- Quick Open file picker
- Go to Symbol and Go to Line
- Chord shortcuts
- All tests pass
