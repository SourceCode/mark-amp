# Phase 22: Output Panel V2

## Overview

Overhaul the existing `OutputPanel` from a basic text display into a full-featured, multi-channel output viewer with log level filtering, ANSI color rendering, search/filter, auto-reveal behavior, channel badges, and configurable display options. This phase builds on the existing `OutputChannelService` and `OutputPanel` infrastructure while adding the polish and functionality expected of a professional IDE output panel.

## Prerequisites

- Phase 21 (Integrated Terminal Panel) -- shares ANSI parsing infrastructure
- Existing `OutputPanel` at `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- Existing `OutputChannelService` at `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h`
- Existing `PluginOutputRouter` in same file for extension channel routing
- Phase 02 (Control Visual Tokens) -- themed control styling

## Target Files

| Action | File |
|--------|------|
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/LogLevel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_output_panel_v2.cpp` |

## Tasks

### Task 1: Define Log Level Enum and Utilities

**Title:** Create a shared LogLevel type with string conversion and icon mapping

**Description:** Define a `LogLevel` enum that can be used across the output panel, problems panel, and debug console. Include utility functions for converting log levels to display strings, severity colors, and icon identifiers.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/LogLevel.h`:
```cpp
#pragma once
#include <cstdint>
#include <string>

namespace markamp::core
{

enum class LogLevel : uint8_t
{
    kTrace = 0,
    kDebug = 1,
    kInfo = 2,
    kWarning = 3,
    kError = 4,
    kFatal = 5,
};

[[nodiscard]] constexpr auto log_level_name(LogLevel level) -> const char*
{
    switch (level)
    {
        case LogLevel::kTrace: return "TRACE";
        case LogLevel::kDebug: return "DEBUG";
        case LogLevel::kInfo: return "INFO";
        case LogLevel::kWarning: return "WARN";
        case LogLevel::kError: return "ERROR";
        case LogLevel::kFatal: return "FATAL";
    }
    return "UNKNOWN";
}

[[nodiscard]] constexpr auto log_level_from_string(const char* str) -> LogLevel;
[[nodiscard]] constexpr auto operator<=(LogLevel a, LogLevel b) -> bool
{
    return static_cast<uint8_t>(a) <= static_cast<uint8_t>(b);
}

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/LogLevel.h` (create)

**Acceptance Criteria:**
- LogLevel enum has six severity levels from Trace to Fatal
- Conversion functions are constexpr where possible
- String-to-enum and enum-to-string round-trip correctly
- Comparison operators allow severity filtering (e.g., `level >= kWarning`)
- No dependencies on wxWidgets -- pure core type

**Dependencies:** None

---

### Task 2: Extend OutputChannel with Log Level Support

**Title:** Add per-line log level tracking to OutputChannel

**Description:** Modify `OutputChannel` to store log level metadata per line of output. Each `append_line()` call can optionally include a `LogLevel`. The channel also stores an ANSI-tagged variant of each line for colorized rendering.

**Implementation Details:**
Add to `OutputChannel`:
```cpp
struct OutputLine
{
    std::string text;
    std::string ansi_text; // Original with ANSI sequences
    LogLevel level{LogLevel::kInfo};
    std::chrono::system_clock::time_point timestamp;
};

void append_line(const std::string& text, LogLevel level = LogLevel::kInfo);
[[nodiscard]] auto lines() const -> const std::vector<OutputLine>&;
[[nodiscard]] auto lines_filtered(LogLevel min_level) const -> std::vector<const OutputLine*>;
[[nodiscard]] auto line_count() const -> std::size_t;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h`
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`

**Acceptance Criteria:**
- Each output line stores text, level, and timestamp
- `lines_filtered()` returns only lines at or above the specified level
- Existing `append()` calls remain backward-compatible (default to `kInfo`)
- Timestamps use `std::chrono::system_clock::now()`
- Line count is O(1) via stored vector size

**Dependencies:** Task 1

---

### Task 3: Define Output Panel Events

**Title:** Add EventBus events for output panel state changes

**Description:** Declare events for channel content changes, channel creation, active channel changes, and auto-reveal triggers so that the output panel and other UI components can stay in sync.

**Implementation Details:**
Add to `/Users/ryanrentfro/code/markamp/src/core/Events.h`:
```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelContentEvent)
    std::string channel_name;
    int new_line_count{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelCreatedEvent)
    std::string channel_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputChannelActiveChangedEvent)
    std::string channel_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutputAutoRevealEvent)
    std::string channel_name;
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All four event types compile and are publishable via EventBus
- Events include the channel name for routing
- Content event includes the count of new lines for badge updates
- Auto-reveal event is published when a channel receives content while not visible

**Dependencies:** None

---

### Task 4: Implement Channel Selector Dropdown

**Title:** Replace basic wxChoice with a themed dropdown showing channel names and badges

**Description:** Replace the existing `wxChoice` channel selector with a custom-drawn dropdown that displays channel names, new-content badges (unread line counts), and channel type icons. The dropdown is rendered using `wxGraphicsContext` for theme consistency.

**Implementation Details:**
Replace `wxChoice* channel_selector_` with a custom `ChannelDropdown` widget:
```cpp
struct ChannelInfo
{
    std::string name;
    int unread_count{0};
    bool is_active{false};
    std::string icon_type; // "build", "git", "extension", "task", "log"
};
```
The dropdown button shows the active channel name with an unread badge. Clicking it opens a popup list of all channels.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Dropdown displays all available channels sorted alphabetically
- Active channel is visually highlighted
- Channels with unread content show a numeric badge
- Badge count resets when the user switches to that channel
- Default channels include: Build, Git, Tasks, Extension Host, Log
- Channel icons differentiate system vs extension channels

**Dependencies:** Task 2, Task 3

---

### Task 5: Implement Log Level Filter Toggle Buttons

**Title:** Add severity filter toggle buttons (Error/Warning/Info/Debug) to the panel header

**Description:** Add four toggle buttons in the output panel header that filter visible output by log level. Each button shows the severity icon and count. Multiple levels can be active simultaneously. The filter state persists per channel.

**Implementation Details:**
```cpp
struct LevelFilterState
{
    bool show_errors{true};
    bool show_warnings{true};
    bool show_info{true};
    bool show_debug{false};
};
```
Each button is a small toggle icon rendered in the panel header bar area. Active filters show the count of matching lines.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Four toggle buttons render in the panel header: Error (red), Warning (yellow), Info (blue), Debug (gray)
- Each button shows the count of lines at that level
- Toggling a button immediately filters the visible output
- Filter state is independent per channel
- Default: errors, warnings, and info are shown; debug is hidden
- Toggling all off shows "No output matches the current filters" message

**Dependencies:** Task 1, Task 2

---

### Task 6: Implement Clear Output Button

**Title:** Add a clear button that clears the active channel's content

**Description:** Add a trash-can icon button in the panel header that clears all content from the active channel. Provide keyboard shortcut support and confirmation for channels with large amounts of content.

**Implementation Details:**
```cpp
void OutputPanel::ClearActiveChannel()
{
    if (service_ == nullptr) return;
    auto* channel = service_->get_channel(active_channel_);
    if (channel != nullptr)
    {
        channel->clear();
        RefreshContent();
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Clear button in panel header clears the active channel
- Keyboard shortcut Cmd+Shift+Delete clears output
- After clearing, the panel shows empty state message
- Clearing fires a content change listener so badges update
- No confirmation needed for clear (it is non-destructive, channels continue to receive new output)

**Dependencies:** Task 4

---

### Task 7: Implement Lock Scroll Toggle

**Title:** Add a scroll lock toggle that pauses auto-scrolling

**Description:** Add a lock icon toggle button in the panel header. When lock scroll is active, new output does not auto-scroll to the bottom. The button visually indicates the locked state. When the user manually scrolls to the bottom, auto-scroll resumes automatically.

**Implementation Details:**
```cpp
void OutputPanel::OnScroll(wxScrollEvent& event)
{
    if (IsScrolledToBottom())
    {
        unlock_scroll();
    }
    else
    {
        lock_scroll();
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Lock scroll button toggles between locked (padlock) and unlocked (open padlock) icons
- When locked, new output is appended but scroll position does not change
- When unlocked, new output auto-scrolls to the bottom
- Scrolling to the bottom auto-unlocks
- Lock state is preserved per channel
- Default: auto-scroll enabled (unlocked)

**Dependencies:** None

---

### Task 8: Implement Word Wrap Toggle

**Title:** Add a word wrap toggle for output text display

**Description:** Add a toggle button that enables/disables word wrapping in the output display. When word wrap is off, long lines are scrollable horizontally. The setting persists globally across all channels.

**Implementation Details:**
Replace the `wxTextCtrl` with a custom renderer (or configure `wxStyledTextCtrl`) that supports toggling word wrap mode.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Word wrap toggle button in panel header
- When enabled, long lines wrap at the panel boundary
- When disabled, horizontal scrollbar appears for long lines
- Default: word wrap enabled
- Setting persists across sessions in config

**Dependencies:** None

---

### Task 9: Implement Copy Output to Clipboard

**Title:** Copy selected or all output text to clipboard

**Description:** Support text selection in the output panel with copy-to-clipboard (Cmd+C). If no text is selected, Cmd+A selects all content in the active channel. Also add a "Copy All" action in the context menu.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Cmd+C copies selected text to clipboard (without ANSI escape codes)
- Cmd+A selects all output text
- Context menu includes "Copy" and "Copy All" actions
- Copied text preserves line breaks
- Empty selection + Cmd+C is a no-op (does not copy empty string)

**Dependencies:** None

---

### Task 10: Implement Open Output in Editor

**Title:** Open the active channel's content as a read-only document in the editor

**Description:** Add an "Open in Editor" action that takes the current channel's full content and opens it as a new untitled document in the editor panel. This allows the user to use full editor features (search, regex, copy) on the output text.

**Implementation Details:**
Publish a `FileOpenedEvent` with a synthetic URI like `output://Build` and the channel content. The editor treats it as an unsaved document.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- "Open in Editor" button in panel header and context menu
- Opens channel content as a new editor tab with title "[Output] Channel Name"
- Content is the plain text (ANSI codes stripped)
- Document is marked as read-only
- Document is not associated with a file path (untitled)

**Dependencies:** Task 2

---

### Task 11: Implement ANSI Color Rendering in Output

**Title:** Render ANSI color-coded output with correct terminal colors

**Description:** Replace the plain `wxTextCtrl` with a custom-painted output renderer that interprets ANSI escape sequences in output lines and renders them with correct foreground/background colors. Reuse the `AnsiParser` from Phase 21.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.h`:
```cpp
#pragma once
#include "core/AnsiParser.h"
#include "core/LogLevel.h"
#include "core/ThemeEngine.h"
#include <wx/dc.h>
#include <string>
#include <vector>

namespace markamp::ui
{

struct RenderedSegment
{
    std::string text;
    wxColour foreground;
    wxColour background;
    bool bold{false};
    bool italic{false};
    bool underline{false};
};

class OutputPanelRenderer
{
public:
    explicit OutputPanelRenderer(core::ThemeEngine& theme_engine);

    [[nodiscard]] auto render_line(const std::string& ansi_text) -> std::vector<RenderedSegment>;
    void paint_line(wxDC& dc, const std::vector<RenderedSegment>& segments,
                    int x, int y, const wxFont& font);

    [[nodiscard]] auto log_level_color(core::LogLevel level) -> wxColour;

private:
    core::ThemeEngine& theme_engine_;
    core::AnsiParser parser_;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanelRenderer.cpp` (create)

**Acceptance Criteria:**
- ANSI SGR color codes render with correct foreground/background colors
- Bold text renders with the bold font weight
- Colors are mapped to theme-derived values (same mapping as terminal)
- Lines without ANSI codes render as plain text in the default foreground color
- Performance: rendering 10,000 lines completes in under 100ms

**Dependencies:** Phase 21 Task 2 (AnsiParser)

---

### Task 12: Implement Timestamp Toggle

**Title:** Add timestamps to output lines with a toggle to show/hide

**Description:** Each output line has a timestamp from when it was received. Add a toggle button in the panel header that shows/hides timestamps at the beginning of each line. Timestamps format as `[HH:MM:SS.mmm]`.

**Implementation Details:**
```cpp
std::string format_timestamp(std::chrono::system_clock::time_point tp)
{
    // Format as [14:32:05.123]
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Timestamp toggle button in panel header (clock icon)
- Timestamps show as `[HH:MM:SS.mmm]` prefix in gray text
- Toggle applies immediately to all visible lines
- Default: timestamps hidden
- Setting persists across sessions

**Dependencies:** Task 2

---

### Task 13: Implement Output Search and Filter

**Title:** Add search/filter bar for output panel content

**Description:** Add a search bar (activated with Cmd+F) that filters or highlights matching lines in the output. Two modes: "filter" mode shows only matching lines; "highlight" mode shows all lines with matches highlighted. A toggle switches between modes.

**Implementation Details:**
The search bar appears at the top of the output panel content area (below the header actions).
```cpp
struct OutputSearchState
{
    std::string query;
    bool is_regex{false};
    bool case_sensitive{false};
    enum class Mode { kHighlight, kFilter } mode{Mode::kHighlight};
    int match_count{0};
    int current_match{-1};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Cmd+F opens search bar in output panel
- Typing filters/highlights matching lines
- Toggle between filter and highlight modes
- Case sensitivity toggle
- Regex toggle
- Match count displayed (e.g., "5 results")
- Enter/Shift+Enter navigate between matches
- Escape closes search bar

**Dependencies:** None

---

### Task 14: Implement Auto-Reveal on New Content

**Title:** Automatically reveal and focus the output panel when new content arrives

**Description:** When a channel receives new content while the output panel is hidden or showing a different channel, optionally auto-reveal the panel and switch to the active channel. This behavior is configurable per channel and defaults to "on" for the Build channel and "off" for others.

**Implementation Details:**
Add to `OutputChannel`:
```cpp
bool auto_reveal{false};
```
When content is appended to a channel with `auto_reveal == true`, publish `OutputAutoRevealEvent`. The `LayoutManager` subscribes and shows the output panel.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h`
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Build channel auto-reveals on new content by default
- Extension channels do not auto-reveal by default
- Auto-reveal can be toggled per channel via context menu
- Auto-reveal switches to the channel that triggered it
- Auto-reveal does not steal focus from the editor (panel opens, but editor stays focused)
- Rate-limited: at most one auto-reveal per 2 seconds to prevent flicker

**Dependencies:** Task 3

---

### Task 15: Implement Channel Badges

**Title:** Show unread content count badges on channel tabs/dropdown items

**Description:** Track unread line counts per channel (lines added since the user last viewed that channel). Display the count as a badge on the channel selector dropdown and on the Output tab in the bottom panel bar.

**Implementation Details:**
```cpp
struct ChannelBadgeState
{
    std::string channel_name;
    int unread_lines{0};
    bool has_errors{false};
    bool has_warnings{false};
};
```
The badge resets when the user switches to that channel.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Inactive channels show unread line count badge
- Badge shows red dot if unread lines include errors
- Badge shows yellow dot if unread lines include warnings
- Badge resets to zero when user views the channel
- Bottom panel "Output" tab shows aggregate unread count across all channels
- Badge count updates in real-time as new content arrives

**Dependencies:** Task 4

---

### Task 16: Implement Output Font Settings

**Title:** Configurable font for the output panel

**Description:** Allow the user to configure the output panel font independently from the editor. Defaults to the same monospace font as the terminal. Settings are in config YAML under `output.font`.

**Implementation Details:**
```cpp
struct OutputFontConfig
{
    std::string family; // defaults to terminal font
    int size{12};
    float line_height{1.3f};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Font family, size, and line height are configurable
- Changes apply immediately
- Invalid font families fall back to platform default monospace
- Config persists across sessions

**Dependencies:** None

---

### Task 17: Implement Default System Channels

**Title:** Create default output channels for Build, Git, Tasks, Extension Host, and Log

**Description:** On application startup, pre-create standard output channels so they are always available in the channel selector even before any output is written to them.

**Implementation Details:**
In `MarkAmpApp::OnInit()` or `OutputChannelService` constructor:
```cpp
auto default_channels = {"Build", "Git", "Tasks", "Extension Host", "Log"};
for (const auto& name : default_channels)
{
    service->create_channel(name);
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- Five default channels exist at startup
- Channels appear in the dropdown immediately
- Extension-created channels appear alongside default channels
- Default channels cannot be removed by extensions
- "Log" channel receives application log output (wired to MARKAMP_LOG macros)

**Dependencies:** Task 4

---

### Task 18: Implement Output Panel Context Menu

**Title:** Right-click context menu for the output panel

**Description:** Add a context menu with actions: Copy, Copy All, Select All, Clear, Open in Editor, Toggle Timestamps, Toggle Word Wrap, and channel-specific options.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Right-click shows context menu at mouse position
- Copy is disabled when there is no selection
- All actions invoke the correct panel methods
- Menu items show keyboard shortcut hints
- Menu styling matches the application theme

**Dependencies:** Task 6, Task 9, Task 10, Task 12

---

### Task 19: Redesign OutputPanel Rendering Architecture

**Title:** Replace wxTextCtrl with custom-painted virtual list for performance

**Description:** Replace the current `wxTextCtrl* text_area_` with a custom-drawn virtual list renderer. The virtual list only renders visible lines, enabling smooth scrolling through millions of output lines. Each line is painted with ANSI color segments.

**Implementation Details:**
The output panel becomes an owner-drawn panel similar to `TerminalPanel`:
```cpp
void OutputPanel::OnPaint(wxPaintEvent& event)
{
    wxAutoBufferedPaintDC dc(this);
    auto visible_start = scroll_offset_;
    auto visible_end = scroll_offset_ + visible_line_count();
    for (int i = visible_start; i < visible_end; ++i)
    {
        const auto& line = filtered_lines_[i];
        renderer_.paint_line(dc, line.segments, x, y, font_);
        y += line_height_;
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Virtual list renders only visible lines (no full-content wxTextCtrl)
- Smooth scrolling through 100,000+ lines
- ANSI colors render correctly per segment
- Text selection works across multiple lines
- Horizontal scrolling works when word wrap is off
- Frame time under 16ms for paint operations

**Dependencies:** Task 11

---

### Task 20: Wire Output Events to OutputChannelService

**Title:** Publish EventBus events from OutputChannelService on content changes

**Description:** Modify `OutputChannelService` to accept an `EventBus` reference and publish events when channels are created, content changes, or the active channel switches.

**Implementation Details:**
```cpp
class OutputChannelService
{
public:
    OutputChannelService() = default;
    explicit OutputChannelService(EventBus& event_bus);
    // ... existing API ...
private:
    EventBus* event_bus_{nullptr};
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.h`
- `/Users/ryanrentfro/code/markamp/src/core/OutputChannelService.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- `OutputChannelContentEvent` published on every `append_line()`
- `OutputChannelCreatedEvent` published on `create_channel()`
- `OutputChannelActiveChangedEvent` published on `set_active_channel()`
- EventBus is optional (null-checked) for backward compatibility with tests
- Existing tests continue to pass with the no-arg constructor

**Dependencies:** Task 3

---

### Task 21: Implement Filtered View Caching

**Title:** Cache filtered line indices for fast filter toggle

**Description:** When log level filters change, rebuild a cached index of visible line indices rather than copying line data. This allows O(1) access to the nth visible line during rendering.

**Implementation Details:**
```cpp
std::vector<int> filtered_indices_; // indices into channel.lines()

void rebuild_filtered_indices()
{
    filtered_indices_.clear();
    const auto& lines = active_channel_lines();
    for (int i = 0; i < static_cast<int>(lines.size()); ++i)
    {
        if (passes_filter(lines[i].level))
        {
            filtered_indices_.push_back(i);
        }
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/OutputPanel.cpp`

**Acceptance Criteria:**
- Filter toggle rebuild completes in under 5ms for 100,000 lines
- Filtered indices update incrementally when new lines arrive
- Scrollbar thumb size reflects the filtered line count
- Scroll position adjusts correctly when filters change

**Dependencies:** Task 5, Task 19

---

### Task 22: Update CMakeLists.txt

**Title:** Add new output panel source files to the build

**Description:** Add `LogLevel.h`, `OutputPanelRenderer.h/.cpp`, and test files to both `add_executable()` and `source_group()`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new files are in both `add_executable()` and `source_group()`
- Build succeeds on macOS with `cmake --preset debug`
- No linker errors from missing symbols

**Dependencies:** All create tasks

---

### Task 23: Write Unit Tests for Log Level Filtering

**Title:** Test log level filter logic in isolation

**Description:** Create Catch2 tests for `OutputChannel` line storage, log level filtering, and `OutputPanelRenderer` segment parsing.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_output_panel_v2.cpp` (create)

**Acceptance Criteria:**
- Tests verify `append_line()` with different log levels
- Tests verify `lines_filtered()` returns correct subsets
- Tests verify timestamp formatting
- Tests verify badge count increments and resets
- Tests verify ANSI parsing into rendered segments
- All tests pass

**Dependencies:** Task 2, Task 11

---

### Task 24: Implement Status Bar Integration

**Title:** Show output channel summary in the status bar

**Description:** Add an output status indicator in the status bar that shows the active channel name and error/warning counts. Clicking it opens the output panel.

**Implementation Details:**
Add to `StatusBarPanel`:
```cpp
void set_output_status(const std::string& channel, int errors, int warnings);
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp`

**Acceptance Criteria:**
- Status bar shows active output channel name when output panel is visible
- Error count shows in red, warning count shows in yellow
- Clicking the status bar item opens/focuses the output panel
- Status updates in real-time as new content arrives

**Dependencies:** Task 3

---

### Task 25: Write Integration Tests for Auto-Reveal and Badges

**Title:** Test auto-reveal behavior and badge tracking end-to-end

**Description:** Test that publishing content to a channel with `auto_reveal == true` triggers the correct event chain, and that badge counts increment and reset correctly.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_output_panel_v2.cpp`

**Acceptance Criteria:**
- Tests verify auto-reveal event published for eligible channels
- Tests verify badge count increments on content append
- Tests verify badge count resets on channel switch
- Tests verify aggregate badge count for panel tab
- Tests verify rate-limiting of auto-reveal events
- All tests pass

**Dependencies:** Task 14, Task 15

---

### Task 26: Migrate Existing OutputPanel Callers

**Title:** Update all existing code that uses OutputPanel to the new API

**Description:** Search for all call sites of the existing `OutputPanel` API (set_service, RefreshContent, ApplyTheme) and update them to use the new constructor and event-driven API. Ensure backward compatibility with the data-only constructor for tests.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- Any existing test files referencing `OutputPanel`

**Acceptance Criteria:**
- All existing call sites compile without warnings
- Data-only constructor still works for unit tests
- UI constructor accepts EventBus reference
- No regressions in existing OutputPanel functionality
- Old `wxTextCtrl`-based rendering is fully replaced

**Dependencies:** Task 19, Task 20

---

### Task 27: Documentation

**Title:** Update architecture documentation for Output Panel V2

**Description:** Document the new output panel architecture including channel management, log level system, ANSI rendering pipeline, and auto-reveal behavior.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md`

**Acceptance Criteria:**
- Architecture doc describes the OutputChannel data model
- Log level system is documented
- ANSI rendering pipeline is described
- Auto-reveal behavior and configuration is documented

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for log level filtering and line storage
- Unit tests for ANSI color segment rendering
- Unit tests for badge counting and reset logic
- Integration tests for auto-reveal event chain
- Performance test: 100,000 lines render in under 100ms
- Manual testing with real build output containing ANSI colors

## Phase Completion Criteria

- Output panel displays multi-channel output with a themed dropdown selector
- Log level filtering works with toggle buttons showing counts
- ANSI color codes render correctly in output
- Auto-reveal brings up the output panel when build output arrives
- Channel badges show unread counts
- Search/filter works within output content
- All unit and integration tests pass
