# Phase 32 -- Rich DataFrame Display and Data Visualization

## Objective

Implement rich rendering for tabular data (pandas DataFrames, R data frames) and inline chart rendering. When a kernel returns an HTML table or a plot image, render it interactively: sortable/filterable tables, zoomable charts, and data export options. This transforms code cell output from static text into an interactive data exploration environment.

## Prerequisites

- Phase 31 (CellOutputRenderer -- MIME bundle rendering)
- Phase 29 (KernelManager -- execution results)

## Feature References (PRD)

- PRD Notebook #4: Rich DataFrame Display
- PRD Notebook #6: Inline Data Visualization
- PRD Notebook #33: Interactive Plot Backends

## Data Structures to Implement

### File: `src/ui/DataFrameRenderer.h`

```cpp
#pragma once

#include <string>
#include <vector>
#include <wx/panel.h>
#include <wx/grid.h>

namespace markamp::core { class EventBus; class ThemeEngine; }

namespace markamp::ui
{

struct DataFrameColumn
{
    std::string name;
    std::string dtype;        // "int64", "float64", "object", "datetime64"
    int width{100};
};

struct DataFrameData
{
    std::vector<DataFrameColumn> columns;
    std::vector<std::vector<std::string>> rows;
    int total_rows{0};        // May exceed rows.size() if truncated
    bool truncated{false};

    [[nodiscard]] auto column_count() const -> int;
    [[nodiscard]] auto row_count() const -> int;
};

struct ChartData
{
    std::string format;       // "png", "svg", "html"
    std::string data;         // Base64 image or HTML content
    int width{0};
    int height{0};
    bool is_interactive{false};  // True for Plotly/Bokeh HTML charts
};

class DataFramePanel : public wxPanel
{
public:
    DataFramePanel(wxWindow* parent,
                   markamp::core::EventBus& event_bus,
                   markamp::core::ThemeEngine& theme_engine);

    /// Load data from HTML table output.
    auto load_from_html(const std::string& html) -> void;

    /// Load data from JSON output.
    auto load_from_json(const std::string& json) -> void;

    /// Sort by column.
    auto sort_by_column(int col_index, bool ascending = true) -> void;

    /// Filter rows by a text query.
    auto filter(const std::string& query) -> void;

    /// Export visible data as CSV.
    [[nodiscard]] auto export_csv() const -> std::string;

    /// Get the underlying data.
    [[nodiscard]] auto data() const -> const DataFrameData&;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;

    DataFrameData data_;
    wxGrid* grid_{nullptr};

    auto populate_grid() -> void;
    auto parse_html_table(const std::string& html) -> DataFrameData;
    auto parse_json_table(const std::string& json) -> DataFrameData;
    auto apply_theme() -> void;
};

class ChartPanel : public wxPanel
{
public:
    ChartPanel(wxWindow* parent,
               markamp::core::EventBus& event_bus,
               markamp::core::ThemeEngine& theme_engine);

    /// Load a chart from image data (PNG/SVG base64).
    auto load_image(const std::string& data, const std::string& format) -> void;

    /// Load an interactive chart from HTML (Plotly, Bokeh).
    auto load_interactive_html(const std::string& html) -> void;

    /// Save chart to file.
    [[nodiscard]] auto save_chart(const std::string& path) const
        -> std::expected<void, std::string>;

    /// Zoom in/out on the chart.
    auto set_zoom(double factor) -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;

    ChartData chart_;
    double zoom_{1.0};

    auto on_paint(wxPaintEvent& event) -> void;
    auto on_mouse_wheel(wxMouseEvent& event) -> void;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`parse_html_table(html)`** -- Parse `<table>` HTML into DataFrameData. Extract `<thead>` for column names. Extract `<tbody><tr><td>` for row data. Handle pandas-generated HTML (with index column, dtypes).

2. **`populate_grid()`** -- Set wxGrid dimensions. Set column labels. Fill cell values. Apply alternating row colors from theme.

3. **`sort_by_column(col, ascending)`** -- Sort data_.rows by the specified column. Detect numeric vs string sort. Refresh grid.

4. **`filter(query)`** -- Filter rows where any cell contains the query string. Show only matching rows in the grid.

5. **`export_csv()`** -- Build CSV string from visible (filtered) data. Properly escape fields with commas/quotes.

6. **`ChartPanel::load_image(data, format)`** -- Decode base64 PNG to wxBitmap or parse SVG. Display in panel. Enable zoom.

7. **`ChartPanel::load_interactive_html(html)`** -- Render HTML with JavaScript (Plotly/Bokeh) in an embedded webview panel.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(DataFrameRenderedEvent)
std::string cell_id;
int rows{0};
int columns{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(ChartRenderedEvent)
std::string cell_id;
std::string format;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_dataframe_renderer.cpp`

1. **Parse HTML table** -- Pandas-style HTML table with 3 cols, 5 rows. Verify correct parsing.
2. **Column extraction** -- Table header with "Name", "Age", "City". Verify 3 columns.
3. **Sort numeric column** -- Sort by numeric column ascending. Verify order.
4. **Sort string column** -- Sort by string column. Verify alphabetical.
5. **Filter rows** -- 10 rows, filter "John". Verify only matching rows.
6. **Export CSV** -- 3x3 data. Export CSV. Verify valid CSV format.
7. **CSV escaping** -- Cell with comma in value. Verify quoted in CSV.
8. **Truncated data** -- 1000 rows, display first 100. Verify truncated=true.
9. **Chart image loading** -- Base64 PNG data. Verify bitmap created.
10. **Chart zoom** -- Set zoom to 2.0. Verify zoom applied.

## Acceptance Criteria

- [ ] HTML table parsing extracts columns and rows correctly
- [ ] Grid display supports sorting by any column
- [ ] Row filtering finds matching text
- [ ] CSV export produces valid formatted output
- [ ] Chart images render from base64 PNG/SVG
- [ ] Interactive charts render in webview
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/DataFrameRenderer.h` | DataFramePanel, ChartPanel |
| CREATE | `src/ui/DataFrameRenderer.cpp` | Full implementation |
| MODIFY | `src/ui/CellOutputRenderer.cpp` | Route table/chart outputs to specialized renderers |
| MODIFY | `src/core/Events.h` | Add 2 data events |
| MODIFY | `src/CMakeLists.txt` | Add DataFrameRenderer.cpp |
| CREATE | `tests/unit/test_dataframe_renderer.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_dataframe_renderer target |

## Estimated Complexity

**L** -- HTML table parsing, wxGrid integration, image rendering, chart zoom, 10 tests.
