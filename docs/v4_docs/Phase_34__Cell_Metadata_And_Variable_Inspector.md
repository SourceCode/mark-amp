# Phase 34 -- Cell Metadata, Execution Tracking, and Variable Inspector

## Objective

Implement the notebook cell management layer: cell metadata (execution count, tags, hidden/collapsed state), execution order tracking with visual indicators, a variable inspector panel showing all kernel variables with types and values, and cell-level controls (run, clear, collapse, move). This completes the notebook experience.

## Prerequisites

- Phase 29 (KernelManager -- execution and introspection)
- Phase 31 (CellOutputRenderer -- output display)

## Feature References (PRD)

- PRD Notebook #11: Cell-Level Execution Control
- PRD Notebook #20: Cell Metadata
- PRD Notebook #21: Variable Inspector Extensions
- PRD Notebook #42: Execution Order Tracking
- PRD Notebook #47: Collapsible Code Cells

## Data Structures to Implement

### File: `src/core/NotebookCellManager.h`

```cpp
#pragma once

#include "KernelManager.h"

#include <chrono>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

class EventBus;

enum class CellType : uint8_t { Code, Markdown, Raw };

struct CellMetadata
{
    std::string cell_id;
    CellType type{CellType::Code};
    std::string language;           // "python", "r", etc.
    int execution_count{0};         // [1], [2], etc.
    bool collapsed_input{false};    // Hide code, show output only
    bool collapsed_output{false};   // Hide output
    bool editable{true};
    bool deletable{true};
    std::vector<std::string> tags;  // User-defined tags
    std::unordered_map<std::string, std::string> custom;  // Extension metadata

    // Execution timing
    std::optional<std::chrono::system_clock::time_point> started_at;
    std::optional<std::chrono::system_clock::time_point> completed_at;
    double execution_time_ms{0.0};

    [[nodiscard]] auto execution_label() const -> std::string;  // "[3]" or "[ ]"
    [[nodiscard]] auto is_running() const -> bool;
};

struct NotebookCell
{
    CellMetadata metadata;
    std::string source;         // Cell source code/markdown
    std::vector<std::string> output_ids;  // References to CellOutputState entries
};

struct VariableInfo
{
    std::string name;
    std::string type_name;      // "int", "pandas.DataFrame", "list"
    std::string value_repr;     // Short representation
    int64_t size_bytes{0};      // Memory size estimate
    std::string shape;          // For arrays/DataFrames: "(100, 5)"
};

class NotebookCellManager
{
public:
    explicit NotebookCellManager(EventBus& event_bus);

    // --- Cell CRUD ---
    auto add_cell(CellType type, int position = -1) -> std::string;  // Returns cell_id
    auto remove_cell(const std::string& cell_id) -> void;
    auto move_cell(const std::string& cell_id, int new_position) -> void;
    auto set_cell_source(const std::string& cell_id, const std::string& source) -> void;

    // --- Cell Queries ---
    [[nodiscard]] auto get_cell(const std::string& cell_id) const -> std::optional<NotebookCell>;
    [[nodiscard]] auto cells() const -> const std::vector<NotebookCell>&;
    [[nodiscard]] auto cell_count() const -> int;
    [[nodiscard]] auto cell_at(int position) const -> const NotebookCell&;

    // --- Cell Metadata ---
    auto set_collapsed_input(const std::string& cell_id, bool collapsed) -> void;
    auto set_collapsed_output(const std::string& cell_id, bool collapsed) -> void;
    auto add_cell_tag(const std::string& cell_id, const std::string& tag) -> void;
    auto record_execution(const std::string& cell_id, int count,
                           double elapsed_ms) -> void;

    // --- Execution Tracking ---
    [[nodiscard]] auto next_execution_count() -> int;
    [[nodiscard]] auto execution_order() const -> std::vector<std::string>;  // Cell IDs in exec order

    // --- Serialization ---
    [[nodiscard]] auto serialize_to_json() const -> std::string;
    auto deserialize_from_json(const std::string& json) -> void;

private:
    EventBus& event_bus_;
    std::vector<NotebookCell> cells_;
    int execution_counter_{0};
    std::vector<std::string> execution_order_;
};

class VariableInspector
{
public:
    VariableInspector(EventBus& event_bus, KernelManager& kernel_manager);

    /// Refresh the variable list from the kernel.
    [[nodiscard]] auto refresh(const std::string& kernel_id)
        -> std::expected<std::vector<VariableInfo>, std::string>;

    /// Get the last refreshed variable list.
    [[nodiscard]] auto variables() const -> const std::vector<VariableInfo>&;

    /// Get detailed inspection of a specific variable.
    [[nodiscard]] auto inspect_variable(const std::string& kernel_id,
                                         const std::string& var_name) const
        -> std::expected<MimeBundle, std::string>;

    /// Delete a variable from the kernel namespace.
    [[nodiscard]] auto delete_variable(const std::string& kernel_id,
                                        const std::string& var_name)
        -> std::expected<void, std::string>;

private:
    EventBus& event_bus_;
    KernelManager& kernel_manager_;
    std::vector<VariableInfo> variables_;
};

} // namespace markamp::core
```

## Key Functions to Implement

1. **`add_cell(type, position)`** -- Generate cell_id. Create NotebookCell with metadata. Insert at position (or end if -1). Publish CellAddedEvent.

2. **`record_execution(cell_id, count, elapsed)`** -- Update cell metadata with execution count, timing. Add to execution_order_. Publish CellExecutionRecordedEvent.

3. **`serialize_to_json()`** -- Convert cells vector to JSON following Jupyter notebook format (.ipynb). Include cell type, source, outputs, metadata, execution_count.

4. **`deserialize_from_json(json)`** -- Parse Jupyter .ipynb format. Create NotebookCell for each cell in the JSON. Restore metadata and outputs.

5. **`VariableInspector::refresh(kernel_id)`** -- Send introspection code to kernel (`%who_ls` for Python, `ls()` for R). Parse response. Build VariableInfo for each variable with name, type, value preview, and size.

6. **`inspect_variable(kernel_id, var_name)`** -- Send `repr(var_name)` or equivalent. Return MimeBundle with the variable's representation.

## Events to Add

```cpp
MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellAddedEvent)
std::string cell_id;
CellType cell_type;
int position{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellRemovedEvent)
std::string cell_id;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellMovedEvent)
std::string cell_id;
int old_position{0};
int new_position{0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CellExecutionRecordedEvent)
std::string cell_id;
int execution_count{0};
double elapsed_ms{0.0};
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(VariablesRefreshedEvent)
int variable_count{0};
MARKAMP_DECLARE_EVENT_END;
```

## Test Cases

File: `tests/unit/test_notebook_cells.cpp`

1. **Add cell** -- Add code cell. Verify cell_count() incremented.
2. **Remove cell** -- Remove cell. Verify count decremented.
3. **Move cell** -- Move cell from position 0 to 2. Verify new order.
4. **Execution count** -- Record execution. Verify execution_label() shows "[1]".
5. **Execution order** -- Execute cells 1, 3, 2. execution_order() = [1, 3, 2].
6. **Collapsed input** -- Collapse code. Verify metadata.collapsed_input=true.
7. **Cell tags** -- Add tag "important" to cell. Verify in metadata.tags.
8. **Serialize/deserialize** -- Create 3 cells. Serialize. Deserialize. Verify identical.
9. **Variable info parsing** -- Mock variable list response. Verify VariableInfo fields.
10. **Variable inspection** -- Inspect a variable. Verify MimeBundle returned.
11. **Cell source update** -- Set cell source. Verify source updated.
12. **Next execution count** -- Call next_execution_count() 3 times. Verify 1, 2, 3.

## Acceptance Criteria

- [ ] Cell CRUD (add, remove, move, edit source) works
- [ ] Execution count tracking with visual `[N]` labels
- [ ] Execution order recorded and queryable
- [ ] Cell collapse (input/output) works
- [ ] Cell metadata tags and custom fields persist
- [ ] Jupyter .ipynb format serialization/deserialization
- [ ] Variable inspector retrieves kernel namespace
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/core/NotebookCellManager.h` | NotebookCellManager, NotebookCell, CellMetadata, VariableInspector |
| CREATE | `src/core/NotebookCellManager.cpp` | Full implementation |
| MODIFY | `src/core/Events.h` | Add 5 cell/variable events |
| MODIFY | `src/CMakeLists.txt` | Add NotebookCellManager.cpp |
| CREATE | `tests/unit/test_notebook_cells.cpp` | 12 Catch2 test cases |
| MODIFY | `tests/CMakeLists.txt` | Add test_notebook_cells target |

## Estimated Complexity

**L** -- Cell management, Jupyter format, variable introspection, execution tracking, 12 tests.
