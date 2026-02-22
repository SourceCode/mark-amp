# Phase 26: Main Toolbar Redesign

## Overview

Redesign the main application toolbar from its current SRC/SPLIT/VIEW toggle layout into a comprehensive action surface modeled after WebStorm/IntelliJ and VS Code. The new toolbar features a run configuration dropdown with play/debug/stop buttons, build status indicator, layout mode toggles, sidebar/panel visibility toggles, a notification bell, and responsive behavior that collapses gracefully at narrow widths. This phase builds on the existing `Toolbar`, `ToolbarModel`, and `ThemeAwareWindow` infrastructure.

## Prerequisites

- Phase 25 (Build and Task Runner) -- build/run actions for toolbar buttons
- Phase 09 (Toolbar Action Surface Redesign -- v10) -- contextual action slots and ToolbarModel
- Existing `Toolbar` at `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- Existing `ToolbarModel` at `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.h`
- Existing `StatusBarPanel` for coordination of status display

## Target Files

| Action | File |
|--------|------|
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/ToolbarModel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Config.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_toolbar_v2.cpp` |

## Tasks

### Task 1: Define Toolbar Layout Architecture

**Title:** Design the new toolbar region layout with left/center/right zones

**Description:** Redesign the toolbar layout into three zones: left (run controls), center (layout/navigation), and right (utilities/notifications). Each zone has explicit spacing and overflow rules.

**Implementation Details:**
```
[Left Zone]                    [Center Zone]              [Right Zone]
[RunConfig v] [>] [D] [X]  |  [Default] [Zen] [Present]  |  [Search] [Sidebar] [Panel] [Bell]
  Run dropdown  Play Debug Stop  Layout mode toggles         Utility toggles  Notification
```

Update `Toolbar.h` with zone-based layout:
```cpp
struct ToolbarZone
{
    enum class Alignment { kLeft, kCenter, kRight };
    Alignment alignment;
    std::vector<ButtonInfo> buttons;
    int total_width{0};
    int x_offset{0};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Toolbar is divided into three distinct zones
- Left zone contains run configuration controls
- Center zone contains layout mode toggles
- Right zone contains utility toggles and notification bell
- Zones are separated by visual dividers
- Total toolbar height remains 40px

**Dependencies:** None

---

### Task 2: Implement Run Configuration Service

**Title:** Service for managing run/debug configurations

**Description:** Create a service that manages named run configurations (command to execute, environment, working directory, build-before-run flag). Configurations are stored in `.markamp/launch.json`.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.h`:
```cpp
#pragma once
#include "EventBus.h"
#include <string>
#include <vector>

namespace markamp::core
{

struct RunConfiguration
{
    std::string name;
    std::string command;
    std::string working_directory;
    std::vector<std::pair<std::string, std::string>> environment;
    bool build_before_run{true};
    bool use_terminal{true};     // Run in integrated terminal
    std::string type;            // "shell", "cmake", "ctest"
    std::string cmake_target;    // For cmake type
};

class RunConfigService
{
public:
    explicit RunConfigService(EventBus& event_bus);

    [[nodiscard]] auto load_from_file(const std::string& path) -> bool;
    [[nodiscard]] auto save_to_file(const std::string& path) const -> bool;

    void add_configuration(RunConfiguration config);
    void remove_configuration(const std::string& name);
    void update_configuration(const RunConfiguration& config);

    [[nodiscard]] auto configurations() const -> const std::vector<RunConfiguration>&;
    [[nodiscard]] auto find(const std::string& name) const -> const RunConfiguration*;
    [[nodiscard]] auto active_configuration() const -> const RunConfiguration*;
    void set_active(const std::string& name);

    [[nodiscard]] auto recent_configurations(int max_count = 5) const -> std::vector<std::string>;

private:
    EventBus& event_bus_;
    std::vector<RunConfiguration> configurations_;
    std::string active_name_;
    std::vector<std::string> recent_;
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/RunConfigService.cpp` (create)

**Acceptance Criteria:**
- Configurations load from and save to `.markamp/launch.json`
- Active configuration is tracked and persisted
- Recent configurations list maintains last 5 used
- Validation rejects configs with empty name or command
- File I/O uses `std::error_code` overloads

**Dependencies:** None

---

### Task 3: Implement Run Configuration Dropdown

**Title:** Custom-drawn dropdown for selecting and managing run configurations

**Description:** Create a dropdown widget that displays the active run configuration name with a chevron, and opens a popup list showing all configurations, recent configurations, and an "Edit Configurations..." option.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/RunConfigService.h"
#include <functional>

namespace markamp::ui
{

class RunConfigDropdown : public ThemeAwareWindow
{
public:
    RunConfigDropdown(wxWindow* parent,
                      core::ThemeEngine& theme_engine,
                      core::RunConfigService& config_service);

    using ConfigSelectedCallback = std::function<void(const std::string& name)>;
    using EditConfigCallback = std::function<void()>;

    void set_on_config_selected(ConfigSelectedCallback cb);
    void set_on_edit_configs(EditConfigCallback cb);

    void Refresh();

    static constexpr int kWidth = 200;
    static constexpr int kHeight = 28;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::RunConfigService& config_service_;
    ConfigSelectedCallback on_selected_;
    EditConfigCallback on_edit_;

    bool is_open_{false};
    int hovered_index_{-1};

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void ShowPopup();
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/RunConfigDropdown.cpp` (create)

**Acceptance Criteria:**
- Dropdown shows the active configuration name with a chevron icon
- Clicking opens a popup with all configurations
- Popup shows recent configurations at the top (if different from full list)
- "Edit Configurations..." option at the bottom opens the config editor
- "Add Configuration..." option creates a new blank config
- Width is capped at 200px with text truncation for long names
- Popup is themed to match the application

**Dependencies:** Task 2

---

### Task 4: Implement Run Button (Green Play)

**Title:** Green play button that executes the active run configuration

**Description:** Add a green play icon button that triggers the active run configuration. When clicked, it runs the build-before-run step (if configured) then executes the command in the integrated terminal or output panel.

**Implementation Details:**
```cpp
void Toolbar::OnRunClicked()
{
    auto* config = run_config_service_.active_configuration();
    if (config == nullptr)
    {
        // Show "No configuration selected" notification
        return;
    }
    if (config->build_before_run)
    {
        if (!build_service_.build_before_run())
        {
            return; // Build failed
        }
    }
    event_bus_.publish(events::RunConfigStartedEvent{.config_name = config->name});
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Green play icon (filled right-pointing triangle) draws correctly
- Click triggers the active run configuration
- Button is disabled (grayed) when no configuration is selected
- Button changes to a "running" state (with subtle animation) while process is active
- Keyboard shortcut: Cmd+R or F5
- Tooltip shows "Run [configuration name]"

**Dependencies:** Task 2, Task 3

---

### Task 5: Implement Debug Button (Green Bug)

**Title:** Debug button that launches the active configuration with debug flags

**Description:** Add a debug button (bug icon in green) that launches the active configuration with additional debug-related flags (e.g., debug build, attach debugger command).

**Implementation Details:**
Draw a bug icon using `wxGraphicsContext`:
```cpp
void Toolbar::DrawBugIcon(wxGraphicsContext& gc, double x, double y, double size) const
{
    // Bug body (oval)
    // Antenna (two curved lines from top)
    // Legs (three pairs of lines from sides)
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Bug icon renders in green, matching the play button color
- Click triggers debug launch of the active configuration
- Button is disabled when no configuration is selected
- Keyboard shortcut: Cmd+D or F9
- Tooltip shows "Debug [configuration name]"

**Dependencies:** Task 2

---

### Task 6: Implement Stop Button (Red Square)

**Title:** Red stop button that terminates the running process

**Description:** Add a red stop button (filled square) that terminates the currently running process. The button is only enabled when a process is running. Uses the two-stage kill approach (SIGHUP then SIGKILL).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Red filled square icon renders correctly
- Button is disabled (grayed) when no process is running
- Click sends stop signal to the running process
- Button becomes enabled as soon as a run/debug starts
- Keyboard shortcut: Shift+F5
- Tooltip shows "Stop"

**Dependencies:** Task 4

---

### Task 7: Implement Build Indicator

**Title:** Animated build status indicator in the toolbar

**Description:** Add a build indicator between the run controls and layout toggles that shows: idle (empty), building (spinner), success (green check for 5s), failure (red X for 5s). The indicator is compact (20x20px) and provides at-a-glance build status.

**Implementation Details:**
```cpp
enum class BuildIndicatorState { kIdle, kBuilding, kSuccess, kFailure };

struct BuildIndicator
{
    BuildIndicatorState state{BuildIndicatorState::kIdle};
    int spinner_frame{0};
    wxTimer decay_timer; // Returns to idle after 5s
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Idle state shows nothing (blank space)
- Building state shows an animated spinner (rotating arc)
- Success state shows a green checkmark for 5 seconds then fades to idle
- Failure state shows a red X for 5 seconds then fades to idle
- Spinner updates at 60fps using a timer
- Subscribes to `BuildStartedEvent` and `BuildFinishedEvent`

**Dependencies:** Phase 25 Task 1

---

### Task 8: Implement Layout Mode Buttons

**Title:** Default, Zen, and Presentation layout mode toggles

**Description:** Replace the existing SRC/SPLIT/VIEW toggles with layout mode buttons: Default (standard layout), Zen (distraction-free, hides all panels), Presentation (large text, no sidebar). Each mode is a preset layout configuration.

**Implementation Details:**
```cpp
enum class LayoutMode { kDefault, kZen, kPresentation };

struct LayoutModePreset
{
    bool show_sidebar{true};
    bool show_panel{true};
    bool show_toolbar{true};
    bool show_status_bar{true};
    bool show_tab_bar{true};
    int editor_font_size_delta{0}; // Added to base font size
};

static constexpr LayoutModePreset kZenPreset{false, false, false, false, false, 2};
static constexpr LayoutModePreset kPresentationPreset{false, false, true, false, true, 6};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Three layout mode buttons render with distinct icons
- Default mode shows all UI elements
- Zen mode hides sidebar, panel, toolbar, status bar, and tab bar
- Presentation mode hides sidebar and increases font size
- Active mode button is highlighted
- Escape key returns from Zen/Presentation to Default mode
- Layout mode persists across sessions

**Dependencies:** None

---

### Task 9: Implement Search Button

**Title:** Search button that opens the global search panel

**Description:** Add a magnifying glass button that opens/focuses the search sidebar panel. This provides discoverability for the search feature beyond the Cmd+Shift+F shortcut.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Magnifying glass icon renders in the right zone
- Click toggles the search sidebar panel
- Button shows active state when search panel is visible
- Tooltip: "Toggle Search (Cmd+Shift+F)"

**Dependencies:** None

---

### Task 10: Implement Toggle Sidebar Button

**Title:** Button to toggle the sidebar panel visibility

**Description:** Add a sidebar toggle button (panel-left icon) that shows/hides the sidebar. The button visually indicates whether the sidebar is currently visible.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Panel-left icon (rectangle with left third filled) renders correctly
- Click toggles sidebar visibility via `LayoutManager`
- Button shows active/inactive state based on sidebar visibility
- Tooltip: "Toggle Sidebar (Cmd+B)"
- Subscribes to sidebar visibility events to stay in sync

**Dependencies:** None

---

### Task 11: Implement Toggle Panel Button

**Title:** Button to toggle the bottom panel visibility

**Description:** Add a panel toggle button (panel-bottom icon) that shows/hides the bottom panel area (terminal, output, problems, etc.).

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Panel-bottom icon (rectangle with bottom third filled) renders correctly
- Click toggles bottom panel visibility via `LayoutManager`
- Button shows active/inactive state
- Tooltip: "Toggle Panel (Cmd+J)"

**Dependencies:** None

---

### Task 12: Implement Breadcrumb Toggle

**Title:** Button to toggle breadcrumb bar visibility

**Description:** Add a toggle for showing/hiding the breadcrumb navigation bar above the editor.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Breadcrumb icon (path segments icon) renders in the right zone
- Click toggles breadcrumb bar visibility
- Setting persists in config

**Dependencies:** None

---

### Task 13: Implement Minimap Toggle

**Title:** Button to toggle the editor minimap visibility

**Description:** Add a minimap toggle button that shows/hides the editor minimap.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Minimap icon (small rectangle with lines) renders correctly
- Click publishes an event that EditorPanel subscribes to
- Button state reflects current minimap visibility
- Setting persists in config

**Dependencies:** None

---

### Task 14: Implement Responsive Collapse

**Title:** Collapse toolbar buttons to icons-only at narrow widths, then to overflow menu

**Description:** At narrow window widths, the toolbar progressively collapses: first, text labels are hidden (icons only); then, less-important buttons move into an overflow chevron menu. The run configuration dropdown also collapses to a shorter form.

**Implementation Details:**
```cpp
void Toolbar::RecalculateButtonRects()
{
    auto available = GetClientSize().GetWidth();
    auto required = CalculateFullWidth();

    if (required > available)
    {
        // Stage 1: Collapse to icons only (hide labels)
        compact_mode_ = true;
        required = CalculateCompactWidth();
    }
    if (required > available)
    {
        // Stage 2: Move right-zone buttons to overflow menu
        overflow_buttons_ = CollectOverflowButtons(available);
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- At full width: all buttons with labels
- Below 900px: icons only (no labels)
- Below 600px: search, minimap, breadcrumb buttons move to overflow menu
- Overflow chevron (>>) appears at the right edge when buttons are hidden
- Clicking overflow chevron shows popup menu with hidden button actions
- Resize updates layout immediately with no flickering

**Dependencies:** Task 1

---

### Task 15: Implement Toolbar Overflow Menu

**Title:** Popup menu for buttons that do not fit in the toolbar

**Description:** When buttons overflow, show a chevron button at the right edge. Clicking it opens a popup menu listing all overflowed actions with their icons and keyboard shortcuts.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Overflow chevron (>>) only appears when buttons are hidden
- Popup menu shows all hidden buttons as menu items
- Menu items have icons matching the toolbar buttons
- Menu items show keyboard shortcuts
- Toggle buttons show their current state in the menu (checkbox)

**Dependencies:** Task 14

---

### Task 16: Implement Notification Bell

**Title:** Notification bell icon with unread count badge

**Description:** Add a bell icon in the right zone that shows a count badge when there are unread notifications. Clicking it opens a notification panel or dropdown showing recent notifications.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h`:
```cpp
#pragma once
#include "EventBus.h"
#include <chrono>
#include <string>
#include <vector>

namespace markamp::core
{

struct Notification
{
    enum class Level : uint8_t { kInfo, kWarning, kError };
    std::string id;
    std::string title;
    std::string message;
    Level level{Level::kInfo};
    std::chrono::system_clock::time_point timestamp;
    bool read{false};
    std::string source; // "build", "extension", "system"
};

class NotificationService
{
public:
    explicit NotificationService(EventBus& event_bus);

    void notify(const std::string& title, const std::string& message,
                Notification::Level level = Notification::Level::kInfo,
                const std::string& source = {});

    [[nodiscard]] auto notifications() const -> const std::vector<Notification>&;
    [[nodiscard]] auto unread_count() const -> int;
    void mark_all_read();
    void mark_read(const std::string& id);
    void clear_all();

private:
    EventBus& event_bus_;
    std::vector<Notification> notifications_;
    int next_id_{1};
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/NotificationService.cpp` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Bell icon renders in the right zone
- Unread count badge (red circle with white number) overlays the bell
- Clicking bell opens a dropdown with recent notifications
- Notifications show title, time ago, and level icon
- "Mark all as read" button clears the badge
- Build failures automatically create a notification
- Badge shows "99+" for counts over 99

**Dependencies:** None

---

### Task 17: Implement Dividers Between Button Groups

**Title:** Visual dividers separating toolbar zones and button groups

**Description:** Draw thin vertical dividers between the left/center/right zones and between logical button groups within zones. Dividers use the theme's border color.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- Dividers are 1px wide, using `border_subtle` theme color
- Dividers have 8px vertical padding from toolbar edges
- At least three dividers: after run controls, before right zone, and between layout buttons
- Dividers do not appear at the edges of the toolbar
- In compact mode, some dividers may be hidden

**Dependencies:** Task 1

---

### Task 18: Implement Toolbar Customization

**Title:** Allow users to customize which buttons appear and their order

**Description:** Add a "Customize Toolbar..." option (right-click context menu) that opens a configuration dialog. Users can add, remove, and reorder toolbar buttons. The configuration persists in the user's config.

**Implementation Details:**
```cpp
struct ToolbarCustomization
{
    std::vector<std::string> left_zone_buttons;
    std::vector<std::string> center_zone_buttons;
    std::vector<std::string> right_zone_buttons;
    bool show_labels{true};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Right-click on toolbar shows "Customize Toolbar..." option
- Customization dialog shows all available buttons with checkboxes
- Drag-and-drop reordering within zones
- Reset to default option
- Customization persists across sessions
- Invalid button IDs in config are silently ignored

**Dependencies:** Task 1

---

### Task 19: Implement Run/Debug Event Flow

**Title:** Define events for run configuration lifecycle

**Description:** Add events for run start, run output, run stop, and run finish so the toolbar, status bar, and terminal all stay in sync.

**Implementation Details:**
Add to Events.h:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigStartedEvent)
    std::string config_name;
    bool is_debug{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigFinishedEvent)
    std::string config_name;
    int exit_code{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(RunConfigStoppedEvent)
    std::string config_name;
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All three events compile and are publishable
- Run button publishes `RunConfigStartedEvent`
- Stop button publishes `RunConfigStoppedEvent`
- Process exit publishes `RunConfigFinishedEvent`

**Dependencies:** None

---

### Task 20: Migrate Existing Toolbar State

**Title:** Preserve existing SRC/SPLIT/VIEW functionality within the new layout

**Description:** The existing view mode toggles (SRC, SPLIT, VIEW) should still work but move to a dropdown or secondary location. Ensure all existing `ViewModeChangedEvent` subscriptions continue to work.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h`
- `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.cpp`

**Acceptance Criteria:**
- View mode buttons are available (in center zone or via dropdown)
- `ViewModeChangedEvent` continues to publish correctly
- Existing subscriptions are not broken
- The Save button action is preserved (moved to Cmd+S only, or kept in right zone)
- Themes and Settings buttons move to the right zone or overflow

**Dependencies:** Task 1

---

### Task 21: Update CMakeLists.txt

**Title:** Add new toolbar source files to the build

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- `RunConfigDropdown`, `RunConfigService`, `NotificationService` files added
- Build succeeds with no linker errors

**Dependencies:** All create tasks

---

### Task 22: Write Unit Tests for RunConfigService

**Title:** Test run configuration loading, saving, and management

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_toolbar_v2.cpp` (create)

**Acceptance Criteria:**
- Tests verify JSON loading of run configurations
- Tests verify active configuration tracking
- Tests verify recent configuration list
- Tests verify validation (reject empty names/commands)
- All tests pass

**Dependencies:** Task 2

---

### Task 23: Write Unit Tests for NotificationService

**Title:** Test notification creation, unread counting, and read marking

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_toolbar_v2.cpp`

**Acceptance Criteria:**
- Tests verify notification creation with all fields
- Tests verify unread count increments and resets
- Tests verify mark_read and mark_all_read
- Tests verify clear_all
- All tests pass

**Dependencies:** Task 16

---

### Task 24: Write Unit Tests for Responsive Collapse

**Title:** Test toolbar layout at different widths

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_toolbar_v2.cpp`

**Acceptance Criteria:**
- Tests verify all buttons visible at wide widths
- Tests verify compact mode triggers at medium widths
- Tests verify overflow collects correct buttons at narrow widths
- Tests verify overflow menu contains all hidden buttons
- All tests pass

**Dependencies:** Task 14

---

### Task 25: Write Integration Tests for Run Flow

**Title:** Test run button event flow end-to-end

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_toolbar_v2.cpp`

**Acceptance Criteria:**
- Tests verify RunConfigStartedEvent published on play click
- Tests verify RunConfigStoppedEvent published on stop click
- Tests verify toolbar button states update on events
- Tests verify build-before-run flow
- All tests pass

**Dependencies:** Task 4, Task 19

---

### Task 26: Documentation

**Title:** Document the new toolbar architecture and customization options

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- Zone-based layout is documented
- Run configuration system is described
- Notification system is documented
- Customization options are listed
- Responsive behavior is described

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for RunConfigService and NotificationService
- Unit tests for responsive collapse logic
- Integration tests for run/debug/stop event flow
- Manual testing at various window widths
- Theme switching with all toolbar states active

## Phase Completion Criteria

- Toolbar has a run configuration dropdown with play/debug/stop buttons
- Build status indicator shows real-time build state
- Layout mode buttons support Default, Zen, and Presentation modes
- Sidebar and panel toggle buttons work correctly
- Notification bell shows unread count
- Toolbar collapses gracefully at narrow widths
- All existing toolbar functionality is preserved or improved
- All unit and integration tests pass
