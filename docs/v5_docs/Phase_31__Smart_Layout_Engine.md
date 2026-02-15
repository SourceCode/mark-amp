# Phase 31 -- Smart Layout Engine

## Objective

Implement auto-arrangement algorithms that organize selected canvas objects into clean layouts: grid, radial, tree, force-directed. Also includes smart sticky aggregation (detecting and suggesting merges for duplicate sticky notes). Users select objects, choose a layout, and objects animate into position.

## Prerequisites

- Phase 01 (CanvasObject, Point2D, AABB)
- Phase 04 (SelectionManager for selected objects)
- Phase 05 (UndoRedoStack for undo)
- Phase 07 (StickyNoteObject for aggregation)
- Existing ForceDirectedLayout (`src/core/ForceDirectedLayout.h`)

## Feature References (PRD)

- PRD #56: Smart Layout
- PRD #92: Smart Sticky Aggregation

## Data Structures to Implement

### File: `src/canvas/LayoutEngine.h`

```cpp
#pragma once

#include "CanvasTypes.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

enum class LayoutType : uint8_t
{
    Grid,
    Radial,
    Tree,
    ForceDirected,
    Horizontal,
    Vertical,
    Circular
};

struct LayoutOptions
{
    LayoutType type{LayoutType::Grid};
    double spacing{40.0};
    int columns{0};  // 0 = auto-compute for grid
    double radius{200.0}; // For radial layout
    bool animate{true};
    double animation_duration{0.5}; // Seconds
};

struct LayoutResult
{
    ObjectId id;
    Point2D new_position;
};

class LayoutEngine
{
public:
    /// Compute new positions for the given objects using the specified layout.
    [[nodiscard]] auto compute_layout(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        const LayoutOptions& options) const -> std::vector<LayoutResult>;

private:
    [[nodiscard]] auto layout_grid(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_radial(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_tree(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_force_directed(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        const LayoutOptions& options) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_linear(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        bool horizontal,
        double spacing) const -> std::vector<LayoutResult>;

    [[nodiscard]] auto layout_circular(
        const std::vector<std::pair<ObjectId, AABB>>& objects,
        double radius) const -> std::vector<LayoutResult>;
};

} // namespace markamp::canvas
```

### File: `src/canvas/StickyAggregator.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "StickyNoteObject.h"

#include <string>
#include <vector>

namespace markamp::canvas
{

struct AggregationSuggestion
{
    ObjectId note_a;
    ObjectId note_b;
    double similarity{0.0}; // 0.0-1.0
    std::string reason;     // "Identical text", "Similar content"
};

class StickyAggregator
{
public:
    /// Find duplicate or near-duplicate sticky notes.
    [[nodiscard]] auto find_duplicates(
        const std::vector<std::pair<ObjectId, const StickyNoteObject*>>& notes,
        double threshold = 0.8) const -> std::vector<AggregationSuggestion>;

private:
    /// Compute text similarity (Jaccard index on word sets).
    [[nodiscard]] auto text_similarity(const std::string& a, const std::string& b) const
        -> double;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `LayoutEngine::layout_grid()` -- Sort objects by current Y then X. Compute optimal column count if not specified (sqrt of n). Place objects in a grid with spacing.

2. `LayoutEngine::layout_radial()` -- Place objects evenly around a circle of given radius. Center = centroid of current positions.

3. `LayoutEngine::layout_circular()` -- Place on circle perimeter, evenly spaced by angle.

4. `LayoutEngine::layout_force_directed()` -- Use the existing ForceDirectedLayout from `src/core/`. Add all objects as nodes with uniform links. Run to convergence.

5. `StickyAggregator::text_similarity()` -- Split texts into word sets. Compute Jaccard index: |A intersect B| / |A union B|.

6. Layout animation: Instead of instant repositioning, interpolate positions over animation_duration using a timer.

## Test Cases

File: `tests/unit/test_layout_engine.cpp`

1. **Grid layout 4 objects** -- 4 objects, grid layout, verify 2x2 positions.
2. **Grid auto columns** -- 9 objects, verify 3x3.
3. **Radial layout** -- 6 objects, verify equidistant from center.
4. **Horizontal layout** -- 3 objects, verify lined up horizontally with spacing.
5. **Vertical layout** -- 3 objects, verify vertical stack.
6. **Circular layout** -- 8 objects, verify on circle.
7. **Force-directed** -- 10 objects, verify no overlapping bounds.

File: `tests/unit/test_sticky_aggregator.cpp`

8. **Identical text** -- Two notes with same text, verify similarity=1.0.
9. **Similar text** -- Two notes with 80% shared words, verify similarity > 0.7.
10. **Different text** -- Two unrelated notes, verify similarity < 0.3.
11. **Threshold filtering** -- Set threshold=0.9, verify only identical pairs returned.

## Acceptance Criteria

- [ ] LayoutEngine supports 7 layout types
- [ ] Grid layout auto-computes optimal column count
- [ ] Radial and circular layouts position objects equidistantly
- [ ] Force-directed layout uses existing ForceDirectedLayout class
- [ ] StickyAggregator detects duplicate sticky notes via Jaccard similarity
- [ ] Layout results are applied as a compound undo command
- [ ] All 11 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/LayoutEngine.h` | Layout computation |
| CREATE | `src/canvas/LayoutEngine.cpp` | Layout algorithms |
| CREATE | `src/canvas/StickyAggregator.h` | Duplicate detection |
| CREATE | `src/canvas/StickyAggregator.cpp` | Similarity computation |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_layout_engine.cpp` | 7 tests |
| CREATE | `tests/unit/test_sticky_aggregator.cpp` | 4 tests |

## Architecture Notes

- LayoutEngine is stateless: takes object bounds in, returns new positions out. The caller applies positions via MoveObjectsCommand.
- The force-directed layout reuses the existing ForceDirectedLayout from src/core/ (designed for the graph view), demonstrating DRY reuse across features.
- Animation is handled at the CanvasPanel level using a wxTimer to interpolate positions.

## Estimated Complexity

**L** -- 7 layout algorithms, text similarity, animation interpolation, 11 tests.
