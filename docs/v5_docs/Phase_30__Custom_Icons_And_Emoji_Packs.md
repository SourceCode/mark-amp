# Phase 30 -- Custom Icons and Emoji Packs

## Objective

Implement an SVG icon library browser, emoji reaction badges that can be placed on or near canvas objects, and custom icon upload support. Includes a built-in set of common icons (arrows, symbols, markers) and the ability to import custom SVG icon packs.

## Prerequisites

- Phase 01 (CanvasObject)
- Phase 02 (CanvasRenderer, IObjectRenderer)
- Phase 09 (ShapeObject with rendering infrastructure)

## Feature References (PRD)

- PRD #72: Custom Icons & Emoji Packs
- PRD #65: Reactions (Emoji)

## Data Structures to Implement

### File: `src/canvas/IconObject.h`

```cpp
#pragma once

#include "CanvasObject.h"
#include "CanvasTypes.h"

#include <string>

namespace markamp::canvas
{

class IconObject : public CanvasObject
{
public:
    IconObject();

    [[nodiscard]] auto icon_id() const -> const std::string&;
    auto set_icon_id(const std::string& id) -> void;

    [[nodiscard]] auto svg_content() const -> const std::string&;
    auto set_svg_content(const std::string& svg) -> void;

    [[nodiscard]] auto icon_color() const -> const CanvasColor&;
    auto set_icon_color(const CanvasColor& color) -> void;

    [[nodiscard]] auto icon_size() const -> double;
    auto set_icon_size(double size) -> void;

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;

private:
    std::string icon_id_;
    std::string svg_content_;
    CanvasColor icon_color_{0, 0, 0, 255};
    double icon_size_{48.0};
};

} // namespace markamp::canvas
```

### File: `src/canvas/IconLibrary.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <filesystem>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

struct IconEntry
{
    std::string id;
    std::string name;
    std::string category;
    std::string svg_content;
};

class IconLibrary
{
public:
    /// Load built-in icons.
    auto load_builtins() -> void;

    /// Import a custom SVG icon pack from a directory.
    auto import_pack(const std::filesystem::path& directory, const std::string& category) -> int;

    /// Get all icons in a category.
    [[nodiscard]] auto icons_in_category(const std::string& category) const
        -> std::vector<const IconEntry*>;

    /// Get all categories.
    [[nodiscard]] auto categories() const -> std::vector<std::string>;

    /// Search icons by name.
    [[nodiscard]] auto search(const std::string& query) const -> std::vector<const IconEntry*>;

    /// Get icon by ID.
    [[nodiscard]] auto get_icon(const std::string& id) const -> const IconEntry*;

    /// Emoji set.
    [[nodiscard]] auto all_emojis() const -> const std::vector<std::string>&;

private:
    std::unordered_map<std::string, IconEntry> icons_;
    std::vector<std::string> emojis_;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `IconRenderer::render()` -- Parse SVG content and render using wxGraphicsContext paths. Apply icon_color as fill. Scale to icon_size.

2. `IconLibrary::load_builtins()` -- Load a bundled set of ~50 common SVG icons (arrows, checkmarks, stars, warning signs, etc.).

3. Icon picker panel: A grid of icon thumbnails organized by category. Search bar to filter. Click to place on canvas.

4. Emoji reactions: Small emoji badges that attach to canvas objects. Rendered as text near the object's corner.

## Test Cases

File: `tests/unit/test_icon_library.cpp`

1. **Load builtins** -- Verify builtins loaded, count > 0.
2. **Search icons** -- Search "arrow", verify matches.
3. **Categories** -- Verify categories list.
4. **Import pack** -- Import SVG files from temp dir, verify count.
5. **Icon object JSON** -- Serialize/deserialize.
6. **Emoji list** -- Verify emojis loaded.

## Acceptance Criteria

- [ ] IconObject with SVG content, color, size
- [ ] IconLibrary with built-in icons and custom import
- [ ] Icon picker panel with category filtering and search
- [ ] Emoji reaction badges on objects
- [ ] SVG rendering via wxGraphicsContext
- [ ] All 6 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/IconObject.h` | Icon data model |
| CREATE | `src/canvas/IconObject.cpp` | Implementation |
| CREATE | `src/canvas/IconRenderer.h` | SVG icon rendering |
| CREATE | `src/canvas/IconRenderer.cpp` | SVG path rendering |
| CREATE | `src/canvas/IconLibrary.h` | Icon library + emoji set |
| CREATE | `src/canvas/IconLibrary.cpp` | Built-in icons + import |
| CREATE | `resources/icons/` | Built-in SVG icon files |
| MODIFY | `src/canvas/BoardSerializer.cpp` | Register factory |
| MODIFY | `CMakeLists.txt` | Add new source files, bundle icons |
| CREATE | `tests/unit/test_icon_library.cpp` | 6 Catch2 tests |

## Architecture Notes

- SVG rendering is simplified: support basic paths (M, L, C, Z), fills, and strokes. Complex SVG features (gradients, filters, text) are not needed for icons.
- Icons are stored as SVG strings in the icon library. The renderer converts SVG path commands to wxGraphicsPath calls.

## Estimated Complexity

**M** -- SVG path parsing, icon library with categories, picker panel, emoji badges, 6 tests.
