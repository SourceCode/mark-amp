# Phase 12 -- Smart Drawing Recognition

## Objective

Implement shape recognition that detects when a freehand stroke resembles a geometric shape (rectangle, circle, triangle, line, arrow) and offers to auto-replace it with the corresponding clean ShapeObject. Uses geometric heuristics rather than ML for a lightweight, deterministic approach.

## Prerequisites

- Phase 11 (FreehandPathObject with samples)
- Phase 09 (ShapeObject for replacement shapes)
- Phase 05 (UndoRedoStack for atomic replace)

## Feature References (PRD)

- PRD #14: Smart Drawing Recognition

## Data Structures to Implement

### File: `src/canvas/ShapeRecognizer.h`

```cpp
#pragma once

#include "CanvasTypes.h"
#include "FreehandPathObject.h"

#include <optional>
#include <vector>

namespace markamp::canvas
{

/// Recognized shape result.
struct RecognizedShape
{
    ShapeType shape_type;
    AABB bounding_box;         // Computed bounding box for the shape
    double confidence{0.0};    // 0.0-1.0 confidence score
    double rotation{0.0};      // Rotation angle for the recognized shape
    std::vector<Point2D> vertices; // Detected vertices (for polygon shapes)
};

/// Analyzes freehand strokes and recognizes geometric shapes.
class ShapeRecognizer
{
public:
    ShapeRecognizer();

    /// Analyze a completed freehand stroke and attempt recognition.
    /// Returns nullopt if no shape is recognized with sufficient confidence.
    [[nodiscard]] auto recognize(const FreehandPathObject& stroke) const
        -> std::optional<RecognizedShape>;

    /// Set the minimum confidence threshold for recognition.
    auto set_min_confidence(double threshold) -> void;
    [[nodiscard]] auto min_confidence() const -> double;

    /// Enable/disable specific shape recognition.
    auto set_recognize_rectangles(bool enabled) -> void;
    auto set_recognize_circles(bool enabled) -> void;
    auto set_recognize_triangles(bool enabled) -> void;
    auto set_recognize_lines(bool enabled) -> void;
    auto set_recognize_arrows(bool enabled) -> void;

private:
    double min_confidence_{0.75};
    bool recognize_rectangles_{true};
    bool recognize_circles_{true};
    bool recognize_triangles_{true};
    bool recognize_lines_{true};
    bool recognize_arrows_{true};

    // --- Shape detectors ---

    /// Check if stroke forms a closed shape (start near end).
    [[nodiscard]] auto is_closed_stroke(const std::vector<PathSample>& samples) const -> bool;

    /// Detect a circle/ellipse by checking circularity ratio.
    [[nodiscard]] auto detect_circle(const std::vector<PathSample>& samples) const
        -> std::optional<RecognizedShape>;

    /// Detect a rectangle by finding 4 corners and near-right angles.
    [[nodiscard]] auto detect_rectangle(const std::vector<PathSample>& samples) const
        -> std::optional<RecognizedShape>;

    /// Detect a triangle by finding 3 corners.
    [[nodiscard]] auto detect_triangle(const std::vector<PathSample>& samples) const
        -> std::optional<RecognizedShape>;

    /// Detect a straight line.
    [[nodiscard]] auto detect_line(const std::vector<PathSample>& samples) const
        -> std::optional<RecognizedShape>;

    /// Detect an arrow (line with a V at one end).
    [[nodiscard]] auto detect_arrow(const std::vector<PathSample>& samples) const
        -> std::optional<RecognizedShape>;

    /// Find dominant corners in the path using curvature analysis.
    [[nodiscard]] auto find_corners(const std::vector<PathSample>& samples,
                                     double angle_threshold = 30.0) const
        -> std::vector<int>; // Indices into samples

    /// Compute the circularity: 4*pi*area / perimeter^2. Perfect circle = 1.0.
    [[nodiscard]] auto circularity(const std::vector<PathSample>& samples) const -> double;

    /// Compute the angle between three consecutive points.
    [[nodiscard]] auto angle_at(const Point2D& a, const Point2D& b, const Point2D& c) const
        -> double;
};

} // namespace markamp::canvas
```

## Key Functions to Implement

1. `ShapeRecognizer::recognize()` -- Pipeline: (1) Check if closed stroke. (2) If closed: try circle, then rectangle, then triangle. (3) If open: try line, then arrow. (4) Return the result with highest confidence above threshold.

2. `detect_circle()` -- Compute bounding box. Check aspect ratio (should be near 1:1 for circle). Compute circularity = 4 * PI * area / perimeter^2. If > 0.8, recognize as circle. Confidence = circularity score.

3. `detect_rectangle()` -- Find corners using curvature analysis. Expect exactly 4 corners. Check that angles are near 90 degrees. Check that opposite sides are similar length. Confidence based on angle deviation and side ratio.

4. `detect_triangle()` -- Find 3 corners. Check that angles sum to ~180 degrees. Check that the stroke is closed.

5. `detect_line()` -- Compute the regression line through all samples. Measure max deviation from the line. If max deviation / length < threshold, recognize as line. Confidence = 1.0 - (max_deviation / length).

6. `detect_arrow()` -- Detect a line with a V-shape at one end. Split stroke into main line + head. Check that the head forms an acute angle.

7. `find_corners()` -- For each sample, compute the angle between the vectors to the previous and next samples (at a look-ahead distance). Points where the angle is below a threshold are corners.

8. `is_closed_stroke()` -- Check if the distance between first and last samples is < 15% of the total stroke length.

9. Integration: After DrawTool::on_mouse_up(), call ShapeRecognizer::recognize(). If recognized, show a tooltip/popup asking "Replace with [shape]?" with Accept/Dismiss. On accept, replace FreehandPathObject with ShapeObject (as a compound undo command).

## Test Cases

File: `tests/unit/test_shape_recognizer.cpp`

1. **Circle detection** -- Generate circular sample points, verify recognized as Circle with confidence > 0.8.
2. **Ellipse detection** -- Generate elliptical samples, verify recognized as Ellipse or Circle.
3. **Rectangle detection** -- Generate rectangular samples (4 corners, right angles), verify Rectangle.
4. **Triangle detection** -- Generate triangular samples, verify Triangle.
5. **Line detection** -- Generate near-linear samples, verify line recognition.
6. **Arrow detection** -- Generate line with V-head, verify arrow recognition.
7. **Scribble rejection** -- Generate random scribble, verify no recognition (nullopt).
8. **Closed stroke detection** -- Samples with first ~= last, verify is_closed_stroke.
9. **Open stroke detection** -- Samples with first far from last, verify not closed.
10. **Corner finding** -- Rectangular path, verify 4 corners found.
11. **Circularity computation** -- Perfect circle samples, verify circularity > 0.9.
12. **Confidence threshold** -- Set high threshold, verify weak matches rejected.

## Acceptance Criteria

- [ ] ShapeRecognizer detects circles, rectangles, triangles, lines, and arrows
- [ ] Confidence scores accurately reflect match quality
- [ ] Corner detection using curvature analysis
- [ ] Circularity metric for circle/ellipse detection
- [ ] Closed vs open stroke classification
- [ ] Auto-replacement prompt after freehand stroke completion
- [ ] Replacement is a compound undo operation (remove stroke + add shape)
- [ ] Recognition runs in < 5ms for strokes with 500 samples
- [ ] All 12 test cases pass

## Files to Create/Modify

| Action | File | Description |
|--------|------|-------------|
| CREATE | `src/canvas/ShapeRecognizer.h` | Shape recognition engine |
| CREATE | `src/canvas/ShapeRecognizer.cpp` | Detection algorithms |
| MODIFY | `src/canvas/DrawTool.cpp` | Integrate recognition after stroke completion |
| MODIFY | `CMakeLists.txt` | Add new source files |
| CREATE | `tests/unit/test_shape_recognizer.cpp` | 12 Catch2 tests |

## Architecture Notes

- ShapeRecognizer is a stateless utility class with no EventBus or UI dependencies.
- The recognition pipeline is designed to be fast (< 5ms) so it can run synchronously on mouse_up without blocking the UI thread.
- The heuristic approach (curvature analysis, circularity, corner detection) is more predictable and debuggable than ML-based approaches, and requires no model files.
- The replacement prompt is rendered as a canvas overlay near the stroke, with Accept/Dismiss buttons.

## Estimated Complexity

**M** -- Geometric analysis algorithms (curvature, circularity, corner detection), 5 shape detectors, confidence scoring, 12 tests.
