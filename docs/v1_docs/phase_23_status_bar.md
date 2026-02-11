# Phase 23: Status Bar Implementation

## Objective

Complete the status bar at the bottom of the application window, displaying real-time information about the editor state, file encoding, Mermaid rendering status, and current theme name. The status bar must match the reference design and update dynamically as the user interacts with the application.

## Prerequisites

- Phase 09 (Layout Manager)
- Phase 12 (Text Editor Component)

## Deliverables

1. Fully functional status bar with real-time updates
2. Left zone: ready state, cursor position, encoding
3. Right zone: Mermaid status, theme name
4. Theme-aware styling
5. Click interactions on status bar items

## Tasks

### Task 23.1: Implement the StatusBar component

Create `/Users/ryanrentfro/code/markamp/src/ui/StatusBar.h` and `StatusBar.cpp`:

```cpp
namespace markamp::ui {

class StatusBar : public ThemeAwareWindow
{
public:
    StatusBar(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // State updates
    void SetReadyState(const std::string& state);    // "Ready", "Saving...", "Loading..."
    void SetCursorPosition(int line, int column);
    void SetEncoding(const std::string& encoding);
    void SetMermaidStatus(bool active);
    void SetThemeName(const std::string& name);
    void SetWordCount(int count);
    void SetFileModified(bool modified);

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    // UI
    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);

    // State
    std::string ready_state_{"Ready"};
    int cursor_line_{1};
    int cursor_column_{1};
    std::string encoding_{"UTF-8"};
    bool mermaid_active_{true};
    std::string theme_name_;
    int word_count_{0};
    bool file_modified_{false};

    // Layout
    struct StatusItem {
        std::string text;
        wxRect bounds;
        bool is_accent{false};   // Use accent_primary color
        bool is_clickable{false};
        std::function<void()> on_click;
    };
    std::vector<StatusItem> left_items_;
    std::vector<StatusItem> right_items_;
    void RebuildItems();

    // Event subscriptions
    core::EventBus& event_bus_;
    core::Subscription cursor_sub_;
    core::Subscription theme_sub_;
    core::Subscription content_sub_;
    core::Subscription file_sub_;
};

} // namespace markamp::ui
```

**Acceptance criteria:**
- Status bar renders with all information zones
- Information updates in real-time
- Theme-aware coloring

### Task 23.2: Implement the status bar layout and drawing

**Visual specifications (matching reference App.tsx lines 218-228):**

- Height: 24px (h-6)
- Background: `bg_panel`
- Top border: 1px `border_light`
- Font: 10px monospace, uppercase
- Text color: `text_muted`
- User-select: none
- Two zones: left and right

**Left zone items (with separators):**
```
READY  |  LN 12, COL 42  |  UTF-8
```

**Right zone items:**
```
MERMAID: ACTIVE  |  Midnight Neon
```

**Drawing with wxDC:**
```cpp
void StatusBar::OnPaint(wxPaintEvent& event)
{
    wxBufferedPaintDC dc(this);
    auto& te = theme_engine();

    // Background
    dc.SetBackground(wxBrush(te.color(ThemeColorToken::BgPanel)));
    dc.Clear();

    // Top border
    dc.SetPen(wxPen(te.color(ThemeColorToken::BorderLight), 1));
    dc.DrawLine(0, 0, GetClientSize().GetWidth(), 0);

    // Font
    dc.SetFont(te.font(ThemeFontToken::UISmall));

    int left_x = 12;
    for (const auto& item : left_items_) {
        dc.SetTextForeground(item.is_accent
            ? te.color(ThemeColorToken::AccentPrimary)
            : te.color(ThemeColorToken::TextMuted));
        dc.DrawText(item.text, left_x, 6);
        left_x += dc.GetTextExtent(item.text).GetWidth() + 16;
    }

    int right_x = GetClientSize().GetWidth() - 12;
    for (auto it = right_items_.rbegin(); it != right_items_.rend(); ++it) {
        int text_width = dc.GetTextExtent(it->text).GetWidth();
        right_x -= text_width;
        dc.SetTextForeground(it->is_accent
            ? te.color(ThemeColorToken::AccentPrimary)
            : te.color(ThemeColorToken::TextMuted));
        dc.DrawText(it->text, right_x, 6);
        right_x -= 16;
    }
}
```

**Acceptance criteria:**
- Status bar renders with correct layout
- Left items are left-aligned with spacing
- Right items are right-aligned with spacing
- Font is 10px monospace, uppercase
- All colors match theme

### Task 23.3: Connect cursor position to status bar

Subscribe to `CursorPositionChangedEvent` from the editor:

```cpp
cursor_sub_ = event_bus_.subscribe<core::events::CursorPositionChangedEvent>(
    [this](const auto& event) {
        SetCursorPosition(event.line, event.column);
        Refresh();
    }
);
```

Display format: `LN {line}, COL {column}`
- Line and column are 1-based
- Update on every cursor movement

**Acceptance criteria:**
- Cursor position updates as the user types or moves the cursor
- Format is "LN X, COL Y" in uppercase
- Updates on mouse click in editor, arrow keys, typing

### Task 23.4: Connect encoding to status bar

Display the detected file encoding from Phase 11:

- Default: "UTF-8"
- Updated when a file is opened with different encoding
- Format: uppercase (e.g., "UTF-8", "UTF-8 BOM", "ASCII")

**Click behavior (optional):**
- Clicking the encoding field could show a dropdown to change encoding
- For initial implementation: display only

**Acceptance criteria:**
- Encoding displays correctly
- Updates when a new file is opened

### Task 23.5: Implement Mermaid status indicator

Display Mermaid rendering status:

**States:**
- "MERMAID: ACTIVE" -- MermaidRenderer is available (displayed in accent_primary color)
- "MERMAID: INACTIVE" -- MermaidRenderer is not available (displayed in text_muted)
- "MERMAID: RENDERING" -- Currently rendering a diagram (displayed in accent_secondary)
- "MERMAID: ERROR" -- Last render failed (displayed in error color)

Subscribe to Mermaid rendering events to update the status.

**Acceptance criteria:**
- Mermaid status is displayed in the correct color
- Status updates when rendering starts, completes, or fails
- "ACTIVE" is in accent_primary (matching reference)

### Task 23.6: Connect theme name to status bar

Display the current theme name:

- Subscribe to `ThemeChangedEvent`
- Update the theme name display
- Format: theme name as-is (e.g., "Midnight Neon")

**Acceptance criteria:**
- Theme name updates when theme changes
- Displayed in text_muted color

### Task 23.7: Add optional status bar items

Add additional useful status information:

1. **Word count**: Display word count of the active document
   - Format: "{N} WORDS"
   - Update on content change (debounced)
   - Position: between encoding and Mermaid status

2. **File modified indicator**: Show a small dot or asterisk when the file has unsaved changes
   - Position: next to "READY" state
   - Color: accent_primary for modified, text_muted for saved

3. **View mode indicator**: Show current view mode
   - Format: "SRC" / "SPLIT" / "VIEW"
   - Position: left zone

**Acceptance criteria:**
- Additional items display correctly
- Word count updates on content change
- Modified indicator reflects actual state

### Task 23.8: Write status bar tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_status_bar.cpp`:

Test cases:
1. Default state shows "READY"
2. Cursor position updates correctly
3. Encoding display
4. Mermaid active status color
5. Mermaid inactive status color
6. Theme name updates on theme change
7. Word count calculation
8. File modified indicator
9. All items fit within status bar width
10. Theme change updates status bar colors

**Acceptance criteria:**
- All tests pass
- At least 12 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/StatusBar.h` | Created |
| `src/ui/StatusBar.cpp` | Created |
| `src/ui/LayoutManager.cpp` | Modified (integrate StatusBar) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_status_bar.cpp` | Created |

## Dependencies

- Phase 09 LayoutManager (status bar zone)
- Phase 12 EditorPanel (cursor position events)
- Phase 08 ThemeEngine
- Phase 05 EventBus

## Estimated Complexity

**Medium** -- The status bar is visually simple but requires integration with multiple event sources. Custom drawing with proper text measurement and layout is straightforward but requires attention to detail for pixel-perfect appearance.
