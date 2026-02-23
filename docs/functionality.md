# Functionality Reference

> Complete feature reference for MarkAmp v2.3.16, organized by system.

---

## 1. Editor

### Core Editing (60+ actions)

- Multi-cursor editing (add cursor above/below, select all occurrences)
- Auto-closing brackets and quotes
- Bracket matching and jump-to-bracket
- Sticky scroll headings
- 8 built-in Markdown snippets
- Right-click context menu
- Line operations: duplicate, delete, move up/down, join, reverse, sort (asc/desc)
- Case transforms: uppercase, lowercase, title case
- Code folding: fold/unfold regions, fold all, unfold all
- Selection: expand line, select word, select paragraph, select all occurrences, add next match
- Indentation: indent/outdent, convert to spaces/tabs
- Comment: add/remove line comment, block comment
- Clipboard: copy line up/down, cut line
- Whitespace: trim trailing, toggle visibility
- Read-only mode toggle
- Line numbers toggle
- Minimap toggle
- Word wrap toggle

### Find & Replace

- Full find/replace with regex support
- Search/replace preview
- Incremental search-as-you-type

### Syntax Highlighting

- 15+ languages: C, C++, Python, JavaScript, TypeScript, Rust, Go, Java, Ruby, Shell, SQL, JSON, YAML, HTML, CSS, Markdown
- Inline color preview
- Font ligature support

### View Modes

| Mode     | Shortcut    | Description           |
| -------- | ----------- | --------------------- |
| Source   | Ctrl+1 / ⌘1 | Editor only           |
| Split    | Ctrl+2 / ⌘2 | Editor + live preview |
| Preview  | Ctrl+3 / ⌘3 | Preview only          |
| Zen Mode | —           | Distraction-free      |

---

## 2. Markdown Rendering

- **GFM** — Full GitHub Flavored Markdown support
- **Math** — LaTeX math via `$...$` (inline) and `$$...$$` (display), rendered as Unicode
- **Mermaid** — Inline Mermaid diagrams with error overlay
- **Code blocks** — Syntax-highlighted with language detection
- **Tables** — Full GFM table rendering
- **Task lists** — Interactive checkboxes
- **Footnotes** — Footnote references and definitions
- **Callouts** — Block callout rendering
- **Embeds** — Audio, video, and iframe embeds

---

## 3. Infinite Canvas

### Objects

Sticky notes, text boxes, images, shapes (rectangle, ellipse, diamond, triangle, hexagon, star), frames, groups, icons, connectors, tables, sections, comments, diagrams, freehand paths, PDF pages, bookmark cards, video embeds, app widgets

### Tools

- **Select** — Click, drag, multi-select, box select
- **Pan** — Pan viewport with drag
- **Draw** — Freehand drawing with shape recognition

### Features

- Board serialization (save/load, format v2)
- Quadtree spatial indexing for efficient hit-testing
- Snap engine (grid, guides, objects)
- Undo/redo stack
- Clipboard (copy, paste, duplicate)
- Export to image/PDF
- Object alignment and distribution
- Z-order layering
- Object grouping/ungrouping
- Object locking
- Object tagging
- Style presets
- Board templates
- Performance monitoring
- Accessibility support

### Collaboration

- Real-time collaboration protocol
- Remote cursor & selection overlays
- Follow mode (follow another user's viewport)
- Collaborative undo/redo
- Offline sync queue
- Cross-board linking

### Specialized Boards

- **Kanban** — Columns, lanes, card management
- **Mind Map** — Hierarchical nodes with force-directed layout
- **Diagrams** — Diagram editing coordination with shape library

---

## 4. Knowledge Graph

- Backlink index and service
- Wiki link parsing (`[[link]]` syntax)
- Local graph visualization
- Graph analytics (centrality, clusters)
- Graph search
- Graph export
- Graph annotations
- Graph evolution tracking
- Reference scanning

---

## 5. Attribute View Database

Notion-style databases with:

- **Table view** — Sortable, filterable columns
- **Gallery view** — Card-based display
- **Kanban view** — Column-based card arrangement
- **Timeline view** — Temporal visualization
- Formula evaluation engine
- Rollup aggregations
- Cross-database relations
- CSV/JSON import/export
- Data validation rules
- Database-level undo/redo

---

## 6. Flashcard System (FSRS)

- FSRS algorithm for optimal scheduling
- Flashcard extraction from documents
- Cloze deletion parsing
- Study streak tracking
- Leech detection for poorly-learned cards
- Import/export flashcard sets
- Review session management

---

## 7. AI Integration

- Writing assistance (summarize, translate, expand, simplify, fix grammar)
- Inline text completion
- Tag suggestions
- Link suggestions
- Document generation
- Multi-provider config (OpenAI, Anthropic, Local/Ollama)
- Session-based chat with history

---

## 8. Export & Import

### Export Formats

HTML, PDF (via Pandoc), Markdown, batch export

### Import Formats

HTML, Markdown, Pandoc-supported formats, PDF annotation import

### Features

- Export templates
- Export validation
- Publishing profiles
- Batch export engine

---

## 9. Plugin System

- 15 built-in plugins
- VS Code-compatible `package.json` manifest
- Lazy activation events
- Dependency resolution (topological sort)
- Extension pack expansion
- Contribution points: Commands, Keybindings, Views, Themes, Languages, Grammars, Settings, Custom Editors
- VSIX install/uninstall
- Extension sandbox with permission grants
- Host crash recovery
- Marketplace gallery search
- Extension recommendations based on file types
- Hot reload
- Plugin quarantine for misbehaving extensions

---

## 10. Version Control

- Git service with core operations
- Blame/annotation engine
- Stash management
- Remote operations
- Commit graph visualization
- Diff engine (unified diff)
- Merge conflict resolution
- File history

---

## 11. Cloud Sync

- Multi-provider sync (S3, WebDAV)
- Scheduled sync
- Conflict resolution
- Selective sync filters
- Sync history logging
- End-to-end encryption

---

## 12. Presentation Mode

- Slide-based presentations from Markdown
- Slide engine and deck management
- Presentation themes
- Export to PDF/HTML
- Progressive reveal

---

## 13. Security & Privacy

- AES-256 encryption (EncryptionService)
- Encrypted vault support
- Content security policies
- PII redaction engine
- Security audit logging
- Input sanitization (URL, clipboard, HTML)
- Safe mode with crash recovery
- Enterprise runtime policies

---

## 14. Notebook System

- Jupyter-style cell execution
- Cell dependency tracking
- Notebook serialization
- Notebook diff/merge (3-way)
- Notebook export (Markdown, HTML, PDF)
- Search across notebooks
- Kernel management

---

## 15. Task & Calendar

- Task management with boards
- Gantt chart visualization
- Recurring task scheduling
- Task reminders
- Task aggregation across documents

---

## 16. WYSIWYG Mode

- Rich-text editing
- Source ↔ WYSIWYG mode switching
- State persistence
- Accessibility support

---

## 17. Internationalization

- Multi-language support via LocaleManager
- RTL/LTR text direction
- Translation catalogs

---

## 18. Accessibility

- **Screen Reader Support** — VoiceOver integration (macOS) and native stubs for Windows/Linux via `ScreenReaderBridge`
- **Focus Management** — `FocusManager` provides global item-level tracking and modal focus trapping
- **Keyboard Navigation** — Dynamic focus rings globally rendered for keyboard users only (mouse input disables them)
- **Skip-to-Content** — Invisible tab-navigable landmark to bypass UI chrome straight to the editor
