# Phase 21 -- Object Metadata System

## Objective

Implement a metadata inspector panel that displays and edits the arbitrary key-value metadata on any canvas object. Includes predefined fields (author, created date, status, priority), custom fields, and a metadata search capability.

## Prerequisites

- Phase 01 (CanvasObject with metadata map)
- Phase 04 (SelectionManager)
- Phase 05 (UndoRedoStack)

## Feature References (PRD)

- PRD #96: Object Metadata

## Data Structures to Implement

### File: `src/canvas/MetadataPanel.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

class wxGraphicsContext;

namespace markamp::canvas
{

struct MetadataField
{
    std::string key;
    std::string value;
    bool is_system{false};     // System fields are read-only
    bool is_predefined{false}; // Predefined fields have type hints
    enum class FieldType : uint8_t { Text, Number, Date, Select, Boolean };
    FieldType type{FieldType::Text};
    std::vector<std::string> options; // For Select type
};

class MetadataPanel
{
public:
    using OnFieldChanged = std::function<void(ObjectId, const std::string& key,
                                               const std::string& value)>;
    using OnFieldRemoved = std::function<void(ObjectId, const std::string& key)>;

    auto set_object(ObjectId id, const std::unordered_map<std::string, std::string>& metadata,
                     const std::vector<std::string>& tags) -> void;
    auto clear() -> void;

    auto render(wxGraphicsContext& gc, double x, double y, double w, double h) -> void;
    auto handle_click(double x, double y) -> bool;

    auto set_on_field_changed(OnFieldChanged cb) -> void;
    auto set_on_field_removed(OnFieldRemoved cb) -> void;

    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

private:
    ObjectId current_id_{kInvalidObjectId};
    std::vector<MetadataField> fields_;
    bool visible_{false};
    OnFieldChanged on_field_changed_;
    OnFieldRemoved on_field_removed_;

    auto rebuild_fields(const std::unordered_map<std::string, std::string>& metadata) -> void;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `MetadataPanel::rebuild_fields()` -- Populate system fields (id, type, z_index), predefined fields (status, priority, author, created, modified), then custom fields from the metadata map.

2. `MetadataPanel::render()` -- Draw a scrollable form with field labels, values, edit buttons, and an "Add Field" button at the bottom.

3. Field editing: Click a field value to edit inline. For Select type, show a dropdown.

4. ChangePropertyCommand integration: All metadata changes go through UndoRedoStack.

## Test Cases

File: `tests/unit/test_metadata_panel.cpp`

1. **Set object metadata** -- Set 3 custom fields, verify fields_ populated.
2. **System fields** -- Verify id, type are present and read-only.
3. **Add custom field** -- Add new key-value, verify.
4. **Remove custom field** -- Remove field, verify callback.
5. **Field types** -- Set field type to Date, verify.
6. **Search metadata** -- Search across all objects for key=value match.

## Acceptance Criteria

- [ ] MetadataPanel shows system, predefined, and custom fields
- [ ] Fields are editable (except system fields)
- [ ] Custom fields can be added and removed
- [ ] Metadata changes are undoable
- [ ] Panel appears when an object is selected
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/MetadataPanel.h` | Metadata inspector panel |
| CREATE | `src/canvas/MetadataPanel.cpp` | Panel implementation |
| MODIFY | `src/canvas/CanvasCommands.h` | Ensure ChangePropertyCommand handles metadata |
| MODIFY | `src/ui/CanvasPanel.cpp` | Render MetadataPanel when selection exists |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_metadata_panel.cpp` | 6 Catch2 tests |

## Architecture Notes

- The metadata panel is rendered as a canvas overlay (side panel), consistent with other canvas panels.
- Metadata key-value pairs are stored on CanvasObject::metadata_ (already exists from Phase 01).
- Future phases can add structured metadata schemas per object type.

## Estimated Complexity

**S** -- Panel rendering, field editing, ChangePropertyCommand integration, 6 tests.
