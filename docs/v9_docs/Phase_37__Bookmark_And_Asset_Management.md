# Phase 37: Bookmark and Asset Management

## Overview
BookmarkService, AssetService, AssetTypes, MetadataScraper, and CardFactory exist in core. Bookmark.h defines bookmark structures. These systems enable document bookmarking and workspace asset management but are not wired to user-facing features. This phase builds bookmark management and asset organization.

## Prerequisites
- Phase 20 (File management and workspace)
- Phase 07 (Editor for bookmark interaction)
- Phase 06 (Workbench navigation for bookmark panel)

## Tasks

### Task 1: Wire BookmarkService for Document Bookmarks
**Files:** `src/core/BookmarkService.cpp`, `src/core/Bookmark.cpp`, `src/core/Bookmark.h`
**Description:** BookmarkService exists. Wire it for document bookmarking: add/remove bookmarks, bookmark with label, bookmark with position (line number).
**Acceptance Criteria:**
- "Bookmark: Toggle" adds/removes bookmark at cursor line
- Bookmarks stored per file with line number
- Bookmark label optional (default: line content)
- Bookmarks persisted in workspace state
- `BookmarkAddedEvent` / `BookmarkRemovedEvent` emitted
- Bookmark gutter indicators in editor

### Task 2: Wire Bookmark Gutter Indicators
**Files:** `src/ui/EditorPanel.cpp`, `src/core/BookmarkService.cpp`
**Description:** Bookmarks shown as colored markers in the editor gutter. Click gutter to toggle bookmark.
**Acceptance Criteria:**
- Bookmark marker in gutter (colored dot or flag icon)
- Click gutter area toggles bookmark
- Bookmarked lines have subtle background highlight
- Multiple bookmarks per file supported
- Bookmark markers update on line insert/delete
- Marker color from theme accent

### Task 3: Wire Bookmark Panel
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/BookmarkService.cpp`
**Description:** Bookmark panel in tool window area: lists all bookmarks across workspace, grouped by file.
**Acceptance Criteria:**
- Bookmarks listed, grouped by file
- Each entry: label, file name, line number, preview
- Click navigates to bookmark location
- Context menu: Remove, Edit Label, Remove All in File
- Filter: search bookmarks by label
- Sort: by file, by date added

### Task 4: Wire Bookmark Navigation
**Files:** `src/ui/EditorPanel.cpp`, `src/core/BookmarkService.cpp`
**Description:** Navigate between bookmarks: F2 next bookmark, Shift+F2 previous bookmark (within file), Cmd+F2 next across files.
**Acceptance Criteria:**
- F2: next bookmark in current file
- Shift+F2: previous bookmark in current file
- Cmd+F2: next bookmark across all files
- Wrap around at end of bookmarks
- Announce navigation to screen reader
- Status bar shows bookmark count

### Task 5: Wire Named Bookmark Groups
**Files:** `src/core/BookmarkService.cpp`
**Description:** Bookmarks can be organized into named groups (e.g., "Review Points", "TODO", "Important"). Groups color-coded.
**Acceptance Criteria:**
- Create bookmark group with name and color
- Assign bookmark to group
- Filter bookmarks by group in panel
- Group-specific gutter colors
- Default groups: General, Important, Review
- Groups persisted in workspace state

### Task 6: Wire AssetService for Workspace Assets
**Files:** `src/core/AssetService.cpp`, `src/core/AssetTypes.h`
**Description:** AssetService and AssetTypes exist. Wire asset management: track all non-Markdown files in workspace (images, PDFs, attachments), provide metadata, detect orphaned assets.
**Acceptance Criteria:**
- Asset index: all non-.md files tracked
- Metadata per asset: file size, type, referenced by (documents)
- Orphan detection: assets not referenced by any document
- Asset types: image, PDF, video, audio, archive, other
- `AssetIndexUpdatedEvent` emitted on change
- Index updates on file system changes

### Task 7: Wire Asset Panel
**Files:** `src/ui/ToolWindowHost.cpp`, `src/core/AssetService.cpp`
**Description:** Asset panel in tool window: lists workspace assets with thumbnails, size, and reference count.
**Acceptance Criteria:**
- Assets listed with thumbnail (images), type icon (others)
- Columns: name, type, size, references
- Click opens asset (image preview, PDF viewer)
- Filter by type: images, PDFs, all
- Sort by name, size, references
- Orphaned assets highlighted

### Task 8: Wire Asset Insertion from Panel
**Files:** `src/ui/EditorPanel.cpp`, `src/core/AssetService.cpp`
**Description:** Drag asset from asset panel to editor inserts appropriate Markdown link: image for images, link for PDFs and others.
**Acceptance Criteria:**
- Drag image: inserts `![alt](path)` at drop position
- Drag PDF: inserts `[filename](path)` at drop position
- Drag other: inserts `[filename](path)` at drop position
- Drop position indicated with cursor
- Asset path relative to document
- Undo support for insertion

### Task 9: Wire Orphaned Asset Cleanup
**Files:** `src/core/AssetService.cpp`
**Description:** "Assets: Clean Up Orphaned" command: find assets not referenced by any document, show list, offer to move to trash.
**Acceptance Criteria:**
- Scan all documents for asset references
- List orphaned assets with size
- Total space reclaimable shown
- Selective deletion: checkbox per asset
- "Delete Selected" moves to OS trash
- Confirmation dialog with total size

### Task 10: Wire Asset Optimization
**Files:** `src/core/AssetService.cpp`
**Description:** "Assets: Optimize Images" command: compress workspace images to reduce size. Show before/after sizes.
**Acceptance Criteria:**
- Scan for images > configurable threshold (500KB)
- Compress PNG/JPEG with quality preservation
- Before/after size comparison
- Selective optimization: checkbox per image
- Configurable quality: low, medium, high
- Original preserved option (rename to .backup)

### Task 11: Wire MetadataScraper for URL Cards
**Files:** `src/canvas/MetadataScraper.cpp`, `src/core/AssetService.cpp`
**Description:** MetadataScraper exists. Wire it to fetch URL metadata: title, description, favicon, preview image. Used for bookmark cards and link previews.
**Acceptance Criteria:**
- Fetch: title, description, og:image, favicon
- Cache fetched metadata (24-hour expiry)
- Offline: return cached data
- Rate limiting: max 5 concurrent fetches
- Error handling: timeout, 404, network error
- Privacy: configurable (disable metadata fetching)

### Task 12: Wire CardFactory for Rich Link Previews
**Files:** `src/core/CardFactory.cpp`, `src/core/CardFactory.h`
**Description:** CardFactory exists. Wire it to generate rich link preview cards from URLs in documents. Cards show title, description, favicon.
**Acceptance Criteria:**
- URL in Markdown: hover shows rich card preview
- Card: title, description, favicon, site name
- Card styling from theme
- Click card opens URL in browser
- Card cached for performance
- Toggle rich previews in settings

### Task 13: Wire Bookmark Import/Export
**Files:** `src/core/BookmarkService.cpp`
**Description:** Export bookmarks to Markdown file. Import bookmarks from Markdown-formatted bookmark list.
**Acceptance Criteria:**
- Export: Markdown with grouped bookmarks
- Format: `## Group Name\n- [Label](file:line)\n`
- Import: parse Markdown bookmark format
- Import validates file references
- Broken references flagged
- Export/Import via command palette

### Task 14: Wire Asset Reference Validation
**Files:** `src/core/AssetService.cpp`, `src/core/DiagnosticsService.cpp`
**Description:** Validate all asset references in documents. Broken references (missing files) shown as diagnostics.
**Acceptance Criteria:**
- Image references validated: file exists, MIME valid
- Link references validated: target exists
- Broken reference: warning diagnostic with fix suggestion
- Fix: "Update Path" or "Remove Reference"
- Validation on save (debounced)
- Bulk fix: "Fix All Broken References"

### Task 15: Wire Bookmark and Asset Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register commands: "Bookmark: Toggle", "Bookmark: Next", "Bookmark: Previous", "Bookmark: Show All", "Assets: Show Panel", "Assets: Clean Up", "Assets: Optimize Images".
**Acceptance Criteria:**
- All commands registered in command palette
- Bookmark commands under "Bookmark:" prefix
- Asset commands under "Assets:" prefix
- Keyboard shortcuts for bookmark toggle and navigation
- Context-aware: bookmark commands only in editor

### Task 16: Wire Bookmark Theme Integration
**Files:** `src/ui/EditorPanel.cpp`, `src/core/ThemeEngine.cpp`
**Description:** Bookmark gutter markers, line highlights, and panel use theme tokens.
**Acceptance Criteria:**
- Bookmark marker from `bookmark_marker` token (or accent)
- Bookmarked line background: subtle tint
- Group colors complement theme palette
- Panel uses standard panel theming
- All 64 themes render bookmarks correctly

### Task 17: Wire Asset Thumbnails
**Files:** `src/core/AssetService.cpp`
**Description:** Generate thumbnails for workspace images. Thumbnails used in asset panel and Markdown editor hover.
**Acceptance Criteria:**
- Thumbnails generated for all image assets
- Thumbnail size: 128x128px
- Thumbnail cache in `.markamp/thumbnails/`
- Lazy generation: only when needed
- Cache invalidation: when source image changes
- Supported: PNG, JPEG, GIF, SVG (rasterized)

### Task 18: Wire Bookmark Sync
**Files:** `src/core/BookmarkService.cpp`, `src/core/CloudSyncService.cpp`
**Description:** Bookmarks synced as part of workspace state via cloud sync. Bookmarks merge correctly from multiple devices.
**Acceptance Criteria:**
- Bookmarks included in workspace state sync
- Merge: bookmarks from both devices combined
- Conflict: same line, different label - keep newest
- Deleted bookmarks propagated
- Sync does not create duplicate bookmarks

### Task 19: Wire Quick Bookmark Access
**Files:** `src/ui/CommandPalette.cpp`, `src/core/BookmarkService.cpp`
**Description:** "Bookmark: Go To" command shows all bookmarks in quick pick format for fast navigation.
**Acceptance Criteria:**
- Quick pick shows all bookmarks
- Fuzzy search by label and file name
- Preview: show bookmark context (surrounding lines)
- Most recent bookmarks ranked higher
- Keyboard shortcut for "Go To Bookmark"

### Task 20: Add Bookmark and Asset Tests
**Files:** `tests/unit/test_bookmark_service.cpp`, `tests/unit/test_asset_service.cpp`
**Description:** Test bookmark and asset systems: CRUD, navigation, orphan detection, metadata scraping.
**Acceptance Criteria:**
- Bookmark CRUD: add, remove, update label, list
- Bookmark navigation: next, previous, cross-file
- Bookmark persistence: save/load round-trip
- Asset index: track, orphan detect, cleanup
- Metadata scraping: parse HTML for og: tags
- Asset reference validation

## Testing Requirements
- Bookmark CRUD and navigation
- Asset index accuracy and orphan detection
- Metadata scraping correctness
- Bookmark group management

## Phase Completion Criteria
- Document bookmarks with gutter indicators
- Bookmark panel with navigation
- Named bookmark groups
- Asset management panel
- Orphaned asset detection and cleanup
- Rich link preview cards
- All tests pass
