# Phase 21: Theme Gallery UI

## Objective

Implement the theme gallery modal dialog matching the reference ThemeGallery.tsx. The gallery displays all available themes as miniature live preview cards, supports theme selection with instant application, and provides visual feedback for the active theme.

## Prerequisites

- Phase 08 (Theme Engine and Runtime Switching)
- Phase 09 (Layout Manager)

## Deliverables

1. Modal theme gallery dialog
2. Theme preview cards with miniature live previews
3. Active theme indicator
4. Grid layout with scrolling for many themes
5. Smooth open/close with backdrop
6. Theme count display

## Tasks

### Task 21.1: Create the ThemeGallery dialog

Create `/Users/ryanrentfro/code/markamp/src/ui/ThemeGallery.h` and `ThemeGallery.cpp`:

```cpp
namespace markamp::ui {

class ThemeGallery : public wxDialog
{
public:
    ThemeGallery(wxWindow* parent, core::ThemeEngine& theme_engine);

    // Open/close
    void ShowGallery();

    // Result: returns the selected theme ID (or empty if cancelled)
    [[nodiscard]] auto GetSelectedThemeId() const -> std::string;

private:
    core::ThemeEngine& theme_engine_;

    // UI components
    wxPanel* header_panel_{nullptr};
    wxPanel* toolbar_panel_{nullptr};
    wxScrolledWindow* grid_panel_{nullptr};
    std::vector<class ThemePreviewCard*> preview_cards_;

    // State
    std::string selected_theme_id_;

    // Layout
    void CreateHeader();
    void CreateToolbar();
    void CreateGrid();
    void PopulateGrid();
    void UpdateActiveIndicators();

    // Event handlers
    void OnClose(wxCloseEvent& event);
    void OnKeyDown(wxKeyEvent& event);  // Escape to close
    void OnThemeCardClicked(const std::string& theme_id);
    void OnImportClicked(wxCommandEvent& event);
    void OnSize(wxSizeEvent& event);
};

} // namespace markamp::ui
```

**Dialog specifications (matching reference ThemeGallery.tsx):**
- Size: `max-w-4xl` (896px), `h-[80vh]` (80% of viewport height)
- Background: `bg_app`
- Border: 2px `border_light`, rounded corners (8px if possible)
- Shadow: large drop shadow
- Centered in parent window
- Modal: blocks interaction with the main window
- Backdrop: semi-transparent black (80% opacity) with blur effect (if possible)

**Acceptance criteria:**
- Gallery opens as a modal dialog
- Gallery is centered in the parent window
- Backdrop covers the main window
- Escape key closes the gallery
- Click outside (on backdrop) closes the gallery

### Task 21.2: Implement the gallery header

**Header layout (matching reference lines 136-152):**
```
+----------------------------------------------------------+
| [Palette Icon] Theme Gallery                        [X]  |
|                Select or Import Appearance Profile        |
+----------------------------------------------------------+
```

- Background: `bg_header`
- Bottom border: 1px `border_dark`
- Padding: 16px
- Left: Palette icon (20px) with `accent_primary` background at 20%, rounded
- Title: "Theme Gallery", 20px, bold, `text_main`
- Subtitle: "Select or Import Appearance Profile", 12px, uppercase, tracking-wider, `text_muted`
- Close button (right): X icon (24px), `text_muted`, hover: red at 20% background

**Acceptance criteria:**
- Header matches the reference design
- Close button works (closes the gallery)
- Close button has red hover effect

### Task 21.3: Implement the gallery toolbar

**Toolbar layout (matching reference lines 155-182):**
```
+----------------------------------------------------------+
| Viewing 8 installed themes          [Error msg] [Import] |
+----------------------------------------------------------+
```

- Background: `bg_panel`
- Bottom border: 1px `border_light`
- Padding: 16px
- Left: "Viewing N installed themes" with N in `accent_primary` monospace
- Right: Error message (if any) in red, "Import Theme" button

**Import button styling:**
- Icon: Upload (16px)
- Text: "IMPORT THEME", 14px, semibold, uppercase, tracking-wide
- Background: `accent_primary` at 10%
- Border: 1px `accent_primary` at 30%
- Hover: `accent_primary` at 20%
- Color: `accent_primary`

**Error display:**
- Icon: AlertCircle (16px)
- Text: error message
- Color: red-400
- Animated pulse (if possible)

**Acceptance criteria:**
- Theme count is accurate
- Import button is styled correctly
- Error messages display when import fails

### Task 21.4: Implement ThemePreviewCard

Create a custom component for each theme preview card:

```cpp
namespace markamp::ui {

class ThemePreviewCard : public wxPanel
{
public:
    ThemePreviewCard(wxWindow* parent, const core::Theme& theme,
                     bool is_active, core::ThemeEngine& theme_engine);

    void SetActive(bool active);
    [[nodiscard]] auto GetThemeId() const -> std::string;

    using ClickCallback = std::function<void(const std::string&)>;
    using ExportCallback = std::function<void(const std::string&)>;
    void SetOnClick(ClickCallback callback);
    void SetOnExport(ExportCallback callback);

private:
    const core::Theme& theme_;
    bool is_active_{false};
    bool is_hovered_{false};

    void OnPaint(wxPaintEvent& event);
    void OnMouseEnter(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);

    void DrawMiniPreview(wxDC& dc, const wxRect& rect);
    void DrawFooter(wxDC& dc, const wxRect& rect);
    void DrawActiveIndicator(wxDC& dc);
    void DrawExportButton(wxDC& dc, const wxRect& rect);

    ClickCallback on_click_;
    ExportCallback on_export_;
};

} // namespace markamp::ui
```

**Card specifications (matching reference ThemePreview component, lines 17-71):**

**Overall card:**
- Rounded corners (8px)
- Border: 2px
  - Active: `accent_primary`, ring-2 `accent_primary` at 30%, scale 105%
  - Inactive: `border_dark`, hover: `border_light`
- Cursor: pointer

**Mini preview area (top section, 112px height, matching lines 26-49):**
Draws a miniature representation of the MarkAmp UI:
1. Header bar: `bg_header` background, two colored dots (accent_primary, accent_secondary), text bar
2. Body with sidebar and content:
   - Sidebar: `bg_panel`, two text bars in `text_muted`
   - Content: text lines in `accent_primary` (heading) and `text_main` (body)

All colors come from the card's own theme, not the current active theme. This creates a "live preview" effect.

**Footer area:**
- Background: `bg_panel` (current active theme)
- Top border: `border_light`
- Padding: 12px
- Left: Theme name (14px, semibold)
  - Active: `accent_primary` color
  - Inactive: `text_main` color
- Right: Export button (download icon, 14px)

**Active indicator (matching line 65-69):**
- Position: top-right corner
- Badge: `accent_primary` filled circle
- Icon: checkmark (12px, stroke-width 4), `bg_app` color
- Shadow around the badge

**Acceptance criteria:**
- Preview card renders a miniature UI preview using the theme's colors
- Active theme shows a checkmark badge
- Hover effect enlarges the card slightly or highlights the border
- Clicking the card applies the theme
- Export button is clickable

### Task 21.5: Implement the theme grid layout

**Grid layout (matching reference line 186):**
- 3 columns on medium+ screens (`md:grid-cols-3`)
- 2 columns on small screens (`sm:grid-cols-2`)
- 1 column on tiny screens
- Gap: 24px between cards
- Scrollable when themes exceed the visible area
- Padding: 24px around the grid

**Implementation with wxWidgets:**
Use a `wxScrolledWindow` containing a `wxGridSizer` or manual positioning:

```cpp
void ThemeGallery::PopulateGrid()
{
    // Calculate columns based on available width
    int available_width = grid_panel_->GetClientSize().GetWidth() - 48; // padding
    int card_width = 280;
    int gap = 24;
    int columns = std::max(1, (available_width + gap) / (card_width + gap));

    // Create grid sizer
    auto* sizer = new wxGridSizer(columns, gap, gap);

    for (const auto& info : theme_engine_.available_themes()) {
        auto theme = theme_engine_.get_theme(info.id);
        if (!theme) continue;

        auto* card = new ThemePreviewCard(grid_panel_, *theme,
            info.id == theme_engine_.current_theme().id, theme_engine_);
        card->SetOnClick([this](const std::string& id) { OnThemeCardClicked(id); });
        card->SetOnExport([this](const std::string& id) { OnExportTheme(id); });
        preview_cards_.push_back(card);
        sizer->Add(card, 0, wxEXPAND);
    }

    grid_panel_->SetSizer(sizer);
}
```

**Responsive layout:**
- Recalculate columns on resize
- Cards maintain a minimum width of 250px

**Acceptance criteria:**
- Grid displays all themes as preview cards
- Grid is scrollable
- Responsive column count based on dialog width
- Spacing between cards is consistent

### Task 21.6: Implement theme selection behavior

When a theme preview card is clicked:

1. Immediately apply the theme via `ThemeEngine::apply_theme()`
2. Update the active indicator on all cards
3. The gallery remains open (user can try different themes)
4. The entire gallery repaints with the new active theme (footer, toolbar, etc.)

**Acceptance criteria:**
- Clicking a card instantly changes the application theme
- The gallery itself updates with the new theme
- Previous active card loses its indicator
- New active card gains the checkmark indicator

### Task 21.7: Implement theme export from gallery

When the export button on a card is clicked:

1. Stop event propagation (do not trigger card click)
2. Show a save file dialog with default filename: `{theme_name_snake_case}.theme.json`
3. Export the theme JSON to the selected path
4. Show success/error feedback

**Acceptance criteria:**
- Export button click does not change active theme
- Save dialog appears with correct default filename
- Exported file is valid JSON
- Error message shows on failure

### Task 21.8: Write theme gallery tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_theme_gallery.cpp`:

Test cases:
1. Gallery shows correct number of theme cards
2. Active theme has checkmark indicator
3. Clicking a card changes the theme
4. Grid recalculates columns on resize
5. Theme count display is accurate
6. Gallery closes on Escape
7. Export button triggers save dialog
8. Import button triggers file dialog
9. Error message displays for invalid import

**Acceptance criteria:**
- All tests pass
- At least 10 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/ThemeGallery.h` | Created |
| `src/ui/ThemeGallery.cpp` | Created |
| `src/ui/Toolbar.cpp` | Modified (connect THEMES button to gallery) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_theme_gallery.cpp` | Created |

## Dependencies

- Phase 08 ThemeEngine
- Phase 07 Theme, ThemeRegistry
- wxWidgets (wxDialog, wxScrolledWindow, wxGridSizer)

## Estimated Complexity

**High** -- The theme preview cards with miniature UI renderings require detailed custom drawing. The responsive grid layout and smooth interactions add complexity. Getting the gallery to look polished and match the reference is significant UI work.
