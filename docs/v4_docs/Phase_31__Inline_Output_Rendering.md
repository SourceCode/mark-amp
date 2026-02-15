# Phase 31 -- Inline Output Rendering and MIME Bundles

## Objective

Implement the cell output rendering pipeline: display execution results inline below code cells. Supports multiple MIME types (text/plain, text/html, image/png, image/svg+xml, application/json), collapsible output areas, output clearing, and streaming output (stdout/stderr displayed in real-time). This is the visual layer for kernel execution results.

## Prerequisites

- Phase 29 (KernelManager -- execution results with MimeBundle)
- Existing EditorPanel, PreviewPanel

## Feature References (PRD)

- PRD Notebook #3: Inline Output Rendering
- PRD Notebook #43: Output MIME Bundles
- PRD Notebook #34: Inline HTML Rendering
- PRD Notebook #12: Output Clearing
- PRD Notebook #49: Inline Error Tracebacks

## Data Structures to Implement

### File: `src/ui/CellOutputRenderer.h`

```cpp
#pragma once

#include "core/KernelManager.h"

#include <string>
#include <vector>
#include <wx/panel.h>

namespace markamp::core { class EventBus; class ThemeEngine; }

namespace markamp::ui
{

enum class OutputType : uint8_t
{
    PlainText,
    Html,
    Image,
    Svg,
    Json,
    Error,
    Stream
};

struct CellOutput
{
    std::string cell_id;
    OutputType type{OutputType::PlainText};
    std::string content;
    std::string mime_type;
    int execution_count{0};
    bool is_error{false};
    std::vector<std::string> traceback;
    std::string stream_name;  // "stdout" or "stderr"
};

struct CellOutputState
{
    std::string cell_id;
    std::vector<CellOutput> outputs;
    bool collapsed{false};
    bool cleared{false};
    int execution_count{0};
};

class CellOutputRenderer
{
public:
    CellOutputRenderer(markamp::core::EventBus& event_bus,
                       markamp::core::ThemeEngine& theme_engine);

    /// Render a MimeBundle as the best available format.
    [[nodiscard]] auto render_output(const markamp::core::MimeBundle& bundle) const
        -> CellOutput;

    /// Render an error with formatted traceback.
    [[nodiscard]] auto render_error(const std::string& error_name,
                                     const std::string& error_value,
                                     const std::vector<std::string>& traceback) const
        -> CellOutput;

    /// Render a stream output (stdout/stderr).
    [[nodiscard]] auto render_stream(const std::string& stream_name,
                                      const std::string& text) const
        -> CellOutput;

    /// Create a wxPanel widget for displaying a cell output.
    [[nodiscard]] auto create_output_panel(wxWindow* parent,
                                            const CellOutput& output) const
        -> wxPanel*;

    /// Create a widget for displaying an image (PNG/SVG).
    [[nodiscard]] auto create_image_panel(wxWindow* parent,
                                           const std::string& data,
                                           const std::string& format) const
        -> wxPanel*;

    /// Create a widget for displaying HTML content.
    [[nodiscard]] auto create_html_panel(wxWindow* parent,
                                          const std::string& html) const
        -> wxPanel*;

    /// Create a widget for displaying error traceback.
    [[nodiscard]] auto create_error_panel(wxWindow* parent,
                                           const CellOutput& error) const
        -> wxPanel*;

    /// Format ANSI escape codes in text to styled text.
    [[nodiscard]] auto ansi_to_styled(const std::string& text) const -> std::string;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;

    [[nodiscard]] auto select_best_mime(const markamp::core::MimeBundle& bundle) const
        -> std::string;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`render_output(bundle)`** -- Select best MIME type from bundle (prefer text/html > image/png > image/svg+xml > text/plain). Create CellOutput with appropriate type and content.

2. **`select_best_mime(bundle)`** -- Priority: text/html, image/png, image/svg+xml, application/json, text/plain. Return the highest-priority available type.

3. **`create_output_panel(parent, output)`** -- Dispatch based on output type: PlainText -> styled text widget. Html -> embedded webview or rich text. Image -> wxStaticBitmap. Error -> red-tinted error display with traceback.

4. **`render_error(name, value, traceback)`** -- Format error: bold error name, error value, then each traceback line with syntax-colored stack frames.

5. **`ansi_to_styled(text)`** -- Parse ANSI escape sequences (\033[31m = red, etc.) and convert to wxTextAttr styles for display in a wxTextCtrl or similar.

6. **`create_image_panel(parent, data, format)`** -- Decode base64 PNG data to wxBitmap. For SVG, render via existing rendering pipeline. Display in a scrollable panel.

7. **`create_error_panel(parent, error)`** -- Display error with red background tint. Traceback lines with monospace font and syntax coloring for file paths and line numbers.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputRenderedEvent)
std::string cell_id;
OutputType output_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputClearedEvent)
std::string cell_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellOutputCollapsedEvent)
std::string cell_id;
bool collapsed{false};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_cell_output.cpp`

1. **Render plain text** -- MimeBundle with text/plain. Verify PlainText output.
2. **Render HTML** -- Bundle with text/html. Verify Html type selected over text/plain.
3. **Render image** -- Bundle with image/png (base64). Verify Image type.
4. **Render SVG** -- Bundle with image/svg+xml. Verify Svg type.
5. **Render error** -- Error with traceback. Verify formatted error output.
6. **Stream stdout** -- Stream output with name="stdout". Verify correct stream type.
7. **Stream stderr** -- stderr stream. Verify stream_name="stderr".
8. **MIME priority** -- Bundle with text/plain and text/html. Verify html selected.
9. **ANSI parsing** -- Text with `\033[31mred\033[0m`. Verify ANSI stripped/converted.
10. **Output clearing** -- Clear outputs for a cell. Verify empty.
11. **Output collapsing** -- Collapse output. Verify collapsed state.
12. **JSON rendering** -- Bundle with application/json. Verify formatted JSON display.

## Acceptance Criteria

- [ ] MimeBundle rendering selects best available format
- [ ] Text, HTML, image, SVG, and JSON outputs render correctly
- [ ] Error tracebacks display with syntax coloring
- [ ] Streaming output appends in real-time
- [ ] ANSI escape codes converted to styled text
- [ ] Output clearing and collapsing work
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/CellOutputRenderer.h` | CellOutputRenderer, CellOutput, CellOutputState |
| CREATE | `src/ui/CellOutputRenderer.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 3 cell output events |
| MODIFY | `src/CMakeLists.txt` | Add CellOutputRenderer.cpp |
| CREATE | `tests/unit/test_cell_output.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_cell_output target |

## Estimated Complexity

**L** -- Multiple output format renderers, ANSI parsing, image decoding, widget creation, 12 tests.
