# Phase 15: GFM Extensions -- Tables, Task Lists, Footnotes

## Objective

Enhance the markdown rendering pipeline with full support for GitHub Flavored Markdown extensions: tables with alignment, interactive task lists, footnotes, strikethrough, and autolinks. These extensions must render correctly in the preview pane with theme-aware styling.

## Prerequisites

- Phase 14 (Markdown-to-Rendered-View Pipeline)

## Deliverables

1. Table rendering with column alignment, header styling, and theme-aware borders
2. Interactive task list checkboxes (display-only in preview, editable behavior optional)
3. Footnote support with reference links
4. Strikethrough text rendering
5. Autolink detection for URLs and email addresses

## Tasks

### Task 15.1: Implement full table rendering

Create `/Users/ryanrentfro/code/markamp/src/rendering/TableRenderer.h` and `TableRenderer.cpp`:

**Table structure (matching reference Preview.tsx lines 50-52):**
```html
<div class="table-wrapper">
  <table>
    <thead>
      <tr><th>Header 1</th><th>Header 2</th></tr>
    </thead>
    <tbody>
      <tr><td>Cell 1</td><td>Cell 2</td></tr>
    </tbody>
  </table>
</div>
```

**Styling (matching reference):**
- Wrapper: `overflow-x: auto; margin: 24px 0; border: 1px solid {border_light}; border-radius: 4px;`
- Table: `width: 100%; text-align: left; border-collapse: collapse;`
- th: `background: {bg_panel}; padding: 12px; border-bottom: 1px solid {border_light}; font-weight: 600; color: {accent_primary};`
- td: `padding: 12px; border-bottom: 1px solid {border_light} at 30% opacity;`

**Alignment support:**
- Left: `text-align: left;`
- Center: `text-align: center;`
- Right: `text-align: right;`

**Edge cases:**
- Tables with no header row
- Tables with varying column counts (fill with empty cells)
- Tables with inline formatting in cells (bold, italic, code, links)
- Very wide tables (horizontal scroll in wrapper)

**Acceptance criteria:**
- Tables render with correct alignment
- Header row is visually distinct (bg_panel background, accent color text)
- Borders use theme colors
- Horizontal scrolling works for wide tables
- Reference README.md table renders correctly

### Task 15.2: Implement task list rendering

**Task list markdown:**
```markdown
- [x] Completed task
- [ ] Incomplete task
```

**Rendering:**
```html
<ul class="task-list">
  <li class="task-item">
    <input type="checkbox" checked disabled> Completed task
  </li>
  <li class="task-item">
    <input type="checkbox" disabled> Incomplete task
  </li>
</ul>
```

**Styling:**
- Checkbox appearance: custom-drawn to match theme
  - Unchecked: border in `border_light`, transparent fill
  - Checked: fill in `accent_primary`, checkmark in white/bg_app
- List style: none (no bullet markers)
- Completed task text: `text_muted` color with `line-through`

**Interactive behavior (optional enhancement):**
- Clicking a checkbox in the preview could update the source markdown
- This requires: finding the corresponding line in the source, toggling `[ ]` to `[x]` or vice versa
- Implementation: parse checkbox position, emit event to editor

For initial implementation, checkboxes are display-only (disabled).

**Acceptance criteria:**
- Task lists render with checkboxes
- Checked items show checkmark and strikethrough text
- Styling matches theme
- Reference TODO.md renders correctly

### Task 15.3: Implement footnote support

**Footnote markdown:**
```markdown
Here is a sentence with a footnote[^1].

[^1]: This is the footnote content.
```

**Rendering:**
```html
<p>Here is a sentence with a footnote<sup><a href="#fn-1">[1]</a></sup>.</p>
...
<section class="footnotes">
  <hr>
  <ol>
    <li id="fn-1">This is the footnote content. <a href="#fnref-1">↩</a></li>
  </ol>
</section>
```

Note: md4c does not natively support footnotes. Options:
1. Pre-process markdown to extract footnotes before passing to md4c
2. Use a post-processing step on the AST
3. Implement a simple regex-based footnote parser

**Recommended: Pre-process approach:**
1. Scan for `[^N]:` definition lines and extract them
2. Replace `[^N]` references with superscript HTML
3. Append a footnotes section at the end

**Styling:**
- Footnote reference: superscript, accent_primary color, clickable
- Footnotes section: separated by hr, smaller text, text_muted color
- Back-reference arrow: accent_secondary color

**Acceptance criteria:**
- Footnote references are rendered as superscript links
- Footnote definitions are collected at the bottom
- Clicking a reference scrolls to the footnote
- Back-reference returns to the reference point

### Task 15.4: Implement strikethrough rendering

**Markdown:** `~~strikethrough text~~`

**Rendering:** `<del>strikethrough text</del>`

**Styling:**
- `text-decoration: line-through;`
- Color: `text_muted` (dimmed compared to normal text)

md4c supports this with the `MD_FLAG_STRIKETHROUGH` flag (already enabled in Phase 13).

**Acceptance criteria:**
- Strikethrough text renders with a line through it
- Color is dimmed to match theme

### Task 15.5: Implement autolink rendering

**Autolinks:** Plain URLs and email addresses that should be automatically linked.

md4c supports this with `MD_FLAG_PERMISSIVEAUTOLINKS` (already enabled in Phase 13).

**Rendering:**
- URLs: `<a href="https://example.com">https://example.com</a>`
- Emails: `<a href="mailto:user@example.com">user@example.com</a>`

**Styling:**
- Same as regular links: accent_primary color, underline on hover

**Acceptance criteria:**
- Plain URLs in text become clickable links
- Email addresses become mailto links
- Existing explicit links are not double-wrapped

### Task 15.6: Enhance CSS for all GFM extensions

Update the CSS generation in `PreviewPanel::GenerateCSS()` to include styles for:

1. Table styles (from Task 15.1)
2. Task list styles (from Task 15.2)
3. Footnote styles (from Task 15.3)
4. Strikethrough styles (from Task 15.4)
5. Autolink styles (same as link styles)

All new CSS rules must use theme colors and update when the theme changes.

**Acceptance criteria:**
- All GFM extension styles are included in the CSS
- No hardcoded colors in the CSS
- CSS regeneration on theme change includes all extensions

### Task 15.7: Create GFM test suite

Create `/Users/ryanrentfro/code/markamp/tests/unit/test_gfm_extensions.cpp`:

Test cases:
1. Simple table with headers
2. Table with column alignment (left, center, right)
3. Table with inline formatting in cells
4. Table with varying column counts
5. Task list with checked and unchecked items
6. Task list mixed with regular list items
7. Nested task lists
8. Footnote reference and definition
9. Multiple footnotes
10. Footnote with multi-line content
11. Strikethrough text
12. Strikethrough with other inline formatting
13. Autolinked URL
14. Autolinked email
15. Complex document with all GFM features combined

**Test markdown document combining all features:**
```markdown
# GFM Test

| Feature | Status | Priority |
|:--------|:------:|--------:|
| Tables | **Done** | High |
| Tasks | ~~Pending~~ Done | Medium |

## Tasks

- [x] Implement tables
- [ ] Implement task lists
- [ ] Add footnotes[^1]

This has ~~old text~~ new text.

Visit https://example.com or email test@example.com.

[^1]: Footnotes are a GFM extension.
```

**Acceptance criteria:**
- All 15+ test cases pass
- Combined document renders all features correctly
- No rendering conflicts between features

## Files Created/Modified

| File | Action |
|---|---|
| `src/rendering/TableRenderer.h` | Created |
| `src/rendering/TableRenderer.cpp` | Created |
| `src/rendering/HtmlRenderer.h` | Modified (add GFM support) |
| `src/rendering/HtmlRenderer.cpp` | Modified (add GFM support) |
| `src/ui/PreviewPanel.cpp` | Modified (enhanced CSS) |
| `src/CMakeLists.txt` | Modified |
| `tests/unit/test_gfm_extensions.cpp` | Created |

## Dependencies

- md4c with GFM flags
- Phase 14 rendering pipeline
- Phase 08 theme engine

## Estimated Complexity

**High** -- Table rendering with proper alignment and overflow handling is non-trivial. Footnote support requires pre/post-processing since md4c does not handle them natively. Interactive task lists (if implemented) require bidirectional editor-preview communication.
