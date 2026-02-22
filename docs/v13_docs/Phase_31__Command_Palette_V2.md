# Phase 31: Command Palette V2

## Overview

Overhaul the existing `CommandPalette` (wxDialog-based) and `CommandPaletteModel` into a
production-grade, multi-mode command palette that rivals VSCode's Ctrl+Shift+P experience.
The current implementation has basic fuzzy matching, mode detection via prefix characters,
MRU history, and category headers. This phase enhances scoring fidelity, adds file/symbol/
line/settings picker modes with rich rendering, integrates virtual scrolling for large result
sets, adds slide-down animation, responsive width, and wires extension-contributed commands
through `CommandRegistry`.

## Existing Code References

| Component | File | Status |
|-----------|------|--------|
| CommandPalette | `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` | Has modes, fuzzy search, MRU, category headers |
| CommandPaletteModel | `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h` | Rich metadata, alias support, pinned, preview |
| CommandRegistry | `/Users/ryanrentfro/code/markamp/src/core/CommandRegistry.h` | Full registry with fuzzy search, usage tracking, when-clause |
| PaletteMode enum | `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` | kCommands, kQuickOpen, kGoToSymbol, kGoToLine |
| ThemeEngine | `/Users/ryanrentfro/code/markamp/src/core/ThemeEngine.h` | Cached wxBrush/wxPen/wxFont |
| EventBus | `/Users/ryanrentfro/code/markamp/src/core/EventBus.h` | Backbone for inter-component communication |

## Dependencies

- Phase 32 (Go-To System) depends on mode integrations built here.
- Phase 33 (Outline Panel) provides symbol data consumed by the @ mode.
- Phase 35 (Quick Open) extends the file-picker mode defined here.

---

## Tasks

### Task 1: Enhanced Fuzzy Scoring Engine

**Title:** Replace naive FuzzyScore with multi-signal scoring algorithm

**Description:** The current `FuzzyScore` static method in `CommandPalette` and the parallel
`fuzzy_score` in `CommandPaletteModel` use simple substring/character matching. Replace both
with a unified scoring engine that considers: consecutive character bonus, word-boundary bonus,
camelCase transitions, acronym matching, exact prefix match boost, and penalty for gaps. This
engine will be shared across all palette modes.

**Implementation Details:**
- Create `FuzzyScorer` class in `/Users/ryanrentfro/code/markamp/src/core/FuzzyScorer.h`
  and `.cpp` with a static `score(query, candidate) -> FuzzyMatchResult` method.
- `FuzzyMatchResult` struct: `int score`, `std::vector<int> match_positions` (for highlighting),
  `bool is_exact_match`.
- Scoring weights: consecutive chars = 5, word boundary = 10, camelCase transition = 8,
  exact prefix = 20, gap penalty = -3, unmatched tail = -1.
- Remove `CommandPalette::FuzzyScore` and `CommandPaletteModel::fuzzy_score`.
- Route both `CommandPalette::ApplyFilter` and `CommandPaletteModel::search` through the
  new `FuzzyScorer`.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/FuzzyScorer.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/FuzzyScorer.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (remove FuzzyScore)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (use FuzzyScorer)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h` (remove fuzzy_score)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp` (use FuzzyScorer)
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (add new sources)

**Acceptance Criteria:**
- Query "fdo" matches "Format Document" with a score above 50.
- Query "TogWW" matches "Toggle Word Wrap" via camelCase transition scoring.
- `FuzzyMatchResult::match_positions` returns correct character indices for highlight rendering.
- All existing `CommandPaletteModel` unit tests pass with the new scorer.
- Scoring is deterministic and benchmark-stable under 1ms for 500 candidates.

**Dependencies:** None (foundational task).

---

### Task 2: Frecency Scoring for Recent Files

**Title:** Implement frecency algorithm for MRU file list ordering

**Description:** Replace simple MRU ordering with a frecency algorithm that combines
frequency of access with recency decay. Files opened recently AND frequently rank highest.
This scoring is used across file picker mode (Cmd+P) and the recently-used commands section.

**Implementation Details:**
- Create `FrecencyTracker` in `/Users/ryanrentfro/code/markamp/src/core/FrecencyTracker.h`.
- Each entry stores `std::string key`, `int access_count`, `std::vector<int64_t> access_timestamps`
  (last 10 timestamps as epoch seconds).
- Score formula: `frecency = sum(weight(age(ts)) * access_count_factor)` where
  `weight` decays exponentially: <4h = 100, <1d = 80, <3d = 60, <7d = 40, <30d = 20, else 10.
- Expose `void record_access(key)`, `double score(key)`, `vector<string> ranked_keys(limit)`.
- Serialize/deserialize to a simple binary or TSV format stored in config directory.
- Wire into `CommandPaletteModel::mru_history()` and the file picker mode.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/FrecencyTracker.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/FrecencyTracker.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.h` (use FrecencyTracker)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp`

**Acceptance Criteria:**
- A file opened 5 times in the last hour scores higher than one opened once yesterday.
- Frecency scores survive application restart via persistence.
- `ranked_keys(10)` returns the correct top-10 ordering.
- Score computation completes in under 0.1ms for 1000 entries.

**Dependencies:** None.

---

### Task 3: File Picker Mode with Icons and Path Preview

**Title:** Implement rich file picker mode (Cmd+P) with file type icons and path display

**Description:** When the palette is in `kQuickOpen` mode (no prefix), display a file list
sourced from the workspace file tree. Each row shows: a file type icon (derived from extension),
the filename in bold, and the relative path in dimmed text below. Arrow-key selection shows
a path preview breadcrumb at the bottom of the palette.

**Implementation Details:**
- Add `PaletteMode::kFilePicker` or reuse `kQuickOpen` with enhanced rendering.
- Source file list from `WorkspaceService::find_files("*")` via EventBus request/response
  or direct service call if available.
- Map file extensions to icon identifiers: `.cpp/.h` = code icon, `.md` = markdown icon,
  `.json/.yaml` = config icon, etc. Store in a `FileIconResolver` utility.
- Override `DrawListItem` in `PaletteListBox` to render: [icon] [bold filename] [dim path].
- On selection change (arrow keys), update a `wxStaticText` at the bottom of the palette
  showing the full path.
- Fuzzy match against both filename and path segments.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/FileIconResolver.h`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (add path preview label)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (file picker rendering)

**Acceptance Criteria:**
- Cmd+P opens palette in file picker mode with all workspace files listed.
- Each file shows an appropriate icon based on its extension.
- The relative path appears dimmed beneath the filename.
- Arrow-key navigation updates the path preview at the bottom.
- Fuzzy search "EdPan" matches "EditorPanel.cpp" in `src/ui/`.

**Dependencies:** Task 1 (FuzzyScorer).

---

### Task 4: Symbol Picker Mode with Type Icons

**Title:** Implement @ symbol picker mode with symbol type icons

**Description:** When the user types `@` in the palette, switch to symbol mode. Query the
`OutlineService` for the current document's headings and display them with heading-level
icons (H1-H6). For code files, display function/class/variable symbols from a symbol
provider interface.

**Implementation Details:**
- Define `ISymbolProvider` interface with `get_symbols(document_id) -> vector<SymbolInfo>`.
- `SymbolInfo` struct: `string name`, `SymbolKind kind` (enum: Function, Class, Variable,
  Heading, Namespace, Interface), `int line`, `string container_name`.
- Default `MarkdownSymbolProvider` wraps `OutlineService::get_flat_outline` and maps
  `OutlineEntry` to `SymbolInfo` with `kind = Heading`.
- Render each row: [kind icon] [symbol name] [dim container/scope].
- Selecting a symbol fires `OutlineScrollToEvent` or equivalent to navigate the editor.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/core/ISymbolProvider.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/MarkdownSymbolProvider.h`
- New: `/Users/ryanrentfro/code/markamp/src/core/MarkdownSymbolProvider.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (@ mode rendering)

**Acceptance Criteria:**
- Typing `@` filters the palette to document symbols.
- Heading symbols show H1-H6 level indicators.
- Selecting a heading scrolls the editor to that heading's line.
- Symbol list is sorted by document position by default.
- Empty query in @ mode shows all symbols.

**Dependencies:** Task 1. Depends on `OutlineService` from core.

---

### Task 5: Line Picker Mode

**Title:** Implement : line number picker mode

**Description:** When the user types `:` followed by a number, the palette switches to
line-picker mode. Show a preview of the target line content. Pressing Enter navigates
the editor to that line.

**Implementation Details:**
- Detect `:` prefix in `DetectModeFromPrefix` and set `current_mode_` to `kGoToLine`.
- Parse the numeric portion after `:`. Show "Go to line N" as the single result item.
- If the editor has content, show a preview of line N (and surrounding +/-2 lines) in
  a dimmed preview area below the input.
- On Enter, publish a `GoToLineEvent{ int line }` via EventBus.
- Handle edge cases: non-numeric input shows "Invalid line number", line exceeding
  document length shows "Line N exceeds document (M lines)".

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (line mode logic)
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (add GoToLineEvent if missing)

**Acceptance Criteria:**
- Typing `:42` shows "Go to line 42" with a preview of that line's content.
- Enter navigates the editor to line 42.
- `:abc` shows "Invalid line number" in the result area.
- `:99999` on a 100-line document shows an appropriate warning.
- Esc from line mode returns to command mode.

**Dependencies:** None.

---

### Task 6: Settings Search Mode

**Title:** Add settings search mode to command palette

**Description:** Add a new palette mode triggered by a `?` prefix (or accessible via
"Preferences: Open Settings" command). Searches the `SettingsCatalog` and displays matching
settings with their current value. Selecting a setting opens `SettingsDialog` scrolled
to that setting.

**Implementation Details:**
- Add `PaletteMode::kSettingsSearch` to the enum.
- Detect `?` prefix in `DetectModeFromPrefix`.
- Query `SettingsCatalog::search(query)` to get matching `CatalogEntry` pointers.
- Render each row: [setting label] [dim: current value] [dim: setting_id].
- On selection, call `SettingsDialog::OpenToSetting(setting_id)`.
- Placeholder text in input changes to "Search settings..." when in this mode.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (add kSettingsSearch, catalog ptr)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (settings mode rendering)

**Acceptance Criteria:**
- Typing `?font` shows "Font Size", "Font Family", and related settings.
- Each result displays the current configuration value.
- Selecting a result opens the settings dialog scrolled to that setting.
- Empty `?` query shows commonly used settings.

**Dependencies:** SettingsCatalog must be wired to the palette.

---

### Task 7: Command Categories with Section Headers

**Title:** Enhance category header rendering with collapsible groups

**Description:** The current implementation has `DisplayItem::Header` but renders them
minimally. Enhance headers to show: category name in bold with an item count badge,
a thin separator line above, and keyboard-navigable collapse/expand behavior.

**Implementation Details:**
- Modify `DrawListItem` for `ItemType::Header` to render: horizontal separator line,
  bold category name, "(N)" count badge in dimmed text, collapse chevron icon.
- Track collapsed categories in a `std::set<std::string> collapsed_categories_`.
- Clicking a header or pressing Enter on a header toggles collapse.
- When collapsed, skip rendering child commands in that category.
- Update `ApplyFilter` to group results by category and insert headers.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (collapsed_categories_)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (header rendering, collapse)

**Acceptance Criteria:**
- Categories show bold header text with item count.
- A separator line appears above each category header.
- Clicking a header collapses/expands that category.
- Collapsed state persists during the current palette session.
- Arrow keys skip collapsed items.

**Dependencies:** None.

---

### Task 8: Command Icons in Palette Results

**Title:** Render command icons alongside command labels

**Description:** Each `CommandMetadata` has a potential icon field (from `CommandEntry::icon`
in the registry). Render a 16x16 icon to the left of each command label. Fall back to
a category-based default icon when no specific icon is set.

**Implementation Details:**
- Define an `IconProvider` that maps icon identifiers to `wxBitmap` or `wxIcon` resources.
- In `DrawListItem` for `ItemType::Command`, draw the icon at the left margin before
  the command label text.
- Default icons per category: File = document icon, Edit = pencil, View = eye,
  Navigation = compass, Terminal = terminal, Extensions = puzzle piece.
- Icons should be resolution-aware (1x and 2x for Retina/HiDPI).

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.h`
- New: `/Users/ryanrentfro/code/markamp/src/ui/IconProvider.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (icon rendering)

**Acceptance Criteria:**
- Each command in the palette shows an icon to the left of its label.
- Commands with specific icon IDs show the correct icon.
- Commands without icons show a category-based default.
- Icons render correctly on both standard and HiDPI displays.

**Dependencies:** None.

---

### Task 9: Keybinding Display in Palette Results

**Title:** Show keyboard shortcut hints aligned to the right of each command row

**Description:** The existing `PaletteCommand::shortcut` and `CommandMetadata::shortcut`
fields store shortcut display strings. Render these right-aligned in a dimmed, monospace
font on each command row to help users learn shortcuts.

**Implementation Details:**
- In `DrawListItem` for commands, measure the shortcut text width and draw it right-aligned
  with a monospace font from `ThemeEngine`.
- Use a dimmed foreground color (50% opacity or a theme token like `palette.shortcutFg`).
- If the shortcut text would overlap the command label (very narrow palette), truncate
  the label with ellipsis instead.
- For commands without shortcuts, leave the right side empty.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (shortcut rendering)

**Acceptance Criteria:**
- Commands with shortcuts show the shortcut right-aligned (e.g., "Cmd+S").
- Shortcut text uses a monospace font and dimmed color.
- Label text is truncated with "..." when it would collide with the shortcut.
- Commands without shortcuts render cleanly without extra space.

**Dependencies:** None.

---

### Task 10: Recently Used Commands Section

**Title:** Show a "Recently Used" section at the top when query is empty

**Description:** When the palette opens with an empty query, show a "Recently Used" section
at the top containing the last 5-8 commands the user executed, followed by all commands
grouped by category. This uses the `CommandPaletteModel::mru_history()` or
`CommandRegistry::get_recently_used()`.

**Implementation Details:**
- In `ApplyFilter`, when `current_filter_` is empty and mode is `kCommands`:
  1. Insert a "Recently Used" header.
  2. Insert up to 8 MRU command entries (looked up by command_id from mru_history_).
  3. Insert a separator.
  4. Insert all commands grouped by category.
- MRU entries should be visually distinct: show a clock icon and slightly different
  background shade.
- If MRU is empty (first launch), skip the section entirely.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (MRU section in ApplyFilter)

**Acceptance Criteria:**
- Empty-query palette shows "Recently Used" section with last 8 commands.
- MRU section appears above the category-grouped command list.
- Executing a command adds it to the MRU section on next palette open.
- If no commands have been used, the MRU section does not appear.
- MRU persists across application restarts (via CommandRegistry history).

**Dependencies:** Task 2 (FrecencyTracker for ordering), CommandRegistry integration.

---

### Task 11: Palette Slide-Down Animation

**Title:** Animate the palette opening with a slide-down effect

**Description:** Instead of abruptly appearing, the palette should slide down from the
top of the window over ~150ms with an ease-out curve, similar to VSCode.

**Implementation Details:**
- In `ShowPalette()`, instead of calling `ShowModal()` directly, position the dialog
  above the visible area and use a `wxTimer` to animate its Y position downward.
- Animation: start at Y = -palette_height, end at Y = target_y (centered at ~15% from top).
- Duration: 150ms with ease-out (cubic: t * (2 - t)).
- Use `SetPosition` on each timer tick (16ms interval for ~60fps).
- On close, reverse the animation (slide up, then `EndModal`).
- Provide a `config` setting `"palette.animationEnabled"` (default: true) to disable.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (animation timer, state)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (animation logic)

**Acceptance Criteria:**
- Palette slides down from top over ~150ms when opened.
- Palette slides up when dismissed.
- Animation is smooth at 60fps on standard hardware.
- Setting `palette.animationEnabled = false` disables animation (instant show).
- No visual artifacts during animation.

**Dependencies:** None.

---

### Task 12: Responsive Palette Width

**Title:** Make palette width responsive to the parent window size

**Description:** The palette should be proportionally sized: 60% of the window width on
windows wider than 1200px, 80% on windows 800-1200px, and 95% on windows narrower than
800px. Minimum width: 400px. Maximum width: 800px.

**Implementation Details:**
- In `ShowPalette()`, compute `palette_width = clamp(parent_width * ratio, 400, 800)`
  where ratio depends on window width breakpoints.
- Center the palette horizontally within the parent window.
- On parent window resize (if the palette is open), recalculate and reposition.
- Store calculated dimensions to avoid recalculation on every frame.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (responsive sizing)

**Acceptance Criteria:**
- Palette width is 60% of window on wide screens, clamped to 800px max.
- Palette width increases to 80% on medium windows.
- Palette width is 95% on narrow windows, clamped to 400px min.
- Palette is always horizontally centered.
- Resizing the parent window while palette is open updates palette dimensions.

**Dependencies:** None.

---

### Task 13: Virtual Scrolling for Large Result Sets

**Title:** Implement virtual scrolling in PaletteListBox for 500+ results

**Description:** The current `wxVListBox` (PaletteListBox) renders all items. For large
result sets (500+ commands from extensions), implement virtual scrolling that only renders
visible items plus a small buffer.

**Implementation Details:**
- `wxVListBox` already supports virtual scrolling natively (it only calls `OnDrawItem`
  for visible items). Verify this is correctly configured.
- Add a max visible results limit: `kMaxDisplayResults = 200` with a "Show more..."
  item at the bottom that loads the next 200.
- Track total result count vs displayed count.
- Show a count indicator: "Showing 200 of 547 results" at the bottom.
- Ensure smooth scrolling behavior with arrow keys and mouse wheel.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (max results, pagination)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (pagination logic)

**Acceptance Criteria:**
- With 500+ results, only visible items are rendered (verified via draw call count).
- "Showing N of M results" indicator appears when results exceed the limit.
- "Show more..." item at the bottom loads additional results.
- Scrolling is smooth and responsive with 1000+ results.
- Memory usage does not scale linearly with hidden items.

**Dependencies:** None.

---

### Task 14: Extension-Contributed Commands Integration

**Title:** Wire extension-contributed commands from CommandRegistry into the palette

**Description:** Currently `CommandPalette` maintains its own `all_commands_` vector
separate from `CommandRegistry`. Unify these so that all commands registered via the
extension API (`CommandRegistry::register_command`) automatically appear in the palette.

**Implementation Details:**
- On `ShowPalette()`, query `CommandRegistry::all_commands()` filtered by
  `active_commands(context_key_service)` to get context-appropriate commands.
- Merge with any palette-specific commands (if any remain).
- Extension-contributed commands show their source in dimmed text: "[ext: extension-name]".
- When-clause evaluation uses the current `ContextKeyService` state.
- Listen for `CommandRegisteredEvent` to invalidate the cached command list.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (CommandRegistry*, ContextKeyService*)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (unified command sourcing)

**Acceptance Criteria:**
- Commands registered via `CommandRegistry` appear in the palette.
- Extension-contributed commands show their source attribution.
- When-clause filters out commands not applicable to the current context.
- Unregistering a command removes it from the palette on next open.
- Built-in commands and extension commands sort/rank uniformly.

**Dependencies:** Task 1 (unified FuzzyScorer).

---

### Task 15: Fuzzy Match Highlighting in Results

**Title:** Highlight matched characters in palette result text

**Description:** Use the `match_positions` from `FuzzyMatchResult` to render matched
characters in a highlight color (bold + accent color) within the command label text.

**Implementation Details:**
- In `DrawListItem`, iterate over the label characters. For each character position
  in `match_positions`, render it with bold font and the theme's accent color.
- Non-matched characters render in the normal foreground color.
- Use `wxDC::DrawText` with alternating fonts/colors for matched/unmatched spans.
- Optimize by pre-computing text spans (contiguous matched and unmatched ranges)
  to minimize DrawText calls.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (highlight rendering)

**Acceptance Criteria:**
- Matched characters are rendered in bold with the accent color.
- Non-matched characters render in the default foreground.
- Highlighting updates in real-time as the user types.
- No visual glitches with mixed-width characters or long labels.
- Performance: rendering 50 visible items with highlighting completes in under 5ms.

**Dependencies:** Task 1 (FuzzyScorer provides match_positions).

---

### Task 16: Palette Input Placeholder and Mode Indicators

**Title:** Dynamic placeholder text and mode indicator badge

**Description:** The text input at the top of the palette should show context-appropriate
placeholder text and a mode indicator badge to the left of the input field.

**Implementation Details:**
- Placeholder text by mode: Commands = "Type a command name...",
  FilePicker = "Type a file name...", GoToSymbol = "Type a symbol name (@)...",
  GoToLine = "Type a line number (:)...", Settings = "Search settings (?)...".
- Mode indicator: a small colored badge/label to the left of the input showing the
  current mode icon (> for commands, @ for symbols, : for line, ? for settings).
- When the user clears the prefix character, revert to default (commands) mode.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (mode indicator widget)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (placeholder, indicator)

**Acceptance Criteria:**
- Placeholder text changes when mode changes.
- Mode indicator badge shows the correct icon/character for each mode.
- Clearing the prefix reverts to command mode with appropriate placeholder.
- Mode indicator uses theme-appropriate colors.

**Dependencies:** Tasks 3, 4, 5, 6 (all modes must exist).

---

### Task 17: Keyboard Navigation Enhancements

**Title:** Full keyboard navigation with Ctrl+N/P, Tab completion, and Escape behavior

**Description:** Enhance keyboard handling for power-user workflows: Ctrl+N/Ctrl+P for
next/previous item (in addition to arrow keys), Tab to accept the top suggestion and
refine, Escape to clear filter first then close palette on second press.

**Implementation Details:**
- In `OnKeyDown`, add handlers for Ctrl+N (move selection down), Ctrl+P (move selection up).
- Tab key: if there is a top result, insert its label text into the input (auto-complete).
- First Escape press: if filter is non-empty, clear it (reset to empty query).
  Second Escape press: close the palette.
- Shift+Enter: execute the command but keep the palette open (for batch operations).
- Home/End: jump to first/last result.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (OnKeyDown enhancements)

**Acceptance Criteria:**
- Ctrl+N moves selection down, Ctrl+P moves selection up.
- Tab inserts the top result's text into the input field.
- First Escape clears the filter; second Escape closes the palette.
- Shift+Enter executes the selected command without closing the palette.
- Home/End jump to first/last result.

**Dependencies:** None.

---

### Task 18: Palette Theme Integration

**Title:** Full ThemeEngine integration for all palette visual elements

**Description:** Apply theme tokens consistently across all palette rendering: background,
input field, result items, headers, separators, selection highlight, scrollbar, shadow,
and mode indicator.

**Implementation Details:**
- Define theme tokens: `palette.background`, `palette.inputBackground`, `palette.inputBorder`,
  `palette.itemBackground`, `palette.itemHoverBackground`, `palette.itemActiveBackground`,
  `palette.headerForeground`, `palette.separatorColor`, `palette.shadowColor`,
  `palette.matchHighlight`, `palette.shortcutForeground`.
- In `ApplyTheme()`, read all tokens from `ThemeEngine` and cache as `wxBrush`/`wxPen`.
- Subscribe to `ThemeChangedEvent` to refresh on theme switch.
- Apply a subtle drop shadow below the palette (platform-specific: NSView shadow on
  macOS, transparent overlay on others).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (full theme application)
- Modified: theme definition files in `/Users/ryanrentfro/code/markamp/themes/` (add palette tokens)

**Acceptance Criteria:**
- All palette visual elements use theme tokens.
- Switching themes while the palette is open updates all colors immediately.
- Drop shadow appears on macOS.
- All built-in themes include palette token definitions.
- High contrast theme maintains WCAG AA contrast ratios.

**Dependencies:** None.

---

### Task 19: Destructive Command Confirmation

**Title:** Show confirmation prompt for destructive commands before execution

**Description:** Leverage the `CommandMetadata::is_destructive` flag. When the user selects
a destructive command, show a confirmation sub-dialog within the palette instead of executing
immediately.

**Implementation Details:**
- When `ExecuteSelected()` detects `is_destructive == true`, replace the result list with
  a confirmation view: "[!] This action is destructive. Press Enter to confirm, Escape to cancel."
- Show the command's `preview_text` (from `CommandPaletteModel::preview_text`) as a description.
- Enter confirms and executes. Escape returns to the normal palette view.
- Destructive commands show a warning icon and orange-tinted background in the result list.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (confirmation flow)

**Acceptance Criteria:**
- Selecting a destructive command shows a confirmation prompt.
- Enter confirms execution; Escape cancels.
- Preview text for the destructive command is displayed.
- Destructive commands are visually distinguished in the result list.
- Non-destructive commands execute immediately as before.

**Dependencies:** None.

---

### Task 20: Palette Open/Close Lifecycle Events

**Title:** Emit EventBus events when palette opens and closes

**Description:** Other components (e.g., extensions, analytics, status bar) need to know
when the palette is active. Emit `PaletteOpenedEvent` and `PaletteClosedEvent`.

**Implementation Details:**
- Declare events in Events.h:
  `MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteOpenedEvent) PaletteMode mode; MARKAMP_DECLARE_EVENT_END;`
  `MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaletteClosedEvent) std::string selected_command; bool cancelled; MARKAMP_DECLARE_EVENT_END;`
- Publish `PaletteOpenedEvent` in `ShowPalette()`.
- Publish `PaletteClosedEvent` in `EndModal` or equivalent close path.
- Extensions can subscribe to these events to contribute dynamic commands.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/core/Events.h` (new events)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (emit events)

**Acceptance Criteria:**
- `PaletteOpenedEvent` fires when the palette opens with the correct mode.
- `PaletteClosedEvent` fires when the palette closes, indicating what was selected or if cancelled.
- Subscribers receive events correctly.
- No event is emitted if the palette was already open (idempotent).

**Dependencies:** None.

---

### Task 21: Pin and Unpin Commands

**Title:** Allow users to pin commands to the top of the palette

**Description:** Wire the `CommandPaletteModel::pin_command` and `unpin_command` to a
UI gesture. Right-clicking a command in the palette shows a context menu with "Pin" or
"Unpin". Pinned commands always appear at the top in a "Pinned" section.

**Implementation Details:**
- Add right-click handler on `PaletteListBox` that shows a context menu.
- Menu items: "Pin to top" / "Unpin" depending on current pin state.
- In `ApplyFilter`, pinned commands form the first section (before MRU and categories).
- Pinned state persists via `CommandPaletteModel` serialization.
- Show a pin icon next to pinned commands.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (context menu, pin section)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPaletteModel.cpp` (persistence)

**Acceptance Criteria:**
- Right-clicking a command shows "Pin to top" option.
- Pinned commands appear in a "Pinned" section at the top.
- Pinned state survives application restart.
- Right-clicking a pinned command shows "Unpin" option.
- Pin icon is visible next to pinned commands.

**Dependencies:** None.

---

### Task 22: Multi-Mode Quick Switch

**Title:** Enable rapid mode switching with keyboard shortcuts within the palette

**Description:** While the palette is open, the user should be able to switch modes
without closing and reopening. Ctrl+P = files, Ctrl+Shift+O = symbols, Ctrl+G = line,
Ctrl+, = settings.

**Implementation Details:**
- In `OnKeyDown`, detect mode-switch shortcuts:
  - Ctrl+P: switch to file picker, clear input, set prefix context.
  - Ctrl+Shift+O: switch to symbol mode, insert `@` prefix.
  - Ctrl+G: switch to line mode, insert `:` prefix.
  - Ctrl+,: switch to settings mode, insert `?` prefix.
- On mode switch, clear the result list, update placeholder, and trigger new filter.
- Animate a brief mode-change indicator (flash the mode badge).

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (mode switch shortcuts)

**Acceptance Criteria:**
- Ctrl+P while palette is open switches to file picker mode.
- Ctrl+Shift+O switches to symbol mode and inserts `@`.
- Ctrl+G switches to line mode and inserts `:`.
- The input is cleared and placeholder updates on mode switch.
- Mode indicator badge updates to reflect the new mode.

**Dependencies:** Tasks 3, 4, 5, 6 (all modes implemented).

---

### Task 23: Empty State and No Results Handling

**Title:** Show helpful empty-state messages for each palette mode

**Description:** When no results match the query, show a mode-appropriate empty state
message instead of a blank list.

**Implementation Details:**
- For each mode, define an empty-state message:
  - Commands: "No commands matching 'X'. Try a different search term."
  - Files: "No files matching 'X' in workspace."
  - Symbols: "No symbols found in the current document."
  - Line: "Enter a valid line number."
  - Settings: "No settings matching 'X'."
- Render the message centered in the result area with a dimmed foreground.
- Show a "Clear filter" link/button that resets the input.
- If the mode source is empty (e.g., no workspace open for files), show a
  different message: "Open a workspace to search files."

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (empty state rendering)

**Acceptance Criteria:**
- Each mode shows an appropriate empty-state message when no results match.
- The message includes the search query for context.
- A "clear filter" action is available.
- Source-empty states (no workspace, no document) show appropriate guidance.

**Dependencies:** Tasks 3, 4, 5, 6.

---

### Task 24: Palette Result Count Badge

**Title:** Show total result count in the palette header area

**Description:** Display a result count badge near the input field showing how many
results match the current query, e.g., "47 results" or "3 of 47".

**Implementation Details:**
- Add a `wxStaticText` widget to the right of the input field (or inside the input's
  right margin) showing "N results" or "N of M" when paginated.
- Update the count on every `ApplyFilter` call.
- For modes with known totals (files, commands), show exact counts.
- Use dimmed text color from the theme.

**Files Affected:**
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.h` (count label widget)
- Modified: `/Users/ryanrentfro/code/markamp/src/ui/CommandPalette.cpp` (count update)

**Acceptance Criteria:**
- Result count badge appears near the input field.
- Count updates in real-time as the user types.
- Shows "N results" for unpaginated results, "N of M" for paginated.
- Shows "No results" when the filter matches nothing.
- Uses theme-appropriate dimmed text styling.

**Dependencies:** None.

---

### Task 25: Unit Tests for Command Palette V2

**Title:** Comprehensive test suite for all Command Palette V2 features

**Description:** Write Catch2 unit tests covering the new FuzzyScorer, FrecencyTracker,
enhanced CommandPaletteModel behavior, mode switching logic, and empty state handling.

**Implementation Details:**
- Test file: `/Users/ryanrentfro/code/markamp/tests/unit/test_command_palette_v2.cpp`
- Test sections:
  - FuzzyScorer: exact match, prefix match, camelCase, acronym, gap penalty, no match.
  - FrecencyTracker: access recording, score decay, persistence round-trip, ranked ordering.
  - CommandPaletteModel: search with aliases, MRU boost, pinned-first ordering,
    destructive command detection.
  - Mode detection: `>`, `@`, `:`, `?` prefix parsing.
  - Empty state: each mode produces correct empty message.
  - Pagination: correct "show more" behavior at 200-item threshold.
- Add test target to CMakeLists.txt.

**Files Affected:**
- New: `/Users/ryanrentfro/code/markamp/tests/unit/test_command_palette_v2.cpp`
- Modified: `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (add test target)

**Acceptance Criteria:**
- All test sections pass under `ctest --output-on-failure`.
- FuzzyScorer tests cover all scoring signals (consecutive, boundary, camelCase, gap).
- FrecencyTracker tests verify decay over time using mocked timestamps.
- Mode detection tests verify all prefix characters.
- At least 30 test cases across all sections.

**Dependencies:** Tasks 1, 2 (FuzzyScorer and FrecencyTracker must be implemented).
