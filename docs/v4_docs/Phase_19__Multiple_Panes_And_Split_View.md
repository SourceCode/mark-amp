# Phase 19 -- Multiple Panes and Split View

## Objective

Implement a multi-pane workspace where users can open multiple documents simultaneously in split views (horizontal and vertical). Extends the existing SplitView component with a pane management system: arbitrary splits, tab groups per pane, independent scroll positions, and linked scrolling between panes. Supports the "compare two documents" and "reference while writing" workflows.

## Prerequisites

- Existing SplitView, SplitterBar, TabBar components
- Existing EditorPanel, PreviewPanel
- Phase 02 (VaultService -- document access)

## Feature References (PRD)

- PRD #25: Multiple Panes
- PRD #10: Customizable UI

## Data Structures to Implement

### File: `src/ui/PaneManager.h`

```cpp
#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#include <wx/panel.h>

namespace markamp::core { class EventBus; class ThemeEngine; class Config; }

namespace markamp::ui
{

struct PaneId
{
    int id{0};
    [[nodiscard]] auto operator==(const PaneId& other) const -> bool { return id == other.id; }
};

struct PaneState
{
    PaneId pane_id;
    std::vector<std::string> open_document_ids;  // Documents open in this pane's tab bar
    std::string active_document_id;               // Currently visible document
    double scroll_position{0.0};
    bool is_focused{false};
};

enum class SplitDirection : uint8_t { Horizontal, Vertical };

struct SplitNode
{
    enum class Type : uint8_t { Leaf, Split } type{Type::Leaf};
    SplitDirection direction{SplitDirection::Horizontal};
    double ratio{0.5};                    // Split ratio (0.0 to 1.0)
    PaneId pane_id;                       // Valid only for Leaf nodes
    std::unique_ptr<SplitNode> first;     // Left or top child
    std::unique_ptr<SplitNode> second;    // Right or bottom child
};

class PaneManager
{
public:
    PaneManager(wxWindow* parent,
                markamp::core::EventBus& event_bus,
                markamp::core::ThemeEngine& theme_engine,
                markamp::core::Config& config);

    /// Split the active pane in the given direction.
    auto split_active(SplitDirection direction) -> PaneId;

    /// Split a specific pane.
    auto split_pane(PaneId pane_id, SplitDirection direction) -> PaneId;

    /// Close a pane. Transfers its documents to the sibling pane.
    auto close_pane(PaneId pane_id) -> void;

    /// Get the currently focused pane.
    [[nodiscard]] auto active_pane() const -> PaneId;

    /// Set focus to a pane.
    auto focus_pane(PaneId pane_id) -> void;

    /// Open a document in a specific pane.
    auto open_in_pane(PaneId pane_id, const std::string& document_id) -> void;

    /// Move a document from one pane to another.
    auto move_document(const std::string& document_id, PaneId from, PaneId to) -> void;

    /// Get all pane states.
    [[nodiscard]] auto pane_states() const -> std::vector<PaneState>;

    /// Get the pane count.
    [[nodiscard]] auto pane_count() const -> int;

    /// Reset to single pane layout.
    auto reset_layout() -> void;

    /// Serialize the split layout for persistence.
    [[nodiscard]] auto serialize_layout() const -> std::string;

    /// Restore a serialized layout.
    auto restore_layout(const std::string& serialized) -> void;

    /// Enable/disable linked scrolling between panes.
    auto set_linked_scrolling(bool enabled) -> void;
    [[nodiscard]] auto linked_scrolling() const -> bool;

    /// Cycle focus to next pane.
    auto focus_next_pane() -> void;

    /// Cycle focus to previous pane.
    auto focus_previous_pane() -> void;

private:
    wxWindow* parent_;
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;
    markamp::core::Config& config_;

    std::unique_ptr<SplitNode> root_node_;
    std::vector<PaneState> panes_;
    PaneId active_pane_id_;
    int next_pane_id_{1};
    bool linked_scrolling_{false};

    auto create_pane_widget(PaneId pane_id) -> wxPanel*;
    auto rebuild_layout() -> void;
    auto find_node(PaneId pane_id, SplitNode* node) const -> SplitNode*;
    auto find_parent_node(PaneId pane_id, SplitNode* node) const -> SplitNode*;
    auto collect_leaf_panes(const SplitNode* node) const -> std::vector<PaneId>;
    auto serialize_node(const SplitNode* node) const -> std::string;
    auto deserialize_node(const std::string& data) -> std::unique_ptr<SplitNode>;

    markamp::core::Subscription theme_changed_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`split_active(direction)`** -- Find the active pane's leaf node in the split tree. Replace it with a Split node containing the original pane and a new pane. Create new EditorPanel for the new pane. Rebuild layout.

2. **`close_pane(pane_id)`** -- Find the pane's parent Split node. Replace parent with the sibling node. Transfer open documents to sibling pane. Destroy the pane's widgets.

3. **`rebuild_layout()`** -- Walk the SplitNode tree. For each Split node, create/position a SplitterBar at the correct ratio. For each Leaf node, create/position the pane widget (EditorPanel + TabBar).

4. **`serialize_layout()`** -- Recursive JSON serialization of the SplitNode tree. Includes split direction, ratio, and leaf pane document lists.

5. **`restore_layout(serialized)`** -- Parse JSON. Rebuild SplitNode tree. Create pane widgets. Open documents in each pane.

6. **`set_linked_scrolling(enabled)`** -- When enabled, scrolling in any pane publishes a scroll event that other panes follow (proportional scrolling).

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneSplitEvent)
int new_pane_id{0};
SplitDirection direction;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneClosedEvent)
int pane_id{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PaneFocusChangedEvent)
int pane_id{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_pane_manager.cpp`

1. **Initial single pane** -- On startup, pane_count() == 1.
2. **Split horizontal** -- Split active pane. Verify pane_count() == 2.
3. **Split vertical** -- Split vertical. Verify correct direction in tree.
4. **Close pane** -- Split then close. Back to pane_count() == 1.
5. **Nested split** -- Split, then split again. Verify 3 panes.
6. **Focus cycling** -- 3 panes. focus_next_pane() cycles through all.
7. **Open in specific pane** -- Open doc in pane 2. Verify pane 2's active doc.
8. **Move document** -- Move doc from pane 1 to pane 2. Verify transfer.
9. **Layout serialization** -- Serialize 3-pane layout. Deserialize. Verify identical structure.
10. **Reset layout** -- 3 panes. reset_layout(). Verify single pane.

## Acceptance Criteria

- [ ] Panes can be split horizontally and vertically
- [ ] Each pane has independent tab bar and document
- [ ] Closing a pane transfers documents to sibling
- [ ] Focus cycling navigates between panes
- [ ] Layout serialization allows save/restore of workspace
- [ ] Linked scrolling synchronizes pane scroll positions
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/PaneManager.h` | PaneManager, SplitNode, PaneState |
| CREATE | `src/ui/PaneManager.cpp` | Full implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Use PaneManager instead of single EditorPanel |
| MODIFY | `src/core/Events.h` | Add 3 pane events |
| MODIFY | `src/CMakeLists.txt` | Add PaneManager.cpp |
| CREATE | `tests/unit/test_pane_manager.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_pane_manager target |

## Architecture Notes

- SplitNode is a binary tree: each internal node holds two children and a split ratio
- Leaf nodes correspond to actual pane widgets (EditorPanel + TabBar)
- Layout persistence uses JSON for the tree structure
- Linked scrolling uses EventBus to broadcast scroll position changes
- Constructor injection for all services

## Estimated Complexity

**XL** -- Binary split tree, dynamic widget creation/destruction, layout serialization, linked scrolling, 10 tests.
