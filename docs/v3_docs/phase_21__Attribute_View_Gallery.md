# Phase 21 -- Attribute View Gallery View

**Priority:** Medium (alternative view layout)
**Estimated Scope:** ~6 new files, ~4 modified files
**Dependencies:** Phase 17 (Data Model), Phase 18 (Column Types), Phase 20 (Filtering & Sorting)

## Objective

Implement the Gallery View for attribute views -- a card/grid layout that displays each row as a visual card with an optional cover image, a title, and configurable fields. The Gallery View provides a visually rich alternative to the Table View, optimized for browsing content where visual thumbnails or summaries are more useful than spreadsheet rows.

Key capabilities:
- Responsive card grid layout with configurable card sizes (Small: 4/row, Medium: 3/row, Large: 2/row)
- Cover image from a designated MAsset column, with configurable aspect ratios
- Card title from the primary Block key (the row's content block)
- Selectable fields shown on each card (configurable per-view)
- Cards display field values using the same formatting from Phase 18
- Click on a card navigates to the block detail or opens an edit panel
- Same filtering and sorting from Phase 20 applies to gallery results
- Pagination for large datasets

## Prerequisites

- **Phase 17** -- `AttributeView`, `AVGalleryView`, `AVCardSize`, `AVCoverAspectRatio`
- **Phase 18** -- `AVColumnTypeRegistry`, formatters for value display
- **Phase 20** -- `AVQueryEngine`, `FilterEngine`, `SortEngine` (gallery uses same query pipeline)

## SiYuan Source Reference

| SiYuan File | Purpose |
|---|---|
| `kernel/sql/av_gallery.go` | Gallery view query generation |
| `kernel/av/av.go` | `GalleryView` struct definition |
| `app/src/protyle/render/av/` | Gallery card rendering in the frontend |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/ui/av/AVGalleryPanel.h` | `markamp::ui::av` | Main gallery view wxScrolledWindow |
| `src/ui/av/AVGalleryPanel.cpp` | `markamp::ui::av` | Card grid layout, scroll, pagination |
| `src/ui/av/AVGalleryCard.h` | `markamp::ui::av` | Single card rendering component |
| `src/ui/av/AVGalleryCard.cpp` | `markamp::ui::av` | Card painting: cover, title, fields |
| `src/ui/av/AVGalleryConfig.h` | `markamp::ui::av` | Gallery config dialog/popover |
| `src/ui/av/AVGalleryConfig.cpp` | `markamp::ui::av` | Config UI: card size, cover field, visible fields |
| `tests/unit/test_av_gallery.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/Events.h` | Add gallery-specific events |
| `src/CMakeLists.txt` | Add new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_gallery` test target |
| `src/ui/av/AVTablePanel.cpp` | Add view-type switching to create gallery panel |

## Data Structures to Implement

### File: `src/ui/av/AVGalleryPanel.h`

```cpp
#pragma once

#include "AVGalleryCard.h"
#include "core/av/AttributeView.h"
#include "core/av/AVColumnType.h"
#include "core/av/AVQueryEngine.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <memory>
#include <string>
#include <vector>
#include <wx/scrolwin.h>

namespace markamp::ui::av
{

// ============================================================================
// Gallery Layout Configuration (computed from AVGalleryView + panel size)
// ============================================================================

struct AVGalleryLayout
{
    int columns_per_row{3};        // Computed from card_size and panel width
    int card_width{0};             // Computed: (panel_width - gaps) / columns_per_row
    int card_height{0};            // Computed from card_width + aspect ratio + field heights
    int gap{12};                   // Gap between cards in pixels
    int padding{16};               // Outer padding
    int cover_height{0};           // Computed from card_width and aspect ratio
    int title_height{28};          // Height reserved for title text
    int field_row_height{22};      // Height per visible field
    int total_rows{0};             // ceil(card_count / columns_per_row)
};

// ============================================================================
// AVGalleryPanel: Scrollable card grid
// ============================================================================

class AVGalleryPanel : public wxScrolledWindow
{
public:
    AVGalleryPanel(wxWindow* parent,
                   core::EventBus* event_bus,
                   core::IThemeEngine* theme_engine,
                   core::av::AVColumnTypeRegistry* type_registry);

    ~AVGalleryPanel() override;

    // ── Data binding ──

    /// Bind this panel to an attribute view and a specific gallery view.
    void set_attribute_view(core::av::AttributeView* av,
                            const std::string& view_id);

    /// Refresh card layout and content.
    void refresh_data();

    // ── Configuration ──

    /// Get the current gallery view config.
    [[nodiscard]] auto gallery_view() const -> const core::av::AVGalleryView*;

    /// Set the card size and re-layout.
    void set_card_size(core::av::AVCardSize size);

    /// Set the cover image column.
    void set_cover_key_id(const std::string& key_id);

    /// Set the cover aspect ratio.
    void set_cover_aspect_ratio(core::av::AVCoverAspectRatio ratio);

    /// Set which fields are visible on cards.
    void set_visible_fields(const std::vector<std::string>& key_ids);

    /// Toggle whether empty values are hidden on cards.
    void set_hide_empty_values(bool hide);

    // ── Selection ──

    /// Get the currently selected card index (-1 if none).
    [[nodiscard]] auto selected_card() const -> int;

    /// Set the selected card.
    void set_selected_card(int index);

    // ── Pagination ──

    [[nodiscard]] auto current_page() const -> int;
    [[nodiscard]] auto total_pages() const -> int;
    void go_to_page(int page);
    void next_page();
    void prev_page();

    // ── Actions ──

    /// Open the gallery configuration popover.
    void show_config_dialog();

private:
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void on_mouse_left_down(wxMouseEvent& event);
    void on_mouse_left_dclick(wxMouseEvent& event);
    void on_key_down(wxKeyEvent& event);
    void on_context_menu(wxContextMenuEvent& event);

    // ── Layout ──
    void recalculate_layout();
    [[nodiscard]] auto card_at_point(wxPoint pt) const -> int;
    [[nodiscard]] auto card_rect(int card_index) const -> wxRect;
    [[nodiscard]] auto compute_columns_per_row() const -> int;
    [[nodiscard]] auto compute_cover_height(int card_width) const -> int;

    // ── Rendering ──
    void render_cards(wxDC& dc);
    void render_card(wxDC& dc, int card_index, wxRect rect);
    void render_cover_image(wxDC& dc, wxRect rect,
                            const core::av::AVValue& asset_value);
    void render_card_title(wxDC& dc, wxRect rect,
                           const std::string& title);
    void render_card_fields(wxDC& dc, wxRect rect, int card_index);
    void render_empty_state(wxDC& dc);
    void render_pagination_bar(wxDC& dc);

    // ── Data ──
    core::av::AttributeView* av_{nullptr};
    core::av::AVView* active_view_{nullptr};
    core::EventBus* event_bus_{nullptr};
    core::IThemeEngine* theme_engine_{nullptr};
    core::av::AVColumnTypeRegistry* type_registry_{nullptr};
    std::unique_ptr<core::av::AVQueryEngine> query_engine_;

    // ── Computed state ──
    AVGalleryLayout layout_;
    core::av::AVQueryResult query_result_;
    int selected_card_{-1};
    int current_page_{0};

    // ── Image cache ──
    std::unordered_map<std::string, wxBitmap> cover_image_cache_;
    static constexpr size_t kMaxCachedImages = 100;

    // ── Subscriptions ──
    std::vector<core::Subscription> subscriptions_;
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVGalleryCard.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"
#include "core/ThemeEngine.h"

#include <string>
#include <vector>
#include <wx/dc.h>
#include <wx/gdicmn.h>

namespace markamp::ui::av
{

// ============================================================================
// Card Field: a single field displayed on a gallery card
// ============================================================================

struct AVCardField
{
    std::string label;             // Key name
    std::string value;             // Formatted value string
    core::av::AVKeyType type{core::av::AVKeyType::Text};
    bool is_empty{false};
};

// ============================================================================
// Card Data: all data needed to render one gallery card
// ============================================================================

struct AVCardData
{
    std::string block_id;          // Row identifier
    std::string title;             // Primary block content (card title)
    std::string cover_image_path;  // Path/URL of cover image (empty if none)
    std::string cover_image_type;  // "file" or "image"
    std::vector<AVCardField> fields; // Visible fields in order
};

// ============================================================================
// Card Renderer: renders a single gallery card
// ============================================================================

class AVGalleryCardRenderer
{
public:
    /// Render a complete card within the given rectangle.
    void render(wxDC& dc,
                const wxRect& card_rect,
                const AVCardData& data,
                int cover_height,
                bool selected,
                bool hovered,
                core::IThemeEngine* theme);

    /// Render the cover image area (image or placeholder).
    void render_cover(wxDC& dc,
                      const wxRect& cover_rect,
                      const std::string& image_path,
                      core::IThemeEngine* theme);

    /// Render the title area.
    void render_title(wxDC& dc,
                      const wxRect& title_rect,
                      const std::string& title,
                      core::IThemeEngine* theme);

    /// Render a single field row (label: value).
    void render_field(wxDC& dc,
                      const wxRect& field_rect,
                      const AVCardField& field,
                      core::IThemeEngine* theme);

    /// Calculate the total card height given cover height and field count.
    [[nodiscard]] auto calculate_card_height(int cover_height,
                                              int title_height,
                                              int field_count,
                                              int field_row_height) const -> int;
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVGalleryConfig.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"

#include <functional>
#include <string>
#include <vector>
#include <wx/dialog.h>

namespace markamp::ui::av
{

// ============================================================================
// Gallery Configuration Dialog
// Popover-style dialog for configuring gallery view settings
// ============================================================================

struct AVGalleryConfigState
{
    core::av::AVCardSize card_size{core::av::AVCardSize::Medium};
    core::av::AVCoverAspectRatio cover_ratio{core::av::AVCoverAspectRatio::Ratio16x9};
    std::string cover_key_id;
    std::vector<std::string> visible_key_ids;
    bool hide_empty_values{true};
    int page_size{50};
};

using AVGalleryConfigCallback = std::function<void(const AVGalleryConfigState& new_config)>;

class AVGalleryConfigDialog : public wxDialog
{
public:
    AVGalleryConfigDialog(wxWindow* parent,
                          const AVGalleryConfigState& current_state,
                          const std::vector<core::av::AVKey>& available_keys,
                          AVGalleryConfigCallback on_apply);

    /// Get the current configuration state from the dialog.
    [[nodiscard]] auto get_state() const -> AVGalleryConfigState;

private:
    void on_apply(wxCommandEvent& event);
    void on_cancel(wxCommandEvent& event);
    void build_ui();

    AVGalleryConfigState state_;
    std::vector<core::av::AVKey> available_keys_;
    AVGalleryConfigCallback on_apply_callback_;

    // UI controls
    wxChoice* card_size_choice_{nullptr};
    wxChoice* aspect_ratio_choice_{nullptr};
    wxChoice* cover_field_choice_{nullptr};
    wxCheckListBox* visible_fields_list_{nullptr};
    wxCheckBox* hide_empty_check_{nullptr};
    wxSpinCtrl* page_size_spin_{nullptr};
};

} // namespace markamp::ui::av
```

## Key Functions to Implement

### AVGalleryPanel.cpp

1. **`AVGalleryPanel::set_attribute_view(av, view_id)`** -- Store AV pointer. Find the gallery view by ID. Initialize `query_engine_`. Execute initial query (no filters, no sorts by default). Call `recalculate_layout()`. Scroll to top. `Refresh()`.

2. **`AVGalleryPanel::recalculate_layout()`** -- Compute `columns_per_row` from card size: Small=4, Medium=3, Large=2. Compute `card_width = (panel_width - padding*2 - gap*(cols-1)) / cols`. Compute `cover_height` from `card_width * aspect_ratio`. Compute card_height = cover_height + title_height + (field_count * field_row_height) + padding. Compute `total_rows`. Set virtual size for scrolling.

3. **`AVGalleryPanel::on_paint(wxPaintEvent&)`** -- Create `wxAutoBufferedPaintDC`. If no data, call `render_empty_state()`. Otherwise call `render_cards()` then `render_pagination_bar()`.

4. **`AVGalleryPanel::render_cards(wxDC&)`** -- Get the current page's row indices from `query_result_.page(current_page_, page_size)`. For each row index, build `AVCardData` from the AV. Call `render_card()` at the computed position.

5. **`AVGalleryPanel::render_card(wxDC&, card_index, rect)`** -- Build `AVCardData`: extract title from Block column, cover image from the configured cover key, visible fields from configured field list. Use `AVGalleryCardRenderer::render()`.

6. **`AVGalleryPanel::render_cover_image(wxDC&, rect, asset_value)`** -- Check image cache. If miss, load bitmap from file path (if local) or show placeholder. Scale to fit the cover rect maintaining aspect ratio. Center-crop if necessary. Draw rounded corners.

7. **`AVGalleryPanel::compute_cover_height(card_width)`** -- Switch on aspect ratio: `Ratio16x9` -> `card_width * 9 / 16`, `Ratio4x3` -> `card_width * 3 / 4`, `Ratio1x1` -> `card_width`, `Auto` -> 0 (no cover).

8. **`AVGalleryPanel::card_at_point(pt)`** -- Compute column from `(pt.x - padding) / (card_width + gap)`. Compute row from `(pt.y - padding + scroll_y) / (card_height + gap)`. Compute index from `row * columns_per_row + col`. Verify within bounds.

9. **`AVGalleryPanel::on_mouse_left_dclick(wxMouseEvent&)`** -- Get card index at point. If valid, publish `AVGalleryCardClickedEvent` with the block_id. This event navigates to the block detail.

10. **`AVGalleryPanel::on_key_down(wxKeyEvent&)`** -- Arrow keys navigate between cards. Enter opens card detail. Page Up/Down navigate pages.

### AVGalleryCard.cpp

11. **`AVGalleryCardRenderer::render(dc, rect, data, cover_height, selected, hovered, theme)`** -- Draw card background (rounded rect with shadow if selected/hovered). Call `render_cover()`, `render_title()`, then `render_field()` for each field.

12. **`AVGalleryCardRenderer::render_cover(dc, rect, image_path, theme)`** -- If image_path is empty, draw placeholder (gray rect with image icon). Otherwise draw the cached bitmap scaled to fit.

13. **`AVGalleryCardRenderer::render_title(dc, rect, title, theme)`** -- Draw title text with bold font. Truncate with ellipsis if too wide. Left-aligned with padding.

14. **`AVGalleryCardRenderer::render_field(dc, rect, field, theme)`** -- Draw label in muted color, value in normal color. Format: "Label: Value". For Select fields, draw color chips. For Checkbox, draw check/cross icon.

### AVGalleryConfig.cpp

15. **`AVGalleryConfigDialog::build_ui()`** -- Create labeled controls: Card Size dropdown (Small/Medium/Large), Aspect Ratio dropdown (16:9, 4:3, 1:1, Auto), Cover Field dropdown (MAsset columns only + "None"), Visible Fields checklist (all columns), Hide Empty checkbox, Page Size spinner. OK/Cancel buttons.

16. **`AVGalleryConfigDialog::on_apply()`** -- Read all control values into `state_`. Call `on_apply_callback_(state_)`. Close dialog.

## Events to Add

```cpp
// ============================================================================
// Attribute View gallery events (Phase 21)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVGalleryCardClickedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVGalleryViewConfigChangedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.gallery.default_card_size` | string | `"medium"` | Default card size: "small", "medium", "large" |
| `av.gallery.default_aspect_ratio` | string | `"16:9"` | Default cover aspect ratio |
| `av.gallery.card_corner_radius` | int | `8` | Card corner radius in pixels |
| `av.gallery.show_card_shadow` | bool | `true` | Show drop shadow on cards |
| `av.gallery.hide_empty_values` | bool | `true` | Hide fields with empty values |

## Test Cases

File: `tests/unit/test_av_gallery.cpp`

All tests use Catch2 with `[av][gallery]` tags.

1. **Gallery layout: columns per row by card size** -- Verify Small -> 4, Medium -> 3, Large -> 2 columns per row.

2. **Gallery layout: card width computation** -- Panel width 1200px, padding 16px, gap 12px, Medium (3 cols). Expected card_width = `(1200 - 32 - 24) / 3 = 381`. Verify computed width matches.

3. **Gallery layout: cover height by aspect ratio** -- Card width 300. Verify 16:9 -> 169, 4:3 -> 225, 1:1 -> 300, Auto -> 0.

4. **Card data extraction: title from Block column** -- Create AV with Block column. Add 3 rows with block content "Note 1", "Note 2", "Note 3". Extract card data. Verify titles match block content.

5. **Card data extraction: cover from MAsset column** -- Create AV with MAsset column designated as cover. Row 1 has an image asset. Row 2 has no assets. Extract card data. Verify row 1 has cover_image_path set. Verify row 2 has empty cover_image_path.

6. **Card data extraction: visible fields filtering** -- Create AV with 5 columns. Configure gallery to show only 2 of them. Extract card data. Verify each card has exactly 2 fields. Verify field labels match the visible column names.

7. **Card data extraction: hide empty values** -- Create AV with 3 columns. Row has values in column 1 and 3, empty in column 2. With `hide_empty_values=true`, card should show 2 fields. With `hide_empty_values=false`, card should show 3 fields.

8. **Gallery pagination** -- Create AV with 25 rows. Gallery page_size=10. Verify total_pages=3. Page 0 has 10 cards. Page 1 has 10 cards. Page 2 has 5 cards.

9. **Gallery with filters applied** -- Create AV with 20 rows. Apply a filter that matches 8 rows. Verify gallery shows only 8 cards. Verify card data corresponds to the filtered rows.

10. **Card hit testing** -- Layout with 3 columns, card_width=300, gap=12, padding=16. Click at (200, 100). Verify card_at_point returns card 0 (first row, first column). Click at (500, 100). Verify card 1. Click at (200, 400) with appropriate card height. Verify card 3 (second row, first column).

## Acceptance Criteria

- [ ] Gallery renders cards in a responsive grid layout
- [ ] Card size options (Small/Medium/Large) change columns per row
- [ ] Cover images render from MAsset column with correct aspect ratio
- [ ] Cards show title from Block column content
- [ ] Configurable fields display on cards with formatted values
- [ ] Empty values are hidden when configured
- [ ] Click on card publishes `AVGalleryCardClickedEvent`
- [ ] Filtering and sorting from Phase 20 apply to gallery results
- [ ] Pagination works correctly
- [ ] Gallery config dialog allows changing all display settings
- [ ] All 10 test cases pass (note: many test data extraction, not pixel rendering)
- [ ] No use of `catch(...)` -- all exceptions typed

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryPanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryCard.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryCard.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryConfig.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVGalleryConfig.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_gallery.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 2 gallery events
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 3 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_gallery` test target
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 5 new `av.gallery.*` config keys
