# Phase 19 -- Attribute View Table View Renderer

**Priority:** High (primary AV interaction surface)
**Estimated Scope:** ~10 new files, ~5 modified files
**Dependencies:** Phase 17 (Data Model), Phase 18 (Column Types)

## Objective

Implement the Table View renderer for attribute views -- a spreadsheet-like grid panel that displays AV data with editable cells, column headers, row operations, and pagination. This is the primary interaction surface for attribute view data, analogous to a Notion database table view or an Airtable grid.

The table view renders inside a wxPanel and provides:
- A fixed column header row with drag-resizable, reorderable columns
- A scrollable grid of rows with type-specific cell renderers
- Inline cell editing with type-appropriate editors (text input, number spinner, date picker, select dropdown, checkbox toggle)
- Column operations (add, remove, reorder, resize, hide, pin, rename, change type)
- Row operations (add, delete, duplicate, drag-reorder)
- Pagination with configurable page size
- Keyboard navigation (arrow keys, Tab, Enter to edit, Escape to cancel)

## Prerequisites

- **Phase 17** -- `AttributeView`, `AttributeViewStore`, `AVKey`, `AVValue`, all type structs
- **Phase 18** -- `AVColumnTypeRegistry`, `IAVColumnTypeHandler`, formatters, validators

## SiYuan Source Reference

| SiYuan File | Purpose |
|---|---|
| `app/src/protyle/render/av/cell.ts` | Cell rendering per type, inline editing |
| `app/src/protyle/render/av/col.ts` | Column header rendering, column operations |
| `app/src/protyle/render/av/row.ts` | Row rendering, row operations |
| `app/src/protyle/render/av/action.ts` | User action handlers (click, drag, context menu) |
| `app/src/protyle/render/av/keydown.ts` | Keyboard navigation within the table |
| `kernel/sql/av_table.go` | Table view SQL query generation |

## MarkAmp Integration Points

### New Files to Create

| File | Namespace | Purpose |
|---|---|---|
| `src/ui/av/AVTablePanel.h` | `markamp::ui::av` | Main table view wxPanel |
| `src/ui/av/AVTablePanel.cpp` | `markamp::ui::av` | Table layout, scroll, pagination |
| `src/ui/av/AVCellRenderer.h` | `markamp::ui::av` | Base cell renderer + per-type subclasses |
| `src/ui/av/AVCellRenderer.cpp` | `markamp::ui::av` | Cell rendering implementations |
| `src/ui/av/AVCellEditor.h` | `markamp::ui::av` | Base cell editor + per-type subclasses |
| `src/ui/av/AVCellEditor.cpp` | `markamp::ui::av` | Inline editor implementations |
| `src/ui/av/AVColumnHeader.h` | `markamp::ui::av` | Column header bar with resize handles |
| `src/ui/av/AVColumnHeader.cpp` | `markamp::ui::av` | Header rendering, drag, context menu |
| `src/ui/av/AVTableController.h` | `markamp::core::av` | Controller mediating between UI and data model |
| `src/ui/av/AVTableController.cpp` | `markamp::core::av` | CRUD operations, undo, event publishing |
| `tests/unit/test_av_table_renderer.cpp` | (anonymous) | Catch2 tests |

### Files to Modify

| File | Change |
|---|---|
| `src/core/Events.h` | Add cell/column/row edit events |
| `src/CMakeLists.txt` | Add 6 new `.cpp` sources |
| `tests/CMakeLists.txt` | Add `test_av_table_renderer` test target |
| `src/ui/MainFrame.h` | Add method to host AVTablePanel |
| `src/ui/MainFrame.cpp` | Wire AV table panel creation |

## Data Structures to Implement

### File: `src/ui/av/AVTablePanel.h`

```cpp
#pragma once

#include "AVCellEditor.h"
#include "AVCellRenderer.h"
#include "AVColumnHeader.h"
#include "core/av/AttributeView.h"
#include "core/av/AVColumnType.h"
#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <memory>
#include <optional>
#include <string>
#include <vector>
#include <wx/panel.h>
#include <wx/scrolwin.h>

namespace markamp::ui::av
{

// ============================================================================
// Cell Address (row + column identifier)
// ============================================================================

struct AVCellAddress
{
    int row{-1};                   // Row index (0-based, relative to current page)
    int col{-1};                   // Column index (0-based, relative to visible columns)

    [[nodiscard]] auto is_valid() const -> bool
    {
        return row >= 0 && col >= 0;
    }

    [[nodiscard]] auto operator==(const AVCellAddress& other) const -> bool = default;
};

// ============================================================================
// Table Panel Configuration
// ============================================================================

struct AVTablePanelConfig
{
    int row_height{32};            // Height of each data row in pixels
    int header_height{36};         // Height of the column header row
    int min_column_width{60};      // Minimum column width during resize
    int max_column_width{800};     // Maximum column width
    int default_column_width{200}; // Default width for new columns
    int page_size{50};             // Rows per page
    bool show_row_numbers{false};  // Show row number column
    bool stripe_rows{true};        // Alternating row background colors
    bool show_grid_lines{true};    // Show cell borders
};

// ============================================================================
// AVTablePanel: Main table view wxPanel
// ============================================================================

class AVTablePanel : public wxPanel
{
public:
    AVTablePanel(wxWindow* parent,
                 core::EventBus* event_bus,
                 core::IThemeEngine* theme_engine,
                 core::av::AVColumnTypeRegistry* type_registry);

    ~AVTablePanel() override;

    // ── Data binding ──

    /// Bind this panel to an attribute view. Rebuilds the entire table.
    void set_attribute_view(core::av::AttributeView* av);

    /// Get the currently bound attribute view.
    [[nodiscard]] auto attribute_view() const -> const core::av::AttributeView*;

    /// Refresh the display from the current data (e.g. after external modification).
    void refresh_data();

    // ── View configuration ──

    /// Set the active view (Table view within the AV).
    void set_active_view(const std::string& view_id);

    /// Get the current table panel configuration.
    [[nodiscard]] auto config() const -> const AVTablePanelConfig&;

    /// Update configuration and re-layout.
    void set_config(const AVTablePanelConfig& cfg);

    // ── Selection ──

    /// Get the currently focused cell address.
    [[nodiscard]] auto focused_cell() const -> AVCellAddress;

    /// Set the focused cell.
    void set_focused_cell(AVCellAddress addr);

    /// Get all selected row indices.
    [[nodiscard]] auto selected_rows() const -> std::vector<int>;

    // ── Pagination ──

    /// Get the current page number (0-based).
    [[nodiscard]] auto current_page() const -> int;

    /// Get the total number of pages.
    [[nodiscard]] auto total_pages() const -> int;

    /// Navigate to a specific page.
    void go_to_page(int page);

    /// Navigate to next/previous page.
    void next_page();
    void prev_page();

    // ── Cell editing ──

    /// Begin editing the focused cell. Opens the appropriate inline editor.
    void begin_edit();

    /// Commit the current edit and close the editor.
    void commit_edit();

    /// Cancel the current edit and close the editor.
    void cancel_edit();

    /// Returns true if a cell is currently being edited.
    [[nodiscard]] auto is_editing() const -> bool;

    // ── Column operations ──

    /// Add a new column after the specified column index (-1 = at end).
    void add_column(core::av::AVKeyType type,
                    const std::string& name,
                    int after_col = -1);

    /// Remove a column by index.
    void remove_column(int col);

    /// Reorder a column: move from old_index to new_index.
    void reorder_column(int old_index, int new_index);

    /// Resize a column to a specific width.
    void resize_column(int col, int width);

    // ── Row operations ──

    /// Add a new row at the end (or at the specified index).
    void add_row(int at_index = -1);

    /// Remove rows by indices.
    void remove_rows(const std::vector<int>& row_indices);

    /// Duplicate a row.
    void duplicate_row(int row_index);

private:
    // ── Event handlers ──
    void on_paint(wxPaintEvent& event);
    void on_size(wxSizeEvent& event);
    void on_mouse_left_down(wxMouseEvent& event);
    void on_mouse_left_up(wxMouseEvent& event);
    void on_mouse_motion(wxMouseEvent& event);
    void on_mouse_left_dclick(wxMouseEvent& event);
    void on_key_down(wxKeyEvent& event);
    void on_mouse_wheel(wxMouseEvent& event);
    void on_context_menu(wxContextMenuEvent& event);

    // ── Layout helpers ──
    [[nodiscard]] auto cell_at_point(wxPoint pt) const -> AVCellAddress;
    [[nodiscard]] auto cell_rect(AVCellAddress addr) const -> wxRect;
    [[nodiscard]] auto visible_row_range() const -> std::pair<int, int>;
    [[nodiscard]] auto visible_columns() const -> std::vector<int>;
    void recalculate_layout();

    // ── Rendering helpers ──
    void render_header(wxDC& dc);
    void render_rows(wxDC& dc);
    void render_cell(wxDC& dc, AVCellAddress addr, wxRect rect);
    void render_selection(wxDC& dc);
    void render_grid_lines(wxDC& dc);
    void render_pagination_bar(wxDC& dc);

    // ── Column header context menu ──
    void show_column_context_menu(int col_index, wxPoint pos);

    // ── Data ──
    core::av::AttributeView* av_{nullptr};
    core::av::AVView* active_view_{nullptr};
    core::EventBus* event_bus_{nullptr};
    core::IThemeEngine* theme_engine_{nullptr};
    core::av::AVColumnTypeRegistry* type_registry_{nullptr};

    AVTablePanelConfig config_;
    AVCellAddress focused_cell_;
    std::vector<int> selected_rows_;
    int current_page_{0};
    int scroll_offset_y_{0};

    // ── Cell editing state ──
    bool editing_{false};
    std::unique_ptr<IAVCellEditor> active_editor_;

    // ── Column resize state ──
    int resize_col_{-1};
    int resize_start_x_{0};
    int resize_start_width_{0};

    // ── Column reorder drag state ──
    int drag_col_{-1};
    int drag_target_col_{-1};

    // ── Cached layout ──
    std::vector<int> column_x_positions_;  // Left edge X of each visible column
    std::vector<int> column_widths_;       // Width of each visible column
    int total_content_width_{0};
    int total_content_height_{0};

    // ── EventBus subscriptions ──
    std::vector<core::Subscription> subscriptions_;
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVCellRenderer.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"
#include "core/ThemeEngine.h"

#include <memory>
#include <string>
#include <wx/dc.h>
#include <wx/gdicmn.h>

namespace markamp::ui::av
{

/// Base interface for rendering a single cell in the table grid.
class IAVCellRenderer
{
public:
    virtual ~IAVCellRenderer() = default;

    /// Render the cell content into the given rectangle on the DC.
    virtual void render(wxDC& dc,
                        const wxRect& rect,
                        const core::av::AVValue& value,
                        const core::av::AVKey& key,
                        bool selected,
                        bool focused,
                        core::IThemeEngine* theme) = 0;

    /// Calculate the preferred height for this cell given a width.
    [[nodiscard]] virtual auto preferred_height(wxDC& dc,
                                                 int width,
                                                 const core::av::AVValue& value) const -> int
    {
        (void)dc;
        (void)width;
        (void)value;
        return 32; // Default row height
    }
};

// ── Concrete Cell Renderers ──

class TextCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class NumberCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class DateCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class SelectCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class CheckboxCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class URLCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class RelationCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

class RollupCellRenderer : public IAVCellRenderer
{
public:
    void render(wxDC& dc, const wxRect& rect,
                const core::av::AVValue& value,
                const core::av::AVKey& key,
                bool selected, bool focused,
                core::IThemeEngine* theme) override;
};

/// Factory: returns the correct renderer for a given key type.
[[nodiscard]] auto create_cell_renderer(core::av::AVKeyType type)
    -> std::unique_ptr<IAVCellRenderer>;

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVCellEditor.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"
#include "core/av/AVValue.h"

#include <functional>
#include <memory>
#include <string>
#include <wx/gdicmn.h>
#include <wx/panel.h>

namespace markamp::ui::av
{

/// Callback invoked when the user commits an edit.
using AVCellEditCallback = std::function<void(core::av::AVValueVariant new_value)>;

/// Callback invoked when the user cancels an edit.
using AVCellCancelCallback = std::function<void()>;

/// Base interface for inline cell editors.
class IAVCellEditor
{
public:
    virtual ~IAVCellEditor() = default;

    /// Create and show the editor widget at the given rect within the parent.
    virtual void begin_edit(wxWindow* parent,
                            const wxRect& cell_rect,
                            const core::av::AVValue& current_value,
                            const core::av::AVKey& key,
                            AVCellEditCallback on_commit,
                            AVCellCancelCallback on_cancel) = 0;

    /// Commit the current editor value and close.
    virtual void commit() = 0;

    /// Cancel editing and close without saving.
    virtual void cancel() = 0;

    /// Returns true if the editor is currently visible/active.
    [[nodiscard]] virtual auto is_active() const -> bool = 0;
};

// ── Concrete Cell Editors ──

/// Text editor: single-line wxTextCtrl for Text, URL, Email, Phone types.
class TextCellEditor : public IAVCellEditor
{
public:
    void begin_edit(wxWindow* parent, const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    wxTextCtrl* text_ctrl_{nullptr};
    AVCellEditCallback on_commit_;
    AVCellCancelCallback on_cancel_;
    core::av::AVKeyType type_{core::av::AVKeyType::Text};
};

/// Number editor: wxTextCtrl with numeric validation.
class NumberCellEditor : public IAVCellEditor
{
public:
    void begin_edit(wxWindow* parent, const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    wxTextCtrl* text_ctrl_{nullptr};
    AVCellEditCallback on_commit_;
    AVCellCancelCallback on_cancel_;
    core::av::AVNumberFormat format_{core::av::AVNumberFormat::None};
};

/// Date editor: popup calendar/date picker.
class DateCellEditor : public IAVCellEditor
{
public:
    void begin_edit(wxWindow* parent, const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    wxPanel* popup_{nullptr};
    AVCellEditCallback on_commit_;
    AVCellCancelCallback on_cancel_;
};

/// Select editor: dropdown with color chips for Select/MSelect types.
class SelectCellEditor : public IAVCellEditor
{
public:
    void begin_edit(wxWindow* parent, const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    wxPanel* popup_{nullptr};
    AVCellEditCallback on_commit_;
    AVCellCancelCallback on_cancel_;
    bool multi_select_{false};
    std::vector<core::av::AVSelectOption> selected_;
};

/// Checkbox editor: immediate toggle (no popup needed).
class CheckboxCellEditor : public IAVCellEditor
{
public:
    void begin_edit(wxWindow* parent, const wxRect& cell_rect,
                    const core::av::AVValue& current_value,
                    const core::av::AVKey& key,
                    AVCellEditCallback on_commit,
                    AVCellCancelCallback on_cancel) override;
    void commit() override;
    void cancel() override;
    [[nodiscard]] auto is_active() const -> bool override;

private:
    AVCellEditCallback on_commit_;
    bool active_{false};
};

/// Factory: returns the correct editor for a given key type.
[[nodiscard]] auto create_cell_editor(core::av::AVKeyType type)
    -> std::unique_ptr<IAVCellEditor>;

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVColumnHeader.h`

```cpp
#pragma once

#include "core/av/AVTypes.h"
#include "core/ThemeEngine.h"

#include <functional>
#include <string>
#include <vector>
#include <wx/dc.h>
#include <wx/gdicmn.h>

namespace markamp::ui::av
{

/// Represents one column header in the table.
struct AVColumnHeaderInfo
{
    std::string key_id;
    std::string name;
    core::av::AVKeyType type{core::av::AVKeyType::Text};
    std::string icon;
    int width{200};
    bool pinned{false};
    bool sort_ascending{false};
    bool sort_descending{false};
    bool has_filter{false};
};

/// Renders the column header bar and handles drag interactions.
class AVColumnHeaderBar
{
public:
    /// Set the column header data.
    void set_columns(std::vector<AVColumnHeaderInfo> columns);

    /// Render the header bar onto a DC at the given y-offset.
    void render(wxDC& dc,
                int y_offset,
                int header_height,
                int scroll_x,
                core::IThemeEngine* theme);

    /// Hit-test: which column index is under this x-coordinate?
    /// Returns -1 if none.
    [[nodiscard]] auto column_at_x(int x, int scroll_x) const -> int;

    /// Hit-test: is the point on a resize handle?
    [[nodiscard]] auto resize_handle_at_x(int x, int scroll_x) const -> int;

    /// Get column info by index.
    [[nodiscard]] auto column_info(int index) const -> const AVColumnHeaderInfo&;

    /// Get all column infos.
    [[nodiscard]] auto columns() const -> const std::vector<AVColumnHeaderInfo>&;

    /// Column context menu item IDs.
    enum class ColumnAction
    {
        SortAscending,
        SortDescending,
        ClearSort,
        AddFilter,
        HideColumn,
        DeleteColumn,
        DuplicateColumn,
        RenameColumn,
        ChangeType,
        EditOptions,     // For Select/MSelect columns
        PinColumn,
        UnpinColumn
    };

    /// Callback for when a column context menu action is selected.
    using ColumnActionCallback = std::function<void(int col_index, ColumnAction action)>;
    void set_column_action_callback(ColumnActionCallback cb);

private:
    std::vector<AVColumnHeaderInfo> columns_;
    ColumnActionCallback action_callback_;
};

} // namespace markamp::ui::av
```

### File: `src/ui/av/AVTableController.h`

```cpp
#pragma once

#include "core/av/AttributeView.h"
#include "core/av/AttributeViewStore.h"
#include "core/av/AVColumnType.h"
#include "core/EventBus.h"

#include <string>
#include <vector>

namespace markamp::core::av
{

/// Controller mediating between the AVTablePanel (UI) and the data model.
/// All mutations go through this controller, which handles:
///   - Data validation before writes
///   - Event publishing after successful mutations
///   - Auto-save triggering
///   - Undo/redo stack (future)
class AVTableController
{
public:
    AVTableController(EventBus* event_bus,
                      AttributeViewStore* store,
                      AVColumnTypeRegistry* type_registry);

    /// Bind to a specific attribute view.
    void set_attribute_view(AttributeView* av);

    // ── Cell editing ──

    /// Update a cell value. Validates, writes, publishes AVValueChangedEvent.
    [[nodiscard]] auto set_cell_value(const std::string& key_id,
                                       const std::string& block_id,
                                       AVValueVariant new_value)
        -> std::expected<void, std::string>;

    // ── Column operations ──

    [[nodiscard]] auto add_column(AVKeyType type, const std::string& name)
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto remove_column(const std::string& key_id)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto rename_column(const std::string& key_id,
                                      const std::string& new_name)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto change_column_type(const std::string& key_id,
                                           AVKeyType new_type)
        -> std::expected<void, std::string>;

    // ── Row operations ──

    [[nodiscard]] auto add_row()
        -> std::expected<std::string, std::string>;

    [[nodiscard]] auto remove_row(const std::string& block_id)
        -> std::expected<void, std::string>;

    [[nodiscard]] auto duplicate_row(const std::string& block_id)
        -> std::expected<std::string, std::string>;

    // ── Persistence ──

    /// Save the current AV to disk.
    [[nodiscard]] auto save() -> std::expected<void, std::string>;

    /// Trigger auto-save if enabled and debounce timer has elapsed.
    void mark_dirty();

private:
    EventBus* event_bus_{nullptr};
    AttributeViewStore* store_{nullptr};
    AVColumnTypeRegistry* type_registry_{nullptr};
    AttributeView* av_{nullptr};
    bool dirty_{false};
};

} // namespace markamp::core::av
```

## Key Functions to Implement

### AVTablePanel.cpp

1. **`AVTablePanel::set_attribute_view(av)`** -- Store pointer, call `recalculate_layout()`, set page 0, call `Refresh()`.
2. **`AVTablePanel::recalculate_layout()`** -- From the active view's `AVTableView::columns`, compute `column_x_positions_` and `column_widths_`. Compute `total_content_width_` and `total_content_height_` based on row count and page size.
3. **`AVTablePanel::on_paint(wxPaintEvent&)`** -- Create `wxAutoBufferedPaintDC`. Call `render_header()`, `render_rows()`, `render_grid_lines()`, `render_selection()`, `render_pagination_bar()`.
4. **`AVTablePanel::render_header(wxDC&)`** -- Delegate to `AVColumnHeaderBar::render()`. Draw sort indicators (up/down arrow) and filter indicators (funnel icon) on active columns.
5. **`AVTablePanel::render_rows(wxDC&)`** -- For each visible row in `[current_page * page_size, (current_page+1) * page_size)`, render each visible column cell using the appropriate `IAVCellRenderer`.
6. **`AVTablePanel::render_cell(wxDC&, addr, rect)`** -- Look up the `AVKey` and `AVValue` for the cell. Get the renderer from `create_cell_renderer(key.type)`. Call `renderer->render()`.
7. **`AVTablePanel::cell_at_point(wxPoint)`** -- Binary search `column_x_positions_` for the column. Compute row from `(y - header_height + scroll_offset_y) / row_height + current_page * page_size`.
8. **`AVTablePanel::on_key_down(wxKeyEvent&)`** -- Arrow keys move focus. Tab moves to next cell (wrapping to next row). Enter begins editing. Escape cancels editing. Delete clears cell value.
9. **`AVTablePanel::begin_edit()`** -- Get the `AVKey` for the focused column. Create editor via `create_cell_editor(key.type)`. Call `editor->begin_edit()` with the cell rect, current value, and commit/cancel callbacks.
10. **`AVTablePanel::commit_edit()`** -- Call `active_editor_->commit()`. The commit callback calls `AVTableController::set_cell_value()`. Clear editing state. Repaint.
11. **`AVTablePanel::on_mouse_motion(wxMouseEvent&)`** -- If resize is active (`resize_col_ >= 0`), calculate new width. If drag is active (`drag_col_ >= 0`), update drop target indicator. If neither, set cursor (resize cursor near column edges, normal otherwise).
12. **`AVTablePanel::show_column_context_menu(col, pos)`** -- Build a `wxMenu` with Sort Ascending, Sort Descending, Filter, Hide, Delete, Duplicate, Rename, Change Type. For Select columns, add "Edit Options". Connect menu item IDs to `AVColumnHeaderBar::ColumnAction` callback.

### AVCellRenderer.cpp

13. **`TextCellRenderer::render()`** -- Draw text left-aligned with padding. Truncate with ellipsis if wider than rect. Use theme's editor font.
14. **`NumberCellRenderer::render()`** -- Format number using `format_number()`. Draw right-aligned.
15. **`DateCellRenderer::render()`** -- Format date using `format_date()`. Draw with calendar icon prefix.
16. **`SelectCellRenderer::render()`** -- Draw colored rounded-rect "chips" for each selected option. Chip background = option color at 20% opacity, text = option color.
17. **`CheckboxCellRenderer::render()`** -- Draw a centered checkbox square. Fill with checkmark if checked. Use theme accent color.
18. **`URLCellRenderer::render()`** -- Draw as underlined text in link color. Show globe icon prefix.
19. **`RelationCellRenderer::render()`** -- Draw block reference "chips" (similar to select chips but with link icon).
20. **`RollupCellRenderer::render()`** -- Format the rollup result. Draw as plain text (with appropriate formatting for the underlying type).

### AVCellEditor.cpp

21. **`TextCellEditor::begin_edit()`** -- Create `wxTextCtrl` at cell rect. Set current value as text. Select all. Bind Enter to commit, Escape to cancel, focus-lost to commit.
22. **`NumberCellEditor::begin_edit()`** -- Create `wxTextCtrl` with `wxFILTER_NUMERIC` validator. Set formatted current value. Bind Enter/Escape.
23. **`DateCellEditor::begin_edit()`** -- Create a popup panel below the cell with a `wxCalendarCtrl` and optional time picker. Bind date selection to commit.
24. **`SelectCellEditor::begin_edit()`** -- Create a popup panel with a list of options as colored checkboxes (for MSelect) or radio buttons (for Select). Each option shows a color chip and name.
25. **`CheckboxCellEditor::begin_edit()`** -- Immediately toggle the value (no visible editor needed). Call commit callback with the toggled value.

### AVTableController.cpp

26. **`AVTableController::set_cell_value(key_id, block_id, new_value)`** -- Validate using `type_registry_->validate_value()`. If valid, call `av_->set_value()`. Publish `AVValueChangedEvent`. Call `mark_dirty()`.
27. **`AVTableController::add_column(type, name)`** -- Create `AVKey` with generated UUID. Call `av_->add_key()`. Publish `AVKeyAddedEvent`. Call `mark_dirty()`.
28. **`AVTableController::add_row()`** -- Generate block_id UUID. Call `av_->add_row()`. Set Created/Updated timestamps. Publish `AVRowAddedEvent`. Call `mark_dirty()`.
29. **`AVTableController::save()`** -- Call `store_->save(*av_)`. Publish `AVSavedEvent`. Clear dirty flag.
30. **`AVTableController::change_column_type(key_id, new_type)`** -- Find the key. Update its type. For each row value, attempt to convert the existing value to the new type using `parse_input()` on the string representation. Values that fail conversion become empty.

## Events to Add

Add to `src/core/Events.h`:

```cpp
// ============================================================================
// Attribute View table interaction events (Phase 19)
// ============================================================================

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVCellEditedEvent)
std::string av_id;
std::string key_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnAddedEvent)
std::string av_id;
std::string key_id;
std::string key_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnRemovedEvent)
std::string av_id;
std::string key_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnReorderedEvent)
std::string av_id;
std::string key_id;
int old_index;
int new_index;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVColumnResizedEvent)
std::string av_id;
std::string key_id;
int new_width;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowAddedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVRowRemovedEvent)
std::string av_id;
std::string block_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVViewChangedEvent)
std::string av_id;
std::string view_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(AVPageChangedEvent)
std::string av_id;
int page;
MARKAMP_DECLARE_EVENT_END;
```

Note: `AVRowAddedEvent` and `AVRowRemovedEvent` were already declared in Phase 17. If they are identical, do not re-declare them. The above events should be deduplicated against Phase 17 declarations. The new events unique to this phase are: `AVCellEditedEvent`, `AVColumnAddedEvent`, `AVColumnRemovedEvent`, `AVColumnReorderedEvent`, `AVColumnResizedEvent`, `AVViewChangedEvent`, `AVPageChangedEvent`.

## Config Keys to Add

| Key | Type | Default | Description |
|---|---|---|---|
| `av.table.default_row_height` | int | `32` | Default row height in pixels |
| `av.table.header_height` | int | `36` | Column header height |
| `av.table.default_column_width` | int | `200` | Default new column width |
| `av.table.show_row_numbers` | bool | `false` | Show row number column |
| `av.table.stripe_rows` | bool | `true` | Alternating row colors |
| `av.table.show_grid_lines` | bool | `true` | Show cell border lines |

## Test Cases

File: `tests/unit/test_av_table_renderer.cpp`

All tests use Catch2 with `[av][table]` tags. Note: UI rendering tests focus on the controller and data flow, not pixel-level rendering (which requires a running wxWidgets app).

1. **AVTableController: set cell value publishes event** -- Create a controller with a mock EventBus. Bind an AV with one Text column and one row. Call `set_cell_value()` with a new text value. Verify the AVValueChangedEvent was published. Verify the AV's value was updated.

2. **AVTableController: add column creates key and empty values** -- Call `add_column(AVKeyType::Number, "Score")`. Verify the AV now has 2 keys. Verify each existing row has a new empty value for the new column.

3. **AVTableController: remove column cascades to all rows** -- Create AV with 3 columns and 5 rows. Remove the middle column. Verify `key_count() == 2`. Verify each row has exactly 2 values. Verify the removed column's key_id no longer appears in any view configuration.

4. **AVTableController: add row populates all columns** -- Create AV with 3 columns. Call `add_row()`. Verify `row_count()` increased by 1. Verify the new row has an empty value for every column. Verify Created and Updated columns (if present) have the current timestamp.

5. **AVTableController: duplicate row copies all values** -- Create AV with 2 columns. Add a row and set values. Call `duplicate_row()`. Verify the new row has identical values to the original (except different block_id and value IDs).

6. **AVTableController: change column type converts values** -- Create AV with a Text column. Add 3 rows with values "42", "not a number", "3.14". Change column type to Number. Verify row 1 value is `AVValueNumber{42.0, true, ...}`. Verify row 2 value is empty (conversion failed). Verify row 3 value is `AVValueNumber{3.14, true, ...}`.

7. **AVTableController: validation rejects invalid values** -- Create AV with a Number column. Attempt to set a value with `AVValueText{"abc"}` (wrong variant type). Verify `set_cell_value()` returns an error. Verify the original value is unchanged.

8. **AVCellAddress: valid/invalid** -- Default-constructed address is invalid. Address with row=0, col=0 is valid. Address with row=-1 is invalid.

9. **AVColumnHeaderBar: column_at_x hit testing** -- Set up 3 columns with widths [100, 200, 150]. Verify `column_at_x(50)` returns 0. Verify `column_at_x(250)` returns 1. Verify `column_at_x(400)` returns 2. Verify `column_at_x(500)` returns -1.

10. **AVTablePanelConfig: pagination math** -- Config with `page_size=10`. AV with 35 rows. Verify `total_pages() == 4` (ceil(35/10)). Page 0 shows rows 0-9. Page 3 shows rows 30-34.

11. **Cell renderer factory: returns correct type** -- Call `create_cell_renderer()` for each `AVKeyType`. Verify non-null return. Verify `dynamic_cast` to expected concrete type succeeds.

12. **Cell editor factory: returns correct type** -- Call `create_cell_editor()` for each editable `AVKeyType`. Verify non-null return for Text, Number, Date, Select, MSelect, Checkbox, URL, Email, Phone. Verify nullptr for non-editable types (Template, Created, Updated, LineNumber, Rollup).

## Acceptance Criteria

- [ ] `AVTablePanel` renders column headers with names, type icons, sort/filter indicators
- [ ] `AVTablePanel` renders rows with type-specific cell content
- [ ] Keyboard navigation works: arrows, Tab, Enter to edit, Escape to cancel
- [ ] Text cells support inline editing via `wxTextCtrl`
- [ ] Number cells validate numeric input
- [ ] Checkbox cells toggle on click/Enter without a popup
- [ ] Select cells show dropdown popup with color chips
- [ ] Column context menu provides Sort/Filter/Hide/Delete/Rename/ChangeType
- [ ] Column resize via drag on header edge works
- [ ] Pagination controls navigate between pages
- [ ] `AVTableController` validates values before writing
- [ ] All mutations publish the correct events via EventBus
- [ ] All 12 test cases pass
- [ ] No use of `catch(...)` -- all exceptions typed
- [ ] All query methods have `[[nodiscard]]`

## Files to Create

- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTablePanel.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellRenderer.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVCellEditor.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVColumnHeader.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVColumnHeader.cpp`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTableController.h`
- `/Users/ryanrentfro/code/markamp/src/ui/av/AVTableController.cpp`
- `/Users/ryanrentfro/code/markamp/tests/unit/test_av_table_renderer.cpp`

## Files to Modify

- `/Users/ryanrentfro/code/markamp/src/core/Events.h` -- Add 7 new table interaction events
- `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt` -- Add 6 new `.cpp` source files
- `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt` -- Add `test_av_table_renderer` test target
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.h` -- Add `show_attribute_view_table()` method
- `/Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp` -- Wire AVTablePanel creation and layout
- `/Users/ryanrentfro/code/markamp/resources/config_defaults.json` -- Add 6 new `av.table.*` config keys
