# Phase 18: Preview Component with Theme-Aware Rendering

## Objective

Complete the preview component by integrating all rendering sub-systems (markdown, GFM extensions, syntax-highlighted code blocks, Mermaid diagrams) into a cohesive, theme-aware preview pane that faithfully reproduces the visual output of the reference Preview.tsx component.

## Prerequisites

- Phase 14 (Markdown-to-Rendered-View Pipeline)
- Phase 15 (GFM Extensions)
- Phase 16 (Code Block Rendering with Syntax Highlighting)
- Phase 17 (Mermaid Diagram Integration)

## Deliverables

1. Complete, integrated preview rendering with all features
2. Pixel-accurate theme-aware styling matching the reference
3. Bevel panel overlay on the preview pane
4. Image rendering (local files)
5. Print/export support (PDF, HTML export)
6. Full visual comparison with reference implementation

## Tasks

### Task 18.1: Integrate all rendering sub-systems

Update `PreviewPanel` to use all rendering components in a unified pipeline:

```cpp
void PreviewPanel::RenderContent(const std::string& markdown)
{
    // 1. Parse markdown
    auto doc = parser_->parse(markdown);
    if (!doc) { ShowError(doc.error()); return; }

    // 2. Render HTML with all features
    rendering::HtmlRenderer renderer;
    renderer.set_syntax_highlighter(&syntax_highlighter_);
    renderer.set_mermaid_renderer(&mermaid_renderer_);
    renderer.set_table_renderer(&table_renderer_);

    auto body_html = renderer.render(*doc);

    // 3. Generate theme CSS (including all sub-component styles)
    auto css = GenerateFullCSS();

    // 4. Assemble and display
    auto full_html = WrapInHtmlDocument(body_html, css);
    DisplayHtml(full_html);
}
```

**Integration points:**
- HtmlRenderer delegates to CodeBlockRenderer for code blocks
- HtmlRenderer delegates to MermaidBlock for mermaid blocks
- HtmlRenderer delegates to TableRenderer for tables
- All renderers receive theme context for styling

**Acceptance criteria:**
- A document with headings, code blocks, tables, task lists, and Mermaid diagrams renders all elements correctly in a single preview
- No conflicts between rendering sub-systems
- Theme changes update all rendered elements

### Task 18.2: Implement complete CSS generation

Create the comprehensive CSS that styles every rendered element:

The CSS must cover all elements from the reference Preview.tsx:

```css
/* Body */
body { background: {bg_app}; color: {text_main}; font-family: 'Rajdhani', sans-serif; }

/* Headings */
h1 { font-size: 28px; font-weight: bold; color: {accent_primary};
     border-bottom: 1px solid {border_light}; padding-bottom: 8px; margin-bottom: 24px; }
h2 { font-size: 22px; font-weight: 600; color: {accent_primary};
     margin-top: 32px; margin-bottom: 16px; }
h3 { font-size: 18px; font-weight: 500; color: {text_main};
     margin-top: 24px; margin-bottom: 12px; }

/* Text */
p { margin-bottom: 16px; line-height: 1.7; color: {text_main_90}; }
a { color: {accent_primary}; text-decoration: none; }
a:hover { text-decoration: underline; text-underline-offset: 4px; }

/* Blockquotes */
blockquote { border-left: 4px solid {accent_primary}; padding-left: 16px;
             padding-top: 4px; padding-bottom: 4px; font-style: italic;
             background: {accent_primary_5}; margin: 16px 0; }

/* Lists */
ul { list-style-type: disc; padding-left: 24px; margin-bottom: 16px; }
ol { list-style-type: decimal; padding-left: 24px; margin-bottom: 16px; }
li::marker { color: {accent_primary}; }

/* Tables (from Phase 15) */
/* Code blocks (from Phase 16) */
/* Mermaid containers (from Phase 17) */
/* Task lists (from Phase 15) */

/* Inline code */
code.inline-code { background: {accent_primary_20}; color: {accent_primary};
                   padding: 2px 6px; border-radius: 4px; font-size: 0.9em;
                   font-family: 'JetBrains Mono', monospace; }

/* Horizontal rule */
hr { border: none; border-top: 1px solid {border_light}; margin: 32px 0; }

/* Images */
img { max-width: 100%; height: auto; border-radius: 4px;
      border: 1px solid {border_light}; }

/* Selection */
::selection { background: {accent_primary_30}; }
```

**Acceptance criteria:**
- Every element type has themed CSS
- No hardcoded color values
- CSS is regenerated on theme change
- Visual output matches the reference for all 8 built-in themes

### Task 18.3: Add bevel panel overlay to preview pane

The reference App.tsx applies a bevel effect to both the editor and preview panes (lines 200-202, 211-213):

```css
border-top: 2px solid rgba(0, 0, 0, 0.2);
border-left: 2px solid rgba(0, 0, 0, 0.2);
border-bottom: 2px solid rgba(255, 255, 255, 0.05);
border-right: 2px solid rgba(255, 255, 255, 0.05);
```

This creates a sunken/inset appearance. The overlay must be non-interactive (not intercept clicks/scrolls).

**Implementation:**
- Add a `BevelPanel` overlay to the `PreviewPanel` container
- The bevel draws on top of the preview content
- Use `wxWindow::SetTransparent()` or paint the bevel as a non-interactive overlay

**Acceptance criteria:**
- Bevel effect is visible on the preview pane border
- Bevel does not intercept mouse events
- Bevel colors are theme-aware

### Task 18.4: Implement local image rendering

Support displaying local images referenced in markdown:

```markdown
![Alt text](./screenshot.png)
![Alt text](../images/diagram.png)
```

**Implementation:**
1. When rendering an image tag, resolve the path relative to the current file's directory
2. Check if the file exists and is a supported format (PNG, JPG, GIF, SVG, BMP)
3. If using wxHtmlWindow: embed as `data:` URI (base64 encoded)
4. If using wxWebView: use `file://` protocol with the absolute path

**Security restrictions:**
- Only load images from the local filesystem
- Do not load remote URLs (`http://`, `https://`)
- Validate file extensions (no executables)
- Limit image size (max 10MB per image, max 50MB total per document)

**Missing image handling:**
- Show a placeholder: gray box with "Image not found: filename" text
- Style: border in `border_light`, text in `text_muted`

**Acceptance criteria:**
- Local images display correctly in the preview
- Relative paths are resolved correctly
- Missing images show placeholder
- Remote URLs are blocked
- Large images are handled (displayed at max-width: 100%)

### Task 18.5: Implement print/export functionality

Add the ability to export rendered content:

**Export to HTML:**
1. Generate the full HTML (same as preview) with embedded CSS
2. Save to a file chosen by the user
3. Inline all images as data URIs
4. Result: a self-contained HTML file

**Export to PDF (optional):**
1. Use wxWidgets print framework (`wxPrintout`)
2. Or use the wxWebView print capability
3. Generate a PDF with the rendered content

**Print:**
1. Use `wxPrinter` with a `wxPrintout` implementation
2. Render the markdown content for print (simplified styling, dark-on-white)

**Acceptance criteria:**
- HTML export produces a valid, self-contained HTML file
- Exported HTML opens correctly in a browser
- Print functionality works on at least one platform

### Task 18.6: Implement scroll synchronization with editor

When in split view, synchronize scrolling between editor and preview:

**Approach 1: Proportional scroll**
- Map editor scroll position to preview scroll position proportionally
- `preview_scroll_pct = editor_visible_line / editor_total_lines`
- Apply: `preview_scroll_pos = preview_scroll_pct * preview_total_height`

**Approach 2: Heading-based sync (better accuracy)**
- Find the heading closest to the editor cursor
- Scroll the preview to show that heading
- Requires tracking heading positions in both editor and preview

Start with Approach 1, upgrade to Approach 2 if needed.

**Acceptance criteria:**
- Scrolling the editor approximately scrolls the preview to the corresponding content
- Sync is not jittery (debounce scroll events)
- Sync can be toggled on/off via a preference

### Task 18.7: Visual comparison with reference

Systematically compare the C++ preview output with the React reference:

1. Render each sample file in both implementations
2. Compare visually:
   - Heading sizes and colors
   - Code block styling
   - Table rendering
   - List indentation and markers
   - Blockquote styling
   - Mermaid diagram rendering
   - Overall spacing and typography

3. Document any differences and create tasks to fix them

**Acceptance criteria:**
- Side-by-side comparison shows near-identical output
- Major visual differences are documented with fix plan
- No elements are missing from the C++ version

### Task 18.8: Write preview integration tests

Create `/Users/ryanrentfro/code/markamp/tests/integration/test_preview.cpp`:

Test cases:
1. Render empty content (no crash)
2. Render single heading
3. Render complex markdown with all element types
4. Theme change updates all CSS
5. Mermaid block renders or shows error
6. Code block with known language highlights correctly
7. Table renders with correct structure
8. Task list renders with checkboxes
9. Local image path resolution
10. Remote image URL blocking
11. Scroll position preservation on re-render
12. Debounced rendering (content change -> wait -> render)

**Acceptance criteria:**
- All tests pass
- Integration tests verify the full pipeline (parse -> render -> display)
- At least 15 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/PreviewPanel.h` | Modified (complete integration) |
| `src/ui/PreviewPanel.cpp` | Modified (complete integration) |
| `src/rendering/HtmlRenderer.h` | Modified (sub-system integration) |
| `src/rendering/HtmlRenderer.cpp` | Modified (sub-system integration) |
| `src/ui/LayoutManager.cpp` | Modified (bevel overlay on preview) |
| `src/CMakeLists.txt` | Modified |
| `tests/integration/test_preview.cpp` | Created |

## Dependencies

- All rendering sub-systems from Phases 14-17
- Phase 08 ThemeEngine
- Phase 05 EventBus
- wxWidgets (wxHtmlWindow or wxWebView, wxPrintout)

## Estimated Complexity

**High** -- Integrating multiple rendering sub-systems into a cohesive preview is complex. Ensuring visual fidelity across all themes and element types requires careful CSS tuning. Scroll synchronization and export functionality add additional scope.
