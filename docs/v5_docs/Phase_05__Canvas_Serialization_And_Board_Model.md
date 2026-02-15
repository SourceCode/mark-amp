# Phase 05 -- Canvas Serialization and Board Model

## Objective

Implement the Board data model (a named collection of canvas objects), JSON serialization/deserialization for complete board state, and a command-based undo/redo stack that records all canvas mutations as reversible commands. This phase makes board state persistable and all operations undoable.

## Prerequisites

- Phase 01 (CanvasObject, CanvasTypes)
- Phase 02 (CanvasPanel with object storage)
- Phase 04 (SelectionManager for move/resize/rotate operations)

## Feature References (PRD)

- PRD #20: Version History (serialization is prerequisite)
- PRD #85: Board Duplication with History (clone board)
- PRD #88: Board Archiving (board state management)

## Data Structures to Implement

### File: `src/canvas/Board.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <chrono>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Metadata for a canvas board.
struct BoardMetadata
{
    std::string id;
    std::string name;
    std::string description;
    std::string author;
    std::chrono::system_clock::time_point created_at;
    std::chrono::system_clock::time_point modified_at;
    std::vector<std::string> tags;
    bool archived{false};

    /// Board-level settings.
    CanvasColor background_color{245, 245, 245, 255};
    bool grid_visible{true};
    double grid_spacing{20.0};
    double default_zoom{1.0};
    Point2D default_pan{0.0, 0.0};
};

/// A Board is the top-level container for a canvas workspace.
/// It owns all CanvasObjects and their spatial relationships.
class Board
{
public:
    explicit Board(const std::string& name = "Untitled Board");

    // --- Identity ---
    [[nodiscard]] auto id() const -> const std::string&;
    [[nodiscard]] auto metadata() const -> const BoardMetadata&;
    auto set_metadata(const BoardMetadata& meta) -> void;

    // --- Object management ---
    auto add_object(std::unique_ptr<CanvasObject> obj) -> ObjectId;
    auto remove_object(ObjectId id) -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto get_object(ObjectId id) -> CanvasObject*;
    [[nodiscard]] auto get_object(ObjectId id) const -> const CanvasObject*;
    [[nodiscard]] auto object_count() const -> size_t;
    [[nodiscard]] auto all_object_ids() const -> std::vector<ObjectId>;

    /// Get objects sorted by z-index (ascending).
    [[nodiscard]] auto objects_by_z_order() const -> std::vector<const CanvasObject*>;

    // --- Serialization ---
    [[nodiscard]] auto to_json() const -> std::string;
    [[nodiscard]] static auto from_json(const std::string& json) -> std::unique_ptr<Board>;

    /// Save to a .markboard file (JSON).
    [[nodiscard]] auto save_to_file(const std::filesystem::path& path) const -> bool;
    [[nodiscard]] static auto load_from_file(const std::filesystem::path& path)
        -> std::unique_ptr<Board>;

    // --- Dirty tracking ---
    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto mark_clean() -> void;

    // --- Deep clone ---
    [[nodiscard]] auto clone() const -> std::unique_ptr<Board>;

    // --- Computed bounds ---
    [[nodiscard]] auto content_bounds() const -> AABB;

    // --- Z-index management ---
    [[nodiscard]] auto next_z_index() const -> int;
    auto bring_to_front(ObjectId id) -> void;
    auto send_to_back(ObjectId id) -> void;
    auto bring_forward(ObjectId id) -> void;
    auto send_backward(ObjectId id) -> void;

private:
    BoardMetadata metadata_;
    std::unordered_map<ObjectId, std::unique_ptr<CanvasObject>> objects_;
    bool dirty_{false};
};

} // namespace markamp::canvas
```

### File: `src/canvas/BoardSerializer.h`

```cpp
#pragma once

#include "Board.h"
#include "CanvasObject.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>

namespace markamp::canvas
{

/// Factory function type for creating CanvasObject subclasses from JSON.
using ObjectFactory = std::function<std::unique_ptr<CanvasObject>(const std::string& json)>;

/// Handles serialization/deserialization of Board and all CanvasObject subclasses.
/// Uses a factory registry to instantiate the correct subclass during deserialization.
class BoardSerializer
{
public:
    BoardSerializer();

    /// Register a factory for a CanvasObjectType.
    auto register_factory(CanvasObjectType type, ObjectFactory factory) -> void;

    /// Serialize a board to JSON string.
    [[nodiscard]] auto serialize(const Board& board) const -> std::string;

    /// Deserialize a board from JSON string.
    [[nodiscard]] auto deserialize(const std::string& json) const -> std::unique_ptr<Board>;

    /// Serialize a single object to JSON.
    [[nodiscard]] auto serialize_object(const CanvasObject& obj) const -> std::string;

    /// Deserialize a single object from JSON.
    [[nodiscard]] auto deserialize_object(const std::string& json) const
        -> std::unique_ptr<CanvasObject>;

private:
    std::unordered_map<CanvasObjectType, ObjectFactory> factories_;
};

} // namespace markamp::canvas
```

### File: `src/canvas/UndoRedoStack.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace markamp::core
{
class EventBus;
} // namespace markamp::core

namespace markamp::canvas
{

/// A reversible command that can be executed and undone.
class ICanvasCommand
{
public:
    virtual ~ICanvasCommand() = default;

    /// Execute the command (do/redo).
    virtual auto execute() -> void = 0;
    /// Reverse the command (undo).
    virtual auto undo() -> void = 0;

    /// Human-readable description (for UI display in Edit menu).
    [[nodiscard]] virtual auto description() const -> std::string = 0;

    /// Whether consecutive commands of the same type can be merged
    /// (e.g., multiple small moves into one).
    [[nodiscard]] virtual auto can_merge_with(const ICanvasCommand& other) const -> bool;
    virtual auto merge_with(const ICanvasCommand& other) -> void;
};

/// Undo/redo stack for canvas operations.
class UndoRedoStack
{
public:
    explicit UndoRedoStack(core::EventBus& event_bus, size_t max_history = 200);

    /// Execute a command and push it onto the undo stack.
    auto execute(std::unique_ptr<ICanvasCommand> command) -> void;

    /// Undo the last command.
    auto undo() -> void;

    /// Redo the last undone command.
    auto redo() -> void;

    [[nodiscard]] auto can_undo() const -> bool;
    [[nodiscard]] auto can_redo() const -> bool;
    [[nodiscard]] auto undo_description() const -> std::string;
    [[nodiscard]] auto redo_description() const -> std::string;

    /// Clear all history.
    auto clear() -> void;

    /// Begin a compound command (groups multiple commands into one undo step).
    auto begin_compound(const std::string& description) -> void;
    /// End the compound command.
    auto end_compound() -> void;

    [[nodiscard]] auto history_size() const -> size_t;

private:
    core::EventBus& event_bus_;
    std::vector<std::unique_ptr<ICanvasCommand>> undo_stack_;
    std::vector<std::unique_ptr<ICanvasCommand>> redo_stack_;
    size_t max_history_;
    bool in_compound_{false};
    std::vector<std::unique_ptr<ICanvasCommand>> compound_commands_;
    std::string compound_description_;

    auto publish_state_event() -> void;
};

} // namespace markamp::canvas
```

### Built-in Command Implementations

```cpp
// src/canvas/CanvasCommands.h

class AddObjectCommand : public ICanvasCommand { /* ... */ };
class RemoveObjectCommand : public ICanvasCommand { /* ... */ };
class MoveObjectsCommand : public ICanvasCommand { /* ... */ };
class ResizeObjectsCommand : public ICanvasCommand { /* ... */ };
class RotateObjectsCommand : public ICanvasCommand { /* ... */ };
class ChangePropertyCommand : public ICanvasCommand { /* ... */ };
class CompoundCommand : public ICanvasCommand { /* ... */ };
class ReorderZCommand : public ICanvasCommand { /* ... */ };
```

## Key Functions to Implement

1. `Board::to_json()` -- Serialize metadata (id, name, timestamps, settings) and all objects. Each object serialized via its virtual `to_json()`. Output is a JSON object with "metadata" and "objects" arrays.

2. `Board::from_json()` -- Parse JSON, reconstruct metadata, use BoardSerializer to instantiate objects from the "objects" array.

3. `Board::clone()` -- Deep copy: new Board with cloned metadata and each object cloned via `clone()`.

4. `UndoRedoStack::execute()` -- If in compound mode, push to compound_commands_. Otherwise push to undo_stack_ (clearing redo_stack_). If undo_stack exceeds max_history, evict oldest. Try merging with top of stack if `can_merge_with()`.

5. `MoveObjectsCommand` -- Stores object IDs, delta, and board reference. Execute applies delta. Undo applies negative delta. Can merge consecutive moves.

6. `Board::bring_to_front()` -- Set z_index of target to `next_z_index()`.

7. `Board::save_to_file()` -- Serialize to JSON, write to disk with std::error_code. Use `.markboard` file extension.

## Events to Add

```cpp
// In Events.h

MARKAMP_DECLARE_EVENT_WITH_FIELDS(CanvasUndoRedoChangedEvent)
bool can_undo{false};
bool can_redo{false};
std::string undo_description;
std::string redo_description;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardSavedEvent)
std::string board_id;
std::string file_path;
MARKAMP_DECLARE_EVENT_END;

MARKAMP_DECLARE_EVENT_WITH_FIELDS(BoardLoadedEvent)
std::string board_id;
std::string board_name;
MARKAMP_DECLARE_EVENT_END;
```

## JSON Format

```json
{
  "version": 1,
  "metadata": {
    "id": "board_abc123",
    "name": "Sprint Planning",
    "description": "",
    "created_at": "2026-02-15T10:00:00Z",
    "modified_at": "2026-02-15T12:30:00Z",
    "tags": ["planning", "sprint"],
    "archived": false,
    "background_color": "#f5f5f5",
    "grid_visible": true,
    "grid_spacing": 20.0
  },
  "objects": [
    {
      "type": "StickyNote",
      "id": 1,
      "name": "",
      "transform": { "tx": 100, "ty": 200, "rotation": 0, "scale_x": 1, "scale_y": 1 },
      "z_index": 0,
      "locked": false,
      "visible": true,
      "opacity": 1.0,
      "metadata": {},
      "tags": [],
      "parent_id": 0,
      "color": "#ffffcc",
      "text": "Review API design"
    }
  ]
}
```

## Shortcut Registrations

| Shortcut | Action | Context |
|----------|--------|---------|
| Cmd/Ctrl+Z | Undo | canvas |
| Cmd/Ctrl+Shift+Z | Redo | canvas |
| Cmd/Ctrl+S | Save board | canvas |

## Test Cases

File: `tests/unit/test_board_model.cpp`

1. **Board create** -- Create board, verify ID generated, name set.
2. **Add/remove objects** -- Add 3 objects, verify count. Remove one, verify count decrements.
3. **Z-order sorting** -- Add objects with z-indices 5, 1, 3. Verify objects_by_z_order returns 1, 3, 5.
4. **Bring to front** -- Object at z=1, bring_to_front, verify z > all others.
5. **Send to back** -- Object at z=5, send_to_back, verify z < all others.
6. **Content bounds** -- Add objects at known positions, verify content_bounds encloses all.
7. **Board clone** -- Clone board, verify separate IDs, same data.

File: `tests/unit/test_board_serializer.cpp`

8. **JSON round-trip** -- Serialize board, deserialize, verify metadata and object count match.
9. **Object JSON round-trip** -- Serialize a CanvasObject, deserialize, verify fields.
10. **Metadata serialization** -- Verify all metadata fields survive round-trip including timestamps.
11. **File save/load** -- Save to temp file, load, verify board state.
12. **Empty board serialization** -- Serialize board with 0 objects, verify valid JSON.

File: `tests/unit/test_undo_redo.cpp`

13. **Execute and undo** -- Execute MoveObjectsCommand, verify objects moved. Undo, verify original positions.
14. **Redo** -- Execute, undo, redo, verify objects at moved positions.
15. **Redo cleared on new command** -- Execute, undo, execute new command, verify can_redo is false.
16. **Command merging** -- Execute two MoveObjectsCommands, verify merged into one undo step.
17. **Compound command** -- Begin compound, execute 3 commands, end compound. Single undo reverts all 3.
18. **History limit** -- Set max_history=5, execute 10 commands, verify undo stack has 5.
19. **UndoRedoChangedEvent** -- Execute command, verify event published with correct state.

## Acceptance Criteria

- [ ] Board class manages objects with add/remove/get/z-order operations
- [ ] BoardSerializer serializes/deserializes Board to/from JSON using factory pattern
- [ ] .markboard file format saves and loads complete board state
- [ ] UndoRedoStack supports execute, undo, redo with max history limit
- [ ] ICanvasCommand interface with execute/undo/description
- [ ] Built-in commands: Add, Remove, Move, Resize, Rotate, ChangeProperty, Compound, ReorderZ
- [ ] Command merging for consecutive move operations
- [ ] Compound commands group multiple operations into one undo step
- [ ] Events published for undo/redo state changes, board save/load
- [ ] Board::clone() performs deep copy
- [ ] All 19 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/Board.h` | Board model + BoardMetadata |
| CREATE | `src/canvas/Board.cpp` | Board implementation |
| CREATE | `src/canvas/BoardSerializer.h` | Factory-based serialization |
| CREATE | `src/canvas/BoardSerializer.cpp` | JSON serialize/deserialize |
| CREATE | `src/canvas/UndoRedoStack.h` | Command stack |
| CREATE | `src/canvas/UndoRedoStack.cpp` | Undo/redo implementation |
| CREATE | `src/canvas/CanvasCommands.h` | Built-in command classes |
| CREATE | `src/canvas/CanvasCommands.cpp` | Command implementations |
| MODIFY | `src/core/Events.h` | Add undo/redo, board save/load events |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_board_model.cpp` | 7 tests |
| CREATE | `tests/unit/test_board_serializer.cpp` | 5 tests |
| CREATE | `tests/unit/test_undo_redo.cpp` | 7 tests |

## Architecture Notes

- Board is a pure data container with no EventBus dependency. It does not know about CanvasPanel.
- BoardSerializer uses a factory registry so that as new CanvasObject subclasses are added in later phases, they register their factory during app initialization.
- UndoRedoStack takes an EventBus reference only for publishing state events. Commands themselves hold references to the Board, not to UI components.
- The `.markboard` file format is a single JSON file. Future phases may add binary attachments (images) as sidecar files.
- nlohmann/json is used for JSON serialization (already available in the project).
- Timestamps use ISO 8601 UTC strings in JSON, converted to/from std::chrono::system_clock::time_point.

## Estimated Complexity

**XL** -- Board model with z-ordering, factory-based serializer, undo/redo command stack with merging and compound commands, 8 built-in command types, file I/O, 19 tests.
