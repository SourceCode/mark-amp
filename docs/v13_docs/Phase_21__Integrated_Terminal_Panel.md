# Phase 21: Integrated Terminal Panel

## Overview

Implement a fully-featured embedded terminal emulator panel within MarkAmp Studio. The terminal provides direct shell access inside the IDE, supporting multiple sessions via tabs, split layouts, theme-aware coloring, and deep shell integration including CWD tracking. This replaces the current stub `core::TerminalService` with a real PTY-backed implementation and introduces the `TerminalPanel` UI component.

## Prerequisites

- Phase 09 (Toolbar Action Surface Redesign) -- toolbar action slots for terminal toggle
- Phase 03 (Layout Density and Spacing) -- panel sizing contracts
- Existing `core::TerminalService` stub at `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`
- Existing `LayoutManager` at `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
- Existing `ThemeAwareWindow` base class at `/Users/ryanrentfro/code/markamp/src/ui/ThemeAwareWindow.h`
- Existing `EventBus` and `Events.h` at `/Users/ryanrentfro/code/markamp/src/core/`

## Target Files

| Action | File |
|--------|------|
| Create | `/Users/ryanrentfro/code/markamp/src/core/PtyProcess.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/PtyProcess.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.h` |
| Create | `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/Events.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h` |
| Modify | `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp` |
| Modify | `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_terminal_panel.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_ansi_parser.cpp` |
| Create | `/Users/ryanrentfro/code/markamp/tests/unit/test_terminal_buffer.cpp` |

## Tasks

### Task 1: Define Terminal Event Contracts

**Title:** Declare terminal lifecycle and I/O events in Events.h

**Description:** Add event types for terminal session creation, destruction, data output, data input, title change, CWD change, bell, and resize. These events are the backbone of terminal-to-UI communication via the EventBus.

**Implementation Details:**
Add to `/Users/ryanrentfro/code/markamp/src/core/Events.h`:
```cpp
// ── Terminal Events ────────────────────────────────────────────
MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalCreatedEvent)
    int terminal_id{0};
    std::string name;
    std::string shell_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDestroyedEvent)
    int terminal_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDataOutputEvent)
    int terminal_id{0};
    std::string data;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalDataInputEvent)
    int terminal_id{0};
    std::string data;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalTitleChangedEvent)
    int terminal_id{0};
    std::string title;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalCwdChangedEvent)
    int terminal_id{0};
    std::string cwd;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalBellEvent)
    int terminal_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(TerminalResizedEvent)
    int terminal_id{0};
    int cols{80};
    int rows{24};
MARKAMP_DECLARE_EVENT_END;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/Events.h`

**Acceptance Criteria:**
- All eight terminal event types compile and are usable with `EventBus::publish()`
- Events follow the existing `MARKAMP_DECLARE_EVENT_WITH_FIELDS` macro convention
- No existing event contracts are broken

**Dependencies:** None

---

### Task 2: Implement ANSI Escape Sequence Parser

**Title:** Create AnsiParser for terminal color and control sequence interpretation

**Description:** Build a streaming ANSI escape sequence parser that translates VT100/VT220/xterm sequences into structured attribute commands. This parser handles SGR (Select Graphic Rendition) color codes, cursor movement, screen clearing, and title setting sequences. It does not render -- it produces a stream of parsed operations.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.h`:
```cpp
#pragma once
#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace markamp::core
{

struct AnsiColor
{
    enum class Type : uint8_t { kDefault, kStandard, kBright, k256, kRgb };
    Type type{Type::kDefault};
    uint8_t index{0};       // For kStandard, kBright, k256
    uint8_t r{0}, g{0}, b{0}; // For kRgb
};

struct AnsiAttributes
{
    AnsiColor foreground;
    AnsiColor background;
    bool bold{false};
    bool italic{false};
    bool underline{false};
    bool strikethrough{false};
    bool inverse{false};
    bool dim{false};
    bool hidden{false};
};

struct TextOutput { std::string text; AnsiAttributes attrs; };
struct CursorMove { int delta_row{0}; int delta_col{0}; };
struct CursorPosition { int row{0}; int col{0}; };
struct EraseDisplay { int mode{0}; }; // 0=below, 1=above, 2=all
struct EraseLine { int mode{0}; };
struct SetTitle { std::string title; };
struct Bell {};
struct LineFeed {};
struct CarriageReturn {};

using AnsiOp = std::variant<
    TextOutput, CursorMove, CursorPosition,
    EraseDisplay, EraseLine, SetTitle, Bell,
    LineFeed, CarriageReturn
>;

class AnsiParser
{
public:
    AnsiParser() = default;
    [[nodiscard]] auto parse(const std::string& raw) -> std::vector<AnsiOp>;
    void reset();
private:
    enum class State : uint8_t { kGround, kEscape, kCsi, kOsc, kOscString };
    State state_{State::kGround};
    std::string accumulator_;
    AnsiAttributes current_attrs_;
    void process_csi(const std::string& params, char final_char);
    void process_sgr(const std::string& params);
    void process_osc(const std::string& content);
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.cpp` (create)

**Acceptance Criteria:**
- Parses SGR color codes (standard 16, 256-color, and 24-bit RGB)
- Handles cursor movement CSI sequences (CUU, CUD, CUF, CUB, CUP)
- Handles erase sequences (ED, EL)
- Parses OSC title-set sequences (OSC 0 and OSC 2)
- Bell character (0x07) produces a `Bell` operation
- Streaming: partial sequences buffered across multiple `parse()` calls
- Unit tests cover all SGR variants, cursor moves, and title sets

**Dependencies:** None

---

### Task 3: Implement Terminal Character Buffer

**Title:** Create TerminalBuffer for scrollback and screen state management

**Description:** Implement a ring-buffer-backed terminal character buffer that maintains the visible screen grid (rows x cols) plus a configurable scrollback buffer. Each cell stores a character and its ANSI attributes. The buffer processes `AnsiOp` commands from the parser and maintains cursor position.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.h`:
```cpp
#pragma once
#include "AnsiParser.h"
#include <string>
#include <vector>

namespace markamp::core
{

struct TerminalCell
{
    char32_t character{U' '};
    AnsiAttributes attributes;
};

class TerminalBuffer
{
public:
    TerminalBuffer(int cols = 80, int rows = 24, int max_scrollback = 10000);

    void apply(const AnsiOp& op);
    void apply_batch(const std::vector<AnsiOp>& ops);

    void resize(int new_cols, int new_rows);

    [[nodiscard]] auto cols() const -> int;
    [[nodiscard]] auto rows() const -> int;
    [[nodiscard]] auto cursor_row() const -> int;
    [[nodiscard]] auto cursor_col() const -> int;

    [[nodiscard]] auto cell_at(int row, int col) const -> const TerminalCell&;
    [[nodiscard]] auto get_line_text(int row) const -> std::string;
    [[nodiscard]] auto scrollback_lines() const -> int;
    [[nodiscard]] auto get_scrollback_line(int index) const -> std::string;

    void clear_screen();
    void clear_scrollback();

    [[nodiscard]] auto get_selection_text(int start_row, int start_col,
                                          int end_row, int end_col) const -> std::string;

private:
    int cols_;
    int rows_;
    int max_scrollback_;
    int cursor_row_{0};
    int cursor_col_{0};
    AnsiAttributes current_attrs_;

    std::vector<std::vector<TerminalCell>> screen_;
    std::vector<std::vector<TerminalCell>> scrollback_;

    void scroll_up();
    void ensure_cursor_bounds();
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.cpp` (create)

**Acceptance Criteria:**
- Buffer correctly maintains an 80x24 (configurable) character grid
- Cursor movement operations clamp to grid bounds
- Line feed at bottom row scrolls content into scrollback
- `EraseDisplay` and `EraseLine` clear correct regions
- Scrollback ring buffer respects `max_scrollback` limit
- `get_selection_text` returns correct multi-line selection with newlines
- Unit tests verify all `AnsiOp` variants produce correct buffer state

**Dependencies:** Task 2

---

### Task 4: Implement PTY Process Wrapper

**Title:** Create PtyProcess for spawning and communicating with shell processes

**Description:** Implement a platform-abstracted PTY (pseudo-terminal) process manager. On macOS/Linux, use `forkpty()` from `<util.h>` (macOS) or `<pty.h>` (Linux). On Windows, use `CreatePseudoConsole`. The wrapper handles process spawning, I/O multiplexing via file descriptors, window resize signaling (SIGWINCH), and process lifecycle management.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/core/PtyProcess.h`:
```cpp
#pragma once
#include <functional>
#include <string>
#include <thread>
#include <atomic>
#include <vector>

namespace markamp::core
{

class PtyProcess
{
public:
    using DataCallback = std::function<void(const std::string& data)>;
    using ExitCallback = std::function<void(int exit_code)>;

    PtyProcess(const std::string& shell_path,
               const std::string& working_directory,
               int cols, int rows);
    ~PtyProcess();

    PtyProcess(const PtyProcess&) = delete;
    auto operator=(const PtyProcess&) -> PtyProcess& = delete;

    [[nodiscard]] auto start() -> bool;
    void stop();
    [[nodiscard]] auto is_running() const -> bool;

    void write(const std::string& data);
    void resize(int cols, int rows);

    void set_on_data(DataCallback callback);
    void set_on_exit(ExitCallback callback);

    [[nodiscard]] auto pid() const -> int;
    [[nodiscard]] auto shell_path() const -> const std::string&;
    [[nodiscard]] auto working_directory() const -> const std::string&;

private:
    std::string shell_path_;
    std::string working_directory_;
    int cols_;
    int rows_;
    int master_fd_{-1};
    int child_pid_{-1};
    std::atomic<bool> running_{false};
    std::thread read_thread_;
    DataCallback on_data_;
    ExitCallback on_exit_;

    void read_loop();
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PtyProcess.h` (create)
- `/Users/ryanrentfro/code/markamp/src/core/PtyProcess.cpp` (create)

**Acceptance Criteria:**
- Successfully spawns bash/zsh/fish using `forkpty()` on macOS
- Read thread continuously reads PTY master fd and invokes `on_data_` callback
- `write()` sends data to the PTY master fd
- `resize()` sends TIOCSWINSZ ioctl and SIGWINCH to the child process
- `stop()` sends SIGHUP then SIGTERM and joins the read thread
- Process exit triggers `on_exit_` callback with correct exit code
- Destructor safely cleans up even if `stop()` was not called
- All filesystem operations use `std::error_code` overloads per CLAUDE.md

**Dependencies:** None

---

### Task 5: Replace TerminalService Stub with Real Implementation

**Title:** Evolve TerminalService from stub to real PTY-backed session manager

**Description:** Replace the current stub implementation of `TerminalService` (which returns `nullopt` for all operations) with a real implementation that manages multiple `PtyProcess` instances, routes output through the `EventBus`, and maintains session metadata.

**Implementation Details:**
Rewrite `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`:
```cpp
#pragma once
#include "EventBus.h"
#include "PtyProcess.h"
#include "TerminalBuffer.h"
#include "AnsiParser.h"
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

struct TerminalSession
{
    int terminal_id{0};
    std::string name;
    std::string shell_path;
    std::string working_directory;
    std::unique_ptr<PtyProcess> process;
    std::unique_ptr<AnsiParser> parser;
    std::unique_ptr<TerminalBuffer> buffer;
};

class TerminalService
{
public:
    explicit TerminalService(EventBus& event_bus);

    auto create_terminal(const std::string& name,
                         const std::string& shell_path = {},
                         const std::string& working_directory = {}) -> std::optional<int>;
    auto send_text(int terminal_id, const std::string& text) -> bool;
    [[nodiscard]] auto terminals() const -> std::vector<Terminal>;
    auto close_terminal(int terminal_id) -> bool;
    void close_all_terminals();

    [[nodiscard]] auto get_buffer(int terminal_id) -> TerminalBuffer*;
    void resize_terminal(int terminal_id, int cols, int rows);

    [[nodiscard]] auto active_terminal_id() const -> int;
    void set_active_terminal(int terminal_id);

    [[nodiscard]] auto detect_default_shell() const -> std::string;

private:
    EventBus& event_bus_;
    std::unordered_map<int, std::unique_ptr<TerminalSession>> sessions_;
    int next_id_{1};
    int active_terminal_id_{0};

    void on_terminal_data(int terminal_id, const std::string& data);
    void on_terminal_exit(int terminal_id, int exit_code);
};

} // namespace markamp::core
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h` (rewrite)
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp` (rewrite)

**Acceptance Criteria:**
- `create_terminal()` spawns a real shell process and returns a valid terminal ID
- `detect_default_shell()` reads `$SHELL` on macOS/Linux, falls back to `/bin/zsh`
- Terminal output is parsed through `AnsiParser` into `TerminalBuffer` and published as `TerminalDataOutputEvent`
- `close_terminal()` kills the process and publishes `TerminalDestroyedEvent`
- All session lookups guard against invalid terminal IDs without crashing
- Constructor injection of `EventBus` per CLAUDE.md convention

**Dependencies:** Task 1, Task 2, Task 3, Task 4

---

### Task 6: Create TerminalPanel UI Component

**Title:** Implement the main TerminalPanel wxWidgets component

**Description:** Create the `TerminalPanel` as a `ThemeAwareWindow` subclass that renders the terminal buffer contents using custom painting. The panel handles keyboard input, mouse selection, and scroll events. It communicates with `TerminalService` via the `EventBus` for all I/O.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/TerminalService.h"
#include <wx/timer.h>
#include <string>

namespace markamp::ui
{

class TerminalPanel : public ThemeAwareWindow
{
public:
    TerminalPanel(wxWindow* parent,
                  core::ThemeEngine& theme_engine,
                  core::EventBus& event_bus,
                  core::TerminalService& terminal_service);

    void SetActiveTerminal(int terminal_id);
    [[nodiscard]] auto active_terminal_id() const -> int;

    void ScrollToBottom();
    void ScrollUp(int lines);
    void ScrollDown(int lines);

    void SelectAll();
    void CopySelection();
    void PasteClipboard();
    void ClearTerminal();

    void FindInTerminal(const std::string& query);
    void FindNext();
    void FindPrevious();

    static constexpr int kDefaultFontSize = 13;
    static constexpr int kCursorBlinkMs = 500;
    static constexpr int kMinCols = 20;
    static constexpr int kMinRows = 4;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::TerminalService& terminal_service_;
    int active_terminal_id_{0};

    // Rendering
    wxFont terminal_font_;
    int char_width_{0};
    int char_height_{0};
    int scroll_offset_{0};

    // Selection
    bool selecting_{false};
    int sel_start_row_{0};
    int sel_start_col_{0};
    int sel_end_row_{0};
    int sel_end_col_{0};

    // Cursor blink
    wxTimer cursor_blink_timer_;
    bool cursor_visible_{true};

    // Search
    std::string find_query_;
    std::vector<std::pair<int, int>> find_matches_;
    int find_current_index_{-1};

    // Event subscriptions
    core::Subscription data_sub_;
    core::Subscription destroyed_sub_;

    void OnPaint(wxPaintEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnChar(wxKeyEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnCursorBlink(wxTimerEvent& event);

    void CalculateCellDimensions();
    void CalculateTerminalSize();
    auto ScreenToCell(const wxPoint& point) -> std::pair<int, int>;
    auto MapAnsiColorToWx(const core::AnsiColor& color, bool is_foreground) -> wxColour;
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp` (create)

**Acceptance Criteria:**
- Renders terminal buffer contents with correct colors mapped from ANSI to theme colors
- Keyboard input is sent to the active terminal via `TerminalService::send_text()`
- Mouse selection highlights cells and `CopySelection()` puts text on the clipboard
- Panel resize triggers `TerminalService::resize_terminal()` with new col/row counts
- Cursor blinks at 500ms intervals
- Scrolling via mouse wheel navigates scrollback buffer
- Theme changes update all terminal colors immediately

**Dependencies:** Task 5

---

### Task 7: Create Terminal Tab Bar

**Title:** Implement TerminalTabBar for multiple terminal session management

**Description:** Create a tab bar specifically for terminal sessions, displayed at the top of the terminal panel area. Each tab represents one terminal session showing the session name and shell type. Supports new terminal creation, tab close, and tab switching.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.h`:
```cpp
#pragma once
#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include <string>
#include <vector>

namespace markamp::ui
{

class TerminalTabBar : public ThemeAwareWindow
{
public:
    TerminalTabBar(wxWindow* parent,
                   core::ThemeEngine& theme_engine,
                   core::EventBus& event_bus);

    struct TerminalTab
    {
        int terminal_id{0};
        std::string name;
        std::string shell_type;
        bool is_active{false};
        wxRect rect;
        wxRect close_rect;
    };

    void AddTab(int terminal_id, const std::string& name, const std::string& shell_type);
    void RemoveTab(int terminal_id);
    void SetActiveTab(int terminal_id);
    void RenameTab(int terminal_id, const std::string& new_name);
    [[nodiscard]] auto tab_count() const -> int;

    using TabSelectedCallback = std::function<void(int terminal_id)>;
    using NewTerminalCallback = std::function<void()>;
    using CloseTerminalCallback = std::function<void(int terminal_id)>;
    using SplitTerminalCallback = std::function<void(int terminal_id)>;

    void set_on_tab_selected(TabSelectedCallback cb);
    void set_on_new_terminal(NewTerminalCallback cb);
    void set_on_close_terminal(CloseTerminalCallback cb);
    void set_on_split_terminal(SplitTerminalCallback cb);

    static constexpr int kHeight = 28;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    std::vector<TerminalTab> tabs_;

    TabSelectedCallback on_tab_selected_;
    NewTerminalCallback on_new_terminal_;
    CloseTerminalCallback on_close_terminal_;
    SplitTerminalCallback on_split_terminal_;

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void RecalculateTabRects();

    int hovered_tab_{-1};
};

} // namespace markamp::ui
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.h` (create)
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.cpp` (create)

**Acceptance Criteria:**
- Tabs are rendered with terminal name, shell icon, and close button
- Clicking a tab switches the active terminal in `TerminalPanel`
- "+" button at the end creates a new terminal session
- Close button on tab sends close callback
- Right-click shows context menu with Rename, Split, Kill options
- Tab bar height is 28px, consistent with existing panel header heights

**Dependencies:** Task 6

---

### Task 8: Implement Shell Profile Detection and Selection

**Title:** Detect available shells and support profile selection per terminal

**Description:** Implement shell detection that discovers available shells on the system (bash, zsh, fish, powershell, sh) by checking known paths and `/etc/shells`. Provide a dropdown in the terminal creation flow to select which shell to use. Remember the user's preferred shell in config.

**Implementation Details:**
Add to `TerminalService`:
```cpp
struct ShellProfile
{
    std::string name;       // "zsh", "bash", "fish"
    std::string path;       // "/bin/zsh"
    std::string icon_hint;  // "shell_zsh"
    bool is_default{false};
};

[[nodiscard]] auto detect_available_shells() const -> std::vector<ShellProfile>;
[[nodiscard]] auto preferred_shell() const -> std::string;
void set_preferred_shell(const std::string& shell_path);
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Detects at least bash, zsh, and sh on macOS
- Reads `/etc/shells` for the full list on POSIX systems
- Default shell is read from `$SHELL` environment variable
- Preferred shell is persisted in config YAML
- Invalid shell paths are filtered out (must exist and be executable)

**Dependencies:** Task 5

---

### Task 9: Implement Terminal Split View

**Title:** Support horizontal split within the terminal panel area

**Description:** Allow the user to split the terminal panel horizontally, showing two terminal sessions side by side. Use a `wxSplitterWindow` or the existing `SplitterBar` component to manage the split. Each split pane contains its own `TerminalPanel` instance.

**Implementation Details:**
The terminal area becomes a container that can hold 1 or 2 `TerminalPanel` instances. Add split/unsplit methods to the terminal area host:
```cpp
class TerminalAreaHost : public ThemeAwareWindow
{
public:
    void SplitHorizontal(int left_terminal_id, int right_terminal_id);
    void Unsplit();
    [[nodiscard]] auto is_split() const -> bool;
    [[nodiscard]] auto left_panel() -> TerminalPanel*;
    [[nodiscard]] auto right_panel() -> TerminalPanel*;
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- Split creates two equally-sized terminal panes with a draggable divider
- Each pane can run an independent terminal session
- Unsplit removes the right pane and expands the left to full width
- Focus follows the pane the user clicks in
- Splitting preserves the existing terminal session in the left pane
- Splitter bar uses theme colors consistent with the editor split view

**Dependencies:** Task 6, Task 7

---

### Task 10: Implement Terminal Theme Color Mapping

**Title:** Map terminal ANSI colors to the active application theme

**Description:** Create a mapping table that translates the 16 standard ANSI colors (and their bright variants) to colors derived from the active `Theme`. The terminal background and foreground use the theme's `bg_editor` and `fg_primary` tokens. The 16 ANSI palette colors are mapped to theme-derived values so terminals always look cohesive.

**Implementation Details:**
```cpp
struct TerminalColorScheme
{
    wxColour background;
    wxColour foreground;
    wxColour cursor;
    wxColour selection_bg;
    std::array<wxColour, 16> ansi_palette; // 0-7 standard, 8-15 bright
};

auto BuildTerminalColorScheme(const core::Theme& theme) -> TerminalColorScheme;
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Dark themes produce dark terminal backgrounds with light text
- Light themes produce light terminal backgrounds with dark text
- ANSI red, green, yellow, blue, magenta, cyan are visually distinct in both dark and light themes
- 256-color and RGB colors pass through unchanged
- Theme changes trigger immediate terminal repaint with new colors
- Selection highlight uses theme `bg_selection` token with alpha blending

**Dependencies:** Task 6

---

### Task 11: Implement Copy and Paste Support

**Title:** Full clipboard integration for terminal panels

**Description:** Implement copy (Cmd+C / Ctrl+C), paste (Cmd+V / Ctrl+V), and select-all (Cmd+A) for the terminal. Copy extracts the selected text from the `TerminalBuffer` selection range. Paste reads from the system clipboard and writes to the terminal process. When there is no selection, Ctrl+C sends the interrupt signal (0x03) to the process.

**Implementation Details:**
In `TerminalPanel::OnKeyDown()`:
- If Cmd/Ctrl+C and has selection: copy selection to clipboard
- If Cmd/Ctrl+C and no selection: send `\x03` to the PTY
- If Cmd/Ctrl+V: read clipboard, send text to PTY
- If Cmd/Ctrl+A: select all visible + scrollback text
- If Cmd/Ctrl+Shift+C: always copy (even with no selection, copies visible screen)

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Copy with selection puts multi-line text on clipboard with correct newlines
- Paste sends clipboard content to the PTY process character by character
- Ctrl+C without selection correctly interrupts the running process
- Select-all highlights all content including scrollback
- Clipboard operations work on macOS (using wxClipboard)

**Dependencies:** Task 6

---

### Task 12: Implement Link Detection and Click-to-Open

**Title:** Detect URLs and file paths in terminal output for click-to-open

**Description:** Scan terminal buffer content for URLs (http://, https://, file://) and local file paths. When the user Cmd-clicks (or Ctrl-clicks on Linux) a detected link, open it in the default browser or navigate to the file in the editor. Detected links are rendered with an underline decoration on hover.

**Implementation Details:**
```cpp
struct TerminalLink
{
    int start_row, start_col;
    int end_row, end_col;
    std::string url;
    enum class Type { kUrl, kFilePath, kFileLineRef }; // file:line:col
    Type type;
};

class TerminalLinkDetector
{
public:
    [[nodiscard]] auto detect_links(const TerminalBuffer& buffer, int visible_start, int visible_end)
        -> std::vector<TerminalLink>;
private:
    static const std::regex url_regex_;
    static const std::regex file_path_regex_;
    static const std::regex file_line_regex_; // matches "file.cpp:42:10"
};
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- URLs starting with `http://` or `https://` are detected and clickable
- File paths matching common patterns (`/path/to/file.ext:line:col`) open in the editor
- Cmd+Click opens URLs in the default browser
- Cmd+Click on file paths publishes `FileOpenedEvent` with the path
- Links show underline decoration when Cmd is held and mouse hovers over them
- Detection runs incrementally on visible rows only (not full scrollback)

**Dependencies:** Task 6, Task 3

---

### Task 13: Implement Find in Terminal

**Title:** Add text search within terminal buffer content

**Description:** Implement a find bar that appears at the top of the terminal panel when Cmd+F is pressed. The find bar searches all terminal buffer content (visible screen + scrollback) and highlights matches. Supports next/previous match navigation.

**Implementation Details:**
Reuse the find bar pattern from `EditorPanel` but adapted for terminal:
- Search runs against `TerminalBuffer::get_line_text()` for all rows
- Matches are stored as (row, col, length) tuples
- Current match is scrolled into view and highlighted differently
- Case-insensitive search by default with a toggle

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Cmd+F shows the find bar at the top of the terminal panel
- Typing in the find bar highlights all matches in the terminal buffer
- Enter navigates to the next match; Shift+Enter to the previous
- Match count is displayed (e.g., "3 of 12")
- Escape closes the find bar
- Find bar uses theme colors consistent with the editor find bar

**Dependencies:** Task 6, Task 3

---

### Task 14: Implement Clear Terminal and Scrollback

**Title:** Add clear screen and clear scrollback commands

**Description:** Implement clear terminal (Cmd+K equivalent) that clears the visible screen and scrollback buffer, and a soft clear that sends the clear screen escape sequence to the shell. Both are accessible via keyboard shortcut, context menu, and the terminal header action bar.

**Implementation Details:**
```cpp
// In TerminalPanel:
void ClearTerminal();     // Clears buffer + scrollback
void SoftClear();         // Sends "\x1b[2J\x1b[H" to the PTY
void ClearScrollback();   // Clears only scrollback, keeps visible screen
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Cmd+K clears the terminal buffer and scrollback completely
- Soft clear sends escape sequences so the shell redraws its prompt
- Clear scrollback removes history but preserves the visible screen
- All three operations are available in the terminal context menu
- After clear, the terminal cursor is at row 0, col 0

**Dependencies:** Task 6, Task 3

---

### Task 15: Implement Terminal Font Settings

**Title:** Configurable terminal font family, size, and line height

**Description:** Allow the user to configure the terminal font independently from the editor font. The terminal defaults to a monospace font (Menlo on macOS, Consolas on Windows, DejaVu Sans Mono on Linux) at 13px with 1.2 line height. Settings are persisted in config and applied immediately.

**Implementation Details:**
```cpp
struct TerminalFontConfig
{
    std::string family{"Menlo"};
    int size{13};
    float line_height{1.2f};
    bool ligatures{false};
};
```
Add to config YAML under `terminal.font`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Font family, size, and line height are configurable
- Changes apply immediately without restarting the terminal session
- Font changes trigger recalculation of char_width/char_height and terminal resize
- Invalid font families fall back to the platform default monospace font
- Config persistence round-trips correctly

**Dependencies:** Task 6

---

### Task 16: Implement Terminal Resize Handling

**Title:** Dynamic terminal resize on panel geometry changes

**Description:** When the terminal panel is resized (window resize, splitter drag, panel show/hide), recalculate the terminal dimensions in columns and rows based on the current font metrics, and send a resize signal to the PTY process. The resize must debounce to avoid flooding the shell with SIGWINCH signals.

**Implementation Details:**
```cpp
void TerminalPanel::OnSize(wxSizeEvent& event)
{
    CalculateCellDimensions();
    auto new_cols = GetClientSize().GetWidth() / char_width_;
    auto new_rows = GetClientSize().GetHeight() / char_height_;
    new_cols = std::max(new_cols, kMinCols);
    new_rows = std::max(new_rows, kMinRows);
    // Debounce resize: restart a 50ms timer
    resize_debounce_timer_.StartOnce(50);
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Panel resize recalculates cols/rows from pixel dimensions and font metrics
- Resize is debounced with a 50ms delay to prevent rapid SIGWINCH signals
- Minimum terminal size is enforced (20 cols, 4 rows)
- Buffer resize reflows content correctly (lines wrap/unwrap)
- Resize is communicated to `TerminalService::resize_terminal()` which sends TIOCSWINSZ

**Dependencies:** Task 6, Task 5

---

### Task 17: Implement Shell Integration (CWD Tracking)

**Title:** Track the terminal's current working directory via shell integration

**Description:** Implement shell integration that tracks the terminal's current working directory (CWD). On zsh/bash, inject a precmd hook that emits an OSC 7 sequence with the current directory. The `AnsiParser` interprets OSC 7 and publishes a `TerminalCwdChangedEvent`. This CWD is displayed in the terminal tab tooltip and can be used for "Open Terminal Here" features.

**Implementation Details:**
Add OSC 7 handling to `AnsiParser::process_osc()`:
```cpp
void AnsiParser::process_osc(const std::string& content)
{
    if (content.starts_with("7;"))
    {
        // OSC 7; file://hostname/path
        auto url = content.substr(2);
        // Extract path from file:// URL
    }
    // ... existing OSC 0/2 title handling
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.h`
- `/Users/ryanrentfro/code/markamp/src/core/AnsiParser.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp`

**Acceptance Criteria:**
- OSC 7 sequences are parsed and CWD is extracted
- `TerminalCwdChangedEvent` is published when CWD changes
- Terminal tab tooltip shows the current working directory
- CWD tracking works with zsh and bash (the two most common macOS shells)
- Invalid OSC 7 payloads are silently ignored

**Dependencies:** Task 2, Task 5

---

### Task 18: Implement Terminal Context Menu

**Title:** Right-click context menu for terminal panels

**Description:** Implement a context menu for the terminal panel with standard actions: Copy, Paste, Select All, Clear Terminal, Split Terminal, Find, and Kill Process. The menu uses the existing `ContextMenuModel` architecture from Phase 16.

**Implementation Details:**
Build context menu items using `ContextMenuItem` structs:
```cpp
void TerminalPanel::ShowContextMenu()
{
    wxMenu menu;
    menu.Append(ID_COPY, "Copy\tCmd+C");
    menu.Append(ID_PASTE, "Paste\tCmd+V");
    menu.AppendSeparator();
    menu.Append(ID_SELECT_ALL, "Select All\tCmd+A");
    menu.Append(ID_FIND, "Find...\tCmd+F");
    menu.AppendSeparator();
    menu.Append(ID_CLEAR, "Clear Terminal\tCmd+K");
    menu.Append(ID_SPLIT, "Split Terminal");
    menu.AppendSeparator();
    menu.Append(ID_KILL, "Kill Terminal Process");
    // Enable/disable based on state
    menu.Enable(ID_COPY, HasSelection());
    PopupMenu(&menu);
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Right-click shows the context menu at the mouse position
- Copy is disabled when there is no selection
- Kill confirms before sending SIGTERM to the process
- All menu items have keyboard shortcut hints displayed
- Menu styling matches the application theme

**Dependencies:** Task 6, Task 11

---

### Task 19: Implement Kill Terminal Process

**Title:** Graceful and forced process termination for terminal sessions

**Description:** Implement terminal process killing with a two-stage approach: first send SIGHUP (allow graceful shutdown), wait 2 seconds, then send SIGKILL if still running. Show a confirmation dialog before killing. After the process exits, display an exit message in the terminal buffer and allow the tab to remain open for inspection.

**Implementation Details:**
```cpp
void TerminalService::kill_terminal(int terminal_id, bool force)
{
    auto it = sessions_.find(terminal_id);
    if (it == sessions_.end()) return;

    if (force)
    {
        it->second->process->stop(); // SIGHUP then SIGTERM
    }
    else
    {
        it->second->process->write("\x03"); // Send Ctrl+C first
    }
}
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.h`
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- Soft kill sends Ctrl+C interrupt
- Hard kill sends SIGHUP, waits 2s, then SIGKILL
- Exit message "Process exited with code N" is appended to the terminal buffer
- Terminal tab remains open after process exit (user can close manually)
- Confirmation dialog appears before hard kill
- Killing a terminal that has already exited is a no-op

**Dependencies:** Task 5, Task 7

---

### Task 20: Implement Terminal Bell Notification

**Title:** Visual bell notification when a terminal emits BEL character

**Description:** When the terminal receives a BEL character (0x07), flash the terminal tab with a highlight color and optionally show a notification badge. Do not play an audio bell. The flash duration is 300ms.

**Implementation Details:**
When `AnsiParser` emits a `Bell` operation, `TerminalService` publishes `TerminalBellEvent`. The `TerminalTabBar` subscribes and flashes the relevant tab.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalTabBar.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`

**Acceptance Criteria:**
- BEL character triggers a visual flash on the terminal tab (not the active tab)
- Flash uses the theme's accent color with 300ms duration
- No audio is played
- Multiple rapid BEL characters coalesce into a single flash
- Active terminal tab does not flash (user is already looking at it)

**Dependencies:** Task 2, Task 7

---

### Task 21: Implement Working Directory from Active File

**Title:** Open new terminals with CWD set to the active file's directory

**Description:** When creating a new terminal, if the user has a file open in the editor, set the terminal's initial working directory to that file's parent directory. If no file is open, use the workspace root. Provide an "Open Terminal Here" action in the file tree context menu.

**Implementation Details:**
Listen to `ActiveFileChangedEvent` to track the current file. Extract the parent directory and pass it to `TerminalService::create_terminal()`.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/FileTreeCtrl.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`

**Acceptance Criteria:**
- New terminal opens in the directory of the currently active file
- "Open Terminal Here" in file tree context menu opens terminal in that directory
- If no file is open, terminal opens in workspace root
- If no workspace is open, terminal opens in user's home directory
- CWD is passed to `PtyProcess` which uses it as the child process's initial directory

**Dependencies:** Task 5, Task 18

---

### Task 22: Integrate Terminal Panel into LayoutManager

**Title:** Wire TerminalPanel into the bottom panel area of the main layout

**Description:** Add the terminal panel as a new bottom panel option alongside OutputPanel and ProblemsPanel. The terminal is accessible via the panel bar, keyboard shortcut (Cmd+`), and the toolbar toggle. The layout manager handles show/hide/resize of the terminal panel.

**Implementation Details:**
In `LayoutManager`, add:
```cpp
enum class BottomPanelType { kOutput, kProblems, kTerminal, kDebugConsole, kBuild };
void ShowBottomPanel(BottomPanelType type);
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h`
- `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.cpp`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`

**Acceptance Criteria:**
- Terminal panel appears in the bottom panel area when activated
- Cmd+` (backtick) toggles the terminal panel
- Panel tabs at the top of the bottom area show: Output, Problems, Terminal
- Switching between bottom panels preserves terminal session state
- Terminal panel is lazily created on first access
- Layout state (which bottom panel is active) persists across sessions

**Dependencies:** Task 6, Task 7

---

### Task 23: Wire TerminalService into PluginContext

**Title:** Expose TerminalService to extensions via PluginContext

**Description:** Update `PluginContext` to include a pointer to the real `TerminalService` (replacing the stub pointer) and ensure it is populated during application startup in `MarkAmpApp::OnInit()`.

**Implementation Details:**
Update `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`:
```cpp
// Already has: TerminalService* terminal_service{nullptr};
// Just ensure MarkAmpApp populates it with the real instance
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/PluginContext.h`
- `/Users/ryanrentfro/code/markamp/src/app/MarkAmpApp.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/PluginManager.cpp`

**Acceptance Criteria:**
- `PluginContext::terminal_service` points to the real `TerminalService` instance
- Extensions can call `create_terminal()`, `send_text()`, and `close_terminal()`
- Null-check guard in `PluginManager` logs a warning if terminal service is unavailable
- Existing extension API surface continues to compile without changes

**Dependencies:** Task 5

---

### Task 24: Update CMakeLists.txt

**Title:** Add all new terminal source files to the build

**Description:** Add all new `.h` and `.cpp` files to both `add_executable()` and `source_group()` in `CMakeLists.txt`. Add platform-specific compile flags for PTY support.

**Implementation Details:**
Add to `src/CMakeLists.txt`:
```cmake
# Terminal sources
src/core/PtyProcess.h
src/core/PtyProcess.cpp
src/core/TerminalBuffer.h
src/core/TerminalBuffer.cpp
src/core/AnsiParser.h
src/core/AnsiParser.cpp
src/ui/TerminalPanel.h
src/ui/TerminalPanel.cpp
src/ui/TerminalTabBar.h
src/ui/TerminalTabBar.cpp
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

**Acceptance Criteria:**
- All new files are in both `add_executable()` and `source_group()`
- `source_group()` entries match the directory structure
- macOS build links against `-lutil` for `forkpty()`
- Build succeeds with `cmake --preset debug && cmake --build build/debug -j$(sysctl -n hw.ncpu)`

**Dependencies:** All create tasks (1-7)

---

### Task 25: Write Unit Tests for AnsiParser

**Title:** Comprehensive unit tests for ANSI escape sequence parsing

**Description:** Create a Catch2 test file that covers all ANSI parser functionality including SGR codes, cursor movement, screen clearing, OSC title setting, bell, and streaming partial sequences.

**Implementation Details:**
Create `/Users/ryanrentfro/code/markamp/tests/unit/test_ansi_parser.cpp`:
```cpp
#include <catch2/catch_test_macros.hpp>
#include "core/AnsiParser.h"

TEST_CASE("AnsiParser basic text", "[terminal][ansi]")
{
    markamp::core::AnsiParser parser;
    auto ops = parser.parse("Hello World");
    REQUIRE(ops.size() == 1);
    auto* text = std::get_if<markamp::core::TextOutput>(&ops[0]);
    REQUIRE(text != nullptr);
    CHECK(text->text == "Hello World");
}
// ... 20+ test sections covering SGR, cursor, erase, OSC, bell, streaming
```

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_ansi_parser.cpp` (create)
- `/Users/ryanrentfro/code/markamp/tests/unit/CMakeLists.txt`

**Acceptance Criteria:**
- Tests cover: plain text, SGR bold/italic/underline, SGR 16 colors, SGR 256 colors, SGR RGB
- Tests cover: cursor up/down/left/right, cursor absolute position
- Tests cover: erase display (modes 0/1/2), erase line (modes 0/1/2)
- Tests cover: OSC 0 title set, OSC 7 CWD, bell character
- Tests cover: streaming (partial escape sequence across two parse() calls)
- All tests pass with `ctest --output-on-failure`

**Dependencies:** Task 2

---

### Task 26: Write Unit Tests for TerminalBuffer

**Title:** Comprehensive unit tests for terminal buffer state management

**Description:** Create a Catch2 test file covering buffer initialization, text insertion, cursor movement, scrolling, scrollback, selection, resize, and all erase operations.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_terminal_buffer.cpp` (create)
- `/Users/ryanrentfro/code/markamp/tests/unit/CMakeLists.txt`

**Acceptance Criteria:**
- Tests verify initial buffer state (blank screen, cursor at 0,0)
- Tests verify text output updates correct cells
- Tests verify line feed at bottom row scrolls to scrollback
- Tests verify scrollback size limit is enforced
- Tests verify `get_selection_text()` for single-line and multi-line selections
- Tests verify `resize()` reflows content correctly
- Tests verify all erase operations clear correct regions
- All tests pass

**Dependencies:** Task 3

---

### Task 27: Write Integration Tests for TerminalPanel

**Title:** Integration tests for terminal panel data-only mode

**Description:** Create a test file that exercises the terminal panel creation flow using data-only constructors where possible, and verifies event wiring between `TerminalService`, `AnsiParser`, `TerminalBuffer`, and event bus.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/tests/unit/test_terminal_panel.cpp` (create)
- `/Users/ryanrentfro/code/markamp/tests/unit/CMakeLists.txt`

**Acceptance Criteria:**
- Tests verify terminal creation publishes `TerminalCreatedEvent`
- Tests verify terminal destruction publishes `TerminalDestroyedEvent`
- Tests verify data flow from service through parser to buffer
- Tests verify terminal resize publishes `TerminalResizedEvent`
- Tests verify CWD change publishes `TerminalCwdChangedEvent`
- All tests pass

**Dependencies:** Task 5, Task 6

---

### Task 28: Implement Terminal Scrollback Buffer Configuration

**Title:** Configurable scrollback buffer size

**Description:** Allow the user to configure the terminal scrollback buffer size (default 10,000 lines, min 1,000, max 100,000). The setting is in config YAML under `terminal.scrollback_lines`. Changing the setting applies to new terminal sessions only.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/core/TerminalBuffer.h`
- `/Users/ryanrentfro/code/markamp/src/core/TerminalService.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/Config.cpp`

**Acceptance Criteria:**
- Default scrollback is 10,000 lines
- Config value is validated (clamped to 1,000-100,000 range)
- New terminals use the configured scrollback size
- Existing terminals are not affected by config changes
- Scrollback buffer correctly drops oldest lines when limit is reached

**Dependencies:** Task 3, Task 5

---

### Task 29: Implement Keyboard Shortcut Bindings for Terminal

**Title:** Register all terminal keyboard shortcuts in the shortcut system

**Description:** Register terminal-specific keyboard shortcuts that are active when the terminal panel has focus. These must not conflict with editor shortcuts when the editor has focus.

**Implementation Details:**
| Action | Shortcut (macOS) | Shortcut (Other) |
|--------|-------------------|-------------------|
| New Terminal | Cmd+Shift+` | Ctrl+Shift+` |
| Toggle Terminal | Cmd+` | Ctrl+` |
| Copy | Cmd+C | Ctrl+Shift+C |
| Paste | Cmd+V | Ctrl+Shift+V |
| Clear Terminal | Cmd+K | Ctrl+K |
| Find in Terminal | Cmd+F | Ctrl+F |
| Split Terminal | Cmd+\ | Ctrl+\ |
| Kill Terminal | Cmd+Shift+K | Ctrl+Shift+K |
| Next Terminal | Cmd+Shift+] | Ctrl+Shift+] |
| Previous Terminal | Cmd+Shift+[ | Ctrl+Shift+[ |

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/src/ui/TerminalPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/core/ShortcutService.cpp`

**Acceptance Criteria:**
- All shortcuts are registered and active only when terminal has focus
- Shortcuts do not conflict with editor shortcuts
- Shortcuts appear in the keyboard shortcut editor
- Shortcuts are discoverable via the command palette
- Platform-specific modifier keys are used correctly

**Dependencies:** Task 6

---

### Task 30: Documentation and Phase ADR

**Title:** Write terminal panel architecture decision record and user documentation

**Description:** Document the architectural decisions made in this phase (PTY approach, ANSI parser design, buffer architecture), alternatives considered (using an external terminal widget library vs custom), and the integration points with the rest of the application.

**Files Affected:**
- `/Users/ryanrentfro/code/markamp/docs/architecture.md` (update)

**Acceptance Criteria:**
- ADR documents the PTY-based approach and why it was chosen over libvterm or xterm.js
- Integration points are documented (EventBus events, PluginContext, LayoutManager)
- Known limitations are listed (no sixel graphics, no true color on Windows console)
- User-facing documentation describes how to use the terminal panel

**Dependencies:** All previous tasks

## Testing Requirements

- Unit tests for `AnsiParser` cover all SGR, CSI, and OSC sequences
- Unit tests for `TerminalBuffer` cover all buffer operations and scrollback
- Integration tests verify end-to-end data flow from PTY to rendered buffer
- Manual testing on macOS with bash, zsh, and fish shells
- Resize behavior tested with rapid window resizing
- Theme switching tested while terminal is active

## Phase Completion Criteria

- `TerminalService` is no longer a stub -- it manages real PTY processes
- Terminal panel renders shell output with correct ANSI colors
- Multiple terminal sessions are supported via tabs
- Split terminal displays two sessions side by side
- Terminal theme colors adapt to the active application theme
- Copy/paste, find, clear, and link detection all function correctly
- All unit and integration tests pass
- Terminal is accessible via Cmd+` keyboard shortcut
