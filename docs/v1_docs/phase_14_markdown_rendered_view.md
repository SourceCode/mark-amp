# Phase 14: Markdown-to-Rendered-View Pipeline

## Objective

Create the rendering pipeline that takes parsed markdown and displays it as a styled, interactive rendered view within a wxWidgets panel. This is the "preview pane" -- the core visual output of MarkAmp. It must render markdown with theme-aware styling matching the reference Preview.tsx component.

## Prerequisites

- Phase 13 (Markdown Parsing Engine Integration)
- Phase 08 (Theme Engine and Runtime Switching)

## Deliverables

1. MarkdownRenderer that converts MarkdownDocument AST to styled wxWidgets rendering
2. Theme-aware HTML/rich text display using wxHtmlWindow or custom rendering
3. Styled headings, paragraphs, lists, blockquotes, tables, links, images
4. Custom CSS generation from theme colors
5. Smooth scrolling and resize handling
6. Async rendering pipeline (non-blocking for large documents)

## Tasks

### Task 14.1: Choose and implement the rendering backend

Evaluate rendering approaches for displaying rich markdown:

**Option A: wxHtmlWindow**
- Pros: Built into wxWidgets, supports basic HTML/CSS, links, images
- Cons: Limited CSS support, no custom scrollbar, limited table styling
- Verdict: Good starting point, may need enhancement

**Option B: wxWebView (embedded browser)**
- Pros: Full HTML/CSS/JS support, perfect rendering
- Cons: Heavy dependency (WebKit/Chromium), security concerns, harder to theme
- Verdict: Best rendering quality but adds significant complexity

**Option C: Custom wxPanel with direct drawing**
- Pros: Full control over rendering, perfect theme integration, best performance
- Cons: Must implement all layout/rendering from scratch, significant effort
- Verdict: Maximum control but enormous implementation scope

**Recommended: Use wxWebView (Option B)** as the rendering backend. The UI must look EXACTLY like the React reference implementation, which requires full CSS support including flexbox, gradients, rgba colors, custom fonts, and precise spacing. wxHtmlWindow's limited CSS support cannot achieve this. wxWebView provides a real browser engine (WebKit on macOS, Edge WebView2 on Windows, WebKitGTK on Linux) that can render the identical CSS used in the React reference.

**CRITICAL**: See `00_visual_fidelity_reference.md` for the exact pixel-level specifications that must be matched. wxHtmlWindow CANNOT achieve this fidelity.

Create `/Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.h` and `PreviewPanel.cpp`:

```cpp
namespace markamp::ui {

class PreviewPanel : public ThemeAwareWindow
{
public:
    PreviewPanel(wxWindow* parent, core::ThemeEngine& theme_engine, core::EventBus& event_bus);

    // Content
    void SetMarkdownContent(const std::string& markdown);
    void Clear();

    // Scrolling
    void ScrollToTop();
    void ScrollToPosition(int y);
    [[nodiscard]] auto GetScrollPosition() const -> int;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    wxHtmlWindow* html_view_{nullptr};
    core::EventBus& event_bus_;
    core::MarkdownParser* parser_{nullptr};

    // Rendering pipeline
    void RenderContent(const std::string& markdown);
    auto GenerateCSS() const -> std::string;
    auto GenerateFullHtml(const std::string& body_html) const -> std::string;

    // Event handling
    void OnLinkClicked(wxHtmlLinkEvent& event);
    void OnSize(wxSizeEvent& event);

    // Debouncing
    wxTimer render_timer_;
    std::string pending_content_;
    void OnRenderTimer(wxTimerEvent& event);

    core::Subscription content_changed_sub_;
};

} // namespace markamp::ui
```

**Acceptance criteria:**
- Preview panel displays rendered markdown
- HTML is generated from the parsed AST
- Basic elements (headings, paragraphs, lists) render correctly

### Task 14.2: Generate theme-aware CSS

Create the CSS that styles the rendered HTML to match the theme:

```cpp
auto PreviewPanel::GenerateCSS() const -> std::string
{
    const auto& t = theme();
    return std::format(R"(
        body {{
            background-color: {};
            color: {};
            font-family: 'Rajdhani', sans-serif;
            font-size: 14px;
            line-height: 1.6;
            padding: 24px;
            max-width: none;
        }}
        h1 {{
            font-size: 28px;
            font-weight: bold;
            color: {};
            border-bottom: 1px solid {};
            padding-bottom: 8px;
            margin-bottom: 24px;
        }}
        h2 {{
            font-size: 22px;
            font-weight: 600;
            color: {};
            margin-top: 32px;
            margin-bottom: 16px;
        }}
        h3 {{
            font-size: 18px;
            font-weight: 500;
            color: {};
            margin-top: 24px;
            margin-bottom: 12px;
        }}
        // ... (all elements styled per reference Preview.tsx)
    )",
        t.colors.bg_app.to_hex(),
        t.colors.text_main.to_hex(),
        t.colors.accent_primary.to_hex(),
        t.colors.border_light.to_hex(),
        t.colors.accent_primary.to_hex(),
        t.colors.text_main.to_hex()
    );
}
```

**CSS rules matching reference Preview.tsx (lines 42-53):**
- h1: 3xl (28px), bold, accent_primary, border-bottom border_light, pb-2, mb-6
- h2: 2xl (22px), semibold, accent_primary, mt-8, mb-4
- h3: xl (18px), medium, text_main, mt-6, mb-3
- p: mb-4, leading-relaxed, text_main at 90% opacity
- a: accent_primary, underline on hover
- blockquote: border-left 4px accent_primary, pl-4, italic, accent_primary bg at 5%
- ul: disc markers in accent_primary
- ol: decimal markers in accent_primary
- table: border border_light, rounded
- th: bg_panel background, p-3, border-bottom, semibold, accent_primary text
- td: p-3, border-bottom at 30%
- hr: border_light color, my-8
- code (inline): accent_primary bg at 20%, accent_primary text, px-1.5, rounded
- code (block): black/30% bg, p-4, rounded, border border_light

**Acceptance criteria:**
- CSS matches the reference styling for all elements
- Theme changes regenerate the CSS
- All text is readable against the background

### Task 14.3: Implement the full HTML generation pipeline

The complete pipeline: markdown string -> md4c parse -> AST -> HTML render -> CSS injection -> display.

```cpp
void PreviewPanel::RenderContent(const std::string& markdown)
{
    // 1. Parse markdown to AST
    auto doc_result = parser_->parse(markdown);
    if (!doc_result) {
        // Show error in preview
        DisplayError(doc_result.error());
        return;
    }

    // 2. Render AST to HTML body
    rendering::HtmlRenderer renderer;
    auto body_html = renderer.render(*doc_result);

    // 3. Generate full HTML with CSS
    auto full_html = GenerateFullHtml(body_html);

    // 4. Display in wxHtmlWindow
    html_view_->SetPage(full_html);
}

auto PreviewPanel::GenerateFullHtml(const std::string& body_html) const -> std::string
{
    return std::format(R"(
        <!DOCTYPE html>
        <html>
        <head>
            <meta charset="utf-8">
            <style>{}</style>
        </head>
        <body>
            {}
        </body>
        </html>
    )", GenerateCSS(), body_html);
}
```

**Acceptance criteria:**
- Complete pipeline produces rendered output
- All markdown elements are rendered
- Mermaid blocks are rendered as placeholders (full rendering in Phase 17)
- Invalid markdown shows an error message instead of crashing

### Task 14.4: Implement debounced rendering

Render updates should be debounced to avoid excessive re-rendering during typing:

1. When new content arrives (via EditorContentChangedEvent):
   - Store the content as `pending_content_`
   - Start/reset a 300ms timer
2. When the timer fires:
   - Render `pending_content_`
   - Clear the timer
3. Immediate render on:
   - Initial content load
   - View mode switch
   - Theme change

**Acceptance criteria:**
- Rapid typing does not cause visible lag
- Preview updates within 300ms of typing pause
- No rendering artifacts or partial updates

### Task 14.5: Implement link handling

When a link is clicked in the preview:

**External links (http/https):**
- Open in the system default browser using `wxLaunchDefaultBrowser()`
- Do NOT navigate within the preview pane

**Internal links (relative paths, anchors):**
- Anchor links (`#heading`): scroll to the heading in the preview
- Relative file links (`./other.md`): open the file in the editor

**Image handling:**
- Load local images relative to the current file's directory
- Show a placeholder for missing images
- External image URLs: show placeholder (no remote loading for security)

**Acceptance criteria:**
- Clicking external links opens the system browser
- Internal anchor links scroll the preview
- Relative file links open the referenced file
- No remote content is loaded (security)

### Task 14.6: Implement scroll position management

Manage scroll position for a good editing experience:

1. **Scroll sync (editor to preview):**
   - When the editor cursor moves, scroll the preview proportionally
   - Calculate relative position: `preview_scroll = (editor_cursor_line / editor_total_lines) * preview_scroll_max`
2. **Scroll preservation:**
   - When re-rendering, save scroll position, render, restore scroll position
   - Prevents the preview from jumping to top on every update
3. **Scroll-to-top:**
   - When opening a new file, scroll to top

**Acceptance criteria:**
- Preview scroll roughly follows editor cursor in split mode
- Re-rendering does not reset scroll position
- New file opens at top of document

### Task 14.7: Handle rendering errors gracefully

When rendering fails:

1. Show an error overlay in the preview pane:
   - Light red/pink background
   - Error message in monospace
   - "Parsing error at line X" if position is known
2. Keep the last successful render visible (with error overlay on top)
3. Log the error for debugging

**Acceptance criteria:**
- Errors are displayed to the user
- Application does not crash on malformed markdown
- Previous render remains visible with error overlay

### Task 14.8: Write rendering pipeline tests

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_markdown_renderer.cpp`:

Test cases:
1. Empty markdown produces empty body
2. Single paragraph renders correctly
3. Heading levels 1-6 render with correct tags
4. Bold and italic render correctly
5. Links render with href and title
6. Code blocks render with language class
7. Tables render with correct structure
8. Lists render with correct nesting
9. Mermaid blocks render as placeholder divs
10. CSS generation includes all theme colors
11. Full HTML document structure is valid
12. Theme change regenerates CSS
13. Reference sample files render without errors

**Acceptance criteria:**
- All tests pass
- HTML output is validated for well-formedness
- At least 20 test assertions

## Files Created/Modified

| File | Action |
|---|---|
| `src/ui/PreviewPanel.h` | Created |
| `src/ui/PreviewPanel.cpp` | Created |
| `src/rendering/HtmlRenderer.h` | Modified (enhanced) |
| `src/rendering/HtmlRenderer.cpp` | Modified (enhanced) |
| `src/ui/LayoutManager.cpp` | Modified (add preview panel) |
| `src/CMakeLists.txt` | Modified (add html component to wxWidgets) |
| `tests/unit/test_markdown_renderer.cpp` | Created |

## Dependencies

- wxWidgets 3.2.9 (wxHtmlWindow or wxWebView)
- Phase 13 MarkdownParser, MarkdownDocument
- Phase 08 ThemeEngine
- Phase 05 EventBus

## Estimated Complexity

**High** -- Using wxWebView as the rendering backend provides full CSS fidelity matching the React reference, but requires platform-specific setup (WebKit on macOS, Edge WebView2 on Windows, WebKitGTK on Linux). The debounced rendering pipeline, scroll management, and C++-to-JS bridge for Mermaid rendering add additional complexity. Security is critical since wxWebView embeds a browser engine -- all content must be locally generated, no remote loading.
