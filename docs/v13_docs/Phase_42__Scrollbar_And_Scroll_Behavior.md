# Phase 42 -- Scrollbar and Scroll Behavior

## Objective

Overhaul the scrollbar system to match VSCode/WebStorm quality: thin overlay scrollbars that expand on hover, themed track with decorations (search matches, git changes, diagnostics), smooth scroll physics with momentum and overscroll bounce, and scroll position persistence per file. Build on the existing ThemedScrollbar infrastructure and extend it to all scrollable panels across the application.

## Prerequisites

- Phase 41 complete (Component Library Polish)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` -- existing 8px scrollbar with auto-hide timer
- `/Users/ryanrentfro/code/markamp/src/ui/ThemeAwareWindow.h` -- base class for themed controls
- `/Users/ryanrentfro/code/markamp/src/core/FrameScheduler.h` -- frame-budget-aware scheduling
- `/Users/ryanrentfro/code/markamp/src/core/AdaptiveThrottle.h` -- typing/idle mode switching
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h` -- performance monitoring

## VSCode / WebStorm Reference Behavior

- Default scrollbar width: 10px, shrinks to 6px when not interacting
- Overlay mode: scrollbar appears on scroll, fades after 1.5s of inactivity
- Scroll track shows colored markers for: search result positions (orange), git modified lines (blue), git added lines (green), diagnostics errors (red), diagnostics warnings (yellow)
- Smooth scrolling: 120ms ease-out animation per scroll step
- Momentum scrolling on trackpad with deceleration curve
- Overscroll bounce: 10px elastic bounce at content boundaries
- Minimap (optional): renders the full document as a zoomed-out column, acts as scrollbar alternative
- Horizontal scrollbar only appears when content exceeds viewport width

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollPhysicsEngine.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollPhysicsEngine.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MinimapScrollbar.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/MinimapScrollbar.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollPositionStore.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/ScrollPositionStore.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/HorizontalScrollbar.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/HorizontalScrollbar.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_scroll_system.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Refactor ThemedScrollbar to Support Variable Width

**Description:** Modify the existing ThemedScrollbar to support dynamic width that transitions between 6px (resting) and 10px (hover/active). The current hardcoded `kWidth = 8` must become a range with animated transitions.

**Implementation Details:** Replace the static `kWidth = 8` constant with `kWidthResting = 6` and `kWidthExpanded = 10`. Add a `float current_width_` member that interpolates between the two values. On mouse-enter, start a 100ms animation from resting to expanded. On mouse-leave (when not dragging), animate back to resting. Use the AnimationDriver from Phase 41 Task 19 for the interpolation. The thumb rect calculation must use `current_width_` instead of the fixed constant.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.cpp` (modify)

**Acceptance Criteria:**
- Scrollbar renders at 6px when idle
- Scrollbar expands to 10px on hover with smooth animation
- Scrollbar stays expanded while dragging even if mouse moves off
- Animation completes in 100ms with ease-out curve
- No visual glitches during width transition

**Dependencies:** Phase 41 Task 19 (AnimationDriver)

---

### Task 02 -- Implement Overlay Scrollbar Mode

**Description:** Add an overlay mode where the scrollbar is transparent and invisible by default, fading in when the user scrolls and fading out after 1.5s of scroll inactivity. The scrollbar floats over content rather than taking layout space.

**Implementation Details:** Add `ScrollbarMode` enum (`Classic`, `Overlay`). In Overlay mode, the ThemedScrollbar renders with a transparent background (no track fill) and the thumb uses 50% opacity when idle, 80% on hover, 100% when dragging. The auto-hide timer (already exists at `kAutoHideDelayMs = 1500`) drives fade-out using `thumb_opacity_`. The scrollbar must not consume layout width -- it overlaps the content area by positioning itself with `SetPosition()` at the right edge of the parent. Parent panels need `wxCLIP_CHILDREN` to prevent flicker.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.cpp` (modify)

**Acceptance Criteria:**
- Overlay mode renders scrollbar over content without taking layout space
- Scrollbar fades in on scroll events
- Scrollbar fades out after 1.5s of inactivity
- Thumb opacity transitions smoothly (not abrupt)
- User can switch between Classic and Overlay modes via settings

**Dependencies:** Task 01

---

### Task 03 -- Create ScrollDecorationProvider Interface

**Description:** Define an interface for providing colored markers on the scrollbar track. Each decoration represents a line position with a color, allowing search results, git changes, and diagnostics to be visualized on the scrollbar.

**Implementation Details:** Create `IScrollDecorationProvider` interface with method `decorations(int total_lines) -> std::vector<ScrollDecoration>`. `ScrollDecoration` struct: `{ int line; wxColour color; ScrollDecorationShape shape; }` where shape is `Line` (1px), `Block` (3px), or `Arrow`. Create concrete providers: `SearchMatchDecorationProvider` (subscribes to `SearchResultsEvent`, returns orange markers), `GitChangeDecorationProvider` (subscribes to `GitDiffEvent`, returns green/blue markers), `DiagnosticsDecorationProvider` (subscribes to `DiagnosticsEvent`, returns red/yellow markers).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.cpp` (create)

**Acceptance Criteria:**
- IScrollDecorationProvider interface compiles with all three concrete implementations
- SearchMatchDecorationProvider returns correct line positions from search results
- GitChangeDecorationProvider maps diff hunks to line positions
- DiagnosticsDecorationProvider maps error/warning locations to line positions
- Decorations update reactively when source data changes

**Dependencies:** None (interface definition)

---

### Task 04 -- Render Scroll Track Decorations

**Description:** Integrate ScrollDecorationProvider with ThemedScrollbar to paint colored markers on the scroll track alongside the thumb.

**Implementation Details:** ThemedScrollbar holds a `std::vector<IScrollDecorationProvider*>` registered via `add_decoration_provider()`. During OnPaint, after drawing the track background and before drawing the thumb, iterate all providers and call `decorations(total_lines)`. For each decoration, map the line number to a Y pixel position using `content_to_pixel()`, then draw a horizontal line or block at that position using the decoration's color. Decorations render at 2px height for Line shape, 4px for Block. Multiple decorations at the same position stack (draw on top of each other with later providers winning).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ThemedScrollbar.cpp` (modify)

**Acceptance Criteria:**
- Search match markers render as orange lines on the scroll track
- Git change markers render as green (added) and blue (modified) lines
- Diagnostic markers render as red (error) and yellow (warning) lines
- Markers update when search results, git status, or diagnostics change
- Markers are visible in both Classic and Overlay scrollbar modes

**Dependencies:** Tasks 02, 03

---

### Task 05 -- Create ScrollPhysicsEngine for Smooth Scrolling

**Description:** Implement a physics engine that provides smooth scrolling with configurable duration and easing, momentum scrolling for trackpad gestures, and overscroll bounce at content boundaries.

**Implementation Details:** ScrollPhysicsEngine is a standalone class (no wxWidgets dependency for testability). It manages: (1) `smooth_scroll(from, to, duration_ms, easing)` for discrete scroll steps (mouse wheel), (2) `momentum_scroll(velocity)` for trackpad fling with exponential deceleration (`velocity * e^(-decay * t)`), (3) `bounce(overshoot_pixels)` for elastic overscroll (spring physics: `F = -k * x`, max 10px). The engine exposes `update(delta_ms) -> double new_position` for each frame. The caller (scrollable panel) calls this in its frame callback. Deceleration constant: `decay = 3.0` (reaches 95% stop in 1s). Spring constant: `k = 0.3` with damping `0.8`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollPhysicsEngine.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollPhysicsEngine.cpp` (create)

**Acceptance Criteria:**
- Smooth scroll completes in configured duration with correct easing
- Momentum scroll decelerates to stop within 1-2 seconds
- Overscroll bounce springs back to boundary within 300ms
- All physics calculations produce deterministic results for same inputs
- Unit test verifies scroll position at t=0, t=half, t=end for each mode

**Dependencies:** None (pure computation)

---

### Task 06 -- Integrate ScrollPhysicsEngine with Editor Panel

**Description:** Wire ScrollPhysicsEngine into EditorPanel so mouse wheel events produce smooth scrolling and trackpad events produce momentum scrolling with overscroll bounce.

**Implementation Details:** In EditorPanel, intercept `wxEVT_MOUSEWHEEL` events. Instead of directly setting scroll position, feed the scroll delta into `ScrollPhysicsEngine::smooth_scroll()`. Register a FrameScheduler task (TaskPriority::Paint) that calls `engine.update(delta)` each frame and sets the editor scroll position. For trackpad events (detected via `wxMouseEvent::IsPageScroll()` returning false and rotation being fine-grained), use `momentum_scroll()` instead. Detect overscroll (position < 0 or position > max) and trigger `bounce()`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.h` (modify)

**Acceptance Criteria:**
- Mouse wheel produces smooth animated scrolling (120ms per step)
- Trackpad swipe produces momentum scrolling that decelerates naturally
- Scrolling past top or bottom boundary produces elastic bounce
- Scrolling remains 60fps during smooth scroll animation
- Performance budget is not violated during scroll animation

**Dependencies:** Task 05, Phase 41 Task 19 (AnimationDriver)

---

### Task 07 -- Implement Scroll Position Persistence Per File

**Description:** Create a store that remembers the scroll position for each open file so that switching tabs or reopening a file restores the user to their previous position.

**Implementation Details:** ScrollPositionStore manages a `std::unordered_map<string, ScrollPositionEntry>` where the key is the document_id (file path). `ScrollPositionEntry` stores: `scroll_line`, `scroll_column`, `cursor_line`, `cursor_column`, and `fold_state` (vector of folded line ranges). The store subscribes to EventBus events: on `DocumentFocusLostEvent`, save current position; on `DocumentFocusGainedEvent`, restore saved position. The store persists to a JSON file at `{workspace}/.markamp/scroll_positions.json` on application close and loads on startup.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollPositionStore.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollPositionStore.cpp` (create)

**Acceptance Criteria:**
- Switching between tabs restores scroll and cursor position
- Closing and reopening the application restores positions for previously open files
- Positions are stored per-workspace (not global)
- Store handles missing files gracefully (returns default position)
- Fold state is preserved across tab switches

**Dependencies:** None

---

### Task 08 -- Implement Horizontal Scrollbar

**Description:** Create a horizontal scrollbar that appears only when content exceeds the viewport width. Uses the same visual style as the vertical ThemedScrollbar but oriented horizontally.

**Implementation Details:** HorizontalScrollbar extends ThemeAwareWindow (does not extend ThemedScrollbar to avoid orientation complexity). Renders a horizontal track at the bottom of the parent panel. Width transitions: 6px height resting, 10px on hover. Thumb width is proportional to `viewport_width / content_width`. Only becomes visible when `content_width > viewport_width`. Supports the same overlay mode and decoration providers as the vertical scrollbar. Positioned at the bottom-left corner, spanning the full parent width minus the vertical scrollbar width.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/HorizontalScrollbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/HorizontalScrollbar.cpp` (create)

**Acceptance Criteria:**
- Horizontal scrollbar only appears when content exceeds viewport width
- Scrollbar hides when content fits within viewport
- Thumb width proportionally represents visible portion
- Drag and click-on-track work correctly for horizontal scrolling
- Corner area (where vertical and horizontal scrollbars meet) renders cleanly

**Dependencies:** Task 01

---

### Task 09 -- Deploy Scrollbars to All Panels

**Description:** Replace native scrollbars with ThemedScrollbar/HorizontalScrollbar in all scrollable panels: editor, file tree, output panel, problems panel, search results, extension browser, settings panel.

**Implementation Details:** For each scrollable panel, hide the native wxScrollBar by setting `wxScrolledWindow` styles to `wxVSCROLL | wxHSCROLL` with hidden scrollbars (via `ShowScrollbars(wxSHOW_SB_NEVER, wxSHOW_SB_NEVER)`). Create ThemedScrollbar and HorizontalScrollbar as sibling overlay controls. Bind the panel's scroll events to update the scrollbar positions. Bind the scrollbar's drag events to scroll the panel content. This wiring should be extracted into a reusable `ScrollbarAdapter` helper class.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ProblemsPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SearchPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/ExtensionsBrowserPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (modify)

**Acceptance Criteria:**
- All panels use themed scrollbars instead of native
- ScrollbarAdapter reusable class eliminates duplicated wiring code
- All panels support both Classic and Overlay scrollbar modes
- Scrollbar decorations work in editor panel (search, git, diagnostics)
- No scrolling regressions in any panel

**Dependencies:** Tasks 01, 02, 04, 08

---

### Task 10 -- Implement Minimap as Scrollbar Alternative

**Description:** Create a minimap panel that renders a zoomed-out view of the entire document as a narrow column on the right side of the editor, functioning as both a document overview and an alternative scroll mechanism.

**Implementation Details:** MinimapScrollbar extends ThemeAwareWindow. Width: 60px. Renders each line of the document as a 1-2px tall strip using simplified color mapping (keyword color, string color, comment color, default color from the syntax highlighter). The visible viewport is shown as a semi-transparent highlighted rectangle. Clicking on the minimap scrolls to that position. Dragging the viewport rectangle scrolls proportionally. The minimap subscribes to `DocumentContentChangedEvent` and `SyntaxHighlightCompleteEvent` to update its rendering. Rendering is done to a cached wxBitmap that is only regenerated when content changes (not on every paint).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/MinimapScrollbar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/MinimapScrollbar.cpp` (create)

**Acceptance Criteria:**
- Minimap renders entire document as colored strips
- Viewport indicator shows current visible region
- Clicking minimap scrolls editor to clicked position
- Dragging viewport indicator scrolls proportionally
- Minimap updates when document content changes
- Cached bitmap prevents re-rendering on every frame

**Dependencies:** Task 04 (decoration rendering concept reused)

---

### Task 11 -- Add Scroll-to-Top and Scroll-to-Bottom Shortcuts

**Description:** Implement keyboard shortcuts for scrolling to the top and bottom of the document, with smooth scroll animation.

**Implementation Details:** Register two commands in the Command system: `editor.scrollToTop` (Cmd+Home on macOS, Ctrl+Home on Windows/Linux) and `editor.scrollToBottom` (Cmd+End / Ctrl+End). Both commands feed a long-distance smooth scroll into the ScrollPhysicsEngine with a 300ms duration and ease-in-out-quad easing. The cursor should optionally follow (configurable via `editor.scrollToCursorOnBoundary` setting, default true).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Cmd+Home scrolls to top with smooth animation
- Cmd+End scrolls to bottom with smooth animation
- Animation completes in 300ms with ease-in-out curve
- Cursor follows scroll when setting is enabled
- Commands appear in Command Palette

**Dependencies:** Task 06

---

### Task 12 -- Implement Scroll Snap for Page Up/Down

**Description:** Make Page Up and Page Down scroll by exactly one viewport height minus 2 lines (context overlap), with smooth animation.

**Implementation Details:** Override `wxEVT_KEY_DOWN` for Page Up/Page Down in EditorPanel. Calculate target position as `current_position +/- (viewport_lines - 2)`. Feed into `ScrollPhysicsEngine::smooth_scroll()` with 200ms duration. Ensure the cursor also moves to maintain its relative position within the viewport. If the target position would go past document boundaries, clamp to the boundary.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Page Down scrolls exactly viewport_height - 2 lines
- Page Up scrolls exactly viewport_height - 2 lines upward
- 2-line overlap provides reading context continuity
- Scroll animation completes in 200ms
- Cursor position updates to maintain relative viewport position

**Dependencies:** Task 06

---

### Task 13 -- Implement Touch/Trackpad Two-Finger Scroll

**Description:** Add native support for two-finger trackpad scrolling with inertial momentum on macOS and smooth scroll on Windows/Linux.

**Implementation Details:** On macOS, use `wxEVT_GESTURE_PAN` (or fall back to `wxEVT_MOUSEWHEEL` with fine-grained rotation values) to detect trackpad gestures. Pass the gesture velocity to `ScrollPhysicsEngine::momentum_scroll()`. On Windows, handle `WM_GESTURE` via the platform abstraction layer. On Linux, handle `GDK_SCROLL_SMOOTH` events. The key is to distinguish between discrete mouse wheel clicks (fixed delta) and continuous trackpad gestures (variable velocity) and route them to different ScrollPhysicsEngine methods.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/platform/MacPlatform.mm` (modify)

**Acceptance Criteria:**
- Two-finger trackpad scroll feels natural with momentum
- Discrete mouse wheel clicks produce fixed smooth-scroll steps
- Momentum decelerates naturally over 1-2 seconds
- Scroll direction respects OS "Natural Scrolling" preference
- No jitter or stuttering during fast trackpad swipes

**Dependencies:** Task 06

---

### Task 14 -- Implement Overscroll Bounce Visual Effect

**Description:** When the user scrolls past the top or bottom of the document, render an elastic bounce effect that pulls the content back to the boundary.

**Implementation Details:** When `ScrollPhysicsEngine::update()` returns a position beyond content boundaries, render the overscroll as a visual offset on the content panel (translate the content by the overscroll amount). The bounce spring physics from Task 05 handle the return animation. Maximum overscroll distance is 10px. The overscroll region renders with a subtle gradient overlay (10% accent color fading to transparent) to indicate the boundary. Overscroll can be disabled via `editor.overscrollBounce` setting (default true).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Scrolling past top shows elastic bounce up to 10px
- Scrolling past bottom shows elastic bounce up to 10px
- Content springs back to boundary within 300ms
- Subtle gradient overlay indicates boundary
- Bounce can be disabled via settings

**Dependencies:** Task 06

---

### Task 15 -- Add Scrollbar Settings to Configuration

**Description:** Expose scrollbar configuration options in the Settings panel: scrollbar mode (Classic/Overlay), smooth scrolling (on/off), smooth scroll duration, minimap visibility, overscroll bounce (on/off), scroll speed multiplier.

**Implementation Details:** Add settings to Config with keys: `editor.scrollbar.mode` (string: "classic" | "overlay"), `editor.scrollbar.smoothScrolling` (bool, default true), `editor.scrollbar.smoothScrollDuration` (int, default 120), `editor.scrollbar.minimap` (bool, default true), `editor.scrollbar.overscrollBounce` (bool, default true), `editor.scrollbar.scrollSpeed` (double, default 1.0). Create a "Scrollbar" subsection in the Settings panel using the new ThemedDropdown and ThemedToggleSwitch controls from Phase 41.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/SettingsPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/core/Config.h` (modify, add default keys)

**Acceptance Criteria:**
- All scrollbar settings appear in Settings panel under "Editor > Scrollbar"
- Changing mode switches between Classic and Overlay immediately
- Toggling smooth scrolling enables/disables animation
- Minimap toggle shows/hides the minimap column
- Settings persist across application restarts

**Dependencies:** Tasks 02, 10, 14, Phase 41 Task 06 (ThemedDropdown)

---

### Task 16 -- Implement Scroll Position Indicator in Status Bar

**Description:** Display the current scroll position (line number / total lines, and percentage) in the status bar, updating in real time as the user scrolls.

**Implementation Details:** Add a new status bar segment in StatusBarPanel that subscribes to `ScrollPositionChangedEvent` (new event type). Display format: "Ln {line}, Col {col}" (already exists) plus "({percentage}%)" appended. The scroll position change event is published by EditorPanel whenever the scroll position changes (debounced to avoid flooding EventBus during smooth scroll -- use the existing CoalescingTask from core).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Status bar shows current line and percentage
- Updates in real time during scrolling
- Debounced to prevent EventBus flooding (max 10 updates/second)
- Percentage shows 0% at top, 100% at bottom
- No performance impact from status bar updates during fast scrolling

**Dependencies:** Task 06

---

### Task 17 -- Synchronized Scroll for Split Panes

**Description:** When two panes show related documents (e.g., side-by-side diff, or same file in split view), synchronize their scroll positions so scrolling one pane scrolls the other.

**Implementation Details:** Add a `ScrollSyncGroup` concept to PaneManager. Panes in the same sync group share scroll events. When a pane in a sync group scrolls, it publishes `ScrollSyncEvent { group_id, scroll_position, source_pane_id }`. Other panes in the same group receive this event and update their scroll position (ignoring events from themselves to prevent loops). Sync is line-based (not pixel-based) to handle different font sizes. Sync can be toggled per pane pair via a button in the editor tab bar.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/PaneManager.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Scrolling one pane in a sync group scrolls the other
- Sync toggle button appears in editor tab bar when panes are split
- No infinite scroll loops between synced panes
- Sync is line-based and works across different zoom levels
- Disabling sync stops scroll propagation immediately

**Dependencies:** Task 06

---

### Task 18 -- Add CMake Integration and Unit Tests

**Description:** Add all new scroll system source files to CMakeLists.txt and create comprehensive unit tests for ScrollPhysicsEngine, ScrollPositionStore, ScrollDecorationProvider, and ScrollbarAdapter.

**Implementation Details:** Add all new .h and .cpp files to CMakeLists.txt add_executable and source_group targets. Create `test_scroll_system.cpp` using Catch2 with sections: (1) ScrollPhysicsEngine smooth scroll produces correct positions at key timestamps, (2) momentum scroll decelerates to zero, (3) overscroll bounce returns to boundary, (4) ScrollPositionStore saves and restores positions, (5) ScrollPositionStore handles missing files, (6) ScrollDecorationProvider maps line numbers to pixel positions correctly, (7) ScrollbarAdapter wires panel scroll to scrollbar position.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_scroll_system.cpp` (create)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles all new scroll sources without errors
- All Catch2 test sections pass
- ScrollPhysicsEngine tests are deterministic (no time-dependent flakiness)
- No undefined symbol errors
- source_group entries match add_executable for all new files

**Dependencies:** Tasks 01-17

---

### Task 19 -- Scroll Performance Benchmarking

**Description:** Measure and validate that smooth scrolling maintains 60fps under all conditions: large files (100K+ lines), files with many decorations (1000+ search matches), and during active typing.

**Implementation Details:** Create a benchmark test using the existing EditorPerformanceBudget infrastructure. Open a 100K-line test file. Simulate continuous smooth scrolling for 5 seconds. Record frame times. Assert that 95th percentile frame time is under 16ms. Repeat with 1000 search match decorations active. Repeat while simulating keystroke input (via AdaptiveThrottle in Typing mode). Log results to a performance report file.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_scroll_system.cpp` (extend)

**Acceptance Criteria:**
- 95th percentile frame time under 16ms during smooth scroll
- Decorations do not measurably impact scroll performance
- Scroll remains smooth during active typing (AdaptiveThrottle manages budget)
- Performance results are logged for CI tracking
- No frame drops during minimap concurrent rendering

**Dependencies:** Tasks 06, 10, 16

---

### Task 20 -- Scroll Acceleration for Mouse Wheel

**Description:** Implement scroll acceleration so rapid mouse wheel scrolling covers more distance per step, matching the behavior of modern editors.

**Implementation Details:** Track the time between consecutive `wxEVT_MOUSEWHEEL` events. If events arrive faster than 50ms apart, multiply the scroll delta by an acceleration factor that increases up to 3x. The acceleration curve: `factor = min(3.0, 1.0 + (50 - interval_ms) / 50.0)`. When events slow down (interval > 100ms), reset factor to 1.0. This only applies to discrete mouse wheel events, not trackpad gestures. The acceleration is configurable via `editor.scrollbar.scrollAcceleration` (bool, default true).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Rapid scrolling accelerates up to 3x speed
- Slow scrolling maintains 1x speed
- Acceleration resets after 100ms pause
- Acceleration only applies to mouse wheel, not trackpad
- Can be disabled via settings

**Dependencies:** Task 06

---

### Task 21 -- Implement Scroll Margin (Cursor Padding)

**Description:** When the cursor approaches the top or bottom edge of the viewport (within N lines), auto-scroll to keep N lines of context visible around the cursor.

**Implementation Details:** Add `editor.scrollbar.cursorMarginLines` setting (int, default 5). During cursor movement (arrow keys, typing, goto-line), check if the cursor line is within `cursorMarginLines` of the viewport edge. If so, scroll to maintain the margin. The scroll uses smooth animation (200ms). This margin does not apply at the actual document boundaries (first/last N lines). The existing `EnsureCursorVisible()` method in EditorPanel is extended to respect this margin.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Cursor never gets closer than 5 lines to viewport edge during navigation
- Setting to 0 disables cursor margin
- Smooth scroll animation when margin correction triggers
- Works correctly at document boundaries (no infinite scroll loops)
- Typing at the last visible line scrolls to maintain margin

**Dependencies:** Task 06

---

### Task 22 -- Implement Find-in-Scrollbar Highlights

**Description:** When Find/Search is active, highlight the positions of all matches on the vertical scrollbar track, allowing the user to see the distribution of matches across the document at a glance.

**Implementation Details:** This is a specific integration of the ScrollDecorationProvider system (Task 03/04). The SearchMatchDecorationProvider subscribes to `SearchResultsUpdatedEvent` from the find/replace system. When active, it provides orange (3px wide, Block shape) decorations at each match line position. The current match (the one highlighted in the editor) uses a brighter orange with a 1px white border to distinguish it. When search is dismissed, the provider clears all decorations.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.cpp` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp` (modify)

**Acceptance Criteria:**
- Orange markers appear on scrollbar for every search match
- Current match marker is visually distinct (brighter with border)
- Markers update in real time as user types in search field
- Markers clear when search is dismissed
- Performance: rendering 1000+ markers does not impact scroll performance

**Dependencies:** Task 04

---

### Task 23 -- Implement Git Change Scrollbar Indicators

**Description:** Show git change indicators on the scrollbar track: green for added lines, blue for modified lines, red for deleted lines, allowing the user to see where changes are in the document.

**Implementation Details:** GitChangeDecorationProvider subscribes to `GitStatusChangedEvent`. It maps git diff hunks to line ranges and provides decorations: green (`#2ea043`) for added lines, blue (`#1f6feb`) for modified lines, and red (`#f85149`) for deleted line positions. The decorations use the `Line` shape (1px height). When no git repository is active, the provider returns an empty decoration set.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.cpp` (modify)

**Acceptance Criteria:**
- Green markers for added lines appear on scrollbar
- Blue markers for modified lines appear on scrollbar
- Red markers for deleted line positions appear on scrollbar
- Markers update when file is saved or git status refreshes
- No markers shown when not in a git repository

**Dependencies:** Task 04

---

### Task 24 -- Implement Diagnostics Scrollbar Indicators

**Description:** Show error and warning indicators on the scrollbar track: red for errors, yellow for warnings, allowing the user to see where issues are in the document.

**Implementation Details:** DiagnosticsDecorationProvider subscribes to `DiagnosticsUpdatedEvent`. It maps diagnostic locations to line numbers and provides decorations: red (`#f85149`) for errors, yellow (`#d29922`) for warnings. Errors use the `Block` shape (3px) to be more prominent than warnings (`Line` shape, 1px). Information-level diagnostics are not shown on the scrollbar to avoid clutter.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/ScrollDecorationProvider.cpp` (modify)

**Acceptance Criteria:**
- Red block markers for errors appear on scrollbar
- Yellow line markers for warnings appear on scrollbar
- Info-level diagnostics are not shown on scrollbar
- Markers update when diagnostics change
- Markers correctly map to line positions in the document

**Dependencies:** Task 04

---

### Task 25 -- Cross-Panel Scroll Behavior Audit

**Description:** Verify that all scrollable panels in the application use the new scroll system consistently: themed scrollbars, smooth scrolling (where applicable), and correct keyboard shortcuts.

**Implementation Details:** Create a checklist of all scrollable panels and verify each one: EditorPanel, FileTreeCtrl, OutputPanel, ProblemsPanel, SearchPanel, ExtensionsBrowserPanel, SettingsPanel, BacklinksPanel, HistoryPanel, GraphViewPanel, FlashcardBrowserPanel, PDFViewerPanel. For each, verify: (1) themed scrollbar renders correctly, (2) mouse wheel scrolls smoothly, (3) Page Up/Down work, (4) Home/End scroll to boundaries, (5) scrollbar mode respects user setting. Document any panel that cannot use the standard ScrollbarAdapter and note the reason.

**Files Affected:**
- All panels listed above (.cpp files, read-only audit)

**Acceptance Criteria:**
- All 12+ scrollable panels use themed scrollbars
- No panels use native wxScrollBar rendering
- Consistent scroll behavior across all panels
- Any exceptions are documented with rationale
- Settings changes affect all panels immediately

**Dependencies:** Task 09

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- ThemedScrollbar supports 6px/10px width transition and overlay mode
- Scroll decorations render for search, git, and diagnostics
- Smooth scrolling works at 60fps for large files
- Scroll position persists per file across tab switches
- Minimap renders and functions as alternative scrollbar
- All scrollable panels use themed scrollbars
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
