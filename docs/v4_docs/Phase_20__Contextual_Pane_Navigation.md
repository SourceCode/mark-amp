# Phase 20 -- Contextual Pane Navigation

## Objective

Implement navigation behaviors that integrate with the multi-pane system: clicking a wikilink opens the target in an adjacent pane (or creates a new split), breadcrumb navigation shows the link path, back/forward navigation works per-pane, and hover-preview shows a floating snippet of linked documents. This makes link-following a natural spatial activity.

## Prerequisites

- Phase 19 (PaneManager -- multi-pane system)
- Phase 01 (WikiLink -- link parsing)
- Phase 02 (VaultService -- document access)
- Existing BreadcrumbBar

## Feature References (PRD)

- PRD #38: Contextual Pane Navigation

## Data Structures to Implement

### File: `src/ui/NavigationService.h`

```cpp
#pragma once

#include <string>
#include <vector>

namespace markamp::core { class EventBus; class VaultService; }
namespace markamp::ui { class PaneManager; }

namespace markamp::ui
{

struct NavigationEntry
{
    std::string document_id;
    int scroll_position{0};
    int cursor_line{0};
};

struct NavigationHistory
{
    std::vector<NavigationEntry> entries;
    int current_index{-1};

    [[nodiscard]] auto can_go_back() const -> bool;
    [[nodiscard]] auto can_go_forward() const -> bool;
    auto push(const NavigationEntry& entry) -> void;
    [[nodiscard]] auto go_back() -> NavigationEntry;
    [[nodiscard]] auto go_forward() -> NavigationEntry;
};

enum class LinkOpenBehavior : uint8_t
{
    SamePane,       // Open in same pane (replace current)
    AdjacentPane,   // Open in adjacent pane (create split if needed)
    NewPane,        // Always create a new split
    Popup           // Show in floating popup
};

class NavigationService
{
public:
    NavigationService(markamp::core::EventBus& event_bus,
                      markamp::core::VaultService& vault_service,
                      PaneManager& pane_manager);

    /// Navigate to a document (respects configured open behavior).
    auto navigate_to(const std::string& document_id,
                      LinkOpenBehavior behavior = LinkOpenBehavior::SamePane) -> void;

    /// Navigate via wikilink (resolve + navigate).
    auto follow_wikilink(const std::string& link_target,
                          LinkOpenBehavior behavior) -> void;

    /// Go back in the current pane's history.
    auto go_back() -> void;

    /// Go forward in the current pane's history.
    auto go_forward() -> void;

    /// Get the current pane's navigation history.
    [[nodiscard]] auto current_history() const -> const NavigationHistory&;

    /// Show a hover preview of a document at screen coordinates.
    auto show_hover_preview(const std::string& document_id,
                             int screen_x, int screen_y) -> void;

    /// Hide the hover preview.
    auto hide_hover_preview() -> void;

    /// Get breadcrumb trail for the current navigation path.
    [[nodiscard]] auto breadcrumb_trail() const -> std::vector<std::string>;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::VaultService& vault_service_;
    PaneManager& pane_manager_;

    // Per-pane navigation histories
    std::unordered_map<int, NavigationHistory> pane_histories_;

    markamp::core::Subscription link_clicked_sub_;
    markamp::core::Subscription active_file_sub_;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`navigate_to(document_id, behavior)`** -- Based on behavior: SamePane opens in active pane. AdjacentPane finds or creates a split. NewPane always splits. Record in navigation history.

2. **`follow_wikilink(target, behavior)`** -- Resolve wikilink target via VaultService. If resolved, navigate_to(). If unresolved, offer to create new document.

3. **`go_back() / go_forward()`** -- Pop/push from the active pane's NavigationHistory. Navigate to the entry's document and restore scroll position.

4. **`show_hover_preview(document_id, x, y)`** -- Create a small floating window showing the first ~200 characters of the target document (rendered). Position at screen coordinates. Auto-dismiss after 2 seconds or on mouse leave.

5. **`breadcrumb_trail()`** -- Return the sequence of document titles from navigation history (like browser breadcrumbs).

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WikiLinkClickedEvent)
std::string link_target;
bool ctrl_held{false};    // Ctrl+click = new pane
bool alt_held{false};     // Alt+click = hover preview
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(NavigationEvent)
std::string document_id;
std::string direction;    // "forward", "back", "direct"
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_navigation_service.cpp`

1. **Navigate same pane** -- Navigate to doc. Active pane shows doc.
2. **Navigate adjacent pane** -- AdjacentPane creates split if single pane. Opens doc in new pane.
3. **Back/forward** -- Navigate A -> B -> C. go_back() returns to B. go_back() returns to A. go_forward() returns to B.
4. **History truncation** -- Navigate A -> B -> C. go_back() to B. Navigate to D. Forward history (C) is cleared.
5. **Wikilink follow resolved** -- Follow `[[Existing Doc]]`. Navigates to it.
6. **Wikilink follow unresolved** -- Follow `[[New Doc]]`. Offers creation prompt.
7. **Ctrl+click opens new pane** -- Verify ctrl_held triggers NewPane behavior.
8. **Breadcrumb trail** -- Navigate A -> B -> C. Breadcrumb shows [A, B, C].
9. **Per-pane history** -- Pane 1 and Pane 2 have independent histories.
10. **Hover preview** -- show_hover_preview shows floating window with content snippet.

## Acceptance Criteria

- [ ] Links open in same pane, adjacent pane, or new pane based on behavior
- [ ] Back/forward navigation works per-pane with correct history
- [ ] Ctrl+click opens in a new split pane
- [ ] Hover preview shows document snippet at link position
- [ ] Unresolved wikilinks offer document creation
- [ ] Breadcrumb trail reflects navigation path
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/NavigationService.h` | NavigationService, NavigationHistory |
| CREATE | `src/ui/NavigationService.cpp` | Full implementation |
| MODIFY | `src/ui/EditorPanel.cpp` | Wire link clicks to NavigationService |
| MODIFY | `src/ui/BreadcrumbBar.cpp` | Show navigation breadcrumbs |
| MODIFY | `src/core/Events.h` | Add 2 navigation events |
| MODIFY | `src/CMakeLists.txt` | Add NavigationService.cpp |
| CREATE | `tests/unit/test_navigation_service.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_navigation_service target |

## Estimated Complexity

**M** -- Per-pane history, link behavior dispatch, hover preview, breadcrumb, 10 tests.
