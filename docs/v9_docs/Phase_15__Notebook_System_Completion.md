# Phase 15: Notebook System Completion

## Overview
NotebookCellManager, KernelManager, CellOutputRenderer, DataFrameRenderer, MagicCommandManager, and NotebookExportEngine exist in core but the notebook surface is not reachable as a workspace mode. This phase connects notebook infrastructure to a functional user experience.

## Prerequisites
- Phase 06 (Workbench navigation with Notebook mode in activity bar)
- Phase 07 (Editor core for cell editing)

## Tasks

### Task 1: Register Notebook as First-Class Workspace Mode
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/WorkspaceLayout.cpp`
**Description:** Wire notebook mode into the layout manager panel registry. Selecting Notebook mode in the activity bar activates a notebook workspace with cell list, output area, and kernel status bar.
**Acceptance Criteria:**
- Notebook mode in activity bar shows notebook workspace panel
- Layout: cell list (flex), output sidebar (300px), kernel status bar (24px)
- Context bar shows notebook title and kernel selector
- Theme-aware: all notebook chrome uses theme tokens

### Task 2: Wire NotebookCellManager to Cell UI
**Files:** `src/core/NotebookCellManager.cpp`, `src/ui/CellOutputRenderer.cpp`
**Description:** NotebookCellManager manages cell lifecycle. Wire it to render cells as editable Scintilla instances with output areas below each code cell.
**Acceptance Criteria:**
- Code cells render with Scintilla editor and syntax highlighting
- Markdown cells render as formatted text (editable on double-click)
- Output area below code cells shows execution results
- Cell type indicator (Code/Markdown) in cell gutter
- Cell execution order number shown

### Task 3: Wire KernelManager for Code Execution
**Files:** `src/core/KernelManager.cpp`, `src/core/KernelManager.h`
**Description:** KernelManager exists for managing compute kernels. Wire kernel lifecycle: start, stop, restart, and interrupt. Connect cell execution to kernel via message protocol.
**Acceptance Criteria:**
- Kernel start/stop from toolbar and command palette
- Cell execution sends code to kernel, receives output
- Execution state: idle, busy, error shown in status bar
- Interrupt running execution (Cmd+.)
- Kernel restart clears all cell outputs

### Task 4: Wire CellOutputRenderer for Rich Output
**Files:** `src/ui/CellOutputRenderer.cpp`, `src/ui/CellOutputRenderer.h`
**Description:** CellOutputRenderer exists. Wire it to display multiple output types: text/plain, text/html, image/png, application/json, and error tracebacks.
**Acceptance Criteria:**
- Plain text output renders in monospace font
- HTML output renders in embedded webview
- Image output renders inline with zoom on click
- Error tracebacks render with red background and stack trace
- Multiple outputs per cell supported

### Task 5: Wire DataFrameRenderer for Tabular Output
**Files:** `src/ui/DataFrameRenderer.cpp`, `src/ui/DataFrameRenderer.h`
**Description:** DataFrameRenderer exists for displaying tabular data. Wire it to render DataFrames as scrollable tables with sorting and pagination.
**Acceptance Criteria:**
- DataFrame output renders as interactive table
- Column headers with sort on click
- Pagination for large DataFrames (50 rows per page)
- Row count and column count summary
- Copy cell/row/column to clipboard

### Task 6: Wire MagicCommandManager
**Files:** `src/core/MagicCommandManager.cpp`
**Description:** MagicCommandManager exists for Jupyter-style magic commands. Wire parsing and execution: `%time`, `%timeit`, `%run`, `%load`, `%who`, `%matplotlib inline`.
**Acceptance Criteria:**
- Line magics (%) parsed and executed before cell code
- Cell magics (%%) apply to entire cell
- `%time` and `%timeit` report execution timing
- `%run` executes external script
- `%who` lists variables in kernel namespace
- Unknown magic shows error with suggestions

### Task 7: Wire Cell Operations (Add, Delete, Move, Split, Merge)
**Files:** `src/core/NotebookCellManager.cpp`
**Description:** Complete cell manipulation operations: add cell above/below, delete cell, move cell up/down, split cell at cursor, merge cells.
**Acceptance Criteria:**
- Add cell: above (A) or below (B) current cell
- Delete cell: with undo support
- Move cell: up/down with keyboard shortcuts
- Split cell: at cursor position creates two cells
- Merge cells: merge selected cell with cell below
- All operations are undoable

### Task 8: Wire Cell Execution Controls
**Files:** `src/ui/MainFrame.cpp`, `src/core/NotebookCellManager.cpp`
**Description:** Wire execution commands: Run Cell (Shift+Enter), Run Cell and Move Below, Run All Cells, Run All Above, Run All Below, Clear All Outputs.
**Acceptance Criteria:**
- Shift+Enter runs current cell, moves to next
- Ctrl+Enter runs current cell, stays in place
- "Run All" executes all cells sequentially
- "Run All Above" runs cells above current
- "Clear All Outputs" clears all cell outputs
- Execution order numbers update correctly

### Task 9: Wire NotebookExportEngine
**Files:** `src/core/NotebookExportEngine.cpp`
**Description:** NotebookExportEngine exists. Wire export to formats: .ipynb (Jupyter), .md (Markdown), .html (static page), .pdf (via HTML).
**Acceptance Criteria:**
- Export to .ipynb preserves cell types, source, and outputs
- Export to Markdown renders cells as code blocks with output
- Export to HTML creates standalone page with styling
- Export command in command palette and File menu
- `NotebookExportedEvent` emitted with format and path

### Task 10: Wire Notebook Serialization
**Files:** `src/core/NotebookCellManager.cpp`, `src/core/Config.h`
**Description:** Notebooks saved as .markamp-nb files (JSON). Auto-save on change (debounced 2 seconds). Load restores cells, outputs, and kernel state reference.
**Acceptance Criteria:**
- Notebook saved as JSON with cells, outputs, metadata
- Auto-save on change (2 second debounce)
- Load restores cell order, types, source, and cached outputs
- `NotebookSavedEvent` emitted on save
- File watcher detects external changes

### Task 11: Wire Variable Inspector
**Files:** `src/core/KernelManager.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Variable inspector panel shows all variables in the kernel namespace with type, shape, and value preview.
**Acceptance Criteria:**
- Variable list updates after each cell execution
- Columns: name, type, shape/size, value preview
- Click variable to inspect in detail panel
- Filter/search variables
- Clear variables command

### Task 12: Wire Kernel Selector
**Files:** `src/core/KernelManager.cpp`, `src/ui/StatusBarPanel.cpp`
**Description:** Kernel selector in status bar shows active kernel. Clicking shows available kernels. Support Python, JavaScript, and shell kernels.
**Acceptance Criteria:**
- Status bar shows kernel name and status
- Click opens kernel selector dropdown
- Available kernels discovered from system
- Switching kernel restarts with new language
- Kernel connection status: connected, disconnected, error

### Task 13: Wire Cell Autocomplete
**Files:** `src/core/KernelManager.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Code cells support autocomplete from kernel introspection. Tab triggers completion popup with suggestions from kernel namespace.
**Acceptance Criteria:**
- Tab triggers autocomplete in code cells
- Completions sourced from kernel introspection
- Completion popup shows name, type, and docstring preview
- Fuzzy matching on typed prefix
- Escape dismisses popup

### Task 14: Wire Cell Toolbar
**Files:** `src/ui/CellOutputRenderer.cpp`
**Description:** Each cell has a toolbar on hover: Run, Cell Type toggle, Move Up, Move Down, Delete. Toolbar appears on hover, stays on focus.
**Acceptance Criteria:**
- Toolbar appears on cell hover
- Run button executes cell
- Type toggle switches Code/Markdown
- Move buttons reorder cell
- Delete button with confirmation for non-empty cells
- Toolbar uses theme tokens

### Task 15: Wire Notebook Outline
**Files:** `src/ui/ToolWindowHost.cpp`
**Description:** Notebook outline panel shows Markdown headings from markdown cells as a navigable table of contents.
**Acceptance Criteria:**
- Outline extracts headings from markdown cells
- Heading hierarchy shown with indentation
- Click heading scrolls to cell
- Outline updates on cell edit
- Current section highlighted

### Task 16: Wire Notebook Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register notebook commands: "Notebook: New Notebook", "Notebook: Run All Cells", "Notebook: Clear All Outputs", "Notebook: Restart Kernel", "Notebook: Export", "Notebook: Change Kernel", "Notebook: Toggle Line Numbers".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Notebook:" prefix
- Commands only active when notebook mode is active
- Keyboard shortcuts assigned for frequent operations

### Task 17: Wire Notebook Theme Integration
**Files:** `src/ui/CellOutputRenderer.cpp`, `src/ui/DataFrameRenderer.cpp`
**Description:** Notebook cells, outputs, and chrome use theme tokens. Cell backgrounds, borders, output areas, and toolbar all theme-aware.
**Acceptance Criteria:**
- Code cell background from `editor_bg` token
- Markdown cell background from `panel_bg` token
- Output area background distinct from cell
- Active cell border uses accent color
- Error output uses error color token

### Task 18: Wire Notebook Diff Integration
**Files:** `src/core/NotebookDiffEngine.cpp`, `src/ui/DiffPanel.cpp`
**Description:** NotebookDiffEngine exists. Wire it to show cell-level diffs when comparing notebook versions.
**Acceptance Criteria:**
- Diff shows added/removed/modified cells
- Cell source diff shown inline
- Output changes indicated but not fully diffed
- Side-by-side and unified views
- Navigate between changed cells

### Task 19: Wire Notebook Performance
**Files:** `src/core/NotebookCellManager.cpp`, `src/ui/CellOutputRenderer.cpp`
**Description:** Virtualize cell rendering for large notebooks. Only cells in viewport are fully rendered; off-screen cells use placeholder height.
**Acceptance Criteria:**
- Notebooks with 100+ cells scroll smoothly
- Only visible cells fully rendered
- Cell heights cached for scroll calculation
- Lazy output rendering for large outputs
- Memory budget: 200MB max for all cell outputs

### Task 20: Add Notebook Tests
**Files:** `tests/unit/test_notebook_cells.cpp`, `tests/unit/test_notebook_export.cpp`, `tests/unit/test_cell_output.cpp`
**Description:** Test notebook system: cell lifecycle, execution, serialization, export, and output rendering.
**Acceptance Criteria:**
- Cell CRUD operations test
- Cell execution order tracking
- Notebook serialization round-trip
- Export to all formats
- Output type detection and rendering

## Testing Requirements
- Cell lifecycle: create, edit, execute, move, delete
- Kernel communication: execute, interrupt, restart
- Serialization: save/load preserves all state
- Export: all formats produce valid output

## Phase Completion Criteria
- Notebook reachable as first-class workspace mode
- Cell editing with Scintilla and output rendering
- Kernel management with execution
- Magic commands functional
- Export to .ipynb, .md, .html
- All tests pass
