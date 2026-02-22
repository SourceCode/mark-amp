# Phase 46 -- Diff Editor and Merge Tool

## Objective

Build a production-quality diff editor and 3-way merge tool that rivals VSCode's diff experience. Extend the existing DiffPanel stub and DiffEngine into a fully functional side-by-side and inline diff viewer with gutter indicators, change navigation, individual change accept/reject, minimap decorations, and a complete 3-way merge workflow with conflict resolution. Integrate with the existing git/repository services for SCM panel usage.

## Prerequisites

- Phase 45 complete (Drag and Drop System)
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h` -- existing stub with show_diff(), view mode, navigation
- `/Users/ryanrentfro/code/markamp/src/core/DiffEngine.h` -- Myers diff algorithm, line/hunk-based diffing
- `/Users/ryanrentfro/code/markamp/src/core/BlockDiffEngine.h` -- block-level diff computation
- `/Users/ryanrentfro/code/markamp/src/core/DiffTypes.h` -- DiffResult, DiffHunk, DiffViewMode types
- `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.h` -- git integration
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.h` -- split pane for side-by-side

## VSCode Reference Behavior

- Side-by-side: two synchronized editors, deleted lines on left with red background, added lines on right with green background
- Inline: single editor with deleted lines (red, strikethrough) above added lines (green)
- Gutter indicators: red minus (-) for deleted, green plus (+) for added, blue tilde (~) for modified
- Change navigation: Next/Previous change buttons in toolbar, Cmd+Alt+F5/F3
- Individual change: accept/reject buttons on hover over each change hunk
- Word-level highlighting: within changed lines, individual changed words are highlighted more strongly
- Minimap: diff decorations on scrollbar (green added, red deleted)
- Ignore whitespace toggle: re-computes diff ignoring whitespace differences
- 3-way merge: base (center), theirs (left), yours (right), result (bottom)

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h` | Modify (major rewrite) |
| `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` | Modify (major rewrite) |
| `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DiffGutter.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/DiffGutter.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/WordDiffEngine.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/WordDiffEngine.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/MergeEngine.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/core/MergeEngine.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_diff_editor.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Implement Word-Level Diff Engine

**Description:** Create a word-level diff engine that identifies changed words within changed lines, enabling fine-grained highlighting beyond line-level diffs.

**Implementation Details:** WordDiffEngine operates on pairs of changed lines (one from old, one from new). It tokenizes each line into words (split on whitespace and punctuation boundaries), then runs a simplified diff algorithm (LCS-based) on the word sequences. Output: `WordDiffResult { vector<WordSpan> old_spans; vector<WordSpan> new_spans; }` where `WordSpan { int start_col; int end_col; WordChangeType type; // Unchanged, Modified, Added, Deleted }`. Modified words are highlighted with stronger background color than the line-level highlight.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/WordDiffEngine.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/WordDiffEngine.cpp` (create)

**Acceptance Criteria:**
- Word diff correctly identifies changed words within changed lines
- Tokenization handles whitespace, punctuation, and identifiers
- LCS algorithm produces minimal word-level edits
- Performance: word diff of 200-char lines completes in under 1ms
- Unit test with known word changes validates span positions

**Dependencies:** None

---

### Task 02 -- Implement DiffGutter for Gutter Indicators

**Description:** Create a diff gutter component that renders colored indicators (plus, minus, tilde) in the editor's left margin for each changed line.

**Implementation Details:** DiffGutter extends ThemeAwareWindow. Width: 16px. Renders one indicator per line in the diff: green "+" for added lines, red "-" for deleted lines, blue "~" for modified lines. Unchanged lines have no indicator. The gutter is synchronized with the editor's scroll position. Clicking on a gutter indicator scrolls to center that change and selects it. The gutter highlights the "current change" (the one being navigated) with a stronger indicator.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffGutter.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/DiffGutter.cpp` (create)

**Acceptance Criteria:**
- Green + renders for added lines
- Red - renders for deleted lines
- Blue ~ renders for modified lines
- Gutter scrolls in sync with editor
- Clicking indicator navigates to that change

**Dependencies:** None

---

### Task 03 -- Rewrite DiffPanel as Container for View Modes

**Description:** Rewrite the existing DiffPanel stub as a container that hosts either a SideBySideDiffView or an InlineDiffView, switchable at runtime.

**Implementation Details:** DiffPanel becomes a wxPanel container with: (1) toolbar at top (view mode toggle, navigation buttons, whitespace toggle), (2) content area hosting the active view. `set_view_mode()` switches between views by destroying the current and creating the replacement. The toolbar contains: SegmentedControl (Phase 41) for Inline/Side-by-Side mode, Next/Previous Change buttons (IconButton), "Accept All Theirs" / "Accept All Mine" buttons, and a "Ignore Whitespace" toggle (ThemedCheckbox). The DiffPanel receives a DiffResult and passes it to the active view.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- DiffPanel switches between inline and side-by-side views
- Toolbar renders with mode toggle, navigation, and whitespace toggle
- View switch preserves scroll position (approximately)
- DiffResult is passed to the active view on set
- Existing show_diff() API is preserved

**Dependencies:** Phase 41 Task 15 (SegmentedControl), Phase 41 Task 03 (IconButton)

---

### Task 04 -- Implement Side-by-Side Diff View

**Description:** Create a side-by-side diff editor with two synchronized editor panels, showing the old version on the left and the new version on the right, with colored backgrounds for changed lines.

**Implementation Details:** SideBySideDiffView is a wxPanel containing two EditorPanel instances (read-only) separated by a SplitterBar. Left editor shows old content with deleted lines highlighted in red (`rgba(255, 0, 0, 0.1)`). Right editor shows new content with added lines highlighted in green (`rgba(0, 255, 0, 0.1)`). Modified lines show in both editors with word-level highlighting (Task 01). Alignment: filler lines are inserted in the shorter side to keep corresponding lines vertically aligned. Scroll synchronization: scrolling one editor scrolls the other (using Phase 42 Task 17 scroll sync). Each side has its own DiffGutter.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (create)

**Acceptance Criteria:**
- Old content on left, new content on right
- Deleted lines highlighted red on left side
- Added lines highlighted green on right side
- Filler lines align corresponding content
- Scroll synchronization works between sides
- Word-level highlights show within changed lines

**Dependencies:** Task 01, Task 02

---

### Task 05 -- Implement Inline Diff View

**Description:** Create an inline diff view that shows changes in a single editor with deleted lines (red, strikethrough) above added lines (green).

**Implementation Details:** InlineDiffView is a wxPanel containing a single EditorPanel (read-only) that interleaves old and new content. Deleted lines render with red background and strikethrough text styling. Added lines render with green background. Unchanged context lines render normally. Modified lines show both the old version (red, strikethrough) and the new version (green) in sequence. Word-level highlighting applies to modified lines. A collapse indicator allows hiding unchanged context to focus on changes. DiffGutter renders alongside.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` (create)

**Acceptance Criteria:**
- Deleted lines show with red background and strikethrough
- Added lines show with green background
- Modified lines show both versions in sequence
- Word-level highlighting works for modified lines
- Context lines between changes render normally

**Dependencies:** Task 01, Task 02

---

### Task 06 -- Implement Change Navigation (Next/Previous)

**Description:** Add Next Change and Previous Change navigation that jumps between diff hunks, centering the target change in the viewport.

**Implementation Details:** DiffPanel tracks `current_change_index_` (already exists in the stub). Next Change increments the index and scrolls to center the change in the viewport. Previous Change decrements. When reaching the end, wrap to the beginning (with a toast indicating "Wrapped to beginning"). Navigation works in both inline and side-by-side modes. In side-by-side mode, both editors scroll to the change. The current change hunk is highlighted with a stronger border (2px accent_primary left border). Keyboard shortcuts: `Cmd+Alt+F5` (next), `Cmd+Alt+F3` (previous).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` (modify)

**Acceptance Criteria:**
- Next/Previous buttons navigate between changes
- Current change is centered in viewport
- Current change has distinct visual highlight
- Wrap-around shows toast notification
- Keyboard shortcuts work

**Dependencies:** Tasks 03, 04, 05

---

### Task 07 -- Implement Accept/Reject Individual Changes

**Description:** Add hover-activated accept/reject buttons on each diff hunk, allowing the user to selectively apply or discard individual changes.

**Implementation Details:** When hovering over a diff hunk in the side-by-side view, show action buttons: "Accept Change" (green checkmark) and "Reject Change" (red X) in a floating overlay at the top-right of the hunk. Accept Change applies the new content (replaces old with new for that hunk). Reject Change keeps the old content (discards the change). After accepting or rejecting, the hunk is removed from the diff and the view updates. The action is undoable (Cmd+Z reverts the accept/reject). In the inline view, buttons appear at the right margin of the first line of each hunk.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` (modify)

**Acceptance Criteria:**
- Accept/Reject buttons appear on hunk hover
- Accept applies the change, updates the view
- Reject discards the change, updates the view
- Actions are undoable
- Buttons disappear when mouse leaves the hunk

**Dependencies:** Tasks 04, 05

---

### Task 08 -- Implement Accept All Theirs / Accept All Mine

**Description:** Add toolbar buttons to accept all changes from one side or the other, applying them in bulk.

**Implementation Details:** "Accept All Theirs" replaces all old content with new content (equivalent to accepting every change). "Accept All Mine" discards all new content (equivalent to rejecting every change). Both actions are undoable as a single undo operation. A confirmation dialog (Phase 44) asks "Apply N changes?" before executing. After bulk action, the diff view shows no remaining changes and the toolbar updates to show "No differences".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- Accept All Theirs applies all changes
- Accept All Mine rejects all changes
- Confirmation dialog prevents accidental bulk actions
- Both actions are undoable as single operations
- View updates to show no remaining changes

**Dependencies:** Task 07

---

### Task 09 -- Implement Ignore Whitespace Toggle

**Description:** Add a toggle that re-computes the diff while ignoring whitespace differences, hiding formatting-only changes.

**Implementation Details:** Add "Ignore Whitespace" checkbox to the DiffPanel toolbar. When checked, the DiffEngine is called with a `DiffOptions` struct that sets `ignore_whitespace = true`. The engine normalizes whitespace (collapse multiple spaces, trim trailing) before comparing. Changes that are whitespace-only are excluded from the result. The diff hunk count in the toolbar updates accordingly. Toggle state persists per-session. The DiffEngine already accepts context_lines; add DiffOptions alongside.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DiffEngine.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/DiffEngine.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- Toggle re-computes diff ignoring whitespace
- Whitespace-only changes are hidden
- Hunk count updates when toggle changes
- Toggle state persists during session
- Works in both inline and side-by-side modes

**Dependencies:** Task 03

---

### Task 10 -- Implement Diff Line Count Summary

**Description:** Display a summary of changes in the DiffPanel toolbar: "N additions, M deletions" with green/red coloring.

**Implementation Details:** Add a summary label in the DiffPanel toolbar between the navigation buttons and the whitespace toggle. Format: "+N -M" where N is total added lines (green text) and M is total deleted lines (red text). Also show the total number of hunks: "(K changes)". The summary updates when the diff changes (accept/reject, whitespace toggle). Summary also appears in the tab title: "file.md (Diff: +5 -3)".

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/TabBar.cpp` (modify)

**Acceptance Criteria:**
- Summary shows correct addition and deletion counts
- Green coloring for additions, red for deletions
- Hunk count shown in parentheses
- Summary updates on accept/reject/toggle
- Tab title includes diff summary

**Dependencies:** Task 03

---

### Task 11 -- Implement Diff Decorations in Minimap

**Description:** Show diff change positions on the minimap/scrollbar (from Phase 42) as colored markers.

**Implementation Details:** Create `DiffDecorationProvider` implementing `IScrollDecorationProvider`. It provides: green markers for added line positions, red markers for deleted line positions, blue markers for modified line positions. The provider receives the DiffResult and maps hunk line ranges to decoration positions. Register this provider with the ThemedScrollbar instances in the diff editor panels. Current change position is highlighted with a stronger marker (Block shape instead of Line).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` (modify)

**Acceptance Criteria:**
- Minimap shows colored markers for all diff changes
- Green for additions, red for deletions, blue for modifications
- Current change marker is more prominent
- Markers update when changes are accepted/rejected
- Clicking a marker navigates to that change

**Dependencies:** Task 04, Task 05, Phase 42 Tasks 03-04

---

### Task 12 -- Create MergeEngine for 3-Way Merge

**Description:** Implement a 3-way merge engine that takes base, theirs, and yours versions and produces a merged result with conflict markers.

**Implementation Details:** MergeEngine takes three inputs: `base` (common ancestor), `theirs` (remote changes), `yours` (local changes). It computes diff(base, theirs) and diff(base, yours) independently, then merges the two diffs. Non-overlapping changes are auto-merged. Overlapping changes (both sides modify the same lines) create `MergeConflict` entries. Output: `MergeResult { vector<MergeLine> lines; vector<MergeConflict> conflicts; }` where MergeLine has type (base/theirs/yours/auto-merged/conflict). Each MergeConflict records the conflicting ranges from both sides.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/MergeEngine.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/MergeEngine.cpp` (create)

**Acceptance Criteria:**
- Non-overlapping changes from both sides auto-merge correctly
- Overlapping changes produce conflict entries
- Base content preserved for unchanged regions
- Merge result is deterministic for same inputs
- Unit test with known conflicts validates merge output

**Dependencies:** None

---

### Task 13 -- Implement 3-Way Merge Editor UI

**Description:** Create a 3-way merge editor with four panels: theirs (left), base (center), yours (right), and result (bottom).

**Implementation Details:** MergeEditor extends ThemeAwareWindow with four EditorPanel instances arranged in a 2x2 grid (or 3+1 layout: three side-by-side on top, result below). Top row: theirs (left, read-only), base (center, read-only), yours (right, read-only). Bottom: result (editable). Conflicts in the result are highlighted with conflict markers (<<<<<<< THEIRS, =======, >>>>>>> YOURS). Each conflict region in the result shows inline action buttons: "Accept Theirs", "Accept Yours", "Accept Both" (concatenate both), "Accept None" (keep base). All four editors scroll synchronously.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp` (create)

**Acceptance Criteria:**
- Four panels render with correct content
- Conflict regions show inline action buttons
- Accept Theirs/Yours/Both/None resolve individual conflicts
- All panels scroll synchronously
- Result panel is editable for manual conflict resolution

**Dependencies:** Task 12

---

### Task 14 -- Implement Merge Conflict Navigation

**Description:** Add navigation to jump between merge conflicts in the merge editor.

**Implementation Details:** Add "Next Conflict" and "Previous Conflict" buttons to the merge editor toolbar. Tracks `current_conflict_index_`. Navigation centers the conflict in the result panel and highlights it with a yellow border. Unresolved conflict count shows in the toolbar: "N conflicts remaining". When all conflicts are resolved, the toolbar shows a green "All conflicts resolved" indicator and enables the "Complete Merge" button. Keyboard shortcuts: `Cmd+Shift+M` (next conflict), `Cmd+Shift+Alt+M` (previous conflict).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp` (modify)

**Acceptance Criteria:**
- Navigation jumps between conflicts
- Current conflict is highlighted with yellow border
- Conflict count decrements as conflicts are resolved
- "All resolved" indicator appears when done
- Complete Merge button enables when all resolved

**Dependencies:** Task 13

---

### Task 15 -- Implement Diff File Selector

**Description:** Add a file selector to the DiffPanel that allows choosing which file pair to diff from a list of changed files (for multi-file diffs like git status).

**Implementation Details:** Add a ThemedDropdown (Phase 41) to the DiffPanel toolbar showing the current filename. When clicked, shows all files with changes (from RepositoryService git status). Each item shows: filename, change type indicator (A=added, M=modified, D=deleted, R=renamed), and diff stats (+N -M). Selecting a file loads its diff into the viewer. The dropdown groups files by directory for large changesets. A "Previous/Next File" button navigates sequentially.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- File selector shows all changed files
- Change type indicators are correct
- Selecting a file loads its diff
- Files grouped by directory
- Previous/Next file navigation works

**Dependencies:** Task 03

---

### Task 16 -- Integrate Diff View with SCM Panel

**Description:** Wire the diff editor to open when clicking a changed file in the SCM/Git panel, showing the working tree diff.

**Implementation Details:** When the user clicks a changed file in the RepositoryService/Git panel, open a DiffPanel tab showing the diff between the working copy and HEAD. The tab title shows "filename (Working Tree)". For staged changes, show diff between staged and HEAD. For untracked files, show the entire file as additions (compare against empty). The diff opens in the current editor pane using `PaneManager::open_in_pane()` with a special document type (diff document).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/RepositoryService.cpp` (modify)

**Acceptance Criteria:**
- Clicking changed file in SCM opens diff view
- Working tree vs HEAD diff shows correctly
- Staged vs HEAD diff shows correctly
- Untracked files show as all additions
- Diff tab title indicates comparison type

**Dependencies:** Task 03

---

### Task 17 -- Implement Compare with Clipboard

**Description:** Add a command to compare the active file's content with the clipboard content.

**Implementation Details:** Register command `diff.compareWithClipboard`. When executed: read clipboard text, create a DiffResult comparing clipboard (left/old) with active file content (right/new), open a DiffPanel tab titled "{filename} <-> Clipboard". If clipboard is empty or does not contain text, show an error notification. The command is accessible via Command Palette and right-click context menu in the editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- Command opens diff between active file and clipboard
- Tab title indicates clipboard comparison
- Empty clipboard shows error notification
- Works from Command Palette and context menu
- Diff refreshes if clipboard content changes and command is re-run

**Dependencies:** Task 03

---

### Task 18 -- Implement Compare Active File with Saved Version

**Description:** Add a command to compare the current in-memory editor content with the last saved version on disk.

**Implementation Details:** Register command `diff.compareWithSaved`. When executed: read the file from disk (using the file path from the active document), diff against the current editor buffer content. Open a DiffPanel showing "Saved" (left) vs "Current" (right). This is useful for reviewing unsaved changes before saving. If the file is new/unsaved (no disk version), compare against an empty string. The diff highlights exactly what will change when the user saves.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- Command diffs in-memory content against saved file
- Unsaved files diff against empty string
- Tab title indicates "Saved vs Current"
- Diff updates are possible via a refresh button
- Works from Command Palette

**Dependencies:** Task 03

---

### Task 19 -- Implement Diff Toolbar with Keyboard Shortcuts

**Description:** Ensure all diff operations have keyboard shortcuts and are accessible via the diff toolbar.

**Implementation Details:** Complete keyboard shortcut mapping: `Cmd+Alt+F5` (Next Change), `Cmd+Alt+F3` (Previous Change), `Cmd+Alt+1` (Switch to Inline), `Cmd+Alt+2` (Switch to Side-by-Side), `Cmd+Shift+Enter` (Accept Current Change), `Cmd+Shift+Backspace` (Reject Current Change), `Cmd+Shift+A` (Accept All Theirs), `Cmd+Shift+W` (Toggle Whitespace). All shortcuts are documented in the keyboard shortcut overlay. The toolbar buttons show shortcut hints in their tooltips.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- All keyboard shortcuts work in diff mode
- Shortcuts appear in tooltip hints
- Shortcuts are documented in shortcut overlay
- Shortcuts do not conflict with editor shortcuts
- Commands appear in Command Palette under "Diff" category

**Dependencies:** Task 06, Task 07, Task 08, Task 09

---

### Task 20 -- Implement Syntax Highlighting in Diff Editors

**Description:** Ensure both sides of the diff editor have full syntax highlighting, not just plain text with colored backgrounds.

**Implementation Details:** The EditorPanel instances within SideBySideDiffView and InlineDiffView must have syntax highlighting enabled based on the file extension. Use the existing SyntaxHighlighter and GrammarEngine to tokenize the content. Diff background colors (red/green) must compose correctly with syntax highlighting colors (the diff color acts as a semi-transparent overlay on top of the syntax colors). Filler lines (alignment padding) do not receive syntax highlighting.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/InlineDiffView.cpp` (modify)

**Acceptance Criteria:**
- Both sides of diff show syntax highlighting
- Diff background colors overlay syntax colors correctly
- Filler lines render as plain background
- Performance: syntax highlighting does not delay diff rendering
- Language detection works from file extension

**Dependencies:** Tasks 04, 05

---

### Task 21 -- Implement Diff for Binary Files

**Description:** Handle binary file diffs gracefully by showing a message instead of trying to render binary content as text.

**Implementation Details:** Before computing a diff, detect if either file is binary (check for null bytes in the first 8KB). If binary, the DiffPanel shows a centered message: "Binary files differ" with file sizes for both versions and a "Download" button for each. If one side is binary and the other is text, show "File changed from text to binary" (or vice versa). Image files (.png, .jpg, .gif, .svg) show a visual diff with side-by-side image rendering instead of text diff.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/DiffPanel.cpp` (modify)

**Acceptance Criteria:**
- Binary files show "Binary files differ" message
- File sizes are displayed for both versions
- Image files show visual side-by-side comparison
- Text-to-binary transitions are indicated
- No crash or corruption when diffing binary files

**Dependencies:** Task 03

---

### Task 22 -- Implement Diff Performance for Large Files

**Description:** Ensure diff computation and rendering remain responsive for large files (10K+ lines, 1000+ changes).

**Implementation Details:** Profile DiffEngine::compute_diff() for files with 10K, 50K, and 100K lines. If computation exceeds 500ms, show a progress indicator. For very large diffs (>5000 hunks), truncate the visible hunks and add a "Show All" button. The side-by-side view uses virtual scrolling (only renders visible lines, not all 100K). DiffGutter uses the same virtual approach. Word-level diff is computed lazily (only for visible changed lines, not all changed lines upfront).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/DiffEngine.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SideBySideDiffView.cpp` (modify)

**Acceptance Criteria:**
- 10K line diff computes in under 200ms
- 50K line diff computes in under 1s with progress indicator
- Virtual scrolling handles 100K lines without lag
- Word diff is lazy (computed only for visible lines)
- Memory usage stays under 100MB for large diffs

**Dependencies:** Tasks 01, 04

---

### Task 23 -- Implement Merge Conflict Markers for Result Panel

**Description:** Render conflict markers in the merge result panel with colored regions and inline resolution buttons.

**Implementation Details:** In the MergeEditor result panel, conflict regions render with: theirs content in blue background, yours content in green background, separator line (=======) in gray. Each conflict block shows three inline buttons at the top: "Accept Incoming" (theirs, blue), "Accept Current" (yours, green), "Accept Both" (purple). The buttons are rendered as overlay controls positioned at the top-right of each conflict block. When a conflict is resolved, the markers are removed and the resolved content replaces the conflict block.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MergeEditor.cpp` (modify)

**Acceptance Criteria:**
- Conflict markers render with colored regions
- Inline buttons appear at top of each conflict
- Resolving a conflict removes the markers
- Resolved content replaces the conflict block
- Manual editing in the result panel is supported

**Dependencies:** Task 13

---

### Task 24 -- Add Diff/Merge Commands to Command Palette

**Description:** Register all diff and merge commands in the Command system and expose them via Command Palette.

**Implementation Details:** Register commands: `diff.openSideBySide`, `diff.openInline`, `diff.compareWithClipboard`, `diff.compareWithSaved`, `diff.nextChange`, `diff.previousChange`, `diff.acceptChange`, `diff.rejectChange`, `diff.acceptAllTheirs`, `diff.acceptAllMine`, `diff.toggleWhitespace`, `merge.nextConflict`, `merge.previousConflict`, `merge.acceptTheirs`, `merge.acceptYours`, `merge.acceptBoth`, `merge.completeMerge`. All commands are categorized under "Diff" or "Merge" in the palette.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Command.cpp` (modify)

**Acceptance Criteria:**
- All diff/merge commands appear in Command Palette
- Commands are categorized correctly
- Commands are context-aware (only available when diff/merge is active)
- Keyboard shortcuts are shown next to commands
- Commands execute correct actions

**Dependencies:** Tasks 06-08, 14, 17, 18

---

### Task 25 -- Add CMake Integration and Unit Tests

**Description:** Add all new diff/merge files to CMakeLists.txt and create comprehensive unit tests.

**Implementation Details:** Add all new .h/.cpp files to CMakeLists.txt. Create `test_diff_editor.cpp` with sections: (1) WordDiffEngine identifies changed words correctly, (2) WordDiffEngine handles empty lines and whitespace, (3) MergeEngine auto-merges non-overlapping changes, (4) MergeEngine detects conflicts on overlapping changes, (5) MergeEngine preserves base for unchanged regions, (6) DiffEngine with ignore_whitespace skips whitespace-only changes, (7) DiffGutter maps lines to correct indicator types, (8) Binary file detection works for various file types, (9) Large file diff completes within time budget, (10) Merge conflict resolution removes conflict markers.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_diff_editor.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles without errors
- All 10 test sections pass
- WordDiffEngine and MergeEngine tests are pure logic (no GUI)
- No undefined symbol errors
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Side-by-side diff view renders with synchronized scrolling
- Inline diff view renders with strikethrough deletions
- Word-level highlighting works within changed lines
- Change navigation jumps between hunks
- Individual accept/reject works per hunk
- 3-way merge resolves conflicts with inline buttons
- Diff integrates with SCM panel for git changes
- Compare with clipboard and saved version commands work
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
