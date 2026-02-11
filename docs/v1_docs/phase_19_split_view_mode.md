# Phase 19: Split View Mode Implementation

## Objective

Implement the three view modes from the reference implementation: Source (SRC), Split, and Preview (VIEW). The split view shows the editor and preview side-by-side with a draggable divider. View mode switching must be smooth and preserve state (scroll position, cursor position, content).

## Prerequisites

- Phase 12 (Text Editor Component)
- Phase 18 (Preview Component with Theme-Aware Rendering)

## Deliverables

1. Three view modes: Source, Split, Preview
2. Toolbar with view mode toggle buttons
3. Draggable split divider
4. State preservation across mode switches
5. Bevel effects on both panes
6. Keyboard shortcuts for mode switching

## Tasks

### Task 19.1: Implement the SplitView component

Create `/Users/ryanrentfro/code/markamp/src/ui/SplitView.h` and `SplitView.cpp`:

```cpp
namespace markamp::ui {

enum class ViewMode { Editor, Split, Preview };

class SplitView : public ThemeAwareWindow
{
public:
    SplitView(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // View mode control
    void SetViewMode(ViewMode mode);
    [[nodiscard]] auto GetViewMode() const -> ViewMode;

    // Access child panels
    [[nodiscard]] auto GetEditorPanel() -> EditorPanel*;
    [[nodiscard]] auto GetPreviewPanel() -> PreviewPanel*;

    // Split ratio (0.0 = all editor, 1.0 = all preview, 0.5 = even split)
    void SetSplitRatio(double ratio);
    [[nodiscard]] auto GetSplitRatio() const -> double;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    EditorPanel* editor_panel_{nullptr};
    PreviewPanel* preview_panel_{nullptr};
    wxPanel* divider_panel_{nullptr};

    ViewMode current_mode_{ViewMode::Split};
    double split_ratio_{0.5};

    // Divider dragging
    bool is_dragging_{false};
    void OnDividerMouseDown(wxMouseEvent& event);
    void OnDividerMouseMove(wxMouseEvent& event);
    void OnDividerMouseUp(wxMouseEvent& event);

    // Layout
    void UpdateLayout();
    void OnSize(wxSizeEvent& event);

    // Bevel overlays
    wxPanel* editor_bevel_{nullptr};
    wxPanel* preview_bevel_{nullptr};

    core::EventBus& event_bus_;
    core::Subscription view_mode_sub_;

    static constexpr int kDividerWidth = 4;
    static constexpr double kMinSplitRatio = 0.2;
    static constexpr double kMaxSplitRatio = 0.8;
};

} // namespace markamp::ui
```

**Layout in each mode:**

**Source mode (ViewMode::Editor):**
```
+---------------------------------------+
| EditorPanel (100% width)              |
|                                       |
+---------------------------------------+
```

**Preview mode (ViewMode::Preview):**
```
+---------------------------------------+
| PreviewPanel (100% width)             |
|                                       |
+---------------------------------------+
```

**Split mode (ViewMode::Split):**
```
+------------------+|+-----------------+
| EditorPanel      ||| PreviewPanel    |
| (50% - divider)  ||| (50%)          |
+------------------+|+-----------------+
```
The `||` represents the draggable divider (matching reference: `border-r border-win-borderDark`).

**Acceptance criteria:**
- All three view modes display correctly
- Split mode shows both panels side by side
- Panels resize correctly with the window
- Divider is visible in split mode

### Task 19.2: Implement the toolbar with view mode buttons

Create `/Users/ryanrentfro/code/markamp/src/ui/Toolbar.h` and `Toolbar.cpp`:

**Layout (matching reference App.tsx lines 150-191):**
```
+------------------------------------------------------------------+
| [SRC] [SPLIT] [VIEW]  ...  [Save] [THEMES] [Settings]           |
+------------------------------------------------------------------+
```

- Height: 40px (h-10)
- Background: `bg_panel`
- Bottom border: 1px `border_dark`

**View mode buttons (matching reference lines 152-169):**
Each button:
- Icon + label: `<Code> SRC`, `<Columns> SPLIT`, `<Eye> VIEW`
- Active state: `bg accent_primary/20`, text `accent_primary`, shadow
- Inactive state: text `text_muted`, hover text `text_main`
- Size: icon 14px, text xs, font semibold

**Right-side buttons (matching reference lines 172-191):**
- Save: Save icon (16px), `text_muted` -> `accent_primary` on hover
- Themes: Palette icon + "THEMES" label, opens theme gallery (Phase 21)
- Settings: Settings icon, placeholder for settings dialog

**Icon rendering:**
Draw icons using wxGraphicsContext or load from embedded SVG/bitmap resources:
- Code icon (source): `</>` or brackets
- Columns icon (split): two vertical rectangles
- Eye icon (preview): stylized eye shape
- Save icon: floppy disk shape
- Palette icon: color palette shape
- Settings icon: gear shape

**Acceptance criteria:**
- Toolbar renders with all buttons
- View mode buttons show active state correctly
- Clicking a view mode button switches the mode
- Right-side buttons are positioned correctly
- Theme colors update with theme changes

### Task 19.3: Implement view mode switching

When a view mode button is clicked:

1. Animate the transition (optional):
   - Fade out current layout -> fade in new layout
   - Or: slide panels in/out
   - Or: instant switch (simplest)
2. Show/hide panels as needed
3. Preserve state:
   - Editor: cursor position, scroll position, content
   - Preview: scroll position
4. Publish `ViewModeChangedEvent`
5. Update toolbar button states

**Implementation:**
```cpp
void SplitView::SetViewMode(ViewMode mode)
{
    if (mode == current_mode_) return;

    // Save current state
    auto editor_state = SaveEditorState();
    auto preview_scroll = SavePreviewScroll();

    current_mode_ = mode;

    // Update visibility
    editor_panel_->Show(mode == ViewMode::Editor || mode == ViewMode::Split);
    preview_panel_->Show(mode == ViewMode::Preview || mode == ViewMode::Split);
    divider_panel_->Show(mode == ViewMode::Split);

    // Re-layout
    UpdateLayout();

    // Restore state
    RestoreEditorState(editor_state);
    RestorePreviewScroll(preview_scroll);

    // Notify
    event_bus_.publish(core::events::ViewModeChangedEvent{mode});
}
```

**Acceptance criteria:**
- Mode switches preserve editor cursor and scroll position
- Mode switches preserve preview scroll position
- Content is not lost during mode switches
- Events are published

### Task 19.4: Implement draggable split divider

The divider between editor and preview in split mode:

**Visual specifications:**
- Width: 4px visual, but 8px hit area for easier grabbing
- Background: transparent or `border_dark`
- Hover: cursor changes to `wxCURSOR_SIZEWE`
- Drag: resizes the split ratio

**Drag behavior:**
1. On `EVT_LEFT_DOWN`: begin drag, capture mouse
2. On `EVT_MOTION` (while dragging):
   - Calculate new split ratio based on mouse X position
   - Clamp to [0.2, 0.8] range
   - Update layout in real-time
3. On `EVT_LEFT_UP`: end drag, release capture, save ratio to config

**Acceptance criteria:**
- Divider is draggable
- Split ratio is clamped to valid range
- Both panels resize smoothly during drag
- Cursor changes on hover
- Ratio persists across sessions

### Task 19.5: Apply bevel effects to both panes

Add bevel overlays (from Phase 06/08) to both the editor and preview panes:

**Editor pane bevel (reference line 200-202):**
- Sunken style
- Top/left: `rgba(0, 0, 0, 0.2)`
- Bottom/right: `rgba(255, 255, 255, 0.05)`
- Only visible in split mode (when there is a border between panes)

**Preview pane bevel (reference line 211-213):**
- Same sunken style
- Always visible (gives the preview a recessed appearance)

**Implementation:**
- Create non-interactive overlay panels positioned on top of each pane
- Or paint the bevel in the pane's `OnPaint` handler (as the last draw operation)

**Acceptance criteria:**
- Bevel effects are visible on both panes
- Bevels do not interfere with scrolling or clicking
- Bevels update with theme changes

### Task 19.6: Implement split mode border

In split mode, the editor pane has a right border (matching reference line 198):
```
border-r border-win-borderDark
```

This is a 1px border in `border_dark` color on the right edge of the editor pane.

**Implementation:**
- Draw in the editor panel's `OnPaint`, or
- Use the divider panel itself as the visual border

**Acceptance criteria:**
- Border is visible between editor and preview in split mode
- Border is not visible in source-only or preview-only modes
- Border color matches `border_dark` theme token

### Task 19.7: Add keyboard shortcuts for view modes

Map keyboard shortcuts:

| Shortcut | Action |
|---|---|
| Ctrl+1 / Cmd+1 | Switch to Source mode |
| Ctrl+2 / Cmd+2 | Switch to Split mode |
| Ctrl+3 / Cmd+3 | Switch to Preview mode |
| Ctrl+\ / Cmd+\ | Toggle split/source (alternative) |

Register shortcuts via accelerator table or direct key event handling.

**Acceptance criteria:**
- All shortcuts work
- Shortcuts are platform-appropriate (Cmd on macOS, Ctrl on Windows/Linux)
- Shortcuts do not conflict with editor shortcuts

### Task 19.8: Write split view tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_split_view.cpp`:

Test cases:
1. Default view mode is Split
2. Switch to Source mode hides preview
3. Switch to Preview mode hides editor
4. Switch back to Split shows both
5. Editor state preserved across mode switches
6. Split ratio clamping
7. Split ratio persistence
8. View mode change event published
9. Toolbar button state reflects current mode
10. Keyboard shortcut triggers mode switch

**Acceptance criteria:**
- All tests pass
- Mode transitions are verified
- State preservation is verified
- At least 12 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/SplitView.h` | Created |
| `src/ui/SplitView.cpp` | Created |
| `src/ui/Toolbar.h` | Created |
| `src/ui/Toolbar.cpp` | Created |
| `src/ui/LayoutManager.cpp` | Modified (integrate SplitView and Toolbar) |
| `src/core/Events.h` | Modified (ViewModeChangedEvent) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_split_view.cpp` | Created |

## Dependencies

- Phase 12 EditorPanel
- Phase 18 PreviewPanel
- Phase 08 ThemeEngine
- Phase 09 LayoutManager
- Phase 05 EventBus

## Estimated Complexity

**High** -- Split view with draggable divider, state preservation, and bevel effects requires careful layout management. Three distinct modes with smooth transitions add complexity. The toolbar with custom-drawn icons is additional UI work.
