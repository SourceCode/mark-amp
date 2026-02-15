# Phase 33 -- Interactive Widgets and Plot Backends

## Objective

Implement the ipywidgets-compatible comm channel for two-way communication between kernel and frontend. Supports interactive widgets (sliders, dropdowns, text inputs, buttons) that update Python state and vice versa. Also integrates interactive plot backends (Plotly, Bokeh) that render in embedded webviews with zoom, pan, and hover tooltips.

## Prerequisites

- Phase 29 (KernelManager -- ZeroMQ comm channels)
- Phase 31 (CellOutputRenderer -- output display)
- Phase 32 (ChartPanel -- chart display)

## Feature References (PRD)

- PRD Notebook #8: Interactive Widgets (ipywidgets)
- PRD Notebook #33: Interactive Plot Backends

## Data Structures to Implement

### File: `src/core/WidgetManager.h`

```cpp
#pragma once

#include <functional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

namespace markamp::core
{

class EventBus;
class KernelManager;

using WidgetValue = std::variant<int, double, std::string, bool, std::vector<std::string>>;

struct WidgetState
{
    std::string widget_id;
    std::string widget_type;     // "IntSlider", "Dropdown", "Text", "Button", "Output"
    std::unordered_map<std::string, WidgetValue> properties;
    std::string comm_id;          // ZeroMQ comm channel ID

    [[nodiscard]] auto get_string(const std::string& key) const -> std::string;
    [[nodiscard]] auto get_int(const std::string& key) const -> int;
    [[nodiscard]] auto get_double(const std::string& key) const -> double;
    [[nodiscard]] auto get_bool(const std::string& key) const -> bool;
};

struct CommMessage
{
    std::string comm_id;
    std::string target_name;      // "jupyter.widget"
    std::string msg_type;         // "comm_open", "comm_msg", "comm_close"
    std::string data_json;
};

class WidgetManager
{
public:
    WidgetManager(EventBus& event_bus, KernelManager& kernel_manager);

    /// Handle a comm_open message (kernel creates a widget).
    auto handle_comm_open(const CommMessage& msg) -> void;

    /// Handle a comm_msg (kernel updates widget state).
    auto handle_comm_msg(const CommMessage& msg) -> void;

    /// Handle a comm_close (kernel destroys widget).
    auto handle_comm_close(const CommMessage& msg) -> void;

    /// Send a state update from frontend to kernel (user changed slider, etc.).
    auto send_state_update(const std::string& widget_id,
                            const std::string& property,
                            const WidgetValue& value) -> void;

    /// Send a custom message to a widget's kernel-side counterpart.
    auto send_custom_message(const std::string& widget_id,
                              const std::string& content_json) -> void;

    /// Get the current state of a widget.
    [[nodiscard]] auto get_widget(const std::string& widget_id) const
        -> std::optional<WidgetState>;

    /// Get all active widgets.
    [[nodiscard]] auto active_widgets() const -> std::vector<WidgetState>;

    /// Register a callback for widget state changes.
    auto on_state_change(const std::string& widget_id,
                          std::function<void(const WidgetState&)> callback) -> void;

    /// Clear all widgets (on kernel restart).
    auto clear_all() -> void;

private:
    EventBus& event_bus_;
    KernelManager& kernel_manager_;

    std::unordered_map<std::string, WidgetState> widgets_;
    std::unordered_map<std::string, std::vector<std::function<void(const WidgetState&)>>> callbacks_;

    auto parse_widget_state(const std::string& json) -> WidgetState;
    auto serialize_state_update(const std::string& property,
                                 const WidgetValue& value) const -> std::string;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`handle_comm_open(msg)`** -- Parse widget state from JSON. Create WidgetState entry. Map comm_id to widget_id. Publish WidgetCreatedEvent. Trigger UI creation.

2. **`handle_comm_msg(msg)`** -- Parse state update from JSON. Update WidgetState properties. Notify registered callbacks. Publish WidgetUpdatedEvent.

3. **`send_state_update(widget_id, property, value)`** -- Serialize state delta to JSON. Send comm_msg via KernelManager to the kernel. Kernel-side widget updates accordingly.

4. **`parse_widget_state(json)`** -- Parse ipywidgets state JSON format. Map widget_model_name to widget_type. Extract all properties (value, min, max, description, options, etc.).

5. **UI Widget Factory** (in `src/ui/WidgetFactory.h`) -- Create wxWidgets controls for each ipywidgets type:
   - IntSlider -> wxSlider
   - FloatSlider -> wxSlider with float mapping
   - Dropdown -> wxChoice
   - Text -> wxTextCtrl
   - Checkbox -> wxCheckBox
   - Button -> wxButton
   - Output -> embedded CellOutput panel

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetCreatedEvent)
std::string widget_id;
std::string widget_type;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetUpdatedEvent)
std::string widget_id;
std::string property;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(WidgetDestroyedEvent)
std::string widget_id;
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_widget_manager.cpp`

1. **Comm open creates widget** -- Handle comm_open. Verify widget registered.
2. **State update from kernel** -- Handle comm_msg with value change. Verify state updated.
3. **State update to kernel** -- send_state_update(). Verify comm_msg sent.
4. **Widget type mapping** -- "IntSlider" creates correct WidgetState.
5. **Callback notification** -- Register callback. Update state. Verify callback fired.
6. **Comm close destroys widget** -- Handle comm_close. Verify widget removed.
7. **Multiple widgets** -- Create 3 widgets. active_widgets() returns 3.
8. **Clear all** -- clear_all() removes all widgets.
9. **Get widget** -- get_widget() returns correct state.
10. **WidgetValue variant** -- Store int, double, string, bool. Verify correct access.

## Acceptance Criteria

- [ ] Widget creation from comm_open messages
- [ ] Bidirectional state synchronization (kernel <-> frontend)
- [ ] Widget type mapping to UI controls
- [ ] Callback system for state change notification
- [ ] Widget destruction on comm_close
- [ ] All 10 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/WidgetManager.h` | WidgetManager, WidgetState, CommMessage |
| CREATE | `src/core/WidgetManager.cpp` | Full implementation |
| CREATE | `src/ui/WidgetFactory.h` | Factory for creating wxWidgets from WidgetState |
| CREATE | `src/ui/WidgetFactory.cpp` | Widget creation implementation |
| MODIFY | `src/core/Events.h` | Add 3 widget events |
| MODIFY | `src/core/PluginContext.h` | Add `WidgetManager* widget_manager{nullptr};` |
| MODIFY | `src/CMakeLists.txt` | Add new .cpp files |
| CREATE | `tests/unit/test_widget_manager.cpp` | 10 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_widget_manager target |

## Estimated Complexity

**XL** -- Comm protocol, bidirectional state sync, widget UI factory, 10 tests.
