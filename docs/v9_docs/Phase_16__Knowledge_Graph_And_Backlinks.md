# Phase 16: Knowledge Graph and Backlinks

## Overview
BacklinkIndex, BacklinkService, GraphEngine (LocalGraphEngine), GraphViewPanel, GraphPanel, GraphFilterBar, and GraphMiniMap exist but the knowledge graph surface is not integrated as a workspace mode. BacklinkIndex tracks links but the UI (BacklinksPanel, GraphViewPanel) is not wired to display them. This phase completes the knowledge graph experience.

## Prerequisites
- Phase 06 (Workbench navigation with Graph mode in activity bar)
- Phase 07 (Editor for wiki-link editing)

## Tasks

### Task 1: Register Knowledge Graph as First-Class Workspace Mode
**Files:** `src/ui/LayoutManager.cpp`, `src/ui/GraphViewPanel.cpp`, `src/ui/GraphViewPanel.h`
**Description:** Wire GraphViewPanel into the layout manager panel registry. Selecting Graph mode in the activity bar activates the graph workspace with the force-directed graph visualization, filter bar, and minimap.
**Acceptance Criteria:**
- Graph mode in activity bar shows GraphViewPanel
- Layout: filter bar (32px), graph area (flex), minimap (120px)
- Graph renders all documents as nodes, links as edges
- Theme-aware: node colors, edge colors, background from theme tokens

### Task 2: Wire BacklinkIndex to Real-Time Updates
**Files:** `src/core/BacklinkIndex.cpp`, `src/core/BacklinkIndex.h`
**Description:** BacklinkIndex tracks wiki-link references. Wire it to update on file save, file create, file delete. Index must reflect current workspace state at all times.
**Acceptance Criteria:**
- File save triggers re-scan of outgoing links
- File create adds node to index
- File delete removes node and updates all references
- `BacklinkIndexUpdatedEvent` emitted on change
- Index rebuild on workspace open

### Task 3: Wire BacklinksPanel to Show Incoming Links
**Files:** `src/ui/BacklinksPanel.cpp`, `src/ui/BacklinksPanel.h`
**Description:** BacklinksPanel exists. Wire it to show all documents that link to the currently active document. Each entry shows file name, link context (surrounding text), and click to navigate.
**Acceptance Criteria:**
- Panel shows backlinks for active document
- Each entry: file name, line number, context snippet (50 chars)
- Click navigates to the linking document at the link location
- Count badge shows total backlink count
- Updates when active document changes

### Task 4: Wire GraphViewPanel Node Rendering
**Files:** `src/ui/GraphViewPanel.cpp`, `src/core/LocalGraphEngine.cpp`
**Description:** GraphViewPanel and LocalGraphEngine exist. Wire force-directed layout rendering: documents as circles, links as lines, node size by link count, interactive drag.
**Acceptance Criteria:**
- Each document renders as a labeled circle
- Node size proportional to link count (min 8px, max 32px)
- Edges drawn between linked documents
- Force-directed layout with physics simulation
- Nodes draggable (pin position on drag)

### Task 5: Wire Graph Interaction (Zoom, Pan, Select)
**Files:** `src/ui/GraphViewPanel.cpp`, `src/ui/GraphPanel.cpp`
**Description:** Wire graph interaction: scroll to zoom, drag to pan, click node to select and show details, double-click to open document.
**Acceptance Criteria:**
- Scroll wheel zooms centered on cursor
- Drag empty space pans the view
- Click node selects it (highlights node and connections)
- Double-click opens document in editor
- Right-click node shows context menu (Open, Reveal in Explorer, Show Backlinks)

### Task 6: Wire GraphFilterBar
**Files:** `src/ui/GraphFilterBar.cpp`, `src/ui/GraphFilterBar.h`
**Description:** GraphFilterBar exists. Wire filters: tag filter (show only nodes with tag), path filter (show only nodes in directory), link depth (1-hop, 2-hop, all), orphan toggle.
**Acceptance Criteria:**
- Tag filter dropdown shows all tags in workspace
- Path filter with directory picker
- Link depth slider: 1, 2, 3, all
- "Show Orphans" toggle shows/hides unlinked nodes
- Filters combine (AND logic)
- Filter state persisted for session

### Task 7: Wire GraphMiniMap
**Files:** `src/ui/GraphMiniMap.cpp`, `src/ui/GraphMiniMap.h`
**Description:** GraphMiniMap exists. Wire it to show a scaled-down view of the entire graph with viewport indicator. Click to navigate.
**Acceptance Criteria:**
- Minimap shows all nodes at scale
- Current viewport highlighted
- Click minimap pans graph to that area
- Drag viewport in minimap pans in real-time
- Minimap updates on graph changes

### Task 8: Wire Local Graph View
**Files:** `src/core/LocalGraphEngine.cpp`, `src/ui/GraphViewPanel.cpp`
**Description:** Local graph shows only the neighborhood of the active document: direct links (1-hop) and their connections. Automatically focused on the current file.
**Acceptance Criteria:**
- Toggle: Global Graph / Local Graph
- Local graph shows current document centered
- 1-hop and 2-hop neighbors shown
- Current document node highlighted with accent color
- Updates when active document changes

### Task 9: Wire WikiLink Creation from Graph
**Files:** `src/ui/GraphViewPanel.cpp`, `src/core/WikiLinkParser.cpp`
**Description:** Users can create links by dragging from one node to another in the graph view. This inserts a wiki-link in the source document.
**Acceptance Criteria:**
- Drag from node A to node B starts link creation
- Drop creates `[[Node B]]` wiki-link at end of Node A
- Confirmation dialog shows what will be inserted
- Undo support for the inserted link
- `WikiLinkCreatedEvent` emitted

### Task 10: Wire Tag Graph Overlay
**Files:** `src/core/TagService.cpp`, `src/ui/GraphViewPanel.cpp`
**Description:** Tags can be visualized as nodes in the graph. Documents with the same tag are connected through the tag node, showing tag-based clusters.
**Acceptance Criteria:**
- Toggle: "Show Tags as Nodes"
- Tag nodes rendered as diamonds (distinct from document circles)
- Documents connected to their tag nodes
- Tag node size proportional to document count
- Tag nodes use tag color if defined

### Task 11: Wire Graph Cluster Detection
**Files:** `src/core/LocalGraphEngine.cpp`
**Description:** Detect clusters of densely connected documents. Highlight clusters with distinct background colors. Show cluster list in sidebar.
**Acceptance Criteria:**
- Cluster detection algorithm (community detection)
- Each cluster has a distinct background tint
- Cluster list in sidebar shows name (most common tag or directory)
- Click cluster zooms to fit cluster
- Toggle cluster coloring on/off

### Task 12: Wire Unlinked Mentions Detection
**Files:** `src/core/BacklinkIndex.cpp`, `src/ui/BacklinksPanel.cpp`
**Description:** Detect mentions of document titles in other documents that are not wiki-linked. Show these as "Unlinked Mentions" in the backlinks panel with a "Link" action.
**Acceptance Criteria:**
- Scan documents for title text that matches other document names
- Show unlinked mentions in separate section of BacklinksPanel
- Each mention shows context and "Link It" action
- "Link It" wraps the text in `[[ ]]` wiki-link syntax
- Debounced scan (5 second delay after file change)

### Task 13: Wire Graph Search
**Files:** `src/ui/GraphViewPanel.cpp`
**Description:** Search within the graph view: type to filter nodes, matching nodes highlighted, non-matching nodes dimmed.
**Acceptance Criteria:**
- Search input in graph toolbar
- Matching nodes highlighted with accent border
- Non-matching nodes dimmed (50% opacity)
- Navigate between matches with arrows
- Escape clears search and restores all nodes

### Task 14: Wire Graph Export
**Files:** `src/ui/GraphViewPanel.cpp`
**Description:** Export graph as SVG or PNG image. Export includes all visible nodes and edges with current zoom and position.
**Acceptance Criteria:**
- Export to SVG preserves vector quality
- Export to PNG at 2x resolution
- Export respects current filters and zoom
- Command: "Graph: Export as Image"
- File save dialog for output location

### Task 15: Wire Graph Statistics Panel
**Files:** `src/ui/GraphViewPanel.cpp`
**Description:** Show graph statistics: total nodes, total edges, average connections, most connected nodes, orphan count, cluster count.
**Acceptance Criteria:**
- Statistics panel toggleable in graph workspace
- Metrics: node count, edge count, density, avg degree
- Top 10 most connected documents
- Orphan count with list
- Cluster count

### Task 16: Wire Graph Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register graph commands: "Graph: Open Global Graph", "Graph: Open Local Graph", "Graph: Zoom to Fit", "Graph: Show Orphans", "Graph: Filter by Tag", "Graph: Export".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Graph:" prefix
- Commands only active when graph mode is active
- Keyboard shortcuts for frequent operations

### Task 17: Wire Graph Theme Integration
**Files:** `src/ui/GraphViewPanel.cpp`, `src/ui/GraphPanel.cpp`
**Description:** Graph background, node colors, edge colors, selection highlight, and filter bar all use theme tokens. Theme change updates graph immediately.
**Acceptance Criteria:**
- Graph background from `canvas_bg` token
- Node fill from `graph_node` token
- Edge color from `graph_edge` token
- Selected node uses accent color
- All 64 themes render correctly

### Task 18: Wire Graph Performance for Large Workspaces
**Files:** `src/core/LocalGraphEngine.cpp`, `src/ui/GraphViewPanel.cpp`
**Description:** Optimize graph rendering for workspaces with 1000+ documents. Use spatial indexing for viewport culling, level-of-detail for distant nodes.
**Acceptance Criteria:**
- 1000 nodes renders at 30fps
- Viewport culling: only visible nodes rendered in detail
- Distant nodes render as dots (no labels)
- Physics simulation pauses when graph is idle
- Memory budget: 100MB max for graph data

### Task 19: Wire Backlink Count in File Explorer
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/BacklinkIndex.cpp`
**Description:** Show backlink count badge next to each file in the file explorer tree. Files with many backlinks are visually prominent.
**Acceptance Criteria:**
- Backlink count badge next to file name
- Badge only shown for files with > 0 backlinks
- Badge updates on backlink index change
- Badge format: "3" for counts, "9+" for large counts
- Toggle badge visibility in settings

### Task 20: Add Knowledge Graph Tests
**Files:** `tests/unit/test_graph_engine.cpp`, `tests/unit/test_graph_view_panel.cpp`, `tests/unit/test_backlink_index.cpp`, `tests/unit/test_local_graph.cpp`
**Description:** Test graph system: backlink indexing, graph layout, filtering, and interaction.
**Acceptance Criteria:**
- Backlink index: add, remove, update links correctly
- Graph layout: nodes positioned without overlap
- Filters: tag, path, depth all filter correctly
- Local graph: correct neighborhood extraction
- Unlinked mentions: detection accuracy

## Testing Requirements
- Backlink index accuracy across file operations
- Graph layout convergence and performance
- Filter composition correctness
- Serialization of graph state (positions, filters)

## Phase Completion Criteria
- Knowledge graph reachable as first-class workspace mode
- Backlinks panel shows incoming links with context
- Force-directed graph with zoom, pan, select
- Filters: tag, path, depth, orphans
- Local graph centered on active document
- Unlinked mentions detection
- All tests pass
