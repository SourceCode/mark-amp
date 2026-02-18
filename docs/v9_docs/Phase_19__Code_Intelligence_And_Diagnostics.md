# Phase 19: Code Intelligence and Diagnostics

## Overview
DiagnosticsService, TextEditorService, AsyncHighlighter, SyntaxHighlighter, and SnippetEngine exist in core. ProblemsPanel exists in UI. Code intelligence infrastructure (completion, hover, diagnostics) is partially built but not wired to the editor experience. This phase completes the developer tooling layer.

## Prerequisites
- Phase 07 (Editor core improvements)
- Phase 18 (Tool window system for Problems panel)

## Tasks

### Task 1: Wire DiagnosticsService to Editor Markers
**Files:** `src/core/DiagnosticsService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** DiagnosticsService exists. Wire it to EditorPanel so diagnostics appear as Scintilla margin markers (error/warning/info icons) and inline underlines.
**Acceptance Criteria:**
- Error: red squiggly underline + red margin icon
- Warning: yellow squiggly underline + yellow margin icon
- Info: blue underline + blue margin icon
- Hover over marker shows diagnostic message in tooltip
- Markers update on file save
- `DiagnosticsUpdatedEvent` emitted

### Task 2: Wire DiagnosticsService to ProblemsPanel
**Files:** `src/core/DiagnosticsService.cpp`, `src/ui/ProblemsPanel.cpp`
**Description:** All diagnostics from DiagnosticsService flow to ProblemsPanel. Panel shows aggregated view across all open files.
**Acceptance Criteria:**
- ProblemsPanel receives all diagnostics
- Grouped by file, sorted by severity then line
- Click navigates to error in editor
- Filter by severity and file type
- Counter in panel tab: "3 errors, 5 warnings"
- Clear diagnostics when file is closed

### Task 3: Wire Markdown Linting Diagnostics
**Files:** `src/core/DiagnosticsService.cpp`, `src/core/MarkdownParser.cpp`
**Description:** Add Markdown-specific linting: broken links, missing image files, duplicate headings, unclosed code blocks, inconsistent list markers, trailing whitespace.
**Acceptance Criteria:**
- Broken wiki-links: warning with "target not found"
- Missing images: error with file path
- Duplicate headings: info with "same heading on line X"
- Unclosed code blocks: error with location
- Linting runs on save (configurable toggle)
- Severity configurable per rule

### Task 4: Wire SyntaxHighlighter Completeness
**Files:** `src/core/SyntaxHighlighter.cpp`, `src/ui/EditorPanel.cpp`
**Description:** SyntaxHighlighter exists. Ensure complete highlighting for Markdown elements: headings, bold, italic, code, links, images, blockquotes, lists, frontmatter, footnotes, task items, strikethrough.
**Acceptance Criteria:**
- All Markdown elements highlighted with distinct styles
- Heading levels (H1-H6) have distinct sizes/weights
- Code spans and code blocks highlighted
- Links and images highlighted with underline
- Frontmatter (YAML) highlighted with different background
- Highlighting updates in real-time as user types

### Task 5: Wire AsyncHighlighter for Large Files
**Files:** `src/core/AsyncHighlighter.cpp`, `src/core/AsyncHighlighter.h`
**Description:** AsyncHighlighter exists for background highlighting of large files. Wire it so files over 1000 lines use async highlighting with progressive updates.
**Acceptance Criteria:**
- Files > 1000 lines highlighted asynchronously
- Visible viewport highlighted first (< 50ms)
- Background thread highlights remaining content
- Progressive updates as highlighting completes
- No UI jank during highlighting

### Task 6: Wire SnippetEngine for Markdown Snippets
**Files:** `src/core/SnippetEngine.cpp`, `src/ui/EditorPanel.cpp`
**Description:** SnippetEngine exists. Wire it to provide Markdown snippets: frontmatter template, table, code block, link, image, task list, callout, footnote, Mermaid diagram.
**Acceptance Criteria:**
- Snippet trigger: type prefix then Tab (e.g., `fm` + Tab for frontmatter)
- Tab stops for placeholders within snippet
- At least 15 built-in Markdown snippets
- User can define custom snippets in config
- Snippet picker in command palette: "Insert Snippet"
- `SnippetInsertedEvent` emitted

### Task 7: Wire Autocomplete for Wiki-Links
**Files:** `src/ui/EditorPanel.cpp`, `src/core/BacklinkIndex.cpp`
**Description:** When typing `[[`, show autocomplete popup with all document names. Fuzzy search as user types. Show document path and backlink count in popup.
**Acceptance Criteria:**
- `[[` triggers autocomplete popup
- All documents in workspace listed
- Fuzzy search narrows results
- Each result shows: document name, path, backlink count
- Selecting inserts `[[Document Name]]`
- Popup updates as user types

### Task 8: Wire Autocomplete for Tags
**Files:** `src/ui/EditorPanel.cpp`, `src/core/TagService.cpp`
**Description:** When typing `#`, show autocomplete popup with existing tags. Tags sourced from TagService across the workspace.
**Acceptance Criteria:**
- `#` triggers tag autocomplete (after space or line start)
- All existing tags listed
- Fuzzy search narrows results
- Each result shows: tag name, usage count
- Selecting inserts `#tag-name`
- New tags can be typed (no restriction to existing)

### Task 9: Wire Autocomplete for Headings
**Files:** `src/ui/EditorPanel.cpp`, `src/core/ContentIndexer.cpp`
**Description:** When typing `[[Document#`, show autocomplete for headings within the target document. Heading hierarchy shown.
**Acceptance Criteria:**
- `[[Document#` triggers heading autocomplete
- Headings from target document listed with level
- Hierarchy shown with indentation
- Selecting inserts `[[Document#Heading]]`
- Works for current document: `[[#Heading]]`

### Task 10: Wire Hover Information
**Files:** `src/ui/EditorPanel.cpp`, `src/core/TextEditorService.cpp`
**Description:** Hovering over elements shows context: wiki-links show target document preview, images show thumbnail, tags show usage count, URLs show metadata.
**Acceptance Criteria:**
- Wiki-link hover: first 5 lines of target document
- Image hover: thumbnail preview (max 200x200px)
- Tag hover: document count using this tag
- URL hover: page title if available (from MetadataScraper cache)
- Hover popup styled with theme tokens
- 500ms delay before showing

### Task 11: Wire Go To Definition for Wiki-Links
**Files:** `src/ui/EditorPanel.cpp`, `src/core/SurfaceLink.cpp`
**Description:** Cmd+Click on a wiki-link navigates to the target document. If target does not exist, offer to create it.
**Acceptance Criteria:**
- Cmd+Click navigates to wiki-link target
- Cmd+Click on image opens image preview
- Target not found: offer "Create Document" dialog
- New document created from template if configured
- `NavigateToDefinitionEvent` emitted

### Task 12: Wire Find All References
**Files:** `src/ui/EditorPanel.cpp`, `src/core/BacklinkIndex.cpp`
**Description:** Right-click on heading or document name: "Find All References" shows all documents that reference this heading or link to this document.
**Acceptance Criteria:**
- "Find All References" in context menu for headings
- Results shown in search-style results panel
- Each result shows file, line, and context
- Click navigates to reference
- Count shown in results header

### Task 13: Wire Code Actions (Quick Fixes)
**Files:** `src/core/TextEditorService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Light bulb icon for quick fixes: fix broken link, convert URL to wiki-link, wrap text in formatting, extract heading, insert table of contents.
**Acceptance Criteria:**
- Light bulb appears for applicable code actions
- Click or Cmd+. shows action list
- "Fix broken link": suggest similar document names
- "Convert URL to link": wraps URL in `[title](url)` format
- "Insert Table of Contents": generates TOC from headings
- Actions are undoable

### Task 14: Wire Document Outline Provider
**Files:** `src/core/OutlinePanelController.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** OutlinePanelController exists. Wire it to show document structure in the outline panel: headings, code blocks, frontmatter sections, images.
**Acceptance Criteria:**
- Outline shows heading hierarchy with levels
- Code blocks shown as named entries
- Frontmatter fields shown
- Click navigates to element in editor
- Outline updates on edit (debounced 500ms)
- Breadcrumb shows current heading

### Task 15: Wire Word Count and Document Statistics
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Status bar shows document statistics: word count, character count, line count, reading time estimate.
**Acceptance Criteria:**
- Word count in status bar (updates on edit)
- Click expands: character count, line count, paragraph count
- Reading time estimate (200 words/min)
- Statistics exclude frontmatter and code blocks (configurable)
- Selection: shows selection stats instead of document stats

### Task 16: Wire Bracket and Pair Matching
**Files:** `src/ui/EditorPanel.cpp`
**Description:** Highlight matching brackets, parentheses, and Markdown pairs: `**bold**`, `*italic*`, `` `code` ``, `[[link]]`, `{{template}}`.
**Acceptance Criteria:**
- Matching brackets highlighted when cursor is adjacent
- Markdown formatting pairs highlighted
- Wiki-link open/close brackets highlighted
- Mismatched pairs shown with error color
- Jump to matching pair with keyboard shortcut

### Task 17: Wire Folding for Markdown Sections
**Files:** `src/ui/EditorPanel.cpp`, `src/core/SyntaxHighlighter.cpp`
**Description:** Code folding for Markdown: fold heading sections (H1 folds until next H1), fold code blocks, fold frontmatter, fold list items.
**Acceptance Criteria:**
- Heading sections foldable (click gutter marker)
- Code blocks foldable
- Frontmatter foldable
- Fold indicator in gutter
- Fold all / Unfold all commands
- Folded state persisted per file

### Task 18: Wire Markdown Table Formatting
**Files:** `src/ui/TableEditorOverlay.cpp`, `src/ui/EditorPanel.cpp`
**Description:** TableEditorOverlay exists. Wire it so typing in Markdown tables auto-formats: align columns, add/remove rows and columns via keyboard.
**Acceptance Criteria:**
- Tab moves to next cell in table
- Shift+Tab moves to previous cell
- Columns auto-align on Tab
- Enter creates new row
- Commands: "Add Column", "Remove Column", "Sort Table"
- Table formatting on save (configurable)

### Task 19: Wire Link Validation in Editor
**Files:** `src/core/DiagnosticsService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Validate links in the editor: wiki-links to missing documents, broken image paths, invalid URLs. Show diagnostics inline.
**Acceptance Criteria:**
- Wiki-links to missing documents: warning underline
- Image paths to missing files: error underline
- Malformed URLs: info underline
- Hover shows validation message
- Quick fix: "Create Missing Document" for wiki-links

### Task 20: Add Code Intelligence Tests
**Files:** `tests/unit/test_code_intelligence.cpp`, `tests/unit/test_syntax_highlighter.cpp`
**Description:** Test code intelligence: diagnostics, autocomplete, hover, go-to-definition, and snippets.
**Acceptance Criteria:**
- Markdown linting detects all rule violations
- Wiki-link autocomplete returns correct results
- Tag autocomplete returns existing tags
- Hover information correct for each element type
- Snippet insertion and tab-stop navigation

## Testing Requirements
- Diagnostics: all linting rules produce correct markers
- Autocomplete: wiki-links, tags, headings all work
- Hover: correct information for each element type
- Performance: autocomplete popup appears in < 100ms

## Phase Completion Criteria
- Diagnostics shown in editor and Problems panel
- Markdown-specific linting rules
- Autocomplete for wiki-links, tags, and headings
- Hover information for all link types
- Snippets with tab stops
- Document outline and statistics
- All tests pass
