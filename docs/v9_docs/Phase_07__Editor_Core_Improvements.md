# Phase 07: Editor Core Improvements

## Overview
EditorPanel is the largest UI component (6278 lines) with 60+ editor actions, but several key editor features are partially wired. Productivity modes are stubbed (TODO in line 533), gutter markers have a TODO (line 549), and the Find/Replace experience needs modernization. This phase brings the editor to VSCode/JetBrains parity.

## Prerequisites
- Phase 01 (EventBus hardening)
- Phase 03 (Theme tokens for editor states)

## Tasks

### Task 1: Wire Editor Productivity Modes
**Files:** `src/ui/EditorPanel.cpp`, `src/core/Config.h`
**Description:** Line 533 has `TODO(Phase 10 Batch 5): apply per-mode configs`. Implement three productivity modes: Writing (wide margins, no minimap, word wrap ON, muted diagnostics), Review (diagnostics visible, diff markers, side-by-side), and Refactor (compact, all diagnostics, no word wrap).
**Acceptance Criteria:**
- Three modes selectable via command palette and settings
- Each mode applies a defined set of editor settings
- Mode switch is instant with no flicker
- Current mode persisted per-file or globally (configurable)

### Task 2: Wire Scintilla Gutter Markers
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Line 549 has `TODO(Phase 10 Batch 2): wire to Scintilla margin markers`. Implement gutter markers for: change indicators (modified lines), diagnostics (error/warning/info icons), fold affordances (expand/collapse), and bookmarks.
**Acceptance Criteria:**
- Modified-line markers (yellow dot or bar) in gutter
- Diagnostic markers (red/yellow/blue circles) for error/warning/info
- Fold markers (+/-) for collapsible regions
- Bookmark markers (star or flag) at bookmarked lines
- All markers use theme semantic tokens

### Task 3: Modernize Find/Replace Panel
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Redesign Find/Replace into a docked, keyboard-first panel (similar to VSCode's inline find): search history dropdown, regex toggle with persistence, case-sensitive toggle, whole-word toggle, replace preview with match count, and deterministic focus traversal.
**Acceptance Criteria:**
- Find panel docks at top of editor (not dialog)
- History dropdown with last 20 searches
- Toggles: regex, case-sensitive, whole-word (state persisted)
- Match count displayed ("3 of 42")
- Tab order: search input, toggles, replace input, replace/replace-all buttons
- Escape closes panel and returns focus to editor

### Task 4: Add Go-To-Symbol Quick Navigation
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Add a "Go to Symbol" command (Cmd+Shift+O / Ctrl+Shift+O) that lists all headings, code block labels, and other structural elements in the current document. Selecting one scrolls to that location.
**Acceptance Criteria:**
- Command palette command: "Go to Symbol in Document"
- Lists all Markdown headings (H1-H6)
- Lists code block language labels
- Lists link anchors and footnotes
- Arrow keys to navigate, Enter to jump, Escape to cancel

### Task 5: Add Smart Selection Expansion
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire the `ExpandSelectionRequestEvent` and `ShrinkSelectionRequestEvent` (already declared) to implement smart selection: word -> line -> paragraph -> block -> document (and reverse). Understands Markdown structure.
**Acceptance Criteria:**
- Expand: cursor -> word -> sentence -> paragraph -> heading section -> document
- Shrink: reverse order
- Keyboard shortcuts: Ctrl+Shift+Up/Down (or configurable)
- Works with multi-cursor

### Task 6: Add Inline Diagnostics Rendering
**Files:** `src/ui/EditorPanel.cpp`, `src/core/DiagnosticsService.h`
**Description:** Render inline squiggly underlines for diagnostics: red for error, yellow for warning, blue for info. Hover shows the diagnostic message. Click gutter icon to see details.
**Acceptance Criteria:**
- Squiggly underlines rendered at diagnostic ranges
- Colors match theme error/warning/info tokens
- Hover over squiggle shows tooltip with message
- Gutter icon click shows peek panel
- Next/Previous Diagnostic commands (F8 / Shift+F8)

### Task 7: Add Quick-Fix Lightbulb
**Files:** `src/ui/EditorPanel.cpp`, `src/core/DiagnosticsService.h`
**Description:** When cursor is on a line with diagnostics, show a lightbulb icon in the gutter. Clicking it (or pressing Ctrl+.) shows available quick fixes.
**Acceptance Criteria:**
- Lightbulb appears on lines with fixable diagnostics
- Ctrl+. opens quick fix menu at cursor position
- Quick fix actions apply changes to the document
- Event: `QuickFixRequestedEvent` emitted

### Task 8: Add Peek Problem Inline Panel
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Instead of switching to the Problems panel, show an inline "peek" panel below the diagnostic line with the full message, suggested fixes, and "Go to Problems Panel" link.
**Acceptance Criteria:**
- Peek panel renders inline below the problem line
- Shows: severity icon, message, related information, fixes
- Escape closes the peek panel
- Next/Previous buttons within peek panel

### Task 9: Add Editor Minimap Improvements
**Files:** `src/ui/EditorPanel.cpp`
**Description:** The minimap toggle exists but the minimap itself needs: highlighted current viewport region, diagnostic indicators (colored bars), search match indicators, and selection highlight.
**Acceptance Criteria:**
- Minimap shows scaled-down document view
- Current viewport highlighted with semi-transparent overlay
- Diagnostic lines shown as colored horizontal bars
- Search matches shown as highlight marks
- Click on minimap scrolls editor to that position

### Task 10: Add Bracket Pair Colorization
**Files:** `src/ui/EditorPanel.cpp`, `src/core/SyntaxHighlighter.cpp`
**Description:** Matching brackets, parentheses, and braces should be colorized with distinct colors (cycling through theme accent colors). This helps with deeply nested code and Markdown.
**Acceptance Criteria:**
- Matching pairs colored with cycling palette (up to 3-4 colors)
- Colors from theme bracket_pair_1/2/3/4 tokens
- Unmatched brackets highlighted in error color
- Works with round, square, and curly brackets

### Task 11: Add Sticky Scroll (Heading Context)
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire the `ToggleStickyScrollRequestEvent` to show the current heading context at the top of the editor when scrolled past a heading. Similar to VSCode's sticky scroll.
**Acceptance Criteria:**
- Current heading(s) shown at top of editor when scrolled past
- Multiple heading levels stacked (H1, then H2, etc.)
- Clicking a sticky heading scrolls to its position
- Toggle on/off via setting and command
- Theme-aware background color

### Task 12: Add Code Folding for Markdown Sections
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Implement code folding for Markdown: headings fold their content (until next same-level heading), code blocks fold, list items fold nested items, and frontmatter folds.
**Acceptance Criteria:**
- Heading folds: H1 folds until next H1, H2 until next H2, etc.
- Code blocks: fold to single line showing language tag
- Lists: nested items fold under parent
- Frontmatter: YAML block folds to single line
- Fold All / Unfold All commands work

### Task 13: Add Auto-Pair and Smart Formatting
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire the `AutoPairBoldRequestEvent`, `AutoPairItalicRequestEvent`, `AutoPairCodeRequestEvent` to implement smart auto-pairing: typing `**` with selected text wraps it in bold, `*` wraps in italic, backtick wraps in code.
**Acceptance Criteria:**
- With selection: typing `*` wraps in italic, `**` in bold, `` ` `` in code
- Without selection: inserts pair and places cursor between
- Backspace removes both markers if empty
- Works with multi-cursor

### Task 14: Add Word Wrap Column Indicator
**Files:** `src/ui/EditorPanel.cpp`
**Description:** When word wrap column is configured (e.g., 80 chars), show a subtle vertical line at that column. Color from theme `editor_ruler` token.
**Acceptance Criteria:**
- Vertical line at configured column
- Color from theme semantic token
- Toggle via `ToggleEdgeColumnRulerRequestEvent`
- Column configurable in settings

### Task 15: Add Editor Zoom (Font Size Quick Adjust)
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire `ZoomInRequestEvent` / `ZoomOutRequestEvent` / `ZoomResetRequestEvent` to adjust editor font size by 1pt increments. Show current zoom level in status bar temporarily.
**Acceptance Criteria:**
- Cmd+= / Ctrl+= increases font size
- Cmd+- / Ctrl+- decreases font size
- Cmd+0 / Ctrl+0 resets to default
- Current zoom shown in status bar for 2 seconds after change
- Min 8pt, max 72pt

### Task 16: Add Multiple Cursor Improvements
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire `AddCursorBelowRequestEvent`, `AddCursorAboveRequestEvent`, `AddCursorNextOccurrenceRequestEvent` with visual feedback: each cursor shown with its own blinking caret, each selection shown simultaneously.
**Acceptance Criteria:**
- Alt+Click adds cursor at click location
- Cmd+D / Ctrl+D adds cursor at next occurrence of selection
- All cursors blink simultaneously
- All cursors type simultaneously
- Status bar shows cursor count

### Task 17: Add Indent Guides
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Wire `ToggleIndentGuidesRequestEvent` to show subtle vertical lines at each indentation level. Guides highlight the active scope.
**Acceptance Criteria:**
- Vertical lines at each indentation level
- Active scope guides brighter/thicker
- Color from theme `indent_guide` and `indent_guide_active` tokens
- Toggle via setting and command

### Task 18: Add Line Number Relative Mode
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Add option for relative line numbers (showing distance from cursor line, with absolute number on cursor line). Useful for keyboard-driven editing.
**Acceptance Criteria:**
- Setting: `editor.lineNumbers` = "on" | "off" | "relative"
- Relative mode: cursor line shows absolute number, others show distance
- Toggle via command palette
- Works with folded regions

### Task 19: Add Editor Performance Budgets
**Files:** `tests/unit/test_phase20_perf.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Define and test performance budgets: keystroke-to-screen < 16ms, scroll latency < 8ms, large file (10k+ lines) typing lag < 32ms.
**Acceptance Criteria:**
- Budget: keypress latency < 16ms on standard document
- Budget: scroll frame time < 8ms
- Budget: 10k line document typing lag < 32ms
- Test framework for measuring these metrics

### Task 20: Add Editor State Serialization for Tab Restore
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/TabBar.cpp`
**Description:** When switching tabs or restoring a session, the editor should restore: cursor position, scroll position, fold state, selection, and find/replace state.
**Acceptance Criteria:**
- Tab switch preserves cursor, scroll, selection
- Session restore (after restart) preserves cursor and scroll
- Fold state preserved across tab switches
- Find/replace state preserved (query, toggles)

## Testing Requirements
- All 60+ editor action events verified
- Find/Replace: search, replace, replace-all, regex, case-sensitive
- Diagnostics: inline rendering, gutter markers, peek panel
- Performance: keystroke latency measured and within budget
- Multi-cursor: add, remove, type, select

## Phase Completion Criteria
- Editor productivity modes functional and switchable
- Gutter markers for changes, diagnostics, folds, bookmarks
- Modern Find/Replace with history and toggles
- Inline diagnostics with quick-fix lightbulb
- All editor actions wired and tested
- Performance within budgets
- All tests pass
