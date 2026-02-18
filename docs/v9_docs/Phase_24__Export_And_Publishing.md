# Phase 24: Export and Publishing

## Overview
ExportService, NotebookExportEngine, PresentationEngine, and HtmlRenderer exist but the export pipeline is fragmented. Users cannot export documents to PDF, HTML, or other formats from a unified interface. PresentationEngine exists but is not wired to a presentation mode. This phase builds a complete export and publishing system.

## Prerequisites
- Phase 08 (Markdown rendering completeness)
- Phase 15 (Notebook export)
- Phase 07 (Editor for export source)

## Tasks

### Task 1: Wire ExportService as Unified Export Pipeline
**Files:** `src/core/ExportService.cpp`, `src/core/ExportService.h`
**Description:** ExportService exists. Wire it as the central export coordinator: receives export requests, selects format handler, processes document, and writes output.
**Acceptance Criteria:**
- `export(document, format, options)` method functional
- Supported formats: PDF, HTML, DOCX, Markdown (clean), Plain Text
- Export options: include frontmatter, include TOC, page size, margins
- `ExportCompletedEvent` emitted with output path
- `ExportFailedEvent` emitted with error details

### Task 2: Wire HTML Export
**Files:** `src/rendering/HtmlRenderer.cpp`, `src/core/ExportService.cpp`
**Description:** HtmlRenderer exists. Wire HTML export: render Markdown to standalone HTML with embedded CSS, syntax highlighting, Mermaid diagrams, and math.
**Acceptance Criteria:**
- Standalone HTML with inline CSS
- Syntax highlighting for code blocks (themed)
- Mermaid diagrams rendered as SVG
- Math rendered as MathML or SVG
- Images embedded as base64 or referenced
- Export matches preview rendering

### Task 3: Wire PDF Export
**Files:** `src/core/ExportService.cpp`
**Description:** PDF export via HTML-to-PDF conversion. Support page headers/footers, page numbers, margins, and page breaks.
**Acceptance Criteria:**
- PDF output via HTML intermediate (using system print or wkhtmltopdf)
- Page size: A4, Letter, Legal (configurable)
- Margins configurable
- Page numbers in footer
- Heading-based page breaks (configurable: break before H1)
- Cover page option with title and date

### Task 4: Wire Markdown Clean Export
**Files:** `src/core/ExportService.cpp`
**Description:** Export cleaned Markdown: strip frontmatter optionally, resolve wiki-links to standard links, resolve embeds to inline content, normalize formatting.
**Acceptance Criteria:**
- Frontmatter: include or strip (configurable)
- Wiki-links converted to standard `[text](url)` links
- Embeds resolved to inline content
- Code blocks preserved
- Output is valid CommonMark Markdown
- Configurable line width for wrapping

### Task 5: Wire Plain Text Export
**Files:** `src/core/ExportService.cpp`
**Description:** Export as plain text: strip all Markdown formatting, preserve content structure with blank lines between paragraphs, indent for nested lists.
**Acceptance Criteria:**
- All Markdown formatting stripped
- Headings preserved as uppercase or with level indicator
- Lists preserved with plain bullets (-, *)
- Code blocks preserved with indentation
- Links shown as text with URL in parentheses
- Tables rendered as aligned text

### Task 6: Wire PresentationEngine for Slide Mode
**Files:** `src/ui/PresentationEngine.cpp`, `src/ui/PresentationEngine.h`
**Description:** PresentationEngine exists. Wire it for Markdown-to-slides presentation: `---` separators create slides, headings as slide titles, speaker notes with `???` syntax.
**Acceptance Criteria:**
- `---` horizontal rule creates slide break
- Each slide rendered full-screen
- H1/H2 as slide title
- Content rendered below title
- Speaker notes after `???` (hidden from audience view)
- Keyboard: right arrow next, left arrow previous, Escape exit

### Task 7: Wire Presentation Controls
**Files:** `src/ui/PresentationEngine.cpp`, `src/ui/MainFrame.cpp`
**Description:** Presentation mode controls: slide navigation, slide overview, timer, presenter view.
**Acceptance Criteria:**
- Navigation: arrow keys, Page Up/Down, number+Enter for specific slide
- Slide overview: grid view of all slides (press G)
- Timer: elapsed time shown in presenter view
- Presenter view: current slide + next slide + notes + timer
- Progress bar at bottom
- "Start Presentation" command in palette

### Task 8: Wire Presentation Theme
**Files:** `src/ui/PresentationEngine.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Presentations use the active theme. Slides render with theme background, text colors, and accent colors for headings and links.
**Acceptance Criteria:**
- Slide background from theme
- Text colors from theme
- Code blocks use theme syntax highlighting
- Accent colors for headings and emphasis
- Theme switch updates presentation immediately

### Task 9: Wire Batch Export
**Files:** `src/core/ExportService.cpp`, `src/ui/MainFrame.cpp`
**Description:** Export multiple documents at once: select files in explorer, right-click "Export Selected", choose format and options.
**Acceptance Criteria:**
- Multi-select files in explorer
- "Export Selected" in context menu
- Format and options dialog
- Progress indicator for batch export
- Output to selected directory
- Summary: X files exported successfully, Y failed

### Task 10: Wire Export Template System
**Files:** `src/core/ExportService.cpp`, `src/core/TemplateEngine.cpp`
**Description:** Export templates define custom HTML/CSS for export output. Users can create branded export templates with custom headers, footers, and styling.
**Acceptance Criteria:**
- Export templates in `.markamp/export-templates/`
- Template includes: HTML wrapper, CSS, header, footer
- Variables: `{{title}}`, `{{date}}`, `{{author}}`, `{{content}}`
- Template selector in export dialog
- Built-in templates: Default, Academic, Report, Newsletter

### Task 11: Wire Print Preview
**Files:** `src/ui/PreviewPanel.cpp`, `src/core/ExportService.cpp`
**Description:** Print preview mode shows document as it will appear in PDF/print: page breaks, headers, footers, margins visualized.
**Acceptance Criteria:**
- "Print Preview" command shows paginated view
- Page break indicators
- Header and footer shown
- Margins shown as gray areas
- Page count displayed
- "Print" button sends to system printer

### Task 12: Wire Export to Clipboard
**Files:** `src/core/ExportService.cpp`, `src/core/ClipboardService.cpp`
**Description:** Export to clipboard in various formats: "Copy as HTML", "Copy as Rich Text", "Copy as Plain Text". Useful for pasting into other applications.
**Acceptance Criteria:**
- "Copy as HTML" copies rendered HTML to clipboard
- "Copy as Rich Text" copies formatted text
- "Copy as Plain Text" copies stripped text
- Selection: copies only selected text in chosen format
- Notification: "Copied as [format]"

### Task 13: Wire PDF Import
**Files:** `src/core/ImportService.cpp`, `src/ui/PDFViewerPanel.cpp`
**Description:** Import PDF files: view in PDFViewerPanel, extract text to Markdown, annotate with PDFAnnotationSidebar.
**Acceptance Criteria:**
- PDF files open in PDFViewerPanel
- Text extraction to new Markdown file
- PDFAnnotationSidebar for highlights and notes
- PDFThumbnailStrip for page navigation
- Zoom and scroll in PDF viewer

### Task 14: Wire Image Export from Canvas
**Files:** `src/canvas/CanvasRenderer.cpp`, `src/core/ExportService.cpp`
**Description:** Export canvas boards as images: PNG, SVG. Export whole board or selected objects only.
**Acceptance Criteria:**
- "Export Board as PNG" at 2x resolution
- "Export Board as SVG" for vector output
- "Export Selection as Image" for selected objects
- Transparent background option
- Export includes all visible objects

### Task 15: Wire Publish to Static Site
**Files:** `src/core/ExportService.cpp`
**Description:** Publish workspace as a static site: convert all Markdown files to HTML, maintain directory structure, generate index page and navigation.
**Acceptance Criteria:**
- "Publish as Website" command
- All .md files converted to HTML
- Directory structure preserved
- Index page generated with file listing
- Navigation sidebar generated from file tree
- Custom CSS from export template

### Task 16: Wire Export Progress and Error Handling
**Files:** `src/core/ExportService.cpp`
**Description:** Export shows progress for long operations. Errors are collected and reported at the end.
**Acceptance Criteria:**
- Progress bar for batch exports
- Cancellable long-running exports
- Error collection: continue export, report at end
- Error report: file, error message, suggested fix
- `ExportProgressEvent` emitted with percentage

### Task 17: Wire Export Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register export commands: "Export: PDF", "Export: HTML", "Export: Markdown", "Export: Batch Export", "Presentation: Start", "Print Preview".
**Acceptance Criteria:**
- All export commands registered
- Commands categorized under "Export:" prefix
- Context-aware: export current document or selected files
- Keyboard shortcut for PDF export (Cmd+Shift+P)
- "Start Presentation" with F5

### Task 18: Wire Export Settings
**Files:** `src/core/Config.h`, `src/ui/SettingsPanel.cpp`
**Description:** Export settings in preferences: default format, PDF page size, margins, include TOC, include frontmatter, export template, output directory.
**Acceptance Criteria:**
- Settings section: "Export"
- Default format selector
- PDF options: page size, margins, orientation
- HTML options: embed images, include styles
- Default output directory configurable
- Settings applied as defaults in export dialog

### Task 19: Wire Export Format Detection
**Files:** `src/core/ExportService.cpp`
**Description:** Frontmatter can specify export settings per document: `export: { format: pdf, template: academic, toc: true }`. Document-level settings override global defaults.
**Acceptance Criteria:**
- `export` key in frontmatter parsed
- Format, template, and options respected
- Document settings override global defaults
- Invalid export config: warning diagnostic
- Settings merged: document > global > built-in defaults

### Task 20: Add Export Tests
**Files:** `tests/unit/test_export.cpp`, `tests/unit/test_presentation.cpp`
**Description:** Test export system: HTML rendering, PDF generation, presentation mode, and batch export.
**Acceptance Criteria:**
- HTML export produces valid HTML5
- Markdown export produces valid CommonMark
- Presentation: correct slide count from `---` separators
- Batch export processes all files
- Export template variable substitution

## Testing Requirements
- HTML export: valid output with all elements
- PDF export: page count, margins correct
- Presentation: slide parsing and navigation
- Template: variable substitution and custom CSS

## Phase Completion Criteria
- Export to PDF, HTML, Markdown, Plain Text
- Presentation mode from Markdown
- Print preview with pagination
- Batch export with progress
- Export templates for custom styling
- All tests pass
