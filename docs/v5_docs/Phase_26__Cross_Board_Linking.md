# Phase 26 -- Cross-Board Linking

## Objective

Implement hyperlinks between boards: a link object on one board that navigates to another board (optionally to a specific frame or object within it). Includes a link creation dialog, hover preview of the target board, and deep-link URL routing.

## Prerequisites

- Phase 05 (Board model with ID)
- Phase 14 (FrameObject for frame-specific links)
- Phase 09 (ShapeObject for link badges)

## Feature References (PRD)

- PRD #81: Cross-Board Linking

## Data Structures to Implement

### File: `src/canvas/CrossBoardLink.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <optional>
#include <string>

namespace markamp::canvas
{

struct BoardLinkTarget
{
    std::string board_id;
    std::string board_name;
    std::optional<ObjectId> frame_id;   // Optional: link to specific frame
    std::optional<ObjectId> object_id;  // Optional: link to specific object
};

class CrossBoardLinkObject : public CanvasObject
{
public:
    CrossBoardLinkObject();

    [[nodiscard]] auto target() const -> const BoardLinkTarget&;
    auto set_target(const BoardLinkTarget& target) -> void;

    [[nodiscard]] auto display_text() const -> const std::string&;
    auto set_display_text(const std::string& text) -> void;

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    BoardLinkTarget target_;
    std::string display_text_{"Link to board"};
    double width_{200.0};
    double height_{60.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/BoardNavigator.h`

```cpp
#pragma once

#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace markamp::canvas
{

struct BoardInfo
{
    std::string id;
    std::string name;
    std::filesystem::path path;
};

class BoardNavigator
{
public:
    using OnBoardOpenCallback = std::function<void(const std::string& board_id)>;

    /// Scan a directory for .markboard files.
    auto scan_boards(const std::filesystem::path& directory) -> void;

    [[nodiscard]] auto all_boards() const -> const std::vector<BoardInfo>&;
    [[nodiscard]] auto find_board(const std::string& id) const -> const BoardInfo*;

    auto navigate_to(const std::string& board_id) -> void;
    auto set_on_board_open(OnBoardOpenCallback cb) -> void;

private:
    std::vector<BoardInfo> boards_;
    OnBoardOpenCallback on_board_open_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `CrossBoardLinkRenderer` -- Render as a card with a link icon, board name, and arrow. Blue link styling.

2. Double-click on link: Navigate to the target board via BoardNavigator.

3. Hover preview: Show a small thumbnail of the target board when hovering.

4. Link creation dialog: Choose from available boards, optionally select a frame within the target board.

## Test Cases

File: `tests/unit/test_cross_board_link.cpp`

1. **Link creation** -- Set target board ID, verify.
2. **Display text** -- Set display text, verify.
3. **JSON round-trip** -- Serialize/deserialize link with target.
4. **Board scanner** -- Scan temp directory with .markboard files, verify found.
5. **Navigate callback** -- navigate_to, verify callback fired.
6. **Frame-specific link** -- Set frame_id in target, verify.

## Acceptance Criteria

- [ ] CrossBoardLinkObject with target board ID, optional frame/object
- [ ] Link card rendering with board name and link icon
- [ ] Double-click navigates to target board
- [ ] BoardNavigator scans for available boards
- [ ] Link creation dialog with board picker
- [ ] JSON serialization preserves link targets
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/CrossBoardLink.h` | Link object + target |
| CREATE | `src/canvas/CrossBoardLink.cpp` | Implementation |
| CREATE | `src/canvas/CrossBoardLinkRenderer.h` | Link rendering |
| CREATE | `src/canvas/CrossBoardLinkRenderer.cpp` | Card rendering |
| CREATE | `src/canvas/BoardNavigator.h` | Board scanning + navigation |
| CREATE | `src/canvas/BoardNavigator.cpp` | Implementation |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_cross_board_link.cpp` | 6 Catch2 tests |

## Architecture Notes

- BoardNavigator scans the workspace directory for .markboard files and reads their metadata headers without loading full board state.
- Navigation between boards is handled by publishing a BoardNavigateRequestEvent, which the application layer (MarkAmpApp or MainFrame) handles.

## Estimated Complexity

**M** -- Link object, board scanner, navigation integration, link card rendering, 6 tests.
