# Phase 12 -- Document Outline / Document Map Panel

## Objective

Implement the Document Outline panel: a sidebar showing the heading hierarchy (H1-H6) of the current document as a navigable tree. Clicking a heading scrolls the editor/preview to that section. Supports real-time updates as the user types, drag-and-drop heading reordering, and visual indicators for the currently visible section.

## Prerequisites

- Phase 01 (DocumentModel -- parsed heading structure)
- Existing EditorPanel, PreviewPanel, MainFrame sidebar

## Feature References (PRD)

- PRD #16: Outline / Document Map

## Data Structures to Implement

### File: `src/ui/OutlinePanel.h`

```cpp
#pragma once

#include <wx/panel.h>
#include <wx/treectrl.h>

#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
class ThemeEngine;
}

namespace markamp::ui
{

struct OutlineEntry
{
    std::string text;
    int level{1};           // 1-6 for H1-H6
    int line_number{0};     // Line in source document
    int block_offset{0};    // Character offset in document
    std::string block_id;   // Optional block ID for precise navigation

    [[nodiscard]] auto indent_depth() const -> int;
};

class OutlinePanel : public wxPanel
{
public:
    OutlinePanel(wxWindow* parent,
                 markamp::core::EventBus& event_bus,
                 markamp::core::ThemeEngine& theme_engine);

    /// Update the outline from document content.
    auto update_outline(const std::string& markdown) -> void;

    /// Highlight the heading that contains the current cursor position.
    auto set_active_heading(int line_number) -> void;

    /// Clear the outline.
    auto clear() -> void;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::ThemeEngine& theme_engine_;

    wxTreeCtrl* tree_{nullptr};
    std::vector<OutlineEntry> entries_;
    wxTreeItemId active_item_;

    auto create_ui() -> void;
    auto populate_tree() -> void;
    auto parse_headings(const std::string& markdown) -> std::vector<OutlineEntry>;
    auto on_item_activated(wxTreeEvent& event) -> void;
    auto apply_theme() -> void;

    [[nodiscard]] auto find_heading_for_line(int line) const -> int;

    markamp::core::Subscription content_changed_sub_;
    markamp::core::Subscription cursor_moved_sub_;
    markamp::core::Subscription theme_changed_sub_;
    markamp::core::Subscription active_file_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`update_outline(markdown)`** -- Parse headings from markdown. Build tree control items with proper nesting (H2 under H1, H3 under H2, etc.). Store entries_ for lookup.

2. **`parse_headings(markdown)`** -- Scan markdown line by line. Detect `# Heading` patterns (1-6 `#` marks). Skip headings inside code blocks. Record line number and heading text. Strip `#` prefix and leading/trailing whitespace.

3. **`populate_tree()`** -- Clear tree. For each OutlineEntry, insert as wxTreeItem. Maintain a stack to track current parent at each heading level: H1 is root, H2 is child of most recent H1, etc.

4. **`set_active_heading(line_number)`** -- Find the heading whose line_number is <= the given line (the heading that "contains" the cursor position). Highlight that tree item. Scroll tree to show it.

5. **`on_item_activated(event)`** -- Get the OutlineEntry for the clicked tree item. Publish ScrollToLineRequestEvent with the heading's line_number.

6. **Auto-update** -- Subscribe to FileContentChangedEvent. On change, re-parse and update outline. Debounce to avoid excessive parsing during rapid typing.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(ScrollToLineRequestEvent)
int line_number{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(OutlineUpdatedEvent)
int heading_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_outline_panel.cpp`

1. **Parse headings** -- Markdown with H1, H2, H3. Verify 3 entries with correct levels.
2. **Nested hierarchy** -- H1, H2, H2, H3, H1. Verify tree structure: first H1 has 2 H2 children, second H2 has 1 H3 child.
3. **Skip code block headings** -- `# Heading` inside fenced code. Verify not included.
4. **Active heading tracking** -- Cursor at line 50. Nearest heading at line 45. Verify that heading highlighted.
5. **Click navigation** -- Click H2 entry. Verify ScrollToLineRequestEvent published with correct line.
6. **Real-time update** -- Change document content. Verify outline updates (debounced).
7. **Empty document** -- No headings. Verify outline shows "No headings" placeholder.
8. **Setext headings** -- Headings using `===` and `---` underline syntax. Verify detected.
9. **Long heading truncation** -- Heading with 200 characters. Verify displayed truncated.
10. **Theme colors** -- Theme change updates tree control colors.

## Acceptance Criteria

- [ ] Outline displays heading hierarchy as a tree (H1-H6 nesting)
- [ ] Clicking a heading scrolls the editor to that line
- [ ] Current section is highlighted in the outline as cursor moves
- [ ] Outline updates in real-time as user types (debounced)
- [ ] Headings inside code blocks are excluded
- [ ] Both ATX (`#`) and Setext (`===`) heading styles detected
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/OutlinePanel.h` | OutlinePanel, OutlineEntry |
| CREATE | `src/ui/OutlinePanel.cpp` | Full implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Register OutlinePanel in sidebar |
| MODIFY | `src/core/Events.h` | Add ScrollToLineRequestEvent, OutlineUpdatedEvent |
| MODIFY | `src/CMakeLists.txt` | Add OutlinePanel.cpp |
| CREATE | `tests/unit/test_outline_panel.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_outline_panel target |

## Architecture Notes

- OutlinePanel is a pure UI component; heading parsing is internal
- Debounce content change handler (300ms) to avoid re-parsing on every keystroke
- Tree nesting uses a level stack: when a heading level decreases, pop the stack to find parent
- Constructor injection: OutlinePanel(wxWindow*, EventBus&, ThemeEngine&)

## Estimated Complexity

**M** -- Heading parser, tree control population with nesting, active section tracking, 10 tests.
