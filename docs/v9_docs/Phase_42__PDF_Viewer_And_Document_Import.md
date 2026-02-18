# Phase 42: PDF Viewer and Document Import

## Overview
PDFViewerPanel, PDFAnnotationSidebar, PDFThumbnailStrip exist in UI. ImportService exists in core. However, PDF viewing is not wired to a functional experience and document import (PDF to Markdown, DOCX to Markdown) is not available. This phase builds document import and PDF viewing capabilities.

## Prerequisites
- Phase 20 (File management for import destination)
- Phase 07 (Editor for imported content)
- Phase 06 (Workbench navigation for viewer mode)

## Tasks

### Task 1: Wire PDFViewerPanel for PDF Display
**Files:** `src/ui/PDFViewerPanel.cpp`, `src/ui/PDFViewerPanel.h`
**Description:** PDFViewerPanel exists. Wire it to render PDF documents: page-by-page display, zoom, scroll, search within PDF.
**Acceptance Criteria:**
- PDF files open in PDFViewerPanel
- Page-by-page continuous scroll
- Zoom: fit-width, fit-page, custom (25%-400%)
- Text selection and copy
- Search within PDF (Cmd+F)
- Page count and current page in status bar

### Task 2: Wire PDFThumbnailStrip for Page Navigation
**Files:** `src/ui/PDFThumbnailStrip.cpp`, `src/ui/PDFThumbnailStrip.h`
**Description:** PDFThumbnailStrip exists. Wire it as a sidebar strip showing page thumbnails for quick navigation.
**Acceptance Criteria:**
- Thumbnail strip in left sidebar of PDF viewer
- Thumbnails rendered at reduced scale
- Click thumbnail navigates to page
- Current page highlighted
- Thumbnail strip scrollable for large PDFs
- Toggle strip visibility

### Task 3: Wire PDFAnnotationSidebar for Annotations
**Files:** `src/ui/PDFAnnotationSidebar.cpp`, `src/ui/PDFAnnotationSidebar.h`
**Description:** PDFAnnotationSidebar exists. Wire it for PDF annotations: highlight text, add notes, draw rectangles.
**Acceptance Criteria:**
- Highlight: select text, choose color, save highlight
- Note: click to add note at position, type text
- Rectangle: draw rectangle annotation area
- Annotations saved in `.markamp/annotations/{pdf_hash}.json`
- Annotations shown in sidebar list
- Click annotation navigates to page

### Task 4: Wire PDF Text Extraction
**Files:** `src/core/ImportService.cpp`
**Description:** Extract text from PDF for search and conversion. Text extraction preserves paragraph structure.
**Acceptance Criteria:**
- Text extracted from all pages
- Paragraph boundaries detected
- Heading detection from font size
- List detection from indentation
- Table detection (basic)
- `PDFTextExtractedEvent` emitted

### Task 5: Wire PDF to Markdown Conversion
**Files:** `src/core/ImportService.cpp`
**Description:** Convert PDF to Markdown: headings from font size, paragraphs from text blocks, lists from indentation, images extracted.
**Acceptance Criteria:**
- Headings: large/bold text becomes H1-H3
- Paragraphs: text blocks become paragraphs
- Lists: indented items become bullet lists
- Images: extracted to assets folder, linked in Markdown
- Tables: basic table extraction
- Output: valid Markdown file

### Task 6: Wire DOCX Import
**Files:** `src/core/ImportService.cpp`
**Description:** Import DOCX (Word) documents to Markdown: headings, paragraphs, lists, tables, images.
**Acceptance Criteria:**
- Headings preserved with correct levels
- Paragraphs with formatting (bold, italic)
- Lists: bulleted and numbered
- Tables: converted to Markdown tables
- Images: extracted to assets, linked
- "Import: DOCX to Markdown" command

### Task 7: Wire HTML Import
**Files:** `src/core/ImportService.cpp`
**Description:** Import HTML to Markdown: clean conversion preserving structure. Support for web clipping (paste URL, import content).
**Acceptance Criteria:**
- HTML structure converted to Markdown
- Links preserved
- Images downloaded to assets (optional)
- Tables converted to Markdown
- Code blocks preserved
- "Import: HTML to Markdown" command

### Task 8: Wire Clipboard Import
**Files:** `src/core/ImportService.cpp`, `src/core/ClipboardService.cpp`
**Description:** Paste rich content from clipboard: HTML from web browsers converted to Markdown, images saved to assets.
**Acceptance Criteria:**
- Paste HTML: converted to Markdown inline
- Paste from Word: formatting preserved as Markdown
- Paste image: saved to assets, link inserted
- Paste URL: option to import as bookmark card or plain URL
- Smart paste: detect content type and convert appropriately
- Paste format preference: Markdown or plain text

### Task 9: Wire Batch Import
**Files:** `src/core/ImportService.cpp`, `src/ui/MainFrame.cpp`
**Description:** Import multiple files at once: drag folder of PDFs/DOCXs, batch convert to Markdown.
**Acceptance Criteria:**
- Drag folder: detect importable files
- Batch conversion with progress
- Output directory selection
- Naming convention: original name with .md extension
- Error report: which files failed and why
- "Import: Batch Import" command

### Task 10: Wire Import Preview
**Files:** `src/core/ImportService.cpp`, `src/ui/SplitView.cpp`
**Description:** Before importing, show preview: original document on left, converted Markdown on right. User can adjust before saving.
**Acceptance Criteria:**
- Split view: original (left) and converted (right)
- Converted Markdown editable before save
- "Save" creates Markdown file
- "Cancel" discards conversion
- Quality indicators: conversion confidence per section
- Reconvert option with different settings

### Task 11: Wire Evernote Import
**Files:** `src/core/ImportService.cpp`
**Description:** Import from Evernote export (.enex): notes converted to Markdown files, notebooks become folders, tags preserved.
**Acceptance Criteria:**
- .enex file parsed (XML format)
- Each note becomes a Markdown file
- Note content: HTML to Markdown conversion
- Attachments: extracted to assets folder
- Tags: added as frontmatter tags
- Notebooks: become subdirectories

### Task 12: Wire Notion Import
**Files:** `src/core/ImportService.cpp`
**Description:** Import from Notion export (Markdown + CSV): fix Notion-specific formatting, resolve internal links, organize structure.
**Acceptance Criteria:**
- Notion Markdown export parsed
- Notion-specific formatting cleaned up
- Internal links: resolved to wiki-links
- CSV databases: converted to Attribute Views
- File structure preserved
- Image references fixed

### Task 13: Wire Obsidian Vault Import
**Files:** `src/core/ImportService.cpp`
**Description:** Import Obsidian vault: preserve wiki-links, convert Obsidian-specific syntax (callouts, embeds), import canvas files.
**Acceptance Criteria:**
- Obsidian wiki-links preserved (compatible syntax)
- Callout blocks: converted to MarkAmp callout syntax
- Embeds: converted to MarkAmp embed syntax
- Canvas files (.canvas): converted to MarkAmp boards
- Frontmatter preserved
- Folder structure preserved

### Task 14: Wire PDF Bookmark Integration
**Files:** `src/ui/PDFViewerPanel.cpp`, `src/core/BookmarkService.cpp`
**Description:** PDF documents support bookmarks: bookmark specific pages, annotations linked to bookmarks.
**Acceptance Criteria:**
- Bookmark a PDF page with note
- Bookmarks show in bookmark panel with page number
- Click bookmark opens PDF at page
- PDF bookmarks (TOC) shown in outline panel
- Navigate PDF via outline headings

### Task 15: Wire Import Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Import settings: default import format, image handling (embed/reference), heading detection sensitivity, list detection.
**Acceptance Criteria:**
- Default import format: Markdown
- Image handling: embed as base64, reference in assets, skip
- Heading detection: font size threshold
- Asset directory: configurable path
- Import history: last 20 imports

### Task 16: Wire Import Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register import commands: "Import: PDF to Markdown", "Import: DOCX to Markdown", "Import: HTML to Markdown", "Import: From Evernote", "Import: From Notion", "Import: From Obsidian", "Import: Batch Import".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Import:" prefix
- File picker for import source
- Format auto-detection from file extension
- Progress indicator for large imports

### Task 17: Wire PDF Viewer Theme
**Files:** `src/ui/PDFViewerPanel.cpp`
**Description:** PDF viewer chrome uses theme tokens: toolbar, thumbnail strip, annotation sidebar, status bar.
**Acceptance Criteria:**
- Viewer toolbar from theme panel tokens
- Thumbnail strip background from theme
- Annotation highlight colors from theme palette
- Status bar matches application status bar
- Dark mode: PDF display adjustable (sepia, dark, normal)

### Task 18: Wire PDF Viewer Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** PDF viewer commands: "PDF: Zoom In", "PDF: Zoom Out", "PDF: Fit Width", "PDF: Fit Page", "PDF: Go To Page", "PDF: Extract Text", "PDF: Convert to Markdown".
**Acceptance Criteria:**
- All commands registered
- Commands active when PDF viewer focused
- Keyboard shortcuts for zoom and navigation
- "Go To Page" with page number input
- "Extract Text" creates new Markdown file

### Task 19: Wire PDF Annotation Export
**Files:** `src/ui/PDFAnnotationSidebar.cpp`
**Description:** Export PDF annotations as Markdown: highlights with page references, notes with content, summary document.
**Acceptance Criteria:**
- Export annotations as Markdown file
- Format: `## Page X\n### Highlight\n> quoted text\n### Note\nNote content\n`
- All annotations included
- Page references preserved
- Export command: "PDF: Export Annotations"
- Annotation file linked to source PDF

### Task 20: Add Import and PDF Tests
**Files:** `tests/unit/test_pdf_import.cpp`, `tests/unit/test_import_service.cpp`
**Description:** Test import system: PDF text extraction, DOCX conversion, HTML conversion, and import accuracy.
**Acceptance Criteria:**
- PDF text extraction: paragraph detection
- DOCX conversion: headings, lists, tables
- HTML conversion: structure preservation
- Clipboard import: rich text to Markdown
- Batch import: multiple files processed
- Obsidian import: wiki-link compatibility

## Testing Requirements
- PDF text extraction accuracy
- DOCX to Markdown conversion completeness
- HTML to Markdown structure preservation
- Import round-trip: export then re-import similarity

## Phase Completion Criteria
- PDF viewer with zoom, scroll, search
- PDF annotations with highlights and notes
- Import from PDF, DOCX, HTML
- Import from Evernote, Notion, Obsidian
- Clipboard rich paste to Markdown
- Batch import with progress
- All tests pass
