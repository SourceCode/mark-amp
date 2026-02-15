# Phase 06 -- Backlinks Panel and Backlink Visualization

## Objective

Implement the Backlinks Panel UI: a sidebar panel that displays all documents linking to the currently active document. Shows both explicit wikilinks (linked mentions) and unlinked mentions (text occurrences of the document's title). Supports filtering, sorting, and one-click navigation to the linking document. Also implements the per-document local graph visualization (a mini graph showing the document's immediate link neighborhood).

## Prerequisites

- Phase 03 (BacklinkIndex -- data model and queries)
- Phase 02 (VaultService -- document access)
- Existing UI infrastructure (MainFrame, sidebar panels, ThemeEngine)

## Feature References (PRD)

- PRD #5: Backlinks Panel
- PRD #36: Backlink Visualization Per Note

## Data Structures to Implement

### File: `src/ui/BacklinksPanel.h`

```cpp
#pragma once

#include "core/BacklinkIndex.h"

#include <wx/panel.h>
#include <wx/listctrl.h>
#include <wx/srchctrl.h>
#include <wx/notebook.h>

#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
class VaultService;
class BacklinkIndex;
class ThemeEngine;
}

namespace markamp::ui
{

/// Panel displaying backlinks to the current document.
/// Tabs: "Linked Mentions" and "Unlinked Mentions"
class BacklinksPanel : public wxPanel
{
public:
    BacklinksPanel(wxWindow* parent,
                   markamp::core::EventBus& event_bus,
                   markamp::core::VaultService& vault_service,
                   markamp::core::BacklinkIndex& backlink_index,
                   markamp::core::ThemeEngine& theme_engine);

    /// Refresh the panel for the given document ID.
    auto show_backlinks_for(const std::string& document_id) -> void;

    /// Clear all displayed backlinks.
    auto clear() -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::VaultService& vault_service_;
    markamp::core::BacklinkIndex& backlink_index_;
    markamp::core::ThemeEngine& theme_engine_;

    wxNotebook* tab_notebook_{nullptr};
    wxPanel* linked_panel_{nullptr};
    wxPanel* unlinked_panel_{nullptr};
    wxSearchCtrl* filter_ctrl_{nullptr};

    std::string current_document_id_;
    markamp::core::BacklinkResult current_result_;

    auto create_ui() -> void;
    auto populate_linked_mentions() -> void;
    auto populate_unlinked_mentions() -> void;
    auto apply_filter(const std::string& filter_text) -> void;
    auto apply_theme() -> void;

    // Click handler: navigate to the source document
    auto on_backlink_clicked(const std::string& source_doc_id, int line_number) -> void;

    // Convert unlinked mention to explicit wikilink
    auto on_link_mention(const markamp::core::MentionEntry& mention) -> void;

    markamp::core::Subscription active_file_sub_;
    markamp::core::Subscription backlink_updated_sub_;
    markamp::core::Subscription theme_changed_sub_;
};

/// Mini graph panel showing the local link neighborhood of the current document.
class LocalGraphPanel : public wxPanel
{
public:
    LocalGraphPanel(wxWindow* parent,
                    markamp::core::EventBus& event_bus,
                    markamp::core::BacklinkIndex& backlink_index,
                    markamp::core::ThemeEngine& theme_engine);

    /// Show the local graph centered on a document.
    auto show_graph_for(const std::string& document_id) -> void;

    auto clear() -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::BacklinkIndex& backlink_index_;
    markamp::core::ThemeEngine& theme_engine_;

    std::string center_document_id_;
    std::vector<std::string> connected_ids_;

    // Simple node layout (circular arrangement around center)
    struct NodePosition
    {
        std::string document_id;
        std::string label;
        double x{0.0};
        double y{0.0};
        double radius{6.0};
        bool is_center{false};
    };
    std::vector<NodePosition> nodes_;
    std::vector<std::pair<int, int>> edges_; // index pairs into nodes_

    auto compute_layout() -> void;
    auto on_paint(wxPaintEvent& event) -> void;
    auto on_click(wxMouseEvent& event) -> void;
    auto apply_theme() -> void;

    markamp::core::Subscription active_file_sub_;
    markamp::core::Subscription theme_changed_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`BacklinksPanel::show_backlinks_for(document_id)`** -- Store document_id. Call backlink_index_.get_backlink_result(document_id). Store result. Call populate_linked_mentions() and populate_unlinked_mentions(). Update tab labels with counts.

2. **`populate_linked_mentions()`** -- Clear linked_panel_. For each BacklinkEntry in current_result_.linked_mentions: create a list item showing source document title, context snippet (with the link highlighted), and line number. Group by source document.

3. **`populate_unlinked_mentions()`** -- For each MentionEntry: show source title, matched text highlighted in context, and a "Link" button that converts the mention to an explicit `[[wikilink]]`.

4. **`on_backlink_clicked(source_doc_id, line)`** -- Publish a FileOpenRequestEvent with the source document path and line number for the editor to navigate to.

5. **`on_link_mention(mention)`** -- Open the source document. Find the mention text at the given line. Wrap it in `[[...]]` wikilink syntax. Save the document. Refresh the backlinks panel.

6. **`LocalGraphPanel::show_graph_for(document_id)`** -- Get connected documents from BacklinkIndex. Build node list with center node and neighbors. Compute circular layout. Trigger repaint.

7. **`LocalGraphPanel::compute_layout()`** -- Place center node at canvas center. Arrange connected nodes in a circle around it. Compute edges between linked nodes.

8. **`LocalGraphPanel::on_paint(event)`** -- Draw edges as lines. Draw nodes as circles (sized by connection count). Draw labels. Center node uses accent color. Apply theme colors.

9. **`LocalGraphPanel::on_click(event)`** -- Hit-test click position against node circles. If a node is clicked, publish FileOpenRequestEvent to navigate to that document.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(FileOpenRequestEvent)
std::string file_path;
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BacklinksPanelRefreshedEvent)
std::string document_id;
int linked_count{0};
int unlinked_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Config Keys to Add

| Key | Type | Default | Description |
|-----|------|---------|-------------|
| `backlinks.show_unlinked` | bool | `true` | Show unlinked mentions tab |
| `backlinks.context_lines` | int | `2` | Lines of context around each mention |
| `backlinks.sort_by` | string | `"modified"` | Sort: modified, name, count |
| `local_graph.depth` | int | `1` | BFS depth for local graph |
| `local_graph.show_labels` | bool | `true` | Show node labels in local graph |

## Test Cases

File: `tests/unit/test_backlinks_panel.cpp`

1. **Panel displays linked mentions** -- Set up documents with backlinks. Verify correct entries shown.
2. **Panel displays unlinked mentions** -- Text mention of doc title. Verify shown in unlinked tab.
3. **Filter backlinks** -- Type filter text. Verify only matching entries remain visible.
4. **Click navigates to source** -- Click a backlink entry. Verify FileOpenRequestEvent published.
5. **Link mention converts text** -- Click "Link" on unlinked mention. Verify text wrapped in `[[]]`.
6. **Local graph shows neighbors** -- Center node has 3 neighbors. Verify 4 nodes displayed.
7. **Local graph click navigation** -- Click a neighbor node. Verify navigation event published.
8. **Theme change updates colors** -- Publish ThemeChangedEvent. Verify panel repaints with new theme.
9. **Auto-refresh on active file change** -- Change active file. Verify panel updates to new doc's backlinks.
10. **Empty state** -- Document with no backlinks. Verify "No backlinks found" message shown.

## Acceptance Criteria

- [ ] Backlinks panel shows linked mentions grouped by source document
- [ ] Unlinked mentions tab shows text occurrences with "Link" button
- [ ] Clicking a backlink navigates to the source document at the correct line
- [ ] "Link" button wraps unlinked mentions in [[wikilink]] syntax
- [ ] Filter input narrows displayed results
- [ ] Local graph renders center node with connected neighbors
- [ ] Graph node clicks navigate to the clicked document
- [ ] Panel auto-refreshes when active document changes
- [ ] Theme integration uses ThemeEngine colors throughout
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/BacklinksPanel.h` | BacklinksPanel and LocalGraphPanel |
| CREATE | `src/ui/BacklinksPanel.cpp` | Full UI implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Register BacklinksPanel in sidebar |
| MODIFY | `src/core/Events.h` | Add FileOpenRequestEvent, BacklinksPanelRefreshedEvent |
| MODIFY | `src/CMakeLists.txt` | Add BacklinksPanel.cpp |
| CREATE | `tests/unit/test_backlinks_panel.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_backlinks_panel target |

## Architecture Notes

- BacklinksPanel subscribes to ActiveFileChangedEvent for auto-refresh
- LocalGraphPanel is a lightweight custom-drawn wxPanel (no external graph library)
- The full interactive Graph View (Phase 07-08) uses a more sophisticated layout engine
- Constructor injection for all dependencies
- Theme colors from ThemeEngine for consistent styling

## Estimated Complexity

**L** -- Two panel classes, custom painting for graph, click navigation, mention-to-link conversion, 10 tests.
