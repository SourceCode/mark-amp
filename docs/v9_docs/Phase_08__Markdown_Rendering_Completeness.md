# Phase 08: Markdown Rendering Completeness

## Overview
HtmlRenderer (900 lines) handles core Markdown-to-HTML conversion with CodeBlockRenderer, MermaidBlockRenderer, CalloutBlockRenderer, and various media block renderers. However, preview interactivity is limited, scroll sync has only proportional mode fully working, and source-mapping for click-to-source is not implemented. This phase completes the rendering pipeline.

## Prerequisites
- Phase 03 (Theme tokens for preview styling)
- Phase 07 (Editor improvements for bidirectional navigation)

## Tasks

### Task 1: Implement Source-Location Attributes in Rendered HTML
**Files:** `src/rendering/HtmlRenderer.cpp`, `src/rendering/HtmlRenderer.h`
**Description:** Add `data-source-line` attributes to rendered HTML blocks (headings, paragraphs, code blocks, lists, tables) mapping each block back to its source line number. This enables click-to-source navigation.
**Acceptance Criteria:**
- Every rendered block has `data-source-line` attribute
- Source lines are accurate within 1 line for all Markdown elements
- Incremental re-renders preserve source mapping
- Test: render Markdown, verify source lines for each block type

### Task 2: Implement Click-to-Source Navigation
**Files:** `src/ui/PreviewPanel.cpp`, `src/core/Events.h`
**Description:** When the user clicks a rendered block in the preview, emit `JumpToSourceFromPreviewEvent` with the source line. The editor scrolls to and highlights that line.
**Acceptance Criteria:**
- Clicking any rendered block in preview jumps to source line
- Source line highlighted briefly (yellow flash, 500ms)
- Works for headings, paragraphs, code blocks, tables, lists
- Ctrl+Click (Cmd+Click on macOS) for consistent behavior

### Task 3: Implement Heading-Anchored Scroll Sync
**Files:** `src/ui/PreviewPanel.cpp`, `src/ui/SplitView.cpp`
**Description:** `ScrollSyncMode::HeadingAnchor` is declared but not fully implemented. Implement heading-based scroll sync: when the editor cursor is in a section, the preview scrolls to show that heading's content.
**Acceptance Criteria:**
- Heading anchor mode tracks the nearest heading above cursor
- Preview scrolls to matching heading smoothly
- Works with nested headings (H1>H2>H3)
- Mode selectable via status bar dropdown

### Task 4: Implement Cursor-Anchored Scroll Sync
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** `ScrollSyncMode::CursorAnchored` is declared with a "V8 Phase 9" comment but not implemented. Sync the preview to the cursor line: the rendered element at the cursor line is kept visible.
**Acceptance Criteria:**
- Preview tracks cursor line in real-time
- Element at cursor line is centered vertically when possible
- Smooth scroll (not jump) when cursor moves
- Works during continuous typing

### Task 5: Add Interactive Code Block Controls
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/CodeBlockRenderer.cpp`
**Description:** Rendered code blocks should have hover controls: copy-to-clipboard button, language label, and optional "Edit in Source" button that jumps to the code block in the editor.
**Acceptance Criteria:**
- Copy button appears on code block hover
- Copy button copies code content to clipboard
- Language label shown in top-right corner
- "Edit" button jumps to source line of code block

### Task 6: Add Collapsible Sections in Preview
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Long sections under headings should be collapsible. Add expand/collapse controls to rendered headings. Collapse state persisted for the session.
**Acceptance Criteria:**
- Headings have collapse/expand chevron
- Clicking chevron toggles section visibility
- Collapse state maintained during live preview updates
- H1 sections collapse, H2 sections within collapse independently

### Task 7: Add Sortable Tables in Preview
**Files:** `src/ui/PreviewPanel.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Rendered Markdown tables should support click-to-sort by column header. First click sorts ascending, second descending, third unsorts.
**Acceptance Criteria:**
- Table headers show sort indicator on hover
- Click sorts ascending (up arrow)
- Second click sorts descending (down arrow)
- Third click resets to document order
- Sort is visual only (does not modify source)

### Task 8: Add Collapsible Callouts
**Files:** `src/rendering/CalloutBlockRenderer.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** Callout blocks (info, warning, tip, etc.) should support collapse. Callouts with `[!TIP]- title` syntax render collapsed by default.
**Acceptance Criteria:**
- Callouts with `-` suffix in type render collapsed by default
- Click to expand/collapse
- Callout type styling preserved (color, icon)
- Works with nested callouts

### Task 9: Add Navigable Footnotes
**Files:** `src/rendering/HtmlRenderer.cpp`, `src/ui/PreviewPanel.cpp`
**Description:** Footnote references in rendered content should be clickable links that scroll to the footnote definition, and footnote definitions should have back-links to their references.
**Acceptance Criteria:**
- Footnote references are superscript clickable links
- Clicking scrolls to footnote definition at bottom
- Footnote definition has back-link to reference
- Smooth scroll animation

### Task 10: Improve Mermaid Rendering UX
**Files:** `src/rendering/MermaidBlockRenderer.cpp`, `src/core/MermaidRenderer.h`
**Description:** Mermaid rendering needs: consistent theming with the active MarkAmp theme, better error messages showing the problematic line, and a retry button on render failure.
**Acceptance Criteria:**
- Mermaid diagrams use colors derived from active theme
- Error state shows the problematic line highlighted
- Retry button re-renders the diagram
- Loading state shows a spinner/placeholder during render

### Task 11: Improve Math/LaTeX Rendering
**Files:** `src/core/MathRenderer.cpp`, `src/rendering/HtmlRenderer.cpp`
**Description:** Math rendering should support: inline math ($...$), display math ($$...$$), error messages for invalid LaTeX, and fallback to source display on render failure.
**Acceptance Criteria:**
- Inline math renders inline with text
- Display math renders centered on its own line
- Invalid LaTeX shows error message with the source
- Fallback: source displayed in monospace with red border

### Task 12: Add Incremental Preview Rendering
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** Currently the entire preview re-renders on every edit. Implement incremental rendering: small edits (within a single block) update only the affected block, large edits trigger full re-render.
**Acceptance Criteria:**
- Single-block edits update only that block (< 5ms)
- Multi-block edits update affected blocks only
- Full re-render only on structural changes (heading added/removed)
- No visual flicker during incremental updates

### Task 13: Add Progressive Preview Loading
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** For large documents, first paint with structural placeholders (headings, text blocks), then hydrate heavy blocks (code, Mermaid, math, images) asynchronously.
**Acceptance Criteria:**
- First paint shows text content immediately
- Code blocks show language label placeholder until highlighted
- Mermaid diagrams show loading spinner until rendered
- Full hydration completes within 500ms for typical documents

### Task 14: Add Preview Heading Navigation Overlay
**Files:** `src/ui/PreviewPanel.cpp`, `src/ui/BreadcrumbBar.cpp`
**Description:** Show the current heading context in the preview panel (similar to editor breadcrumbs). As the user scrolls, the heading breadcrumb updates to show the document position.
**Acceptance Criteria:**
- Breadcrumb bar at top of preview shows current heading path
- Updates on scroll
- Clicking a breadcrumb segment scrolls to that heading
- Matches editor breadcrumb style

### Task 15: Add Preview Scroll-to-Top Button
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** When scrolled down in a long document, show a floating "scroll to top" button in the bottom-right corner of the preview panel.
**Acceptance Criteria:**
- Button appears when scrolled past first screen
- Click scrolls to top with smooth animation
- Button fades in/out
- Theme-aware styling

### Task 16: Add Preview Print-Friendly CSS
**Files:** `src/ui/PreviewPanel.cpp`
**Description:** Add a print-friendly CSS mode that removes interactive elements, adjusts margins, and ensures proper page breaks. Accessible via "Print Document" command.
**Acceptance Criteria:**
- Print mode removes hover controls, collapse buttons
- Page margins appropriate for printing
- Code blocks have visible borders
- Page breaks avoid splitting headings from content

### Task 17: Add Preview Reading Profiles
**Files:** `src/ui/PreviewPanel.cpp`, `src/core/Config.h`
**Description:** Define preview reading profiles: Documentation (standard), Presentation (larger font, more spacing), and Print-Ready (serif font, proper margins).
**Acceptance Criteria:**
- Three profiles with distinct typography settings
- Profile selectable via command palette and settings
- Profile persisted per workspace
- Smooth transition when switching

### Task 18: Add Embed and Transclusion Rendering
**Files:** `src/rendering/HtmlRenderer.cpp`, `src/core/EmbedResolver.cpp`
**Description:** EmbedResolver exists (core) but rendering of embedded/transcluded content in preview is not complete. Implement: `![[file]]` renders embedded file content, `![[file#heading]]` renders specific section.
**Acceptance Criteria:**
- `![[file]]` embeds the full content of another file
- `![[file#heading]]` embeds only the heading section
- Embedded content styled with subtle border/background
- Circular embeds detected and shown as error

### Task 19: Add Preview Performance Tests
**Files:** `tests/unit/test_preview_panel.cpp`, `tests/unit/test_html_renderer.cpp`
**Description:** Define and test performance budgets: initial render < 100ms for 500-line document, incremental update < 10ms for single-block change, scroll sync latency < 16ms.
**Acceptance Criteria:**
- Initial render: < 100ms for 500-line Markdown
- Incremental update: < 10ms for single-block edit
- Scroll sync: < 16ms latency
- Mermaid render: < 500ms per diagram

### Task 20: Add Preview Stability Tests
**Files:** `tests/unit/test_crash_regressions.cpp`, `tests/unit/test_live_preview.cpp`
**Description:** Add stability tests: rapid editing (1000 keystrokes in 5 seconds), rapid theme switching while preview is updating, large document (10k+ lines) scrolling.
**Acceptance Criteria:**
- No crash under rapid editing
- No crash under rapid theme switching
- No out-of-memory on large documents
- Preview remains responsive during continuous typing

## Testing Requirements
- Source mapping accuracy for all Markdown element types
- Scroll sync: all three modes tested
- Interactive elements: copy, collapse, sort, navigate
- Performance budgets met
- Stability under stress

## Phase Completion Criteria
- Preview has source mapping with click-to-source
- Three scroll sync modes working correctly
- Interactive code blocks, collapsible sections, sortable tables
- Incremental and progressive rendering for performance
- Mermaid and math rendering with proper error handling
- All tests pass
