# Phase 48: Data Processing and Computation

## Overview
The product vision includes Jupyter-class computation. KernelManager, MagicCommandManager, and CellOutputRenderer exist but the data processing pipeline (data visualization, inline computation, variable inspection) is not fully realized. This phase builds the computational backbone for notebook and editor surfaces.

## Prerequisites
- Phase 15 (Notebook system completion)
- Phase 21 (Attribute View for data display)

## Tasks

### Task 1: Wire Data Visualization in Notebooks
**Files:** `src/ui/CellOutputRenderer.cpp`, `src/ui/DataFrameRenderer.cpp`
**Description:** Wire inline data visualization: chart output from code cells renders as interactive charts (bar, line, scatter, pie).
**Acceptance Criteria:**
- Chart output type detection (matplotlib, plotly, vega-lite formats)
- Bar chart rendering with axis labels
- Line chart with zoom and pan
- Scatter plot with tooltip on hover
- Pie chart with legend
- Charts themed with application theme

### Task 2: Wire Inline Code Execution in Editor
**Files:** `src/ui/EditorPanel.cpp`, `src/core/KernelManager.cpp`
**Description:** Execute code blocks in Markdown documents: "Run Code Block" command executes and shows output below the block.
**Acceptance Criteria:**
- "Run Code Block" (Cmd+Shift+Enter) executes code block at cursor
- Output rendered below code block (inline)
- Output types: text, image, table, chart
- Output persisted in document (as comment or hidden block)
- Clear output command
- Multiple code blocks executable sequentially

### Task 3: Wire Computed Markdown Variables
**Files:** `src/core/TemplateEngine.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Computed variables in Markdown: `{{= expression }}` evaluates expression and renders result inline.
**Acceptance Criteria:**
- `{{= 2 + 2 }}` renders as "4"
- `{{= today() }}` renders current date
- `{{= word_count() }}` renders document word count
- Expressions evaluated on render (not in source)
- Error: expression shown with error tooltip
- Custom functions definable in config

### Task 4: Wire Data Connector Framework
**Files:** `src/core/DataConnector.h`, `src/core/DataConnector.cpp`
**Description:** Create DataConnector framework: connect to data sources (CSV, JSON, SQLite, REST API) for use in notebooks and computed blocks.
**Acceptance Criteria:**
- CSV connector: load CSV files as data frames
- JSON connector: load JSON files/URLs
- SQLite connector: query local databases
- REST API connector: GET/POST with authentication
- Connection configuration in workspace settings
- `DataConnectorLoadedEvent` emitted

### Task 5: Wire DataFrame Operations
**Files:** `src/ui/DataFrameRenderer.cpp`
**Description:** DataFrame operations in notebook output: filter, sort, group, aggregate, pivot. Interactive data exploration.
**Acceptance Criteria:**
- Filter: column filters with operators
- Sort: click column header
- Group by: aggregate by column
- Pivot: create pivot table
- Pagination: 50 rows per page
- Export: filtered data to CSV

### Task 6: Wire Variable Inspector for Notebooks
**Files:** `src/core/KernelManager.cpp`, `src/ui/ToolWindowHost.cpp`
**Description:** Variable inspector panel: shows all variables in kernel namespace with type, size, and interactive value preview.
**Acceptance Criteria:**
- Variable list with name, type, size
- Value preview: scalars shown inline
- DataFrame: click to show in DataFrameRenderer
- Array/list: expandable tree view
- Object: property inspector
- Variables update after each cell execution

### Task 7: Wire Notebook Widgets
**Files:** `src/ui/CellOutputRenderer.cpp`
**Description:** Interactive widgets in notebook output: sliders, dropdowns, text inputs that update cell output when changed.
**Acceptance Criteria:**
- Slider widget: numeric input with range
- Dropdown widget: selection from options
- Text input widget: string input
- Checkbox widget: boolean toggle
- Widget value change re-executes cell
- Widget state persisted in notebook

### Task 8: Wire Data Import from Clipboard
**Files:** `src/core/ClipboardService.cpp`
**Description:** Paste tabular data (from Excel, Google Sheets) into notebook cell as DataFrame initialization code.
**Acceptance Criteria:**
- Paste tab-separated data: create DataFrame code
- Detect column headers from first row
- Type inference for columns
- Preview before inserting
- Alternative: paste as Markdown table
- Paste format: configurable preference

### Task 9: Wire SQL Query in Notebooks
**Files:** `src/core/SqlQuery.cpp`, `src/core/KernelManager.cpp`
**Description:** SqlQuery exists. Wire SQL cells in notebooks: `%%sql` magic command executes SQL queries against connected databases.
**Acceptance Criteria:**
- `%%sql` cell magic for SQL execution
- Query connected SQLite databases
- Results rendered as DataFrameRenderer table
- Query history maintained
- Syntax highlighting for SQL
- Query parameters from notebook variables

### Task 10: Wire Data Export from Notebooks
**Files:** `src/core/NotebookExportEngine.cpp`
**Description:** Export notebook data: export DataFrame outputs to CSV, JSON, Excel. Export charts to PNG, SVG.
**Acceptance Criteria:**
- DataFrame: export to CSV, JSON
- Charts: export to PNG (2x), SVG
- Export specific cell output
- Export all outputs as data package
- Export command per cell output
- Batch export: all data in notebook

### Task 11: Wire Code Cell Language Support
**Files:** `src/core/KernelManager.cpp`, `src/ui/CellOutputRenderer.cpp`
**Description:** Multiple languages in notebooks: Python, JavaScript, Shell. Each cell can specify language.
**Acceptance Criteria:**
- Python cells: full Python support
- JavaScript cells: Node.js execution
- Shell cells: bash/zsh execution
- Language selector per cell
- Syntax highlighting per language
- Multiple kernels active simultaneously

### Task 12: Wire Notebook Cell Dependencies
**Files:** `src/core/NotebookCellManager.cpp`
**Description:** Track cell dependencies: if cell A defines variable X and cell B uses X, cell B depends on A. Re-execute dependencies when upstream changes.
**Acceptance Criteria:**
- Variable reference tracking across cells
- Dependency graph visualization
- "Run Dependencies" executes all upstream cells
- Stale cell indicator: cell output may be outdated
- Circular dependency detection
- Dependency graph in notebook outline

### Task 13: Wire Inline Plot Configuration
**Files:** `src/ui/CellOutputRenderer.cpp`
**Description:** Interactive plot configuration: click chart elements to customize colors, labels, axes, legend position.
**Acceptance Criteria:**
- Chart toolbar on hover: zoom, pan, save, settings
- Axis labels editable
- Legend position configurable
- Color scheme selectable
- Grid toggle
- Save configured chart as image

### Task 14: Wire Notebook Execution Progress
**Files:** `src/ui/CellOutputRenderer.cpp`, `src/core/KernelManager.cpp`
**Description:** Show execution progress: per-cell timer, total execution time, progress bar for known-duration operations.
**Acceptance Criteria:**
- Cell execution timer: elapsed time shown
- Total notebook execution time tracked
- Progress bar if operation reports progress
- Cancel button for long-running cells
- Estimated time remaining (if available)
- Execution history per cell

### Task 15: Wire Computation Cache
**Files:** `src/core/KernelManager.cpp`
**Description:** Cache cell outputs: if cell source hasn't changed, reuse previous output instead of re-executing. Cache invalidated when dependencies change.
**Acceptance Criteria:**
- Cell output cached by source hash
- Cache hit: skip execution, show cached output
- Cache invalidation: source change or dependency change
- Cache size limit: configurable (default 100MB)
- "Clear Cache" command
- Cache statistics visible

### Task 16: Wire Data Processing Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register data commands: "Notebook: Run All Cells", "Data: Import CSV", "Data: Export Results", "Variables: Show Inspector", "SQL: Open Query Panel".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Notebook:", "Data:", "SQL:"
- "Import CSV" opens file picker
- "Show Inspector" opens variable panel
- Keyboard shortcuts for execution

### Task 17: Wire Computation Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Computation settings: default kernel, execution timeout, cache size, auto-execute on open, output size limits.
**Acceptance Criteria:**
- Default kernel selection
- Execution timeout: seconds (default 30)
- Cache size: MB (default 100)
- Auto-execute on notebook open: toggle
- Output size limit: max rows for DataFrames
- Settings section: "Computation"

### Task 18: Wire Computation Safety
**Files:** `src/core/KernelManager.cpp`
**Description:** Safety for code execution: resource limits, execution timeout, dangerous operation warning.
**Acceptance Criteria:**
- Execution timeout: kill cell after limit
- Memory limit: per-kernel (configurable)
- Dangerous operations: `rm`, `del`, `drop` show warning
- Network access: configurable (allow/deny)
- File system access: scoped to workspace
- Safety override: opt-in per notebook

### Task 19: Wire Computation Theme Integration
**Files:** `src/ui/CellOutputRenderer.cpp`, `src/ui/DataFrameRenderer.cpp`
**Description:** Data visualization and computation UI uses theme tokens: chart colors, table styling, variable inspector.
**Acceptance Criteria:**
- Chart colors from theme palette (6 distinct colors)
- Table alternating rows from theme
- Variable inspector uses panel theming
- Widget controls match theme styling
- Progress bar uses accent color

### Task 20: Add Computation Tests
**Files:** `tests/unit/test_computation.cpp`
**Description:** Test computation system: kernel execution, DataFrame operations, cache, SQL queries, and safety limits.
**Acceptance Criteria:**
- Cell execution produces correct output
- DataFrame operations: filter, sort, group
- Cache: hit on unchanged, invalidate on change
- SQL query: correct results from test database
- Safety: timeout kills execution
- Variable inspector: correct type detection

## Testing Requirements
- Cell execution and output rendering
- DataFrame operations accuracy
- Cache correctness and invalidation
- Safety limits enforcement

## Phase Completion Criteria
- Data visualization in notebooks (charts, tables)
- Inline code execution in editor
- Variable inspector
- Data connectors (CSV, JSON, SQLite)
- SQL query cells
- Computation cache
- All tests pass
