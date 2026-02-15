#pragma once

#include "canvas/CanvasTypes.h"

#include <memory>
#include <vector>

namespace markamp::canvas
{

class FreehandPath;
class ShapeObject;

/// Types of shapes that the recognizer can detect from freehand strokes.
enum class RecognizedShape : uint8_t
{
    kNone,
    kLine,
    kRectangle,
    kEllipse,
    kTriangle,
    kArrow
};

/// Result of shape recognition analysis.
struct RecognitionResult
{
    RecognizedShape shape{RecognizedShape::kNone};
    double confidence{0.0}; ///< 0.0–1.0, higher = better match.
    AABB bounds;            ///< Bounding box of the detected shape.
    double angle{0.0};      ///< Rotation angle in radians (for lines/arrows).
};

/// Analyzes freehand point sequences and detects basic geometric shapes.
/// All methods are static; no instance state is needed.
class ShapeRecognizer
{
public:
    /// Minimum confidence threshold for a positive recognition.
    static constexpr double kMinConfidence = 0.65;

    /// Analyze a point sequence and return the best-matching shape.
    [[nodiscard]] static auto recognize(const std::vector<Point2D>& points) -> RecognitionResult;

    /// Convert a recognition result into a clean ShapeObject at the detected bounds.
    [[nodiscard]] static auto to_shape_object(const RecognitionResult& result)
        -> std::unique_ptr<ShapeObject>;

private:
    /// Individual shape detectors. Each returns a confidence score (0.0–1.0).
    [[nodiscard]] static auto detect_line(const std::vector<Point2D>& points,
                                          RecognitionResult& result) -> double;

    [[nodiscard]] static auto detect_rectangle(const std::vector<Point2D>& points,
                                               RecognitionResult& result) -> double;

    [[nodiscard]] static auto detect_ellipse(const std::vector<Point2D>& points,
                                             RecognitionResult& result) -> double;

    [[nodiscard]] static auto detect_triangle(const std::vector<Point2D>& points,
                                              RecognitionResult& result) -> double;

    [[nodiscard]] static auto detect_arrow(const std::vector<Point2D>& points,
                                           RecognitionResult& result) -> double;

    /// Helper: compute centroid of a point sequence.
    [[nodiscard]] static auto centroid(const std::vector<Point2D>& points) -> Point2D;

    /// Helper: compute the bounding box of a point sequence.
    [[nodiscard]] static auto compute_bounds(const std::vector<Point2D>& points) -> AABB;

    /// Helper: check if the path is approximately closed (start ≈ end).
    [[nodiscard]] static auto is_closed(const std::vector<Point2D>& points,
                                        double threshold_ratio = 0.15) -> bool;

    /// Helper: compute path length.
    [[nodiscard]] static auto path_length(const std::vector<Point2D>& points) -> double;

    /// Helper: count dominant corners (significant direction changes).
    [[nodiscard]] static auto count_corners(const std::vector<Point2D>& points,
                                            double angle_threshold_deg = 40.0) -> size_t;
};

} // namespace markamp::canvas
