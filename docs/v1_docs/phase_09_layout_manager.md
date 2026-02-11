# Phase 09: Layout Manager -- Sidebar, Content Area, Status Bar

## Objective

Implement the main application layout: a three-zone structure consisting of a collapsible sidebar on the left, a main content area (which will host the editor and preview), and a status bar at the bottom. The layout must be responsive to window resizing, support animated sidebar show/hide, and use the theme system from Phase 08.

## Prerequisites

- Phase 06 (Custom Window Chrome)
- Phase 08 (Theme Engine and Runtime Switching)

## Deliverables

1. LayoutManager class that orchestrates the main window zones
2. Sidebar container (placeholder content until Phase 10)
3. Content area container (placeholder content until Phases 12/18)
4. Status bar container (placeholder content until Phase 23)
5. Sidebar toggle with smooth animation
6. Splitter between sidebar and content area
7. Proper resize behavior with minimum sizes

## Tasks

### Task 9.1: Implement the LayoutManager

Create `/Users/ryanrentfro/code/markamp/src/ui/LayoutManager.h` and `LayoutManager.cpp`:

The LayoutManager owns the top-level panel arrangement inside `MainFrame` (below the custom chrome).

```cpp
namespace markamp::ui {

class LayoutManager : public ThemeAwareWindow
{
public:
    LayoutManager(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // Zone access (for later phases to populate)
    [[nodiscard]] auto sidebar_container() -> wxPanel*;
    [[nodiscard]] auto content_container() -> wxPanel*;
    [[nodiscard]] auto statusbar_container() -> wxPanel*;

    // Sidebar control
    void toggle_sidebar();
    void set_sidebar_visible(bool visible);
    [[nodiscard]] auto is_sidebar_visible() const -> bool;

    // Sidebar width
    void set_sidebar_width(int width);
    [[nodiscard]] auto sidebar_width() const -> int;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // Layout components
    wxPanel* sidebar_panel_{nullptr};
    wxPanel* content_panel_{nullptr};
    wxPanel* statusbar_panel_{nullptr};

    // Sizer management
    wxBoxSizer* main_sizer_{nullptr};
    wxBoxSizer* body_sizer_{nullptr};

    bool sidebar_visible_{true};
    int sidebar_width_{256}; // 16rem = 256px, matching reference w-64
    static constexpr int kMinSidebarWidth = 180;
    static constexpr int kMaxSidebarWidth = 400;
    static constexpr int kStatusBarHeight = 24; // matching reference h-6

    // Animation
    wxTimer sidebar_anim_timer_;
    int sidebar_anim_target_width_{0};
    void OnSidebarAnimTimer(wxTimerEvent& event);

    void CreateLayout();

    core::EventBus& event_bus_;
    core::Subscription sidebar_toggle_sub_;
};

} // namespace markamp::ui
```

**Layout structure (vertical):**
```
+-------------------------------------------+
| CustomChrome (40px)                       |
+-------------------------------------------+
| Sidebar (256px) | Content Area (flex)     |
|                 |                         |
|                 |                         |
|                 |                         |
+-------------------------------------------+
| StatusBar (24px)                          |
+-------------------------------------------+
```

**Acceptance criteria:**
- Three zones are visible and correctly sized
- Sidebar is 256px wide by default
- Status bar is 24px tall
- Content area fills remaining space
- Window resize adjusts content area (sidebar and status bar maintain size)

### Task 9.2: Implement sidebar container with bevel effects

Create the sidebar container panel:

**Visual specifications (matching reference Sidebar component):**
- Background: `bg_panel`
- Right border: 2px `border_dark`
- Left border: 1px `border_light` (bevel effect)
- Header section: "EXPLORER" text, `accent_primary` color, uppercase, xs font, tracking-widest
- Header background: `bg_header` at 50% opacity
- Header bottom border: `border_dark`
- Footer section: file count stats area
- Footer background: `bg_app` at 50% opacity
- Footer top border: `border_light`

**Implementation:**
- Use a `wxBoxSizer` with vertical orientation
- Header panel at top (fixed height ~40px)
- Scrollable content area in middle (flex)
- Footer panel at bottom (fixed height ~48px)

**Acceptance criteria:**
- Sidebar has correct visual appearance matching reference
- Header shows "EXPLORER" text
- Footer shows placeholder stats
- Borders and bevel effects are visible
- Theme changes update all sidebar colors

### Task 9.3: Implement sidebar toggle with animation

When the sidebar toggle button (in CustomChrome) is clicked or `SidebarToggleEvent` is received:

**Show animation (300ms):**
1. Start with sidebar at 0px width
2. Animate width from 0 to `sidebar_width_` over 300ms using ease-out
3. Content area adjusts in real-time
4. At completion, sidebar content becomes interactive

**Hide animation (200ms):**
1. Start with sidebar at current width
2. Animate width from current to 0 over 200ms using ease-in
3. Content area fills the space in real-time
4. At completion, sidebar is fully hidden (no hit area)

**Implementation using wxTimer:**
- Timer fires every ~16ms (60fps)
- Each tick: calculate interpolated width, update sizer proportions, call `Layout()`
- Use easing function: `ease_out(t) = 1 - pow(1 - t, 3)` for show, `ease_in(t) = pow(t, 3)` for hide

**Alternative: instant toggle (simpler):**
- If animation proves too complex, implement instant show/hide
- Toggle sidebar visibility with `Show()` / `Hide()` and re-layout

**Acceptance criteria:**
- Sidebar toggles between visible and hidden
- Content area fills the space when sidebar is hidden
- Animation is smooth (if implemented)
- Toggle state persists in AppState
- Event bus notification is published on toggle

### Task 9.4: Implement content area container

Create the content area container panel:

**Visual specifications:**
- Background: `bg_app`
- No borders (borders are on child components)
- Will host: toolbar (top), editor/preview panes (middle)

**Structure:**
```
+---------------------------------+
| Toolbar (40px) -- Phase 19     |
+---------------------------------+
| Editor / Split / Preview       |
| (flex, fills remaining space)  |
+---------------------------------+
```

For now, create:
- A toolbar placeholder (fixed 40px, `bg_panel` background, `border_dark` bottom border)
- A content placeholder (fills remaining space, `bg_app` background)

**Acceptance criteria:**
- Content area fills all space not used by sidebar
- Toolbar placeholder is visible at correct height
- Content area background matches theme

### Task 9.5: Implement status bar container

Create the status bar panel:

**Visual specifications (matching reference lines 218-228):**
- Height: 24px (h-6)
- Background: `bg_panel`
- Top border: 1px `border_light`
- Font: 10px monospace, uppercase, `text_muted` color
- User-select: none (not selectable)

**Layout (two zones):**
- Left: Ready state, cursor position, encoding
- Right: Mermaid status, theme name

**For now, placeholder content:**
- Left: "READY" | "LN 1, COL 1" | "UTF-8"
- Right: "MERMAID: ACTIVE" (in accent color) | theme name

**Acceptance criteria:**
- Status bar is visible at the bottom of the window
- Text is correctly styled (10px mono, uppercase, muted)
- Left and right sections are properly aligned
- Theme changes update status bar colors
- Accent color is used for "MERMAID: ACTIVE" text

### Task 9.6: Handle layout persistence

Save and restore layout state in Config:

- `layout.sidebar_visible` (bool, default true)
- `layout.sidebar_width` (int, default 256)
- `layout.window_x`, `layout.window_y` (int, window position)
- `layout.window_width`, `layout.window_height` (int, window size)
- `layout.window_maximized` (bool)

On startup:
1. Load layout config
2. Apply window position and size
3. Validate position is on-screen (handle monitor configuration changes)
4. Apply sidebar state

On shutdown:
1. Save current layout state to config

**Acceptance criteria:**
- Layout state persists across application restarts
- Window appears at the last saved position and size
- Sidebar state (visible/hidden, width) is restored
- Invalid saved positions (off-screen) are corrected to center of primary monitor

### Task 9.7: Implement splitter between sidebar and content

Add a draggable splitter between the sidebar and content area:

**Visual specifications:**
- Width: 4px (draggable hit area)
- Visual width: 1px line in `border_dark` color
- Cursor: `wxCURSOR_SIZEWE` on hover
- Drag behavior: resize sidebar width in real-time

**Implementation:**
- Use a thin `wxPanel` between sidebar and content
- Handle `EVT_LEFT_DOWN`, `EVT_MOTION`, `EVT_LEFT_UP` for drag
- Clamp sidebar width between `kMinSidebarWidth` and `kMaxSidebarWidth`
- On drag end, save the new width to config

**Acceptance criteria:**
- Splitter is visible as a thin line
- Cursor changes on hover
- Dragging resizes the sidebar
- Width is clamped to valid range
- New width persists

### Task 9.8: Update MainFrame to use LayoutManager

Modify `MainFrame` to:

1. Create the `LayoutManager` as its primary child (below CustomChrome)
2. Use a vertical `wxBoxSizer` with:
   - `CustomChrome` at top (fixed height)
   - `LayoutManager` filling remaining space
3. Pass necessary references (ThemeEngine, EventBus) to LayoutManager

**Acceptance criteria:**
- MainFrame displays chrome + layout correctly
- Resizing the window properly adjusts all zones
- No overlapping or gaps between chrome, layout zones, and status bar
- Layout passes visual inspection against the React reference

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/LayoutManager.h` | Created |
| `src/ui/LayoutManager.cpp` | Created |
| `src/ui/MainFrame.h` | Modified |
| `src/ui/MainFrame.cpp` | Modified |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_layout.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (sizers, panels, timers)
- Phase 05 EventBus, Config
- Phase 06 CustomChrome
- Phase 08 ThemeEngine, ThemeAwareWindow

## Estimated Complexity

**High** -- Getting the layout to be responsive, properly themed, and animated requires careful management of wxWidgets sizers, repainting, and event coordination. The sidebar animation adds additional complexity.
