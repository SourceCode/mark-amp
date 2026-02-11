# Phase 17: Mermaid Diagram Integration

## Objective

Implement Mermaid diagram rendering in the preview pane, converting Mermaid source code into SVG diagrams. This must support all common Mermaid diagram types, theme-aware styling, error handling with user-friendly overlays, and non-blocking rendering.

## Prerequisites

- Phase 14 (Markdown-to-Rendered-View Pipeline)

## Deliverables

1. MermaidRenderer that converts Mermaid source to SVG
2. Theme-aware Mermaid diagram styling
3. Error overlay for invalid Mermaid syntax
4. Async rendering (non-blocking UI)
5. Support for: flowcharts, sequence diagrams, class diagrams, state diagrams, gantt charts, pie charts, ER diagrams

## Tasks

### Task 17.1: Choose and integrate a Mermaid rendering approach

Evaluate approaches for rendering Mermaid diagrams in a C++ desktop application:

**Option A: Embedded JavaScript engine (QuickJS or V8)**
- Bundle the Mermaid.js library
- Use QuickJS (lightweight, ~200KB) or Duktape to execute Mermaid.js
- Mermaid.js outputs SVG strings
- Parse the SVG and display in wxWidgets
- Pros: Full Mermaid compatibility, identical output to web version
- Cons: Adds a JS engine dependency, memory overhead

**Option B: Call external mermaid-cli (mmdc)**
- Shell out to `mmdc` (Mermaid CLI, Node.js based)
- Pass Mermaid source, receive SVG output
- Pros: Simplest integration, always up-to-date with Mermaid
- Cons: Requires Node.js/mmdc installed, slow for interactive use

**Option C: Native SVG generation**
- Implement a subset of Mermaid diagram types natively in C++
- Parse Mermaid syntax, compute layout, generate SVG
- Pros: No external dependencies, fast
- Cons: Enormous implementation scope, will never match full Mermaid compatibility

**Option D: Embedded browser rendering (wxWebView)**
- If using wxWebView for preview, load Mermaid.js in the browser context
- Execute Mermaid rendering in the web view
- Pros: Perfect compatibility, same as web version
- Cons: Requires wxWebView (heavier than wxHtmlWindow)

**Recommended: Option A (QuickJS) as primary, Option B (mmdc) as fallback.**

If wxWebView is already used for the preview panel (Phase 14), Option D becomes the simplest path.

Create `/Users/ryanrentfro/code/markamp/src/core/MermaidRenderer.h` and `MermaidRenderer.cpp`:

```cpp
namespace markamp::core {

class MermaidRenderer : public IMermaidRenderer
{
public:
    MermaidRenderer();
    ~MermaidRenderer() override;

    auto render(std::string_view mermaid_source) -> std::expected<std::string, std::string> override;
    auto is_available() const -> bool override;

    // Configuration
    void set_theme(const std::string& mermaid_theme);  // "dark", "default", "forest", "neutral"
    void set_font_family(const std::string& font);

private:
    struct Impl;
    std::unique_ptr<Impl> impl_;

    auto render_via_quickjs(std::string_view source) -> std::expected<std::string, std::string>;
    auto render_via_mmdc(std::string_view source) -> std::expected<std::string, std::string>;
};

} // namespace markamp::core
```

**Acceptance criteria:**
- At least one rendering approach is functional
- Mermaid source is converted to SVG strings
- `is_available()` correctly reports whether rendering is possible

### Task 17.2: Implement QuickJS integration (if using Option A)

Add QuickJS as a dependency (via vcpkg or as a bundled source):

1. Initialize QuickJS runtime
2. Load the Mermaid.js bundle into the runtime
3. Create a rendering function:
   ```javascript
   function renderMermaid(source, config) {
       return mermaid.render('diagram', source, config);
   }
   ```
4. Call the function from C++, passing the Mermaid source
5. Extract the SVG result string
6. Clean up resources properly

**Memory management:**
- Use a single QuickJS runtime instance (reused across renders)
- Set memory limits to prevent runaway diagrams
- Timeout protection for infinite loops in Mermaid.js

**Acceptance criteria:**
- QuickJS initializes and loads Mermaid.js
- Simple diagrams render to SVG
- Memory is properly managed (no leaks)
- Rendering times out after 5 seconds

### Task 17.3: Implement fallback mmdc rendering (Option B)

For systems where QuickJS is not available or fails:

1. Check if `mmdc` is on the PATH: `which mmdc` or `where mmdc`
2. Write Mermaid source to a temp file
3. Execute: `mmdc -i input.mmd -o output.svg -t dark`
4. Read the SVG output
5. Clean up temp files

**Acceptance criteria:**
- Fallback works when mmdc is installed
- `is_available()` returns false when neither QuickJS nor mmdc is available
- Temp files are cleaned up even on error

### Task 17.4: Implement theme-aware Mermaid configuration

Map MarkAmp themes to Mermaid configuration:

```cpp
auto MermaidRenderer::get_mermaid_config(const Theme& theme) -> std::string
{
    // Mermaid theme mapping
    std::string mermaid_theme = theme.is_dark() ? "dark" : "default";

    return std::format(R"({{
        "theme": "{}",
        "themeVariables": {{
            "primaryColor": "{}",
            "primaryTextColor": "{}",
            "primaryBorderColor": "{}",
            "lineColor": "{}",
            "secondaryColor": "{}",
            "tertiaryColor": "{}"
        }},
        "fontFamily": "JetBrains Mono",
        "securityLevel": "strict"
    }})",
        mermaid_theme,
        theme.colors.accent_primary.to_hex(),
        theme.colors.text_main.to_hex(),
        theme.colors.border_light.to_hex(),
        theme.colors.text_muted.to_hex(),
        theme.colors.accent_secondary.to_hex(),
        theme.colors.bg_panel.to_hex()
    );
}
```

**Acceptance criteria:**
- Mermaid diagrams use colors from the current MarkAmp theme
- Dark themes use Mermaid's dark theme
- Light themes use Mermaid's default theme
- Theme changes re-render Mermaid diagrams with new colors

### Task 17.5: Implement SVG display in the preview pane

Create `/Users/ryanrentfro/code/markamp/src/rendering/MermaidBlock.h` and `MermaidBlock.cpp`:

**Display approach:**
- If using wxHtmlWindow: embed SVG as an inline `<img>` tag with data URI
  ```html
  <div class="mermaid-container">
    <img src="data:image/svg+xml;base64,{base64_svg}" />
  </div>
  ```
- If using wxWebView: inject SVG directly as HTML
  ```html
  <div class="mermaid-container">{svg_content}</div>
  ```

**Container styling (matching reference MermaidBlock.tsx line 51-53):**
- `margin: 16px 0;` (my-4)
- `display: flex; justify-content: center;` (centered)
- `background: {bg_panel} at 50%;`
- `padding: 16px;` (p-4)
- `border-radius: 4px;` (rounded)
- `border: 1px solid {border_light};`

**Acceptance criteria:**
- Mermaid SVG diagrams display in the preview
- Diagrams are centered in their container
- Container has correct theme-aware styling
- SVGs render at correct size (responsive to container width)

### Task 17.6: Implement error handling with overlay

When Mermaid rendering fails (matching reference MermaidBlock.tsx lines 42-48):

**Error display:**
```html
<div class="mermaid-error">
  Syntax Error: Invalid Mermaid definition
</div>
```

**Styling:**
- `padding: 16px;` (p-4)
- `border: 1px solid rgba(255, 0, 0, 0.5);` (border-red-500/50)
- `background: rgba(153, 0, 0, 0.2);` (bg-red-900/20)
- `color: #f87171;` (text-red-400)
- `font-family: monospace;`
- `font-size: 14px;`
- `border-radius: 4px;`

**Error information:**
- Show the error message from the Mermaid renderer
- If the error includes a line number, show it
- Keep the error visible until the user fixes the Mermaid source

**Acceptance criteria:**
- Invalid Mermaid source shows error overlay (not crash)
- Error message is descriptive
- Error styling matches the reference red theme
- Fixing the source replaces the error with the rendered diagram

### Task 17.7: Implement async rendering

Mermaid rendering can be slow (especially complex diagrams). Render asynchronously:

1. Show a "Rendering diagram..." placeholder immediately
2. Start rendering on a background thread (or timer-based approach)
3. When rendering completes, update the preview with the SVG
4. If the content changes while rendering, cancel and restart

**Implementation:**
- Use `std::async` or `std::jthread` for background rendering
- Use `wxEvtHandler::CallAfter` to update the UI from the background thread
- Track a generation counter to detect stale renders

**Acceptance criteria:**
- UI remains responsive during Mermaid rendering
- Complex diagrams do not freeze the application
- Stale renders are discarded
- Placeholder is shown while rendering

### Task 17.8: Test all Mermaid diagram types

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_mermaid_renderer.cpp`:

Test cases:
1. Simple flowchart (graph TD)
2. Sequence diagram
3. Class diagram
4. State diagram
5. Gantt chart
6. Pie chart
7. ER diagram
8. Invalid Mermaid syntax (error case)
9. Empty Mermaid block
10. Very large diagram (performance test)
11. Theme configuration application
12. Concurrent rendering (multiple diagrams)
13. Render cancellation

Also test with the reference sample files:
- `architecture.md` contains a flowchart
- `sequence.md` contains a sequence diagram

**Acceptance criteria:**
- All common diagram types render to valid SVG
- Error cases return descriptive error messages
- Performance: simple diagrams render in under 500ms
- Reference sample files render correctly

## Files Created/Modified

| File | Action |
|---|---|
| `src/core/MermaidRenderer.h` | Created |
| `src/core/MermaidRenderer.cpp` | Created |
| `src/rendering/MermaidBlock.h` | Created |
| `src/rendering/MermaidBlock.cpp` | Created |
| `src/rendering/HtmlRenderer.cpp` | Modified (mermaid block rendering) |
| `src/ui/PreviewPanel.cpp` | Modified (mermaid CSS) |
| `src/app/MarkAmpApp.cpp` | Modified (register MermaidRenderer) |
| `vcpkg.json` | Modified (add QuickJS if using Option A) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_mermaid_renderer.cpp` | Created |

## Dependencies

- QuickJS (if using Option A) or mmdc CLI (if using Option B)
- Mermaid.js bundle (if using QuickJS)
- Phase 14 rendering pipeline
- Phase 08 theme engine

## Estimated Complexity

**Critical** -- Mermaid integration is one of the most technically challenging features. Embedding a JavaScript engine adds complexity and potential for memory issues. The fallback CLI approach has performance limitations. SVG handling and display in wxWidgets adds another layer of complexity. This phase may require the most iteration to get right.
