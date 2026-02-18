# Phase 45: Live Preview and WYSIWYG

## Overview
LivePreviewRenderer exists in UI. PreviewPanel provides Markdown preview. EditorPanel uses Scintilla for text editing. However, there is no WYSIWYG or live preview mode where users see formatted content while typing. This phase builds an integrated live preview experience bridging raw Markdown and rendered output.

## Prerequisites
- Phase 07 (Editor core improvements)
- Phase 08 (Markdown rendering completeness)
- Phase 09 (Split view and editor-preview sync)

## Tasks

### Task 1: Wire LivePreviewRenderer for Inline Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/LivePreviewRenderer.h`
**Description:** LivePreviewRenderer exists. Wire it to render Markdown inline in the editor: headings rendered large, bold text rendered bold, code blocks rendered with background, images rendered inline.
**Acceptance Criteria:**
- Headings: rendered at appropriate size (H1 largest)
- Bold/italic: rendered with formatting (not showing `**` markers)
- Code blocks: rendered with background color and syntax highlighting
- Images: rendered inline at reasonable size
- Links: rendered as clickable with underline
- Raw Markdown visible on cursor line (for editing)

### Task 2: Wire Source/Preview Mode Toggle
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/MainFrame.cpp`
**Description:** Three editing modes: Source (raw Markdown), Live Preview (inline rendering), and Split (side-by-side). Toggle via toolbar and keyboard shortcut.
**Acceptance Criteria:**
- Source mode: raw Markdown with syntax highlighting
- Live Preview: inline rendered content
- Split mode: source left, preview right
- Toggle: Cmd+E cycles modes
- Mode indicator in status bar
- Mode persisted per file

### Task 3: Wire Cursor-Aware Raw Mode
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** In live preview mode, the line containing the cursor shows raw Markdown for editing. Adjacent lines show rendered content.
**Acceptance Criteria:**
- Cursor line: raw Markdown with syntax highlighting
- Adjacent lines: rendered content
- Transition: smooth when cursor moves between lines
- Multi-line elements (code blocks): raw when cursor inside
- Block elements: raw when cursor on any line of block
- Performance: mode switch < 16ms

### Task 4: Wire Inline Image Preview
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Images in live preview mode render inline: `![alt](path)` shows the actual image below the line.
**Acceptance Criteria:**
- Image rendered below the Markdown line
- Image scaled to fit editor width (max 80%)
- Click image: open full-size preview
- Image hover: show dimensions and file size
- Broken image: show placeholder with alt text
- Images lazy-loaded for performance

### Task 5: Wire Inline Code Block Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`
**Description:** Code blocks in live preview mode render with: background color, syntax highlighting, language label, copy button.
**Acceptance Criteria:**
- Code block background color from theme
- Syntax highlighting per language
- Language label in top-right corner
- Copy button on hover
- Line numbers toggle
- Code block collapsible

### Task 6: Wire Inline Mermaid Diagram Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/core/IMermaidRenderer.h`
**Description:** Mermaid code blocks render as diagrams inline. Diagram updates on content change (debounced).
**Acceptance Criteria:**
- Mermaid code blocks render as SVG inline
- Live update on typing (debounced 500ms)
- Error: show error message instead of diagram
- Click diagram: zoom to full size
- Cursor in mermaid block: show raw code
- Diagram theme matches editor theme

### Task 7: Wire Inline Math Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/core/IMathRenderer.h`
**Description:** LaTeX math expressions render inline: `$inline$` and `$$block$$` math rendered as formatted output.
**Acceptance Criteria:**
- Inline math: rendered in text flow
- Block math: rendered centered with spacing
- Cursor on math: show raw LaTeX
- Error: show raw with error indicator
- Math rendering from IMathRenderer
- Math font from theme

### Task 8: Wire Inline Table Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/TableEditorOverlay.cpp`
**Description:** Markdown tables in live preview render as formatted tables with borders, alignment, and cell editing.
**Acceptance Criteria:**
- Tables rendered with borders and alignment
- Cell editing on click
- Tab navigation between cells
- Column resize by dragging border
- Sort by column header click
- Cursor in table: show formatted (not raw)

### Task 9: Wire Inline Callout Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/rendering/CalloutBlockRenderer.cpp`
**Description:** Callout blocks (blockquotes with type) render with icons and colored backgrounds in live preview.
**Acceptance Criteria:**
- `> [!note]` renders with note icon and blue background
- `> [!warning]` renders with warning icon and yellow background
- `> [!tip]` renders with tip icon and green background
- `> [!danger]` renders with danger icon and red background
- Callout title editable inline
- Callout collapsible

### Task 10: Wire Inline Embed Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/core/EmbedResolver.cpp`
**Description:** Embed syntax `![[document]]` renders the embedded document content inline in live preview mode.
**Acceptance Criteria:**
- `![[document]]` renders embedded content inline
- Embedded content: indented with visual container
- Click container: navigate to source document
- Nested embeds: rendered with depth limit (3 levels)
- Recursive embed: detected and shown as error
- Embed updates when source changes

### Task 11: Wire Inline Checkbox Interaction
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Task checkboxes in live preview are interactive: click to toggle between checked and unchecked.
**Acceptance Criteria:**
- `- [ ]` renders as unchecked checkbox
- `- [x]` renders as checked checkbox
- Click toggles state in source
- Checked items: optional strikethrough
- Checkbox size: matches text size
- Undo support for toggle

### Task 12: Wire Inline Footnote Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`
**Description:** Footnotes render inline in live preview: footnote marker is superscript link, footnote content shown at bottom or on hover.
**Acceptance Criteria:**
- `[^1]` renders as superscript number
- Hover footnote marker: show footnote content in popup
- Click marker: scroll to footnote definition
- Footnote definitions: rendered in footer section
- Back-reference from footnote to marker
- Multiple references to same footnote handled

### Task 13: Wire Live Preview Performance
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/rendering/ViewportCache.h`
**Description:** Live preview must maintain 60fps during typing. Use viewport-based rendering: only render visible lines, cache rendered content.
**Acceptance Criteria:**
- Typing latency: < 16ms in live preview mode
- Only visible lines rendered
- Cached content reused for unchanged lines
- Image rendering asynchronous
- Mermaid rendering debounced
- Large files (10,000 lines): no degradation

### Task 14: Wire Live Preview Theme Integration
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Live preview uses theme tokens for all rendered elements: heading colors, code background, link color, table borders.
**Acceptance Criteria:**
- Heading color from `heading_color` token
- Code background from `code_bg` token
- Link color from `link_color` token
- Table borders from `border_color` token
- Block element padding from theme
- Theme change updates live preview immediately

### Task 15: Wire Inline Tag Rendering
**Files:** `src/ui/LivePreviewRenderer.cpp`
**Description:** Tags (#tag) in live preview render as colored badges. Click tag shows all documents with that tag.
**Acceptance Criteria:**
- `#tag` renders as colored badge/pill
- Badge color from tag color or theme accent
- Click tag: open tag browser/filter
- Hover: show document count with this tag
- Tag autocomplete still works in raw mode
- Multiple tags render inline

### Task 16: Wire Live Preview Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register live preview commands: "View: Toggle Live Preview", "View: Source Mode", "View: Split Mode", "View: Reading Mode".
**Acceptance Criteria:**
- All commands registered in command palette
- Cmd+E: cycle through modes
- "Reading Mode": full preview, no editing
- Mode commands show current mode state
- Mode transitions animated (configurable)

### Task 17: Wire Reading Mode
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Reading mode: full rendered view with no editing capability. Optimized for reading with larger text, comfortable line width, and navigation.
**Acceptance Criteria:**
- Full rendered content (no raw Markdown anywhere)
- Non-editable (cursor changes to pointer)
- Comfortable reading width (max 700px)
- Larger text option (1.2x default)
- Navigation: scroll, heading links, footnotes
- Exit: Cmd+E or Escape returns to edit mode

### Task 18: Wire Live Preview Accessibility
**Files:** `src/ui/LivePreviewRenderer.cpp`, `src/core/AccessibilityManager.cpp`
**Description:** Live preview accessible: rendered elements have appropriate ARIA roles, images have alt text, headings navigable.
**Acceptance Criteria:**
- Headings: navigable (Alt+H for next heading)
- Images: alt text announced by screen reader
- Code blocks: announced as "code block, language X"
- Links: announced with link text and URL
- Mode change: announced to screen reader
- Focus management: consistent across modes

### Task 19: Wire Live Preview Configuration
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Live preview settings: default mode, rendering options, image rendering, math rendering, embed depth.
**Acceptance Criteria:**
- Default mode: source, live preview, or split
- Image rendering: inline, collapsed, hidden
- Math rendering: enabled/disabled
- Mermaid rendering: enabled/disabled
- Embed depth: 1, 2, 3 levels
- Reading mode text scale: 1.0-1.5x

### Task 20: Add Live Preview Tests
**Files:** `tests/unit/test_live_preview.cpp`
**Description:** Test live preview: inline rendering for all element types, mode switching, cursor-aware raw mode, performance.
**Acceptance Criteria:**
- Heading rendering: all levels
- Code block rendering: background and highlighting
- Image rendering: inline display
- Cursor-aware: raw mode on active line
- Mode switching: source, live, split transitions
- Performance: typing latency benchmark

## Testing Requirements
- Inline rendering for all Markdown elements
- Cursor-aware raw/rendered mode switching
- Performance: 60fps during typing
- Theme integration for all elements

## Phase Completion Criteria
- Live preview mode with inline rendering
- Source, Live Preview, and Split modes
- Cursor-aware raw editing in live preview
- Inline images, code blocks, Mermaid, math
- Interactive checkboxes and tables
- Reading mode for comfortable viewing
- All tests pass
