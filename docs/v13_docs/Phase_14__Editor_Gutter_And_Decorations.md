# Phase 14: Editor Gutter and Decorations

## Objective

Build a comprehensive gutter and decoration system for the `EditorPanel` that provides visual annotations alongside the code: absolute and relative line numbers, fold markers, breakpoint indicators, git change indicators, diagnostic markers, code lens, bookmark indicators, current line highlighting, and merge conflict decorators. The existing `EditorPanel` already has line numbers, fold margins, and some diagnostic support via Scintilla. This phase unifies and extends these into a cohesive, extensible gutter system.

## Prerequisites

- `EditorPanel` at `src/ui/EditorPanel.h` with wxStyledTextCtrl (Scintilla) editor
- Existing line number margin (`show_line_numbers_`, `UpdateLineNumberMargin()`)
- Existing fold margin (`code_folding_`, `ConfigureFoldMargin()`, `kFoldMarginWidth = 14`)
- Existing relative line numbers support (`relative_line_numbers_`, `SetRelativeLineNumbers()`)
- Existing diagnostic indicators (`DiagnosticIndicator`, `diagnostic_indicators_`)
- `ThemeEngine` for color tokens
- `EventBus` for communication

## Deliverables

A unified gutter rendering system with 25+ tasks covering all gutter annotations and inline decorations.

---

## Task 1: Gutter Architecture - Margin Registry

**Title:** Create a margin registry for managing gutter columns

**Description:** Scintilla supports multiple margins (up to 5). Define a clear margin allocation scheme and create a registry that manages which margins are visible and their widths.

**Implementation Details:**
- Define margin indices as constants in `EditorPanel.h`:
  ```cpp
  static constexpr int kMarginLineNumbers = 0;   // Line numbers
  static constexpr int kMarginFolding = 1;        // Fold markers (+/-)
  static constexpr int kMarginBreakpoints = 2;    // Breakpoints (red dots)
  static constexpr int kMarginGitChanges = 3;     // Git change indicators
  static constexpr int kMarginBookmarks = 4;       // Bookmark flags
  ```
- Add `struct MarginConfig { bool visible; int width; }` and `std::array<MarginConfig, 5> margin_configs_`
- Add `void UpdateMarginWidths()` that applies all margin configurations
- Each margin independently toggleable via settings

**Files Affected:**
- `src/ui/EditorPanel.h` (add margin constants, config struct)
- `src/ui/EditorPanel.cpp` (implement `UpdateMarginWidths()`)

**Acceptance Criteria:**
- All 5 margins defined with clear indices
- Each margin independently toggleable
- Total gutter width adjusts dynamically
- Margin widths respect DPI scaling

**Dependencies:** None

---

## Task 2: Absolute Line Numbers

**Title:** Enhance absolute line number rendering

**Description:** Improve the existing line number margin with better formatting: right-aligned numbers, themed coloring, dynamic width based on total line count, and click-to-select-line behavior.

**Implementation Details:**
- Line numbers use `TextMuted` color, active line number uses `TextMain` + semibold
- Width auto-adjusts: `max(3, digits(total_lines)) * char_width + padding`
- Click on line number selects entire line
- Shift+click extends selection to that line
- Ctrl+click adds a cursor at that line (multi-cursor)

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance `UpdateLineNumberMargin()`, add click handlers)

**Acceptance Criteria:**
- Line numbers right-aligned in gutter
- Active line number highlighted
- Width adjusts for files with 1-999999 lines
- Click selects line, Shift+click extends, Ctrl+click adds cursor

**Dependencies:** Task 1

---

## Task 3: Relative Line Numbers

**Title:** Complete the relative line number implementation

**Description:** Finish the existing `relative_line_numbers_` feature. In relative mode, the current line shows the absolute number, and all other lines show their distance from the cursor line. This is essential for Vim-style navigation.

**Implementation Details:**
- In the Scintilla margin handler, when `relative_line_numbers_` is true:
  - Current line: display absolute number
  - Other lines: display `abs(line - cursor_line)`
- Use Scintilla's `SCI_SETMARGINTYPEN` with `SC_MARGIN_RTEXT` for right-aligned text
- Numbers colored with `TextMuted`, current line with `AccentPrimary`
- Toggle via `SetRelativeLineNumbers(true/false)` (already exists)
- Add hybrid mode: absolute number for current line, relative for others (VSCode default)

**Files Affected:**
- `src/ui/EditorPanel.cpp` (implement relative number calculation in margin)

**Acceptance Criteria:**
- Current line shows absolute number
- Other lines show distance from cursor
- Numbers update on every cursor movement
- Hybrid mode works correctly
- Performance acceptable for large files (50K+ lines)

**Dependencies:** Task 1

---

## Task 4: Fold Markers with Hover Expand Preview

**Title:** Enhance fold markers with hover preview of collapsed content

**Description:** Improve the existing fold margin. When hovering over a fold marker for a collapsed region, show a tooltip-style preview of the first few lines of the collapsed content. Add distinct fold marker glyphs (triangle for expandable, minus for collapsible).

**Implementation Details:**
- Use Scintilla's `SCI_SETFOLDFLAGS` and `SCI_MARKERDEFINE` to customize fold markers:
  - Expanded region: downward triangle
  - Collapsed region: rightward triangle
  - Fold body: vertical line connecting top to bottom
- On hover over collapsed fold marker (detected via `SCI_MARGINGETTEXT`):
  - Show a `wxPopupTransientWindow` with the first 5 lines of the collapsed region
  - Preview styled with editor font and syntax highlighting
  - Auto-dismiss on mouse leave
- Add click animation: fold marker rotates 90 degrees over 100ms

**Files Affected:**
- `src/ui/EditorPanel.h` (add fold preview popup)
- `src/ui/EditorPanel.cpp` (implement fold marker customization, hover preview)

**Acceptance Criteria:**
- Fold markers show triangles (not +/-)
- Hover preview shows first 5 lines of collapsed region
- Preview auto-dismisses on mouse leave
- Fold/unfold animation is smooth
- Large documents fold without lag

**Dependencies:** Task 1

---

## Task 5: Breakpoint Indicators (Red Dot)

**Title:** Implement breakpoint gutter markers

**Description:** Add a dedicated gutter margin for breakpoint indicators. Users can click the margin to toggle a red dot breakpoint on that line. Breakpoints are stored per-file and persist across sessions.

**Implementation Details:**
- Use margin index 2 (`kMarginBreakpoints`), width 14px
- Define Scintilla marker:
  ```cpp
  editor_->MarkerDefine(kMarkerBreakpoint, wxSTC_MARK_CIRCLE);
  editor_->MarkerSetBackground(kMarkerBreakpoint, wxColour(220, 50, 50)); // red
  editor_->MarkerSetForeground(kMarkerBreakpoint, wxColour(220, 50, 50));
  ```
- Click margin to toggle breakpoint: `editor_->MarkerAdd(line, kMarkerBreakpoint)`
- Add `std::unordered_map<std::string, std::vector<int>> breakpoints_` (per file)
- Right-click on breakpoint: context menu with "Remove Breakpoint", "Edit Condition...", "Disable"
- Publish `BreakpointToggleEvent(file_path, line, enabled)` on toggle

**Files Affected:**
- `src/ui/EditorPanel.h` (add breakpoint state, marker constant)
- `src/ui/EditorPanel.cpp` (implement breakpoint toggle, margin click handler)
- `src/core/Events.h` (add breakpoint events)

**Acceptance Criteria:**
- Red dot appears on margin click
- Click again removes the breakpoint
- Breakpoints persist per file across tab switches
- Right-click shows breakpoint context menu
- Event published on toggle for debug system integration

**Dependencies:** Task 1

---

## Task 6: Git Change Indicators (Added/Modified/Deleted)

**Title:** Show git diff status in the editor gutter

**Description:** Display colored indicators in a narrow gutter margin showing which lines have been added, modified, or deleted compared to the git HEAD version. Green for added lines, blue for modified lines, red triangle for deleted lines.

**Implementation Details:**
- Use margin index 3 (`kMarginGitChanges`), width 3px (thin color bar)
- Compute diff by running `git diff HEAD -- <file>` and parsing the unified diff output
- Map diff hunks to line ranges:
  - Added lines: green bar
  - Modified lines: blue bar
  - Deleted lines: red triangle marker at the deletion point
- Update on file save and on focus gain (re-read git status)
- Use Scintilla annotations or custom marker drawing
- Cache diff results, invalidate on save

**Files Affected:**
- `src/ui/EditorPanel.h` (add git change state, margin config)
- `src/ui/EditorPanel.cpp` (implement diff computation and margin rendering)
- `src/ui/GitGutterProvider.h` (new -- encapsulates git diff logic)
- `src/ui/GitGutterProvider.cpp` (new)

**Acceptance Criteria:**
- Green bar for newly added lines
- Blue bar for modified lines
- Red triangle for deleted line positions
- Updates on save and focus gain
- Works with uncommitted changes against HEAD
- No gutter shown for untracked files

**Dependencies:** Task 1

---

## Task 7: Diagnostic Markers in Gutter

**Title:** Show error/warning icons in the gutter margin

**Description:** Use the existing `diagnostic_indicators_` to display appropriate icons in the gutter margin: red circle for errors, yellow triangle for warnings, blue "i" for info, green lightbulb for hints with quick-fixes available.

**Implementation Details:**
- Reuse the breakpoint margin (kMarginBreakpoints) with multiple marker types:
  ```cpp
  static constexpr int kMarkerError = 1;
  static constexpr int kMarkerWarning = 2;
  static constexpr int kMarkerInfo = 3;
  static constexpr int kMarkerQuickFix = 4;
  ```
- For each diagnostic in `diagnostic_indicators_`:
  ```cpp
  int marker = (sev == Error) ? kMarkerError : (sev == Warning) ? kMarkerWarning : kMarkerInfo;
  editor_->MarkerAdd(diag.line, marker);
  if (diag.quick_fix_available) {
      editor_->MarkerAdd(diag.line, kMarkerQuickFix);
  }
  ```
- Click on diagnostic marker: show inline diagnostic popup (peek problem)
- Hover: show tooltip with diagnostic message

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance `ApplyDiagnosticMarkers()` with gutter icons)

**Acceptance Criteria:**
- Error lines show red circle in gutter
- Warning lines show yellow triangle
- Info lines show blue "i"
- Quick-fix lines show lightbulb overlay
- Click opens peek problem view
- Hover shows diagnostic message

**Dependencies:** Task 1

---

## Task 8: Code Lens Above Functions

**Title:** Implement code lens annotations above function declarations

**Description:** Display inline annotations above function/method declarations showing metadata: "N references", "N implementations", "Run Test", "Debug Test". For Markdown, show "N links to this heading". Code lens renders as small, clickable text above the line.

**Implementation Details:**
- Use Scintilla's `SCI_ANNOTATIONSETTEXT` to add text above lines:
  ```cpp
  editor_->AnnotationSetText(line, "2 references | Run Test");
  editor_->AnnotationSetStyle(line, STYLE_CODELENS);
  editor_->AnnotationSetVisible(wxSTC_ANNOTATION_STANDARD);
  ```
- Define `STYLE_CODELENS` with small font (9pt), `TextMuted` color, no background
- Create a `CodeLensProvider` interface:
  ```cpp
  struct CodeLensItem { int line; std::string text; std::function<void()> action; };
  ```
- For Markdown: scan for headings and count backlinks
- Code lens clickable to perform action (navigate to references, run test)

**Files Affected:**
- `src/ui/EditorPanel.h` (add code lens state, provider interface)
- `src/ui/EditorPanel.cpp` (implement code lens rendering)
- `src/ui/CodeLensProvider.h` (new)
- `src/ui/CodeLensProvider.cpp` (new)

**Acceptance Criteria:**
- Code lens text appears above decorated lines
- Text is clickable (triggers action)
- Small, unobtrusive font size
- Updates when document changes
- Toggle visibility in settings
- Performance: max 100ms for lens computation

**Dependencies:** Task 1

---

## Task 9: Bookmark Indicators (Blue Flag)

**Title:** Implement line bookmarks with gutter indicators

**Description:** Allow users to bookmark lines with a blue flag icon in the gutter. Bookmarks are navigable (jump to next/previous bookmark) and persist per file.

**Implementation Details:**
- Use margin index 4 (`kMarginBookmarks`), width 14px
- Define marker:
  ```cpp
  editor_->MarkerDefine(kMarkerBookmark, wxSTC_MARK_BOOKMARK);
  editor_->MarkerSetBackground(kMarkerBookmark, wxColour(66, 133, 244)); // blue
  ```
- Toggle via keyboard shortcut (Cmd+F2 or configurable)
- Navigate: F2 (next bookmark), Shift+F2 (previous bookmark)
- Add `void ToggleBookmark(int line)`, `void NextBookmark()`, `void PreviousBookmark()`
- Store bookmarks in `std::set<int> bookmarks_`
- Publish `BookmarkChangedEvent` for status bar badge

**Files Affected:**
- `src/ui/EditorPanel.h` (add bookmark methods, state)
- `src/ui/EditorPanel.cpp` (implement bookmark logic)
- `src/core/Events.h` (add `BookmarkChangedEvent`)

**Acceptance Criteria:**
- Blue flag icon in gutter for bookmarked lines
- Toggle via click on bookmark margin or Cmd+F2
- F2 / Shift+F2 navigates between bookmarks
- Bookmarks persist across tab switches
- Bookmark count available for status bar

**Dependencies:** Task 1

---

## Task 10: Current Line Highlight Band

**Title:** Enhance current line highlighting

**Description:** Improve the existing current line highlight (`highlight_current_line_`) with a full-width highlight band that extends across the entire editor width, including gutters. The band color is a subtle tint of the background.

**Implementation Details:**
- Use Scintilla's `SCI_SETCARETLINEBACK` and `SCI_SETCARETLINEBACKALPHA`:
  ```cpp
  auto bg = theme_engine().color(ThemeColorToken::BgApp);
  editor_->SetCaretLineBackground(bg.ChangeLightness(106));
  editor_->SetCaretLineBackAlpha(40);
  editor_->SetCaretLineVisible(true);
  ```
- Extend highlight across all margins using `SCI_SETCARETLINEVISIBLEALWAYS(true)`
- In `OnThemeChanged()`, update the caret line color
- Add option for line number highlight: current line number gets `AccentPrimary` color

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance `SetHighlightCurrentLine`, theme integration)

**Acceptance Criteria:**
- Current line highlighted with subtle background band
- Band extends full editor width
- Current line number highlighted in accent color
- Band color updates on theme change
- Togglable via settings

**Dependencies:** None

---

## Task 11: Selection Line Highlight

**Title:** Highlight all lines in the current selection

**Description:** When text is selected across multiple lines, highlight the background of all selected lines (not just the selected text) with a subtle tint, making it easier to see the scope of the selection.

**Implementation Details:**
- Subscribe to `SCI_SETSEL` changes
- Get selection range: `editor_->GetSelectionStart()` / `GetSelectionEnd()`
- Compute line range: `LineFromPosition(start)` to `LineFromPosition(end)`
- Apply indicator on those lines using `SCI_INDICATORFILLRANGE` with a custom indicator:
  ```cpp
  static constexpr int kIndicatorSelectionLine = 8;
  editor_->IndicatorSetStyle(kIndicatorSelectionLine, wxSTC_INDIC_FULLBOX);
  editor_->IndicatorSetAlpha(kIndicatorSelectionLine, 20);
  ```
- Clear when selection is empty (just cursor)
- Use a different tint than the text selection highlight

**Files Affected:**
- `src/ui/EditorPanel.h` (add indicator constant)
- `src/ui/EditorPanel.cpp` (implement selection line highlighting)

**Acceptance Criteria:**
- All lines with selected text get subtle background highlight
- Highlight distinct from text selection color
- Clears when selection is empty
- Updates on selection change
- Works with multi-cursor selections

**Dependencies:** None

---

## Task 12: Merge Conflict Decorators

**Title:** Detect and decorate merge conflict markers

**Description:** When a file contains git merge conflict markers (`<<<<<<<`, `=======`, `>>>>>>>`), highlight these regions with colored backgrounds and add inline action buttons: "Accept Current", "Accept Incoming", "Accept Both", "Compare Changes".

**Implementation Details:**
- Scan document for merge conflict markers on load and after each edit:
  ```cpp
  struct MergeConflict {
      int start_line;   // <<<<<<< line
      int separator_line; // ======= line
      int end_line;     // >>>>>>> line
  };
  std::vector<MergeConflict> conflicts_;
  ```
- Highlight regions:
  - Current (above =======): green background tint
  - Incoming (below =======): blue background tint
  - Separator line: yellow/warning background
- Add code-lens-style action buttons above the `<<<<<<<` line:
  - "Accept Current Change | Accept Incoming Change | Accept Both | Compare"
- Each action modifies the document:
  - Accept Current: keep lines above separator, remove rest
  - Accept Incoming: keep lines below separator, remove rest
  - Accept Both: keep all content lines, remove markers

**Files Affected:**
- `src/ui/EditorPanel.h` (add merge conflict detection state)
- `src/ui/EditorPanel.cpp` (implement conflict scanning and decoration)
- `src/ui/MergeConflictResolver.h` (new -- resolution logic)
- `src/ui/MergeConflictResolver.cpp` (new)

**Acceptance Criteria:**
- Merge conflict markers detected automatically
- Color-coded backgrounds for current/incoming regions
- Action buttons render above conflict start
- Accept actions modify document correctly
- Markers re-scanned after accept actions

**Dependencies:** Task 8

---

## Task 13: Gutter Hover Tooltip System

**Title:** Unified tooltip system for gutter hover

**Description:** When hovering over any gutter element (line number, fold marker, breakpoint, git change, diagnostic, bookmark), show an appropriate tooltip with context information.

**Implementation Details:**
- In `OnDwellStart()`, determine which margin the mouse is over:
  ```cpp
  int margin = editor_->GetMarginWidth(...); // calculate from x position
  switch (margin_index) {
      case kMarginLineNumbers:
          tooltip = "Line " + std::to_string(line + 1);
          break;
      case kMarginBreakpoints:
          if (has_breakpoint) tooltip = "Breakpoint at line " + ...;
          if (has_diagnostic) tooltip = diagnostic_message;
          break;
      case kMarginGitChanges:
          tooltip = "Modified: " + original_line_text + " -> " + current_line_text;
          break;
      case kMarginBookmarks:
          tooltip = "Bookmark (F2 to navigate)";
          break;
  }
  ```
- Rich tooltip for diagnostics: show severity icon + message + quick-fix availability

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance `OnDwellStart` for gutter tooltips)

**Acceptance Criteria:**
- Each gutter type shows appropriate tooltip
- Diagnostic tooltips show full message and severity
- Git change tooltips show what changed
- Bookmark tooltip shows navigation hint
- Tooltips dismiss on mouse move

**Dependencies:** Tasks 5, 6, 7, 9

---

## Task 14: Gutter Click Dispatcher

**Title:** Unified click handler for gutter margin clicks

**Description:** Scintilla sends `wxEVT_STC_MARGINCLICK` for gutter clicks. Create a dispatcher that routes clicks to the correct handler based on which margin was clicked.

**Implementation Details:**
- Bind `wxEVT_STC_MARGINCLICK` handler:
  ```cpp
  void EditorPanel::OnMarginClick(wxStyledTextEvent& event) {
      int margin = event.GetMargin();
      int line = editor_->LineFromPosition(event.GetPosition());
      switch (margin) {
          case kMarginLineNumbers:
              SelectLine(line);
              break;
          case kMarginFolding:
              ToggleFold(line);
              break;
          case kMarginBreakpoints:
              ToggleBreakpoint(line);
              break;
          case kMarginBookmarks:
              ToggleBookmark(line);
              break;
      }
  }
  ```
- Support Shift+click for range selection on line numbers
- Support Ctrl+click for multi-cursor on line numbers

**Files Affected:**
- `src/ui/EditorPanel.cpp` (add centralized margin click handler)

**Acceptance Criteria:**
- Clicks on each margin type routed to correct handler
- Line number clicks select lines
- Fold margin clicks toggle folds
- Breakpoint margin clicks toggle breakpoints
- Bookmark margin clicks toggle bookmarks
- Modifier keys (Shift, Ctrl) handled correctly

**Dependencies:** Tasks 2, 4, 5, 9

---

## Task 15: Gutter Decoration API

**Title:** Create an extensible decoration API for gutter markers

**Description:** Design an API that allows extensions and other system components to add custom gutter decorations. This is the extension point for future features (test coverage markers, profiling annotations, etc.).

**Implementation Details:**
- Create `src/ui/GutterDecorationProvider.h`:
  ```cpp
  struct GutterDecoration {
      int line;
      int margin_index;
      wxColour color;
      std::string tooltip;
      std::string icon_name; // or MarkerType enum
      std::function<void(int line)> on_click;
  };

  class IGutterDecorationProvider {
  public:
      virtual ~IGutterDecorationProvider() = default;
      virtual auto GetDecorations(const std::string& file_path) -> std::vector<GutterDecoration> = 0;
      virtual auto GetProviderId() const -> std::string = 0;
  };
  ```
- Add `void RegisterDecorationProvider(std::unique_ptr<IGutterDecorationProvider> provider)` to EditorPanel
- On file open/change, query all providers and apply decorations
- Built-in providers: breakpoints, diagnostics, bookmarks, git changes

**Files Affected:**
- `src/ui/GutterDecorationProvider.h` (new)
- `src/ui/EditorPanel.h` (add provider registration)
- `src/ui/EditorPanel.cpp` (implement provider querying and application)

**Acceptance Criteria:**
- Provider interface is clean and minimal
- Multiple providers can register simultaneously
- Decorations from all providers displayed correctly
- Providers queried on file open and content change
- Decoration precedence: errors > warnings > bookmarks > git changes

**Dependencies:** Tasks 5, 6, 7, 9

---

## Task 16: Inline Error Squiggles

**Title:** Render diagnostic squiggles under problematic text

**Description:** Display red (error) and yellow (warning) squiggly underlines under the specific text ranges identified by diagnostics, not just gutter markers.

**Implementation Details:**
- Use Scintilla indicators for squiggles:
  ```cpp
  static constexpr int kIndicatorError = 9;
  static constexpr int kIndicatorWarning = 10;

  editor_->IndicatorSetStyle(kIndicatorError, wxSTC_INDIC_SQUIGGLE);
  editor_->IndicatorSetForeground(kIndicatorError, error_color);

  editor_->IndicatorSetStyle(kIndicatorWarning, wxSTC_INDIC_SQUIGGLE);
  editor_->IndicatorSetForeground(kIndicatorWarning, warning_color);
  ```
- For each diagnostic with start/end positions:
  ```cpp
  editor_->SetIndicatorCurrent(kIndicatorError);
  editor_->IndicatorFillRange(start_pos, length);
  ```
- Hover over squiggle shows diagnostic message (via dwell)

**Files Affected:**
- `src/ui/EditorPanel.h` (add indicator constants)
- `src/ui/EditorPanel.cpp` (implement squiggle rendering)

**Acceptance Criteria:**
- Error squiggles are red and wavy
- Warning squiggles are yellow and wavy
- Squiggles appear under the exact problematic text range
- Hover shows diagnostic message
- Squiggles clear when diagnostic is resolved

**Dependencies:** Task 7

---

## Task 17: Inline Diagnostic Text

**Title:** Show diagnostic message text inline at end of line

**Description:** Optionally display the diagnostic message text at the end of the affected line, similar to "Error Lens" in VSCode. The message appears in a smaller, muted font after the line content.

**Implementation Details:**
- Use Scintilla's `SCI_ANNOTATIONSETTEXT` in `ANNOTATION_INDENTED` mode
- Or use end-of-line text via `SCI_EOLANNATIONSETTEXT` (Scintilla 5.x):
  ```cpp
  editor_->EOLAnnotationSetText(line, diagnostic_message);
  editor_->EOLAnnotationSetStyle(line, STYLE_DIAGNOSTIC_INLINE);
  ```
- Style: 9pt font, error messages in red, warnings in yellow, 50% opacity
- Toggle via `ShowInlineDiagnostics(bool)` (already exists in EditorPanel)
- Maximum message length: 80 characters, truncated with "..."

**Files Affected:**
- `src/ui/EditorPanel.cpp` (implement inline diagnostic text rendering)

**Acceptance Criteria:**
- Diagnostic message shows at end of line
- Red for errors, yellow for warnings
- Text is smaller and dimmer than code
- Truncated at 80 characters
- Togglable via settings
- Updates when diagnostics change

**Dependencies:** Task 7

---

## Task 18: Changed Line Highlighting (Unsaved Changes)

**Title:** Highlight lines with unsaved changes

**Description:** In addition to git change indicators (Task 6), show which lines have been modified since the last save using a yellow/amber gutter indicator. This gives instant visual feedback about unsaved changes.

**Implementation Details:**
- Track original content on file load/save
- On each edit, compute which lines differ from the saved version:
  ```cpp
  // Simple line-by-line comparison
  auto saved_lines = SplitLines(saved_content_);
  auto current_lines = SplitLines(GetContent());
  for (int i = 0; i < current_lines.size(); ++i) {
      if (i >= saved_lines.size() || current_lines[i] != saved_lines[i]) {
          MarkLineAsChanged(i);
      }
  }
  ```
- Use a thin (2px) amber/yellow bar in the git change margin
- Differentiate from git changes: git = comparison to HEAD, this = comparison to last save
- Clear changed indicators on save

**Files Affected:**
- `src/ui/EditorPanel.h` (add saved content tracking)
- `src/ui/EditorPanel.cpp` (implement unsaved change tracking)

**Acceptance Criteria:**
- Changed lines marked with amber indicator
- Indicators appear immediately on edit
- Clear on save
- Distinct from git change indicators (different color shade)
- Performance: max 50ms for diff computation on 10K line files

**Dependencies:** Task 6

---

## Task 19: Gutter Width Animation

**Title:** Animate gutter width changes smoothly

**Description:** When margins are toggled (e.g., showing/hiding breakpoint column), animate the width change instead of jumping instantly.

**Implementation Details:**
- When any margin visibility toggles:
  ```cpp
  int current_total = GetTotalMarginWidth();
  int target_total = CalculateTargetMarginWidth();
  StartMarginAnimation(current_total, target_total, 150);
  ```
- Animate over 150ms using a timer:
  - Each frame: interpolate margin widths linearly
  - Apply via `editor_->SetMarginWidth(n, interpolated_width)`
- Ensure editor content reflows smoothly during animation

**Files Affected:**
- `src/ui/EditorPanel.h` (add margin animation state)
- `src/ui/EditorPanel.cpp` (implement margin width animation)

**Acceptance Criteria:**
- Margin width transitions are smooth (150ms)
- Editor content reflows without flicker
- Multiple simultaneous margin changes coordinate properly
- Animation timer cleans up after completion

**Dependencies:** Task 1

---

## Task 20: Fold Region Background Tinting

**Title:** Tint the background of foldable regions

**Description:** Show a subtle background tint for foldable code regions (headings in Markdown, functions in code) to make the structure visible at a glance, similar to "bracket pair colorization background".

**Implementation Details:**
- For each fold region, apply a very subtle background tint:
  - Level 1 regions: 2% tint
  - Level 2 regions: 4% tint
  - Level 3+: 6% tint
- Use Scintilla's `SCI_SETFOLDMARGINCOLOUR` and indicators:
  ```cpp
  editor_->IndicatorSetStyle(kIndicatorFoldRegion, wxSTC_INDIC_STRAIGHTBOX);
  editor_->IndicatorSetAlpha(kIndicatorFoldRegion, 6);
  editor_->IndicatorSetUnder(kIndicatorFoldRegion, true);
  ```
- Toggle via `bool fold_region_highlight_{false}` setting

**Files Affected:**
- `src/ui/EditorPanel.h` (add fold region highlight setting)
- `src/ui/EditorPanel.cpp` (implement fold region tinting)

**Acceptance Criteria:**
- Foldable regions have subtle background tint
- Nesting depth increases tint intensity
- Togglable via settings
- Performance: no noticeable lag on large files
- Theme-appropriate tint colors

**Dependencies:** Task 4

---

## Task 21: Gutter Right-Click Context Menu

**Title:** Add context menu for gutter right-click

**Description:** Right-clicking in the gutter area should show a context menu with relevant actions based on which margin was clicked and what annotations exist on that line.

**Implementation Details:**
- On right-click in gutter, show context menu:
  ```cpp
  wxMenu menu;
  if (margin == kMarginBreakpoints) {
      if (HasBreakpoint(line)) {
          menu.Append(1, "Remove Breakpoint");
          menu.Append(2, "Edit Breakpoint Condition...");
          menu.Append(3, "Disable Breakpoint");
      } else {
          menu.Append(4, "Add Breakpoint");
          menu.Append(5, "Add Conditional Breakpoint...");
          menu.Append(6, "Add Log Point...");
      }
  }
  if (margin == kMarginBookmarks) {
      menu.Append(7, HasBookmark(line) ? "Remove Bookmark" : "Add Bookmark");
      menu.Append(8, "Remove All Bookmarks");
  }
  menu.AppendSeparator();
  menu.Append(9, "Toggle Line Numbers");
  menu.Append(10, "Toggle Fold Indicators");
  menu.Append(11, "Toggle Breakpoint Margin");
  menu.Append(12, "Toggle Git Change Margin");
  ```

**Files Affected:**
- `src/ui/EditorPanel.cpp` (add gutter right-click handler)

**Acceptance Criteria:**
- Context menu shows relevant actions for the clicked margin
- Breakpoint actions available when clicking breakpoint margin
- Bookmark actions available when clicking bookmark margin
- Margin visibility toggles available in all context menus
- Actions correctly execute

**Dependencies:** Tasks 5, 9

---

## Task 22: Minimap Gutter Synchronization

**Title:** Show gutter annotations in the minimap

**Description:** Mirror gutter annotations (breakpoints, diagnostics, bookmarks, git changes) as colored markers in the minimap's right edge (overview ruler area).

**Implementation Details:**
- In the minimap rendering code, draw thin horizontal lines at annotation positions:
  ```cpp
  for (const auto& bp : breakpoints_) {
      int minimap_y = LineToMinimapY(bp.line);
      DrawMinimapMarker(gc, minimap_y, wxColour(220, 50, 50)); // red for breakpoint
  }
  for (const auto& diag : diagnostic_indicators_) {
      int minimap_y = LineToMinimapY(diag.line);
      wxColour color = (diag.severity == Error) ? error_color : warning_color;
      DrawMinimapMarker(gc, minimap_y, color);
  }
  ```
- Markers are 3px wide, positioned at the right edge of the minimap
- Different colors for different annotation types

**Files Affected:**
- `src/ui/EditorPanel.cpp` (add annotation markers to minimap rendering)

**Acceptance Criteria:**
- Breakpoints show as red markers in minimap
- Diagnostics show as colored markers (red/yellow)
- Bookmarks show as blue markers
- Git changes show as green/blue markers
- Markers at correct vertical positions

**Dependencies:** Tasks 5, 6, 7, 9

---

## Task 23: Conditional Breakpoint Dialog

**Title:** Implement conditional breakpoint editing

**Description:** When "Edit Breakpoint Condition..." is selected from the gutter context menu, show a dialog where the user can enter a condition expression. Conditional breakpoints show as a yellow/orange dot instead of red.

**Implementation Details:**
- Create `BreakpointConditionDialog` (simple dialog with text input):
  ```cpp
  class BreakpointConditionDialog : public wxDialog {
      wxTextCtrl* condition_input_;
  public:
      BreakpointConditionDialog(wxWindow* parent, const std::string& current_condition);
      auto GetCondition() const -> std::string;
  };
  ```
- Extend breakpoint data:
  ```cpp
  struct Breakpoint {
      int line;
      bool enabled{true};
      std::string condition;
      std::string log_message; // for log points
  };
  ```
- Conditional breakpoints: orange dot instead of red
- Disabled breakpoints: hollow circle
- Log points: diamond shape

**Files Affected:**
- `src/ui/EditorPanel.h` (extend breakpoint struct)
- `src/ui/EditorPanel.cpp` (add conditional breakpoint logic)
- `src/ui/BreakpointConditionDialog.h` (new)
- `src/ui/BreakpointConditionDialog.cpp` (new)

**Acceptance Criteria:**
- Dialog allows entering condition expression
- Conditional breakpoints shown with orange dot
- Disabled breakpoints shown as hollow circle
- Log points shown as diamond
- Condition persists across sessions

**Dependencies:** Task 5

---

## Task 24: Gutter Performance Optimization

**Title:** Optimize gutter rendering for large files

**Description:** Ensure gutter annotations perform well for large files (50K+ lines) by only computing and rendering annotations for visible lines plus a buffer.

**Implementation Details:**
- On paint, determine visible line range:
  ```cpp
  int first_visible = editor_->GetFirstVisibleLine();
  int lines_on_screen = editor_->LinesOnScreen();
  int last_visible = first_visible + lines_on_screen;
  // Buffer of 50 lines above and below
  int range_start = std::max(0, first_visible - 50);
  int range_end = std::min(total_lines, last_visible + 50);
  ```
- Only apply/query decorations within this range
- Git diff: only compute hunks that overlap visible range
- Diagnostic markers: index by line number for O(1) lookup
- Bookmarks: use `std::set<int>` for O(log n) range queries

**Files Affected:**
- `src/ui/EditorPanel.cpp` (add viewport-scoped rendering)
- `src/ui/GitGutterProvider.cpp` (viewport-scoped diff)

**Acceptance Criteria:**
- Smooth scrolling with 50K+ line files
- Gutter annotations only computed for visible range (+buffer)
- No lag when scrolling rapidly
- Memory usage constant regardless of file size (for visible-range data)

**Dependencies:** Tasks 6, 7

---

## Task 25: Gutter Configuration UI

**Title:** Add settings for gutter customization

**Description:** Add a settings section for customizing gutter behavior: which margins are visible, margin widths, colors, and behavior options.

**Implementation Details:**
- Settings in config:
  ```yaml
  editor:
    gutter:
      line_numbers: true
      relative_numbers: false
      folding: true
      breakpoints: true
      git_changes: true
      bookmarks: true
      diagnostic_icons: true
      inline_diagnostics: true
      code_lens: false
      fold_highlight: false
      change_highlight: true
  ```
- Load/save via existing `EditorPanel::LoadPreferences()` / `SavePreferences()`
- Command palette commands: "Toggle Line Numbers", "Toggle Fold Margin", etc.
- Settings UI in preferences panel (future)

**Files Affected:**
- `src/ui/EditorPanel.cpp` (extend LoadPreferences/SavePreferences)
- `src/core/Config.h` (add gutter settings)
- `src/ui/CommandPalette.cpp` (register toggle commands)

**Acceptance Criteria:**
- All gutter features independently toggleable
- Settings persist across sessions
- Commands available in command palette
- Default configuration matches VSCode defaults

**Dependencies:** All previous tasks

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Margin Registry (Task 1) | Low |
| Line Numbers (Tasks 2-3) | Medium |
| Fold Markers (Task 4) | High |
| Breakpoints (Tasks 5, 23) | High |
| Git Changes (Task 6) | High |
| Diagnostics (Tasks 7, 16-17) | Medium |
| Code Lens (Task 8) | High |
| Bookmarks (Task 9) | Medium |
| Current Line (Task 10) | Low |
| Selection Highlight (Task 11) | Low |
| Merge Conflicts (Task 12) | High |
| Tooltips (Task 13) | Low |
| Click Dispatcher (Task 14) | Medium |
| Decoration API (Task 15) | Medium |
| Unsaved Changes (Task 18) | Medium |
| Animation (Task 19) | Low |
| Fold Tinting (Task 20) | Medium |
| Context Menu (Task 21) | Low |
| Minimap Sync (Task 22) | Medium |
| Performance (Task 24) | Medium |
| Configuration (Task 25) | Low |

## Files Created

- `src/ui/GitGutterProvider.h`
- `src/ui/GitGutterProvider.cpp`
- `src/ui/CodeLensProvider.h`
- `src/ui/CodeLensProvider.cpp`
- `src/ui/GutterDecorationProvider.h`
- `src/ui/MergeConflictResolver.h`
- `src/ui/MergeConflictResolver.cpp`
- `src/ui/BreakpointConditionDialog.h`
- `src/ui/BreakpointConditionDialog.cpp`

## Files Modified

- `src/ui/EditorPanel.h`
- `src/ui/EditorPanel.cpp`
- `src/core/Events.h`
- `src/core/Config.h`
- `src/ui/CommandPalette.cpp`
- `CMakeLists.txt`
