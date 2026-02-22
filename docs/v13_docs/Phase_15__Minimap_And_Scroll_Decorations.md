# Phase 15: Minimap and Scroll Decorations

## Objective

Upgrade the existing minimap implementation (a secondary `wxStyledTextCtrl` in read-only mode) into a full-featured overview component with syntax coloring, interactive slider, search/diagnostic/git markers, and an overview ruler. The current minimap (toggled via `ToggleMinimap()`) provides a basic code overview. This phase adds rich visual annotations, click-to-scroll, hover preview, render mode options, and tight integration with the gutter decoration system from Phase 14.

## Prerequisites

- `EditorPanel` with existing minimap (`minimap_` pointer, `CreateMinimap()`, `UpdateMinimapContent()`, `OnMinimapClick()`)
- Phase 14 complete (gutter decorations providing breakpoints, diagnostics, bookmarks, git changes)
- `ThemeEngine` with syntax coloring tokens
- `EventBus` for communication

## Deliverables

A professional minimap with scroll decorations, overview ruler, and rich annotations across 25+ tasks.

---

## Task 1: Minimap Renderer Architecture

**Title:** Replace secondary wxStyledTextCtrl with custom-rendered minimap

**Description:** Replace the current approach (a second Scintilla instance) with a custom `wxPanel` that renders a minimap using `wxGraphicsContext`. This gives full control over rendering: character-level coloring, marker overlays, slider rendering, and performance optimization.

**Implementation Details:**
- Create `src/ui/MinimapPanel.h` / `.cpp`:
  ```cpp
  class MinimapPanel : public ThemeAwareWindow {
  public:
      MinimapPanel(wxWindow* parent, core::ThemeEngine& theme_engine);
      void SetContent(const std::string& content, const std::vector<SyntaxToken>& tokens);
      void SetViewportRange(int first_line, int last_line, int total_lines);
      void SetMarkers(const std::vector<MinimapMarker>& markers);
      // ...
  private:
      wxBitmap render_buffer_; // Pre-rendered minimap bitmap
      void RenderToBuffer();
      void OnPaint(wxPaintEvent& event);
  };
  ```
- Render content as 1-2px colored blocks per character (scaled down)
- Double-buffered via `wxBitmap` render buffer
- Width: 80px default, configurable 40-120px
- Content re-rendered on text change (debounced 300ms)

**Files Affected:**
- `src/ui/MinimapPanel.h` (new)
- `src/ui/MinimapPanel.cpp` (new)
- `src/ui/EditorPanel.h` (replace `wxStyledTextCtrl* minimap_` with `MinimapPanel*`)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Custom minimap renders document overview
- Double-buffered for flicker-free rendering
- Width configurable
- Replaces secondary Scintilla instance
- Rendering debounced for performance

**Dependencies:** None

---

## Task 2: Syntax Coloring in Minimap

**Title:** Apply syntax-aware coloring to minimap characters

**Description:** Each character block in the minimap should be colored according to its syntax token type (keyword=blue, string=green, comment=gray, etc.) rather than uniform gray.

**Implementation Details:**
- Extract syntax tokens from the main editor's Scintilla styling:
  ```cpp
  for (int pos = 0; pos < length; ++pos) {
      int style = editor_->GetStyleAt(pos);
      wxColour color = StyleToColor(style);
      minimap_pixels_.push_back({x, y, color});
  }
  ```
- Map Scintilla style IDs to theme colors:
  - `SCE_MARKDOWN_STRONG` -> `SyntaxKeyword`
  - `SCE_MARKDOWN_EM1` -> `SyntaxString`
  - `SCE_MARKDOWN_CODE` -> `SyntaxType`
  - Default -> `TextMuted` at 60% opacity
- Scale: each character = 2px wide, 1px tall in "character" mode
- Scale: each character = 1px wide, 1px tall in "block" mode (denser)

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (implement syntax-colored rendering)

**Acceptance Criteria:**
- Keywords, strings, comments have distinct colors in minimap
- Colors match theme syntax tokens
- Updates when theme changes
- Visible color differentiation even at minimap scale

**Dependencies:** Task 1

---

## Task 3: Viewport Slider

**Title:** Draw visible region slider on the minimap

**Description:** Draw a semi-transparent rectangle over the minimap showing which portion of the document is currently visible in the editor. The slider moves as the user scrolls and can be dragged to scroll the editor.

**Implementation Details:**
- Calculate slider position:
  ```cpp
  int minimap_height = GetClientSize().GetHeight();
  double visible_fraction = static_cast<double>(lines_on_screen) / total_lines;
  int slider_height = std::max(20, static_cast<int>(minimap_height * visible_fraction));
  double scroll_fraction = static_cast<double>(first_visible_line) / total_lines;
  int slider_y = static_cast<int>(minimap_height * scroll_fraction);
  ```
- Draw slider: semi-transparent accent-colored rectangle with rounded corners
  ```cpp
  wxColour slider_color(accent.Red(), accent.Green(), accent.Blue(), 30);
  gc.SetBrush(wxBrush(slider_color));
  gc.DrawRoundedRectangle(0, slider_y, minimap_width, slider_height, 2);
  // Border
  wxColour border_color(accent.Red(), accent.Green(), accent.Blue(), 60);
  gc.SetPen(wxPen(border_color, 1));
  gc.DrawRoundedRectangle(0, slider_y, minimap_width, slider_height, 2);
  ```
- Slider drags to scroll the editor

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (implement slider drawing and drag)

**Acceptance Criteria:**
- Slider accurately represents visible region
- Slider height proportional to visible fraction
- Minimum slider height of 20px
- Semi-transparent with accent color
- Updates on scroll

**Dependencies:** Task 1

---

## Task 4: Click-to-Scroll

**Title:** Click on minimap to scroll editor to that position

**Description:** Clicking anywhere on the minimap scrolls the editor to center the clicked line in the viewport. The click position maps to a document line based on the minimap scale.

**Implementation Details:**
- In `OnMouseDown()`:
  ```cpp
  int minimap_y = event.GetPosition().y;
  int total_lines = GetTotalLines();
  int target_line = static_cast<int>(
      static_cast<double>(minimap_y) / GetClientSize().GetHeight() * total_lines);
  // Center the target line in the editor viewport
  int lines_on_screen = editor_->LinesOnScreen();
  int first_line = target_line - lines_on_screen / 2;
  editor_->SetFirstVisibleLine(std::max(0, first_line));
  ```
- Smooth scroll animation (100ms ease-out) instead of instant jump
- Hold mouse button + drag for continuous scroll tracking

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (implement click and drag-to-scroll)

**Acceptance Criteria:**
- Click on minimap scrolls editor to that position
- Clicked line centered in viewport
- Drag tracking works for continuous scrolling
- Smooth scroll animation
- Click outside the slider works correctly

**Dependencies:** Task 3

---

## Task 5: Search Result Highlights in Minimap

**Title:** Show search result matches as colored markers in the minimap

**Description:** When find/replace is active, display all search matches as yellow/accent-colored horizontal bars in the minimap, showing the distribution of matches across the document.

**Implementation Details:**
- Subscribe to `FindResultsChangedEvent` (or monitor find indicator ranges)
- For each match position, compute the minimap Y coordinate:
  ```cpp
  for (const auto& match : find_results) {
      int line = editor_->LineFromPosition(match.start);
      int minimap_y = LineToMinimapY(line);
      search_markers_.push_back({minimap_y, theme_engine_.color(ThemeColorToken::WarningColor)});
  }
  ```
- Draw markers as 2px tall, full-width horizontal lines
- Clear markers when find bar is hidden

**Files Affected:**
- `src/ui/MinimapPanel.h` (add search marker list)
- `src/ui/MinimapPanel.cpp` (draw search markers)

**Acceptance Criteria:**
- All search matches visible as markers in minimap
- Markers use highlight/warning color
- Markers appear immediately when search executes
- Markers clear when find bar closes
- Markers visible even when document is very long

**Dependencies:** Task 1

---

## Task 6: Git Change Indicators in Minimap

**Title:** Show git changes as colored markers in the minimap

**Description:** Mirror the git gutter change indicators (Phase 14 Task 6) as thin colored lines in the minimap: green for added, blue for modified, red for deleted.

**Implementation Details:**
- Subscribe to `GitGutterProvider` change data
- For each changed line range:
  ```cpp
  for (const auto& hunk : git_hunks) {
      int start_y = LineToMinimapY(hunk.start_line);
      int end_y = LineToMinimapY(hunk.end_line);
      wxColour color;
      switch (hunk.type) {
          case Added: color = success_color; break;
          case Modified: color = accent_color; break;
          case Deleted: color = error_color; break;
      }
      DrawMinimapStripe(gc, start_y, end_y, color, 3); // 3px wide stripe at left
  }
  ```
- Stripes drawn at the left edge of the minimap (3px wide)

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw git change markers)

**Acceptance Criteria:**
- Added lines: green stripe
- Modified lines: blue stripe
- Deleted lines: red marker
- Stripes at correct positions
- Update on save/git status change

**Dependencies:** Task 1, Phase 14 Task 6

---

## Task 7: Diagnostic Indicators in Minimap

**Title:** Show errors and warnings as markers in the minimap

**Description:** Display diagnostic markers (from Phase 14) in the minimap: red dots for errors, yellow dots for warnings. These help users see the distribution of problems in the file.

**Implementation Details:**
- For each diagnostic:
  ```cpp
  for (const auto& diag : diagnostic_indicators_) {
      int minimap_y = LineToMinimapY(diag.line);
      wxColour color = (diag.severity == Error) ? error_color : warning_color;
      DrawMinimapDot(gc, minimap_width - 6, minimap_y, 4, color);
  }
  ```
- Dots drawn at the right edge of the minimap (4px diameter)
- Error dots layered on top of warning dots (z-order)

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw diagnostic markers)

**Acceptance Criteria:**
- Error locations show red dots in minimap
- Warning locations show yellow dots
- Dots positioned at right edge
- Multiple diagnostics on same line show single merged dot
- Dots update when diagnostics change

**Dependencies:** Task 1, Phase 14 Task 7

---

## Task 8: Current Line Indicator in Minimap

**Title:** Show the current cursor line position in the minimap

**Description:** Draw a thin horizontal line across the minimap at the current cursor position, making it easy to locate the cursor in large documents.

**Implementation Details:**
- Subscribe to cursor position changes
- Draw a 1px accent-colored line across the full minimap width:
  ```cpp
  int cursor_y = LineToMinimapY(cursor_line);
  gc.SetPen(wxPen(accent_color, 1));
  gc.StrokeLine(0, cursor_y, minimap_width, cursor_y);
  ```
- Add a small arrow indicator at the right edge pointing to the cursor line

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw cursor line indicator)

**Acceptance Criteria:**
- Thin accent line at cursor position
- Updates in real-time as cursor moves
- Visible even within the viewport slider
- Small arrow at right edge for precise location

**Dependencies:** Task 1

---

## Task 9: Selection Highlights in Minimap

**Title:** Show selected text regions as highlights in the minimap

**Description:** When text is selected in the editor, highlight the corresponding line range in the minimap with a semi-transparent accent overlay.

**Implementation Details:**
- On selection change:
  ```cpp
  int sel_start_line = editor_->LineFromPosition(editor_->GetSelectionStart());
  int sel_end_line = editor_->LineFromPosition(editor_->GetSelectionEnd());
  int start_y = LineToMinimapY(sel_start_line);
  int end_y = LineToMinimapY(sel_end_line);
  wxColour sel_color(accent.Red(), accent.Green(), accent.Blue(), 40);
  gc.SetBrush(wxBrush(sel_color));
  gc.DrawRectangle(0, start_y, minimap_width, end_y - start_y + 1);
  ```
- Support multiple selections (multi-cursor)
- Clear when selection is empty

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw selection highlights)

**Acceptance Criteria:**
- Selected region highlighted in minimap
- Multi-selection supported
- Highlight clears when selection is empty
- Semi-transparent overlay, not opaque
- Updates in real-time

**Dependencies:** Task 1

---

## Task 10: Overview Ruler (Right Edge of Editor)

**Title:** Implement an overview ruler at the right edge of the editor

**Description:** Add a thin (12px) overview ruler strip at the far right edge of the editor (between the editor content and the minimap). This ruler shows concentrated markers for all annotations: errors, warnings, search results, bookmarks, etc. It provides the same information as minimap markers but in a thinner, always-visible strip.

**Implementation Details:**
- Create `OverviewRulerPanel` (12px wide, full editor height):
  ```cpp
  class OverviewRulerPanel : public ThemeAwareWindow {
      void OnPaint(wxPaintEvent& event);
      struct RulerMarker { int y_position; wxColour color; int height; };
      std::vector<RulerMarker> markers_;
  };
  ```
- Marker sources: errors (red), warnings (yellow), search matches (orange), bookmarks (blue), git changes (green/blue)
- Position between editor and minimap (or between editor and scrollbar if minimap is hidden)
- Markers are 2px tall, 8px wide, centered in the ruler
- Click on ruler marker scrolls to that line

**Files Affected:**
- `src/ui/OverviewRulerPanel.h` (new)
- `src/ui/OverviewRulerPanel.cpp` (new)
- `src/ui/EditorPanel.cpp` (integrate ruler into layout)
- `CMakeLists.txt`

**Acceptance Criteria:**
- Ruler visible at right edge of editor
- All annotation types represented with correct colors
- Click scrolls to the marker's line
- 12px wide, minimal visual footprint
- Hidden when all marker sources are empty

**Dependencies:** Task 1

---

## Task 11: Minimap Hover Preview

**Title:** Show a tooltip-style preview when hovering over the minimap

**Description:** When hovering over the minimap, show a floating preview panel displaying the actual code at that position with syntax highlighting. The preview shows approximately 10 lines centered on the hovered line.

**Implementation Details:**
- On mouse move over minimap:
  ```cpp
  int hover_line = MinimapYToLine(event.GetPosition().y);
  int preview_start = std::max(0, hover_line - 5);
  int preview_end = std::min(total_lines, hover_line + 5);
  ShowHoverPreview(preview_start, preview_end);
  ```
- `HoverPreview`: a `wxPopupTransientWindow` positioned to the left of the minimap:
  - Background: `BgPanel` color
  - Content: 10 lines of code with syntax highlighting (extract from editor)
  - Monospace font at editor font size
  - 2px accent border
  - Line numbers in gutter
  - Current hover line highlighted
- Auto-dismiss when mouse leaves minimap
- Debounce: 100ms before showing

**Files Affected:**
- `src/ui/MinimapPanel.h` (add hover preview state)
- `src/ui/MinimapPanel.cpp` (implement hover preview popup)

**Acceptance Criteria:**
- Hover shows 10-line preview popup
- Preview has syntax coloring
- Preview positioned to left of minimap
- Hovered line highlighted
- Debounced to avoid flicker
- Dismisses on mouse leave

**Dependencies:** Task 1

---

## Task 12: Toggle Minimap Visibility

**Title:** Add toggle command and animation for minimap visibility

**Description:** Support toggling the minimap on/off with an animated slide-in/slide-out transition. Persist the visibility setting.

**Implementation Details:**
- `void ToggleMinimapVisibility()`:
  - If visible: animate width from current to 0 over 200ms
  - If hidden: animate width from 0 to configured width over 200ms
  - Use a timer for animation frames
- Persist `minimap_visible` in config
- Register command: "View: Toggle Minimap" in command palette
- Keyboard shortcut: Cmd+Shift+M

**Files Affected:**
- `src/ui/EditorPanel.cpp` (enhance `ToggleMinimapVisibility` with animation)
- `src/core/Config.h` (persist visibility)
- `src/ui/CommandPalette.cpp` (register command)

**Acceptance Criteria:**
- Minimap slides in/out with 200ms animation
- Visibility persists across sessions
- Command available in palette and keyboard shortcut
- Editor content area resizes smoothly during toggle

**Dependencies:** Task 1

---

## Task 13: Minimap Render Mode - Blocks

**Title:** Implement block render mode for minimap

**Description:** In block render mode, each line is represented by colored rectangles of varying lengths (based on indentation and line length) without individual character detail. This is faster to render and gives a clearer structural overview.

**Implementation Details:**
- Add `enum class MinimapRenderMode { Characters, Blocks, Dots }`
- In `Blocks` mode:
  ```cpp
  for (int line = 0; line < total_lines; ++line) {
      int indent = GetLineIndentation(line);
      int length = GetLineLength(line);
      int block_x = indent * char_scale;
      int block_w = length * char_scale;
      wxColour color = GetDominantColorForLine(line);
      gc.SetBrush(wxBrush(color));
      gc.DrawRectangle(block_x, line * line_scale, block_w, line_scale);
  }
  ```
- `GetDominantColorForLine()`: returns the most common syntax color on that line
- Blocks are 1px tall, variable width

**Files Affected:**
- `src/ui/MinimapPanel.h` (add render mode enum)
- `src/ui/MinimapPanel.cpp` (implement block rendering)

**Acceptance Criteria:**
- Block mode renders colored rectangles per line
- Indentation visible as left margin
- Line length visible as block width
- Dominant syntax color used per block
- Faster rendering than character mode

**Dependencies:** Task 2

---

## Task 14: Minimap Render Mode - Characters

**Title:** Implement character render mode for minimap

**Description:** In character render mode, each character is rendered as a 1-2px colored dot, providing high-fidelity miniature representation of the code.

**Implementation Details:**
- Each character rendered at configured scale (default: 1px wide, 2px tall)
- Character color from syntax token
- Whitespace not rendered (transparent)
- Tab characters: skip to next tab stop
- Maximum characters per line: minimap width / character scale
- Pre-render entire document to `wxBitmap` on content change

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (implement character rendering)

**Acceptance Criteria:**
- Individual characters visible as colored dots
- Syntax coloring per character
- Whitespace gaps visible
- Pre-rendered for scroll performance
- Scales with minimap width setting

**Dependencies:** Task 2

---

## Task 15: Minimap Scale Configuration

**Title:** Configure minimap character scale and width

**Description:** Allow users to configure the minimap rendering scale (how many pixels per character) and total width. Provide presets: "Small" (40px/1x), "Medium" (80px/2x), "Large" (120px/3x).

**Implementation Details:**
- Settings:
  ```yaml
  minimap:
    enabled: true
    render_mode: "characters"  # or "blocks"
    width: 80
    scale: 2
    side: "right"  # or "left"
    show_slider: true
    max_column: 120
  ```
- Scale affects character rendering: `char_width = scale`, `line_height = scale`
- Width constrains total minimap panel width
- `max_column`: only render up to N characters per line
- Presets available in command palette

**Files Affected:**
- `src/ui/MinimapPanel.h` (add configuration)
- `src/ui/MinimapPanel.cpp` (apply configuration to rendering)
- `src/core/Config.h` (add minimap settings)

**Acceptance Criteria:**
- Scale, width, and max_column configurable
- Presets for Small/Medium/Large
- Settings persist across sessions
- Rendering updates immediately on setting change

**Dependencies:** Tasks 13, 14

---

## Task 16: Minimap Left-Side Placement

**Title:** Support minimap on the left side of the editor

**Description:** Allow the minimap to be positioned on the left side of the editor instead of the default right side, matching some users' preference.

**Implementation Details:**
- When `minimap_side == "left"`:
  - Change sizer order: minimap | editor (instead of editor | minimap)
  - Overview ruler moves to left side of editor
  - Slider slider still appears on the minimap
- Toggle via command palette: "Minimap: Toggle Side"

**Files Affected:**
- `src/ui/EditorPanel.cpp` (adjust sizer ordering based on side setting)
- `src/ui/MinimapPanel.cpp` (adjust marker positions for left-side mode)

**Acceptance Criteria:**
- Minimap can be on left or right side
- Layout adjusts correctly when toggled
- Overview ruler follows the minimap
- Setting persists

**Dependencies:** Task 1

---

## Task 17: Bookmark Indicators in Minimap

**Title:** Show bookmark markers in the minimap

**Description:** Display bookmarks (from Phase 14 Task 9) as blue diamond markers in the minimap.

**Implementation Details:**
- For each bookmark:
  ```cpp
  for (int bookmark_line : bookmarks_) {
      int y = LineToMinimapY(bookmark_line);
      DrawMinimapDiamond(gc, minimap_width - 8, y, 6, bookmark_color);
  }
  ```
- Diamond shape: small 6px diamond at right edge
- Blue color from `AccentPrimary`
- Click diamond to scroll to bookmark

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw bookmark markers)

**Acceptance Criteria:**
- Bookmarks shown as blue diamonds
- Correct positions in minimap
- Clickable to scroll
- Update when bookmarks change

**Dependencies:** Task 1, Phase 14 Task 9

---

## Task 18: Breakpoint Indicators in Minimap

**Title:** Show breakpoint markers in the minimap

**Description:** Display breakpoints as red circles in the minimap left edge.

**Implementation Details:**
- For each breakpoint:
  ```cpp
  for (const auto& bp : breakpoints_) {
      if (!bp.enabled) continue;
      int y = LineToMinimapY(bp.line);
      wxColour color = bp.condition.empty() ? error_color : warning_color;
      DrawMinimapCircle(gc, 4, y, 3, color);
  }
  ```
- Red for regular breakpoints, orange for conditional
- Positioned at left edge of minimap

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (draw breakpoint markers)

**Acceptance Criteria:**
- Breakpoints shown as colored circles
- Regular: red, Conditional: orange
- Left edge positioning
- Update on breakpoint change

**Dependencies:** Task 1, Phase 14 Task 5

---

## Task 19: Minimap Drag-to-Scroll

**Title:** Enable dragging the viewport slider to scroll the editor

**Description:** The viewport slider on the minimap should be draggable. Clicking and dragging the slider scrolls the editor proportionally.

**Implementation Details:**
- In `OnMouseDown()`: if click is within slider bounds, start drag mode
- In `OnMouseMove()` during drag:
  ```cpp
  int delta_y = event.GetPosition().y - drag_start_y_;
  double delta_fraction = static_cast<double>(delta_y) / GetClientSize().GetHeight();
  int new_first_line = drag_start_line_ + static_cast<int>(delta_fraction * total_lines);
  editor_->SetFirstVisibleLine(std::clamp(new_first_line, 0, total_lines - lines_on_screen));
  ```
- Visual feedback: slider darkens slightly during drag
- Mouse capture during drag to handle mouse leaving minimap bounds

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (implement slider drag)

**Acceptance Criteria:**
- Slider draggable to scroll editor
- Scroll is proportional to drag distance
- Mouse capture prevents losing drag on fast movement
- Visual feedback during drag
- Smooth scrolling during drag

**Dependencies:** Task 3

---

## Task 20: Minimap Content Caching

**Title:** Optimize minimap rendering with bitmap caching

**Description:** Pre-render the minimap content to a `wxBitmap` and only re-render on content changes, theme changes, or minimap resize. Paint events simply blit the cached bitmap and draw dynamic overlays (slider, markers, cursor line).

**Implementation Details:**
- `wxBitmap content_cache_`: the pre-rendered document content
- `bool cache_dirty_{true}`: flag to trigger re-render
- Mark cache dirty on:
  - Content change (debounced 300ms)
  - Theme change
  - Minimap resize
  - Render mode change
- `OnPaint()`:
  ```cpp
  if (cache_dirty_) {
      RenderContentToCache();
      cache_dirty_ = false;
  }
  dc.DrawBitmap(content_cache_, 0, 0);
  // Draw dynamic overlays (slider, markers, cursor) on top
  DrawSlider(gc);
  DrawMarkers(gc);
  DrawCursorLine(gc);
  ```

**Files Affected:**
- `src/ui/MinimapPanel.h` (add cache members)
- `src/ui/MinimapPanel.cpp` (implement caching strategy)

**Acceptance Criteria:**
- Content renders once and is cached as bitmap
- Dynamic elements drawn on top of cached bitmap
- Cache invalidated on content/theme/size change
- Smooth scrolling performance (no re-render needed)
- Memory efficient for large files

**Dependencies:** Tasks 2, 13, 14

---

## Task 21: Minimap Fold Region Indicators

**Title:** Show fold region boundaries in the minimap

**Description:** Display visual indicators for collapsed fold regions in the minimap. Collapsed regions show as a horizontal line with a fold icon, indicating content is hidden.

**Implementation Details:**
- For each collapsed fold:
  ```cpp
  int fold_line = GetFoldedLineNumber();
  int y = LineToMinimapY(fold_line);
  gc.SetPen(wxPen(theme_engine_.color(ThemeColorToken::TextMuted), 1, wxPENSTYLE_DOT));
  gc.StrokeLine(0, y, minimap_width, y);
  // Draw small fold icon
  gc.DrawText("+", minimap_width / 2 - 3, y - 4);
  ```
- Collapsed lines represented as zero-height in the minimap (the minimap accurately reflects visible line count, not total)

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (handle fold regions in rendering)

**Acceptance Criteria:**
- Collapsed regions shown as dotted lines
- Minimap height reflects visible (unfolded) line count
- Fold indicators positioned correctly
- Expand/collapse updates minimap immediately

**Dependencies:** Task 1

---

## Task 22: Minimap Smooth Scroll Sync

**Title:** Synchronize minimap position with editor scroll smoothly

**Description:** Ensure the minimap viewport slider updates smoothly as the editor scrolls, without jitter or lag.

**Implementation Details:**
- Subscribe to Scintilla's `SCN_UPDATEUI` event with `SC_UPDATE_V_SCROLL` flag
- On scroll event:
  ```cpp
  int first_visible = editor_->GetFirstVisibleLine();
  int lines_on_screen = editor_->LinesOnScreen();
  minimap_->SetViewportRange(first_visible, first_visible + lines_on_screen, total_lines);
  minimap_->Refresh();
  ```
- Use `wxWindow::Update()` after refresh for immediate repaint
- If smooth scrolling is enabled on the editor, interpolate minimap position

**Files Affected:**
- `src/ui/EditorPanel.cpp` (sync minimap on scroll events)
- `src/ui/MinimapPanel.cpp` (smooth viewport update)

**Acceptance Criteria:**
- Minimap slider moves in sync with editor scroll
- No visible lag between editor and minimap
- Smooth movement during rapid scrolling
- Works with both keyboard and mouse scrolling

**Dependencies:** Task 3

---

## Task 23: Minimap Marker Legend

**Title:** Show a legend tooltip explaining minimap marker colors

**Description:** When the user hovers over the minimap margin area (the first or last 10px), show a tooltip legend explaining what each marker color means.

**Implementation Details:**
- On hover near the minimap edges, show a tooltip:
  ```
  Minimap Markers:
  * Red dot: Error
  * Yellow dot: Warning
  * Blue diamond: Bookmark
  * Green stripe: Added line (git)
  * Blue stripe: Modified line (git)
  * Orange highlight: Search match
  * Purple line: Cursor position
  ```
- Only show when hovering near the marker areas (right edge or left edge)

**Files Affected:**
- `src/ui/MinimapPanel.cpp` (add legend tooltip)

**Acceptance Criteria:**
- Legend tooltip shows on edge hover
- All marker types listed with colors
- Tooltip auto-dismisses on mouse move away
- Only shows when markers are present

**Dependencies:** Tasks 5-9, 17, 18

---

## Task 24: Minimap Performance for Large Files

**Title:** Optimize minimap for files with 50K+ lines

**Description:** Ensure the minimap performs well for very large files by rendering only the visible portion plus a buffer, and using progressive detail levels.

**Implementation Details:**
- For files > 10K lines:
  - Reduce to block mode automatically
  - Render at half resolution (2px per line instead of 1px)
  - Only cache the viewport + 500 lines of context
  - Use LOD (level of detail): zoom out shows less detail
- For files > 50K lines:
  - Use 4px per line (maximum compression)
  - Skip marker rendering for offscreen lines
  - Limit marker count to nearest 100 per type
- Add `MinimapPerformanceProfile` enum: `Normal`, `LargeFile`, `HugeFile`

**Files Affected:**
- `src/ui/MinimapPanel.h` (add performance profile)
- `src/ui/MinimapPanel.cpp` (implement adaptive rendering)

**Acceptance Criteria:**
- Smooth scrolling with 50K line file
- Automatic LOD reduction for large files
- Rendering time < 16ms per frame
- Memory usage bounded regardless of file size
- Visual quality gracefully degrades

**Dependencies:** Tasks 13, 14, 20

---

## Task 25: Minimap Integration Tests

**Title:** Write tests for minimap coordinate mapping and marker placement

**Description:** Create unit tests for the minimap's core logic: line-to-pixel mapping, pixel-to-line mapping, viewport calculation, marker positioning, and cache invalidation.

**Implementation Details:**
- Create `tests/unit/test_minimap.cpp`:
  - Test `LineToMinimapY()` for various document sizes and minimap heights
  - Test `MinimapYToLine()` (inverse mapping)
  - Test viewport slider height calculation
  - Test marker positioning for edge cases (first line, last line, empty document)
  - Test cache invalidation triggers
  - Test performance profile selection for different file sizes

**Files Affected:**
- `tests/unit/test_minimap.cpp` (new)
- `CMakeLists.txt` (add test target)

**Acceptance Criteria:**
- Coordinate mapping tests cover edge cases
- Viewport calculation verified for small and large documents
- Marker positioning tested at boundaries
- All tests pass in CI

**Dependencies:** Tasks 1-24

---

## Estimated Complexity

| Area | Effort |
|------|--------|
| Renderer Architecture (Task 1) | High |
| Syntax Coloring (Task 2) | High |
| Viewport Slider (Tasks 3, 19) | Medium |
| Click-to-Scroll (Task 4) | Medium |
| Search/Git/Diagnostic Markers (Tasks 5-7) | Medium |
| Current Line/Selection (Tasks 8-9) | Low |
| Overview Ruler (Task 10) | High |
| Hover Preview (Task 11) | High |
| Toggle/Animation (Task 12) | Medium |
| Render Modes (Tasks 13-14) | High |
| Configuration (Task 15) | Low |
| Left-Side Placement (Task 16) | Low |
| Bookmark/Breakpoint Markers (Tasks 17-18) | Low |
| Content Caching (Task 20) | Medium |
| Fold Indicators (Task 21) | Medium |
| Scroll Sync (Task 22) | Low |
| Legend (Task 23) | Low |
| Performance (Task 24) | High |
| Tests (Task 25) | Medium |

## Files Created

- `src/ui/MinimapPanel.h`
- `src/ui/MinimapPanel.cpp`
- `src/ui/OverviewRulerPanel.h`
- `src/ui/OverviewRulerPanel.cpp`
- `tests/unit/test_minimap.cpp`

## Files Modified

- `src/ui/EditorPanel.h`
- `src/ui/EditorPanel.cpp`
- `src/core/Config.h`
- `src/ui/CommandPalette.cpp`
- `CMakeLists.txt`
