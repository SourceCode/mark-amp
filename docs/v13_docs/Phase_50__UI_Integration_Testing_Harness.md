# Phase 50 -- UI Integration Testing Harness

## Objective

Build a comprehensive UI testing harness that validates visual correctness, interaction behavior, accessibility compliance, and performance budgets across all MarkAmp UI components. The harness provides: visual regression testing via screenshot comparison, UI automation helpers for simulating user interactions, a component isolation renderer for testing individual controls, an accessibility audit runner, theme coverage verification, keyboard navigation tests, screen reader output verification, responsive layout tests, high-DPI render tests, animation frame tests, memory leak detection, and CI integration for automated UI quality gates.

## Prerequisites

- Phase 49 complete (Performance Dashboard and Monitoring)
- `/Users/ryanrentfro/code/markamp/src/ui/RegressionHarnessModel.h` -- existing model with snapshot baselines, keyboard steps
- `/Users/ryanrentfro/code/markamp/src/ui/EditorPerformanceBudget.h` -- performance budget infrastructure
- `/Users/ryanrentfro/code/markamp/src/core/PerformanceCounters.h` -- metrics collection (Phase 49)
- All Phase 41-49 components (full UI surface to test)
- Tests use Catch2 with descriptive section names

## Chrome DevTools / Testing Library Reference

- Puppeteer/Playwright: screenshot comparison with pixel tolerance
- Storybook Chromatic: visual regression CI integration
- axe-core: automated accessibility auditing
- React Testing Library: component isolation testing philosophy

## Target Files

| File | Action |
|------|--------|
| `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/AccessibilityAuditor.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/AccessibilityAuditor.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ThemeCoverageChecker.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ThemeCoverageChecker.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/KeyboardNavigationTest.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/KeyboardNavigationTest.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ResponsiveLayoutTest.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/ResponsiveLayoutTest.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/MemoryLeakDetector.h` | Create |
| `/Users/ryanrentfro/code/markamp/src/testing/MemoryLeakDetector.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/src/ui/RegressionHarnessModel.h` | Modify |
| `/Users/ryanrentfro/code/markamp/src/ui/RegressionHarnessModel.cpp` | Modify |
| `/Users/ryanrentfro/code/markamp/tests/unit/test_ui_harness.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` | Create |
| `/Users/ryanrentfro/code/markamp/CMakeLists.txt` | Modify |

## Tasks

### Task 01 -- Create VisualRegressionRunner Framework

**Description:** Build a framework that captures screenshots of UI components, compares them against golden baselines, and reports pixel-level differences.

**Implementation Details:** VisualRegressionRunner manages a `baselines/` directory containing golden screenshot PNGs. Methods: `capture(wxWindow* target, string test_id) -> wxBitmap` captures a screenshot of a specific window/panel, `compare(test_id, captured_bitmap) -> ComparisonResult` compares against the baseline. ComparisonResult contains: `match_percentage` (0-100), `diff_pixel_count`, `diff_bitmap` (highlighting differences in red), `passed` (true if match >99.5%). A configurable `tolerance_percent` (default 0.5%) accommodates anti-aliasing differences. Failed comparisons save the actual, expected, and diff images side-by-side for debugging.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp` (create)

**Acceptance Criteria:**
- Screenshots captured accurately for any wxWindow
- Pixel comparison identifies differences with configurable tolerance
- Diff image highlights changed pixels in red
- Side-by-side output (expected/actual/diff) saved on failure
- Baseline directory is organized by test ID

**Dependencies:** None

---

### Task 02 -- Implement Baseline Management

**Description:** Create tools for managing visual regression baselines: creating new baselines, updating existing ones, and reviewing differences.

**Implementation Details:** Add methods to VisualRegressionRunner: `update_baseline(test_id)` replaces the golden image with the current capture, `create_baseline(test_id, bitmap)` creates a new golden image, `list_baselines() -> vector<BaselineEntry>` returns all registered baselines. Add a CLI argument `--update-baselines` that runs all visual tests and updates baselines instead of comparing. Add `--review-baselines` that opens a side-by-side viewer for each changed baseline. Baselines are stored in `tests/baselines/` directory, organized by theme (dark/light) and platform (macos/windows/linux).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp` (modify)

**Acceptance Criteria:**
- Baselines update via CLI argument
- Baselines organized by theme and platform
- Review mode shows changed baselines
- Baseline creation captures at consistent size
- Old baselines are backed up before update

**Dependencies:** Task 01

---

### Task 03 -- Create UIAutomation Helpers

**Description:** Build a set of automation helpers that simulate user interactions (click, type, drag, key press) for UI testing without a physical mouse/keyboard.

**Implementation Details:** UIAutomation is a static utility class with methods: `click(wxWindow* target, wxPoint pos)` generates mouse down+up events, `double_click(target, pos)`, `right_click(target, pos)`, `type_text(wxWindow* target, string text)` generates key events for each character, `press_key(target, int keycode, int modifiers)` generates key down+up, `drag(target, wxPoint from, wxPoint to)` generates mouse down, moves, mouse up, `hover(target, pos)` generates mouse enter+move, `scroll(target, int delta)` generates scroll events. All methods use `wxUIActionSimulator` or direct `wxPostEvent()` for headless operation. Add `wait_for_idle(timeout_ms)` that processes pending events.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.cpp` (create)

**Acceptance Criteria:**
- Click generates proper mouse events
- Type generates key events for text input
- Drag generates mouse down, move, up sequence
- Events are processed synchronously for deterministic tests
- wait_for_idle processes all pending events

**Dependencies:** None

---

### Task 04 -- Create ComponentTestHarness

**Description:** Build a harness that renders a single UI component in isolation for testing, without requiring the full application context.

**Implementation Details:** ComponentTestHarness creates a minimal wxFrame with a configurable size (default 400x300), a ThemeEngine instance, an EventBus instance, and a content area where the test component is placed. Methods: `mount<T>(args...) -> T*` creates and places a component in the harness, `set_theme(theme_name)` switches theme, `set_size(width, height)` resizes the harness, `capture() -> wxBitmap` takes a screenshot, `process_events()` pumps the event loop. The harness provides: `theme_engine()`, `event_bus()` for component constructor requirements. Destroying the harness cleans up all components.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.cpp` (create)

**Acceptance Criteria:**
- Component mounts in isolation without full app
- ThemeEngine and EventBus are available
- Theme switching works in harness
- Screenshot capture works for mounted component
- Harness cleanup destroys all components

**Dependencies:** None

---

### Task 05 -- Create AccessibilityAuditor

**Description:** Build an automated accessibility audit tool that checks all UI controls for correct accessible roles, names, states, and keyboard operability.

**Implementation Details:** AccessibilityAuditor recursively walks the wxWindow tree and checks each control: (1) Has an accessible name (not empty), (2) Has a valid accessible role (not wxACC_ROLE_NONE), (3) Focusable controls are in the Tab order, (4) Buttons respond to Space/Enter, (5) Controls report correct state (disabled, checked, selected). Output: `AuditResult { vector<AuditViolation> violations; int total_controls; int compliant_controls; double compliance_rate; }`. Each violation: control class name, violation type (MissingName, MissingRole, NotInTabOrder, etc.), severity (Error, Warning). Target: 100% compliance for all Phase 41-49 controls.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/AccessibilityAuditor.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/AccessibilityAuditor.cpp` (create)

**Acceptance Criteria:**
- Auditor walks entire window tree recursively
- Missing names, roles, and tab order detected
- Audit results include compliance percentage
- All violations include control class and type
- 100% compliance target for custom controls

**Dependencies:** None

---

### Task 06 -- Create ThemeCoverageChecker

**Description:** Verify that all theme tokens defined in ThemeEngine are actually used by at least one component, and that no component uses hardcoded colors.

**Implementation Details:** ThemeCoverageChecker has two analyses: (1) **Token Usage**: scan all .cpp files for references to ThemeColorToken enum values, compile a list of used tokens, compare against the full enum to find unused tokens. (2) **Hardcoded Color Detection**: scan all .cpp files in src/ui/ for patterns like `wxColour(R, G, B)` or `wxColour("#hex")` that do not derive from ThemeEngine -- these are hardcoded colors that should be tokens. Output: `CoverageResult { vector<string> unused_tokens; vector<HardcodedColor> hardcoded_colors; double token_coverage_percent; }`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/ThemeCoverageChecker.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/ThemeCoverageChecker.cpp` (create)

**Acceptance Criteria:**
- All theme tokens checked for usage
- Unused tokens identified
- Hardcoded colors detected in UI source files
- Coverage percentage calculated
- False positives for intentional hardcoded colors can be allow-listed

**Dependencies:** None

---

### Task 07 -- Create KeyboardNavigationTest Suite

**Description:** Build an automated test suite that verifies keyboard navigation works correctly across all panels and controls.

**Implementation Details:** KeyboardNavigationTest defines test scenarios as sequences of key presses and expected focus targets. Each scenario: `{ name, starting_focus, key_sequence, expected_focus_sequence }`. Built-in scenarios: (1) Tab through main layout (sidebar -> editor -> bottom panel), (2) Tab through dialog controls, (3) Arrow keys in file tree, (4) Arrow keys in dropdown, (5) Cmd+Tab between editor panes, (6) Escape from nested contexts (peek -> editor, dialog -> parent). The test runner uses UIAutomation to simulate keys and checks `wxWindow::FindFocus()` against expectations.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/KeyboardNavigationTest.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/KeyboardNavigationTest.cpp` (create)

**Acceptance Criteria:**
- All 6 built-in scenarios pass
- Focus tracking correctly identifies focused control
- Tab order follows logical layout order
- Escape returns focus to correct parent
- Custom scenarios can be added via API

**Dependencies:** Task 03 (UIAutomation)

---

### Task 08 -- Implement Screen Reader Output Verification

**Description:** Create a test helper that captures what a screen reader would announce for UI interactions and verifies it against expected output.

**Implementation Details:** Create `ScreenReaderCapture` class that hooks into `wxAccessible::NotifyEvent()` and records all announcements. Methods: `start_capture()`, `stop_capture() -> vector<Announcement>` where Announcement is `{ event_type, window_name, window_role, value, state }`. Test helper: `assert_announced(expected_text)` checks if any announcement contains the expected text. This is a simulation -- it captures what *would* be announced based on wxAccessible APIs rather than running an actual screen reader.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.h` (extend)
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.cpp` (extend)

**Acceptance Criteria:**
- Announcements captured for all wxAccessible events
- Window name, role, and state included
- assert_announced validates expected output
- Works without an actual screen reader
- Captures both programmatic and user-triggered announcements

**Dependencies:** Task 03

---

### Task 09 -- Implement Responsive Layout Tests

**Description:** Create tests that verify UI layouts render correctly at multiple window sizes, ensuring no content cutoff or overlap.

**Implementation Details:** ResponsiveLayoutTest resizes the main window to a set of test sizes: 800x600 (minimum), 1024x768, 1280x800, 1440x900, 1920x1080, 2560x1440 (ultrawide). At each size, it captures a screenshot and checks: (1) no controls overlap (bounding box intersection test), (2) no controls extend beyond the window bounds, (3) sidebar width is within configured min/max, (4) editor area has at least 200px width, (5) bottom panel height is within configured bounds. Each size is a separate test case.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/ResponsiveLayoutTest.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/ResponsiveLayoutTest.cpp` (create)

**Acceptance Criteria:**
- Tests pass at all 6 standard sizes
- Overlap detection identifies overlapping controls
- Out-of-bounds detection catches truncated controls
- Min/max constraints verified for all panels
- Screenshots captured at each size for review

**Dependencies:** Task 04 (ComponentTestHarness)

---

### Task 10 -- Implement High-DPI Render Tests

**Description:** Create tests that verify controls render correctly at high-DPI (Retina) scale factors.

**Implementation Details:** High-DPI tests use the ComponentTestHarness with simulated DPI scale factors: 1.0x, 1.5x, 2.0x. At each scale: (1) mount a component, (2) capture screenshot, (3) verify icon dimensions match expected scaled size, (4) verify text is crisp (not blurry -- compare against high-DPI baseline), (5) verify border widths scale correctly (1px at 1x = 2px at 2x). On macOS, use `NSWindow::setBackingScaleFactor:` (or the appropriate test API) to simulate scale factors. On platforms without runtime DPI switching, this test verifies that `FromDIP()` calls are present in the source.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.cpp` (modify)

**Acceptance Criteria:**
- Controls render correctly at 1x, 1.5x, 2x
- Icons use wxBitmapBundle for resolution independence
- FromDIP() used for all pixel constants
- Border widths scale proportionally
- Text remains crisp at all scale factors

**Dependencies:** Task 04

---

### Task 11 -- Implement Animation Frame Test

**Description:** Verify that all animations run at 60fps without dropped frames or visual glitches.

**Implementation Details:** Create `AnimationFrameTest` that: (1) triggers an animation (e.g., dialog open, toggle switch, peek view expand), (2) captures screenshots at 16ms intervals during the animation, (3) verifies each frame is different from the previous (animation is progressing), (4) verifies the final frame matches the expected end state, (5) measures total animation duration against expected duration (+/- 10%). Uses a `wxTimer` at 16ms to capture frames during the test. Tests specific animations: dialog backdrop fade (200ms), toggle switch slide (150ms), peek view expand (200ms), tooltip delay (200ms).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.h` (extend)
- `/Users/ryanrentfro/code/markamp/src/testing/UIAutomation.cpp` (extend)

**Acceptance Criteria:**
- Each captured frame differs from previous (progression verified)
- Final frame matches expected end state
- Animation duration within 10% of target
- No dropped frames (consistent 16ms intervals)
- Works for dialog, toggle, peek, and tooltip animations

**Dependencies:** Task 03, Task 04

---

### Task 12 -- Implement Memory Leak Detection for Panels

**Description:** Create a test helper that detects memory leaks when panels are created and destroyed, ensuring no persistent references prevent cleanup.

**Implementation Details:** MemoryLeakDetector wraps panel lifecycle tests: (1) record baseline memory (using PerformanceCounters from Phase 49), (2) create a panel N times (e.g., 100), (3) destroy each panel, (4) force garbage collection / event processing, (5) record final memory, (6) assert that memory growth is under a threshold (1MB for 100 create/destroy cycles). If memory grows linearly with N, report a potential leak with the panel class name. The detector also checks for orphaned wxWindow children and dangling EventBus subscriptions (subscriptions that outlive their subscriber).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/MemoryLeakDetector.h` (create)
- `/Users/ryanrentfro/code/markamp/src/testing/MemoryLeakDetector.cpp` (create)

**Acceptance Criteria:**
- Memory growth detected after 100 create/destroy cycles
- Threshold: under 1MB growth for 100 cycles
- Orphaned wxWindow children detected
- Dangling EventBus subscriptions detected
- Panel class name reported in leak warnings

**Dependencies:** Phase 49 Task 04 (memory instrumentation)

---

### Task 13 -- Implement Cross-Platform Render Comparison

**Description:** Create infrastructure for comparing screenshots across platforms (macOS, Windows, Linux) to detect platform-specific rendering issues.

**Implementation Details:** Extend VisualRegressionRunner with platform-aware baselines: baselines are stored in subdirectories `tests/baselines/{platform}/`. When running on macOS, compare against macOS baselines. A CI workflow captures baselines on all three platforms. A `compare_cross_platform(test_id)` method loads baselines from all platforms and generates a comparison report showing differences. Expected differences (font rendering, scrollbar appearance) are allow-listed. Unexpected differences (layout shifts, missing elements) are flagged.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/VisualRegressionRunner.cpp` (modify)

**Acceptance Criteria:**
- Baselines stored per-platform
- Cross-platform comparison generates reports
- Expected differences allow-listed
- Unexpected layout differences flagged
- CI captures baselines on all platforms

**Dependencies:** Task 01

---

### Task 14 -- Create Test Fixtures for Common UI States

**Description:** Build a library of test fixtures that set up common UI states for testing: empty workspace, file open, split panes, dialog open, notification visible.

**Implementation Details:** Create `UITestFixtures` class with factory methods: `empty_workspace()` (creates harness with no files), `single_file_open(path)` (creates harness with one file in editor), `split_panes(count)` (creates harness with multiple panes), `dialog_open(dialog_type)` (creates harness with a specific dialog visible), `notification_visible(category)` (creates harness with notification). Each fixture returns a `UITestContext` that provides access to the harness, automation helpers, and cleanup. Fixtures are composable: `single_file_open().with_notification().with_sidebar_collapsed()`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.h` (extend)
- `/Users/ryanrentfro/code/markamp/src/testing/ComponentTestHarness.cpp` (extend)

**Acceptance Criteria:**
- All fixture types create correct UI state
- Fixtures are composable
- Cleanup runs automatically on fixture destruction
- Fixtures work in headless and GUI modes
- Test context provides access to all test tools

**Dependencies:** Task 04

---

### Task 15 -- Extend RegressionHarnessModel with Test Run Tracking

**Description:** Extend the existing RegressionHarnessModel to track full test run results: test names, pass/fail status, duration, and screenshots.

**Implementation Details:** Add `TestRunResult` struct: `{ string test_name; bool passed; double duration_ms; string screenshot_path; string error_message; }`. Add `TestRunSummary` struct: `{ int total; int passed; int failed; int skipped; double total_duration_ms; vector<TestRunResult> results; }`. Extend RegressionHarnessModel with: `record_result(TestRunResult)`, `summary() -> TestRunSummary`, `export_report(path) -> bool` (saves as JSON), `export_html_report(path)` (saves as visual HTML report with embedded screenshots).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/RegressionHarnessModel.h` (modify)
- `/Users/ryanrentfro/code/markamp/src/ui/RegressionHarnessModel.cpp` (modify)

**Acceptance Criteria:**
- Test results recorded with all fields
- Summary calculates correct pass/fail counts
- JSON export includes all test data
- HTML report embeds screenshots
- Pass rate calculated correctly

**Dependencies:** None

---

### Task 16 -- Create CI Integration Script

**Description:** Create a script and CMake targets for running UI tests in CI (GitHub Actions/GitLab CI).

**Implementation Details:** Create a new CMake target `ui_tests` that builds the test runner with all UI test files. Add a shell script `scripts/run_ui_tests.sh` that: (1) builds the UI test target, (2) runs tests with `--headless` flag (uses virtual framebuffer on Linux: `xvfb-run`), (3) collects screenshots in `test-results/` directory, (4) generates JUnit XML report for CI, (5) exits with non-zero code on any failure. Add a GitHub Actions workflow `.github/workflows/ui_tests.yml` that runs on push/PR. The workflow caches baseline images and uploads test artifacts (screenshots, reports) on failure.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (create)

**Acceptance Criteria:**
- UI tests build as separate CMake target
- Tests run in headless mode (xvfb on Linux)
- JUnit XML report generated for CI
- Screenshots saved as artifacts on failure
- Non-zero exit code on test failure

**Dependencies:** Tasks 01-15

---

### Task 17 -- Write Visual Regression Tests for All Phase 41 Controls

**Description:** Create visual regression test cases for every control from Phase 41 (Component Library Polish) in all variants and states.

**Implementation Details:** For each Phase 41 control (ThemedButton, ThemedTextInput, ThemedDropdown, ThemedCheckbox, ThemedRadio, ThemedToggleSwitch, ThemedSlider, ThemedProgressBar, Badge, ChipTag, SegmentedControl, NumberStepper), create test cases that mount the control in the ComponentTestHarness at each state (normal, hover, pressed, focused, disabled, selected) and capture screenshots. Compare against baselines. Test both Dark and Light themes. This generates approximately 100+ screenshot comparisons.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- Every Phase 41 control has visual regression tests
- All 6 states tested per control
- Both Dark and Light themes tested
- All screenshots match baselines within tolerance
- New baselines generated for first run

**Dependencies:** Tasks 01, 02, 04, Phase 41

---

### Task 18 -- Write Keyboard Navigation Tests

**Description:** Create automated keyboard navigation tests for major workflows: file tree navigation, tab switching, dialog navigation, command palette.

**Implementation Details:** Test scenarios: (1) Tab from file tree to editor to bottom panel, (2) Ctrl+Tab cycles between editor tabs, (3) Cmd+P opens command palette, type text, Enter selects, (4) Tab through dialog controls, Enter confirms, (5) Arrow keys navigate file tree items, Enter opens, (6) Escape closes context menus, peek views, dialogs in sequence, (7) Cmd+Shift+P opens command palette from any context, (8) Alt+F12 opens peek, Up/Down navigate results, Enter opens. Each test uses UIAutomation to simulate keys and verifies focus targets.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- All 8 scenarios pass
- Focus tracking verifies correct target after each key
- Tests work in headless mode
- Tests complete within 5 seconds each
- Failed tests report which step failed

**Dependencies:** Tasks 03, 07

---

### Task 19 -- Write Accessibility Audit Tests

**Description:** Create automated accessibility audit tests that verify all custom controls comply with accessibility requirements.

**Implementation Details:** Test cases: (1) Run AccessibilityAuditor on the main window in empty state, (2) Run on main window with file open and panels visible, (3) Run on each Phase 41 control in ComponentTestHarness, (4) Run on dialog system (open each dialog type), (5) Run on notification center, (6) Run on peek view. Assert 100% compliance for all custom controls (zero violations of Error severity). Warning-level violations are logged but do not fail the test. Generate a compliance report.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- All 6 test cases pass with 100% compliance
- Zero Error-level violations
- Warning-level violations logged
- Compliance report generated
- Tests cover all custom controls

**Dependencies:** Task 05

---

### Task 20 -- Write Theme Coverage Tests

**Description:** Verify that all theme tokens are used and no hardcoded colors exist in UI source files.

**Implementation Details:** Test case: run ThemeCoverageChecker against the full UI source directory. Assert: (1) token coverage >95% (at most 5% unused tokens, with justification for expected unused ones), (2) zero hardcoded colors in new Phase 41-49 source files (allow-list existing legacy files). Generate a coverage report listing unused tokens and hardcoded colors. Failed test outputs the specific tokens and files.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- Token coverage >95%
- Zero hardcoded colors in Phase 41-49 files
- Allow-list for legacy files
- Coverage report generated
- Specific tokens and files reported on failure

**Dependencies:** Task 06

---

### Task 21 -- Write Responsive Layout Tests

**Description:** Verify layouts at standard window sizes with no overlap or cutoff.

**Implementation Details:** Test at 6 standard sizes (800x600 through 2560x1440). At each size: (1) verify sidebar width within bounds, (2) verify editor area minimum width, (3) verify bottom panel height within bounds, (4) verify no control overlap, (5) verify no controls extend beyond window bounds, (6) capture screenshot for visual verification. Additionally test specific responsive behaviors: Welcome tab column layout switch at 800px, Settings panel column collapse, Extension browser grid reflow.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- All 6 standard sizes pass
- No overlap or cutoff detected
- Responsive behaviors verified
- Screenshots captured for each size
- Min/max constraints enforced

**Dependencies:** Task 09

---

### Task 22 -- Write Memory Leak Tests for Key Panels

**Description:** Run memory leak detection on panels that are frequently created and destroyed: editor tabs, peek views, notification toasts, dialog instances.

**Implementation Details:** Test cases: (1) create/destroy 100 EditorPanel instances -- verify <1MB growth, (2) create/destroy 100 PeekView instances -- verify <1MB growth, (3) show/dismiss 100 notification toasts -- verify <1MB growth, (4) open/close 100 ThemedDialog instances -- verify <1MB growth, (5) create/destroy 100 TabBar entries -- verify <1MB growth. Each test uses MemoryLeakDetector to measure before/after memory. Tests also verify zero orphaned wxWindow children and zero dangling EventBus subscriptions.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- All 5 panel types pass leak test
- Memory growth under 1MB for 100 cycles
- Zero orphaned wxWindow children
- Zero dangling EventBus subscriptions
- Test reports memory delta per panel type

**Dependencies:** Task 12

---

### Task 23 -- Write Animation Performance Tests

**Description:** Verify that all animations maintain 60fps and complete within their target duration.

**Implementation Details:** Test cases: (1) Dialog backdrop fade (200ms target), (2) Toggle switch animation (150ms target), (3) Peek view expand (200ms target), (4) Tooltip show delay (200ms target), (5) Scrollbar width transition (100ms target). Each test triggers the animation, captures frames at 16ms intervals, verifies frame uniqueness (animation is progressing), verifies completion within 10% of target duration, and verifies final frame matches end state baseline.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (extend)

**Acceptance Criteria:**
- All 5 animation tests pass
- Each frame differs from previous
- Duration within 10% of target
- Final frame matches expected state
- No frame drops (consistent 16ms intervals)

**Dependencies:** Task 11

---

### Task 24 -- Create Test Runner Dashboard

**Description:** Build a developer-facing dashboard that displays test results from the UI test harness, with filters, drill-down, and screenshot comparison.

**Implementation Details:** Add a "Test Results" tab to the Performance Dashboard (Phase 49). Layout: (1) summary bar: total/passed/failed/skipped counts, (2) filter controls: by test type (visual/keyboard/accessibility/responsive), by status (pass/fail), (3) scrollable test result list: each row shows test name, status icon, duration, and "View Details" button. Details view shows: for visual tests, expected/actual/diff screenshots side-by-side; for keyboard tests, step-by-step focus log; for accessibility tests, violation list. The dashboard reads from RegressionHarnessModel data.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/PerformanceDashboard.cpp` (modify)

**Acceptance Criteria:**
- Test results display in dashboard tab
- Filters narrow results by type and status
- Visual test details show screenshot comparison
- Keyboard test details show focus log
- Accessibility details show violation list

**Dependencies:** Task 15, Phase 49 Task 08

---

### Task 25 -- Add CMake Integration and Final Validation

**Description:** Add all new testing files to CMakeLists.txt, wire the UI test runner as a separate CTest target, and validate the complete test suite.

**Implementation Details:** Add all new .h/.cpp files under `src/testing/` to CMakeLists.txt. Create a separate executable target `ui_tests` that links against the test harness and Catch2. Add CTest integration: `add_test(NAME ui_tests COMMAND ui_tests --reporter junit --out test-results/ui.xml)`. Create `test_ui_harness.cpp` with unit tests for the harness infrastructure: (1) VisualRegressionRunner captures screenshots correctly, (2) UIAutomation click generates events, (3) ComponentTestHarness creates and destroys cleanly, (4) AccessibilityAuditor detects missing names, (5) ThemeCoverageChecker finds hardcoded colors, (6) MemoryLeakDetector detects intentional leak in test, (7) RegressionHarnessModel calculates pass rate, (8) KeyboardNavigationTest records focus changes, (9) ResponsiveLayoutTest detects overlap, (10) AnimationFrameTest captures frame progression.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/CMakeLists.txt` (modify)
- `/Users/ryanrentfro/code/markamp/tests/unit/test_ui_harness.cpp` (create)
- `/Users/ryanrentfro/code/markamp/tests/ui/ui_test_runner.cpp` (finalize)

**Acceptance Criteria:**
- `cmake --build build/debug` compiles all test harness code
- `ctest --output-on-failure` passes unit tests for harness infrastructure
- `ui_tests` executable runs all UI tests
- JUnit XML report generated for CI
- source_group entries match add_executable

**Dependencies:** Tasks 01-24

## Completion Gates

- All 25 tasks executed or explicitly deferred with rationale
- Visual regression framework captures and compares screenshots
- UI automation helpers simulate all input types
- Component test harness renders controls in isolation
- Accessibility auditor achieves 100% compliance on custom controls
- Theme coverage >95% for token usage
- Keyboard navigation tests pass for all major workflows
- Responsive layout tests pass at 6 standard sizes
- Memory leak tests detect no leaks in key panels
- Animation tests verify 60fps for all animations
- CI integration runs UI tests on push/PR
- `cmake --build build/debug -j$(sysctl -n hw.ncpu)` succeeds
- `cd build/debug && ctest --output-on-failure` passes
