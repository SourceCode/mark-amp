# Phase 21 -- Customizable UI Layout System

## Objective

Implement a flexible layout system where users can rearrange, show/hide, and resize all UI panels (sidebar, outline, backlinks, graph, search, terminal). Supports named layout presets ("Writing", "Research", "Review"), drag-and-drop panel repositioning, and persistent layout save/restore. Extends the existing LayoutManager and ActivityBar.

## Prerequisites

- Phase 19 (PaneManager)
- Existing LayoutManager, ActivityBar, MainFrame sidebar infrastructure

## Feature References (PRD)

- PRD #10: Customizable UI
- PRD #15: Hotkeys (layout-related shortcuts)

## Data Structures to Implement

### File: `src/ui/WorkspaceLayout.h`

```cpp
#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core { class EventBus; class Config; class ThemeEngine; }

namespace markamp::ui
{

enum class PanelPosition : uint8_t
{
    LeftSidebar,
    RightSidebar,
    BottomPanel,
    FloatingWindow,
    Hidden
};

struct PanelLayout
{
    std::string panel_id;                // "backlinks", "outline", "graph", "search", etc.
    PanelPosition position{PanelPosition::LeftSidebar};
    int order{0};                        // Order within position
    int width{250};                      // Width (for sidebars)
    int height{200};                     // Height (for bottom panel)
    bool visible{true};
    bool collapsed{false};               // Collapsed to icon only
};

struct LayoutPreset
{
    std::string name;
    std::string description;
    std::vector<PanelLayout> panels;
    std::string pane_layout_json;        // PaneManager layout (from Phase 19)
    bool is_builtin{false};
};

class WorkspaceLayoutManager
{
public:
    WorkspaceLayoutManager(markamp::core::EventBus& event_bus,
                            markamp::core::Config& config,
                            markamp::core::ThemeEngine& theme_engine);

    /// Apply a layout preset.
    auto apply_preset(const std::string& preset_name) -> void;

    /// Save the current layout as a named preset.
    auto save_preset(const std::string& name, const std::string& description = "") -> void;

    /// Delete a user-defined preset.
    auto delete_preset(const std::string& name) -> void;

    /// List all available presets (builtin + user).
    [[nodiscard]] auto list_presets() const -> std::vector<LayoutPreset>;

    /// Get a specific panel's layout.
    [[nodiscard]] auto get_panel_layout(const std::string& panel_id) const
        -> std::optional<PanelLayout>;

    /// Move a panel to a new position.
    auto move_panel(const std::string& panel_id, PanelPosition new_position, int order = -1) -> void;

    /// Show/hide a panel.
    auto set_panel_visible(const std::string& panel_id, bool visible) -> void;

    /// Toggle a panel's visibility.
    auto toggle_panel(const std::string& panel_id) -> void;

    /// Resize a panel.
    auto resize_panel(const std::string& panel_id, int width, int height) -> void;

    /// Persist current layout to config.
    auto save_layout() -> void;

    /// Restore layout from config.
    auto restore_layout() -> void;

    /// Register a new panel type (for plugins).
    auto register_panel(const std::string& panel_id, const std::string& display_name) -> void;

    /// Get all registered panel IDs.
    [[nodiscard]] auto registered_panels() const -> std::vector<std::string>;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::Config& config_;
    markamp::core::ThemeEngine& theme_engine_;

    std::vector<PanelLayout> current_layout_;
    std::vector<LayoutPreset> presets_;
    std::vector<std::pair<std::string, std::string>> registered_panels_;

    auto load_builtin_presets() -> void;
    auto load_user_presets() -> void;
    auto apply_layout(const std::vector<PanelLayout>& panels) -> void;
    auto serialize_layout() const -> std::string;
    auto deserialize_layout(const std::string& json) -> std::vector<PanelLayout>;
};

} // namespace markamp::ui
```

## Key Functions to Implement

1. **`apply_preset(preset_name)`** -- Find preset. Apply each PanelLayout: show/hide panels, set positions, set sizes. Publish LayoutChangedEvent.

2. **`load_builtin_presets()`** -- Create default presets:
   - "Default": file tree left, editor center, preview right
   - "Writing": editor only, outline right
   - "Research": editor left, backlinks right, graph bottom
   - "Review": split editor, diff view
   - "Presentation": preview only, fullscreen

3. **`move_panel(panel_id, position, order)`** -- Update the panel's position in current_layout_. Trigger MainFrame to reposition the actual wxPanel widget.

4. **`save_layout() / restore_layout()`** -- Serialize current_layout_ to JSON. Store in Config. On restore, deserialize and apply.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(LayoutChangedEvent)
std::string preset_name;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelVisibilityChangedEvent)
std::string panel_id;
bool visible{false};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(PanelMovedEvent)
std::string panel_id;
PanelPosition new_position;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_workspace_layout.cpp`

1. **Default layout** -- On init, default preset is applied.
2. **Apply writing preset** -- Verify only editor and outline visible.
3. **Toggle panel** -- Toggle backlinks panel. Verify visibility changes.
4. **Move panel** -- Move outline from left to right sidebar. Verify position updated.
5. **Save custom preset** -- Save current layout as "My Layout". Verify in list.
6. **Delete preset** -- Delete user preset. Verify removed. Cannot delete builtin.
7. **Layout persistence** -- Save layout. Restore. Verify identical.
8. **Panel registration** -- Register new panel "custom". Verify in registered list.
9. **Resize panel** -- Resize sidebar to 300px. Verify width updated.
10. **Panel collapsed state** -- Collapse panel. Verify collapsed=true.

## Acceptance Criteria

- [ ] Built-in layout presets cover common workflows
- [ ] Panels can be moved between left sidebar, right sidebar, bottom, floating
- [ ] Show/hide/toggle individual panels
- [ ] Custom presets can be saved and deleted
- [ ] Layout persists across sessions
- [ ] Plugin panels can be registered and positioned
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/ui/WorkspaceLayout.h` | WorkspaceLayoutManager, PanelLayout, LayoutPreset |
| CREATE | `src/ui/WorkspaceLayout.cpp` | Full implementation |
| MODIFY | `src/ui/MainFrame.cpp` | Use WorkspaceLayoutManager for panel positioning |
| MODIFY | `src/ui/ActivityBar.cpp` | Toggle panels via activity bar icons |
| MODIFY | `src/core/Events.h` | Add 3 layout events |
| MODIFY | `src/CMakeLists.txt` | Add WorkspaceLayout.cpp |
| CREATE | `tests/unit/test_workspace_layout.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_workspace_layout target |

## Estimated Complexity

**L** -- Layout management, preset system, persistence, panel registration, 10 tests.
