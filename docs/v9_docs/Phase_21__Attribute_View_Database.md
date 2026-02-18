# Phase 21: Attribute View Database

## Overview
The Attribute View (AV) system has a comprehensive infrastructure: AVColumnType, AVFilter, AVSort, AVQueryEngine, AVRollupEngine, AVRelationService, AVValidators, AVFormatters, AttributeView, and AttributeViewStore in core. UI components include AVTablePanel, AVKanbanPanel, AVGalleryPanel, AVFilterBar, AVCellEditor, AVCellRenderer, AVHeaderRenderer, AVRelationEditor, AVRollupConfigDialog, and AVGalleryCard. However, this Notion-style database system is not accessible as a workspace feature. This phase wires the AV system into a functional database experience.

## Prerequisites
- Phase 06 (Workbench navigation for AV sidebar/panel)
- Phase 07 (Editor for inline AV blocks)
- Phase 03 (Theme engine for AV styling)

## Tasks

### Task 1: Wire AttributeView Creation and Storage
**Files:** `src/core/AttributeViewStore.cpp`, `src/core/AttributeView.cpp`
**Description:** AttributeViewStore and AttributeView exist. Wire creation: users create a new Attribute View via command palette, specifying name and initial columns. Views stored in `.markamp/views/`.
**Acceptance Criteria:**
- "New Database" command creates AttributeView
- Initial columns configurable: Title (text), Tags (multi-select), Date (date)
- View stored as JSON in `.markamp/views/{id}.json`
- `AttributeViewCreatedEvent` emitted
- View ID generated and referenced in blocks

### Task 2: Wire AVTablePanel as Primary View
**Files:** `src/ui/AVTablePanel.cpp`, `src/ui/AVTablePanel.h`
**Description:** AVTablePanel exists. Wire it as the default view for Attribute Views: spreadsheet-style table with columns, rows, cell editing, and header controls.
**Acceptance Criteria:**
- Table renders columns with headers
- Rows render with cell values
- Click cell to edit inline
- Column resize by dragging header border
- Column reorder by dragging header
- Row add via "+" button at bottom

### Task 3: Wire AVKanbanPanel View
**Files:** `src/ui/AVKanbanPanel.cpp`, `src/ui/AVKanbanLane.cpp`, `src/ui/AVKanbanCard.cpp`
**Description:** AVKanbanPanel, AVKanbanLane, and AVKanbanCard exist. Wire as a Kanban board view: group rows by a select/multi-select column, render as lanes with draggable cards.
**Acceptance Criteria:**
- Kanban groups rows by selected column
- Each unique value creates a lane
- Cards draggable between lanes (updates column value)
- Card shows configurable fields (title + 2 preview fields)
- "Uncategorized" lane for empty values
- `AVKanbanCardMovedEvent` emitted on drag

### Task 4: Wire AVGalleryPanel View
**Files:** `src/ui/AVGalleryPanel.cpp`, `src/ui/AVGalleryCard.cpp`, `src/ui/AVGalleryConfig.cpp`
**Description:** AVGalleryPanel, AVGalleryCard, and AVGalleryConfig exist. Wire gallery view: rows displayed as cards in a grid layout.
**Acceptance Criteria:**
- Gallery shows rows as cards in grid
- Card layout configurable: cover image, title, properties
- Grid columns: 2, 3, 4 (configurable)
- Card click opens row detail view
- Cover image from URL or file column

### Task 5: Wire AVFilterBar for View Filtering
**Files:** `src/ui/AVFilterBar.cpp`, `src/core/AVFilter.cpp`
**Description:** AVFilterBar and AVFilter exist. Wire filter UI: add filter rules per column, operators vary by column type, filters combine with AND/OR.
**Acceptance Criteria:**
- "Add Filter" button opens filter row
- Column selector, operator selector, value input
- Text operators: contains, equals, starts with, is empty
- Number operators: =, !=, >, <, >=, <=
- Date operators: is, before, after, between
- Multiple filters with AND/OR toggle

### Task 6: Wire AVSort for View Sorting
**Files:** `src/core/AVSort.cpp`, `src/ui/AVTablePanel.cpp`
**Description:** AVSort exists. Wire multi-column sorting: click header to sort, shift-click for secondary sort, sort indicators in headers.
**Acceptance Criteria:**
- Click header sorts ascending
- Click again sorts descending
- Click again removes sort
- Shift+click adds secondary sort
- Sort indicator arrows in headers
- Multi-column sort (up to 3 levels)

### Task 7: Wire AVColumnType System
**Files:** `src/core/AVColumnType.cpp`, `src/ui/AVCellEditor.cpp`, `src/ui/AVCellRenderer.cpp`
**Description:** AVColumnType defines column types. Wire type-specific editors and renderers: Text, Number, Select, Multi-Select, Date, Checkbox, URL, Email, Phone, Formula, Relation, Rollup.
**Acceptance Criteria:**
- Text: single-line input
- Number: numeric input with format (decimal, currency, percent)
- Select: dropdown with color-coded options
- Multi-Select: tag-style multi-picker
- Date: date picker with optional time
- Checkbox: toggle
- URL: clickable link
- Each type has distinct renderer and editor

### Task 8: Wire AVRelationService for Cross-View Relations
**Files:** `src/core/AVRelationService.cpp`, `src/ui/AVRelationEditor.cpp`
**Description:** AVRelationService and AVRelationEditor exist. Wire relations: a column in one view can reference rows in another view, creating a database join.
**Acceptance Criteria:**
- Relation column type links to another AttributeView
- Relation editor shows searchable row picker
- Multiple relations per cell supported
- Clicking relation navigates to target row
- Bidirectional relations: target view shows backlinks

### Task 9: Wire AVRollupEngine for Computed Columns
**Files:** `src/core/AVRollupEngine.cpp`, `src/ui/AVRollupConfigDialog.cpp`
**Description:** AVRollupEngine and AVRollupConfigDialog exist. Wire rollup columns: compute aggregates (count, sum, average, min, max) over related rows.
**Acceptance Criteria:**
- Rollup column configured via dialog
- Source: relation column in current view
- Target: column in related view
- Aggregations: count, sum, average, min, max, percent filled
- Rollup values update automatically when source data changes
- Read-only display (computed values)

### Task 10: Wire AVQueryEngine for Advanced Queries
**Files:** `src/core/AVQueryEngine.cpp`
**Description:** AVQueryEngine exists for SQL-like queries on Attribute Views. Wire it to the query panel for ad-hoc database queries.
**Acceptance Criteria:**
- Query panel accessible from AV toolbar
- SQL-like syntax: `SELECT * FROM view WHERE column = value`
- Results displayed in temporary table view
- Query history maintained
- Export query results to CSV
- `AVQueryExecutedEvent` emitted

### Task 11: Wire AVValidators for Data Integrity
**Files:** `src/core/AVValidators.cpp`
**Description:** AVValidators exists. Wire validation rules per column: required, unique, min/max for numbers, regex for text, date range.
**Acceptance Criteria:**
- Required columns show error on empty
- Unique columns prevent duplicate values
- Number columns enforce min/max range
- Text columns support regex validation
- Validation errors shown inline with red border
- Validation runs on cell edit commit

### Task 12: Wire AVFormatters for Display
**Files:** `src/core/AVFormatters.cpp`, `src/ui/AVCellRenderer.cpp`
**Description:** AVFormatters exists. Wire formatters for display: number formats (1000 vs 1,000), date formats (ISO, localized), currency symbols, percent display.
**Acceptance Criteria:**
- Number format configurable per column
- Date format configurable: ISO, US, EU, relative
- Currency format with symbol selection
- Percent format with decimal places
- Formatters applied in all views (table, kanban, gallery)

### Task 13: Wire Inline AV Blocks in Editor
**Files:** `src/ui/EditorPanel.cpp`, `src/core/AttributeViewStore.cpp`
**Description:** Attribute Views can be embedded in Markdown documents as inline blocks. A special code fence renders the AV table inline in the editor/preview.
**Acceptance Criteria:**
- Syntax: ` ```av\nview: view_id\n``` ` embeds an AV
- Editor renders AV inline (read-only in editor, interactive in preview)
- Preview renders full interactive table
- Changes in inline AV update the source view
- Multiple inline AVs per document

### Task 14: Wire AV View Switching
**Files:** `src/ui/AVTablePanel.cpp`
**Description:** Users can switch between Table, Kanban, and Gallery views for the same data. View configuration (filters, sort, visible columns) saved per view.
**Acceptance Criteria:**
- View switcher in AV toolbar: Table, Kanban, Gallery
- Each view has independent filter/sort/column config
- Switching views preserves data
- Multiple named views per AttributeView
- "Add View" creates new view configuration

### Task 15: Wire AV Row Detail Panel
**Files:** `src/ui/AVTablePanel.cpp`
**Description:** Clicking a row opens a detail panel showing all columns as a form. Users can edit all values in a single view.
**Acceptance Criteria:**
- Row click opens detail panel (sidebar or modal)
- All columns shown as labeled fields
- Fields use type-specific editors
- Changes saved on field blur
- Navigation between rows (prev/next)
- Close detail panel returns to table view

### Task 16: Wire AV Import/Export
**Files:** `src/core/AttributeViewStore.cpp`
**Description:** Import from CSV and export to CSV/JSON. Column mapping UI for imports.
**Acceptance Criteria:**
- "Import CSV" maps CSV columns to AV columns
- Column type inference from data
- Preview before import
- "Export CSV" exports all rows with headers
- "Export JSON" exports structured data
- Date and number formatting in export

### Task 17: Wire AV Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register AV commands: "Database: New Database", "Database: Open Database", "Database: Add Column", "Database: Add Filter", "Database: Switch View", "Database: Import CSV", "Database: Export".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Database:" prefix
- Commands context-aware (only when AV is focused)
- Keyboard shortcuts for frequent operations

### Task 18: Wire AV Theme Integration
**Files:** `src/ui/AVTablePanel.cpp`, `src/ui/AVKanbanPanel.cpp`, `src/ui/AVGalleryPanel.cpp`
**Description:** All AV views use theme tokens: table borders, cell backgrounds, header styling, card borders, tag colors.
**Acceptance Criteria:**
- Table grid lines from `border_color` token
- Header background from `panel_header_bg` token
- Cell edit background from `input_bg` token
- Select tag colors complement theme accent
- All 64 themes render correctly

### Task 19: Wire AV Performance for Large Datasets
**Files:** `src/core/AttributeViewStore.cpp`, `src/ui/AVTablePanel.cpp`
**Description:** Virtualize table rendering for large datasets. Only visible rows rendered, with smooth scrolling and pagination.
**Acceptance Criteria:**
- 10,000 rows scrolls smoothly
- Virtual scrolling: only visible rows rendered
- Pagination option: 50, 100, 500 rows per page
- Sort and filter on 10,000 rows: < 200ms
- Memory: < 50MB for 10,000 rows with 20 columns

### Task 20: Add Attribute View Tests
**Files:** `tests/unit/test_attribute_view.cpp`
**Description:** Test AV system: column types, filtering, sorting, relations, rollups, validation, and serialization.
**Acceptance Criteria:**
- All column types: create, edit, validate
- Filter: all operators for all column types
- Sort: single and multi-column
- Relations: create, navigate, bidirectional
- Rollup: all aggregation functions
- Serialization round-trip

## Testing Requirements
- Column type CRUD for all types
- Filter and sort across all column types
- Relation and rollup computation accuracy
- Serialization preserves all view state

## Phase Completion Criteria
- Attribute Views creatable and editable
- Table, Kanban, and Gallery views functional
- Filtering, sorting, and validation working
- Relations and rollups computed correctly
- Inline AV blocks in Markdown documents
- All tests pass
