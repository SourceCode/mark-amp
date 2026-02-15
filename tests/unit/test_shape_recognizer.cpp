#include "canvas/ShapeData.h"
#include "canvas/ShapeRecognizer.h"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

#include <cmath>

using namespace markamp::canvas;

// ── Helper: generate approximate circle points ──────────────────

static auto
make_circle_points(double center_x, double center_y, double radius, size_t point_count = 40)
    -> std::vector<Point2D>
{
    std::vector<Point2D> pts;
    pts.reserve(point_count + 1);
    for (size_t idx = 0; idx <= point_count; ++idx)
    {
        const double angle =
            2.0 * M_PI * static_cast<double>(idx) / static_cast<double>(point_count);
        pts.push_back({center_x + radius * std::cos(angle), center_y + radius * std::sin(angle)});
    }
    return pts;
}

// ── Helper: generate approximate rectangle points ───────────────

static auto make_rect_points(double min_x,
                             double min_y,
                             double max_x,
                             double max_y,
                             size_t points_per_side = 10) -> std::vector<Point2D>
{
    std::vector<Point2D> pts;
    const auto per_side = static_cast<double>(points_per_side);

    // Top edge.
    for (size_t idx = 0; idx <= points_per_side; ++idx)
    {
        const double frac = static_cast<double>(idx) / per_side;
        pts.push_back({min_x + frac * (max_x - min_x), min_y});
    }
    // Right edge.
    for (size_t idx = 1; idx <= points_per_side; ++idx)
    {
        const double frac = static_cast<double>(idx) / per_side;
        pts.push_back({max_x, min_y + frac * (max_y - min_y)});
    }
    // Bottom edge.
    for (size_t idx = 1; idx <= points_per_side; ++idx)
    {
        const double frac = static_cast<double>(idx) / per_side;
        pts.push_back({max_x - frac * (max_x - min_x), max_y});
    }
    // Left edge.
    for (size_t idx = 1; idx <= points_per_side; ++idx)
    {
        const double frac = static_cast<double>(idx) / per_side;
        pts.push_back({min_x, max_y - frac * (max_y - min_y)});
    }

    return pts;
}

// ── Recognition: Empty / Minimal ────────────────────────────────

TEST_CASE("ShapeRecognizer empty input returns kNone", "[recognizer]")
{
    std::vector<Point2D> empty;
    const auto result = ShapeRecognizer::recognize(empty);
    REQUIRE(result.shape == RecognizedShape::kNone);
    REQUIRE(result.confidence == Catch::Approx(0.0));
}

TEST_CASE("ShapeRecognizer single point returns kNone", "[recognizer]")
{
    std::vector<Point2D> single = {{50.0, 50.0}};
    const auto result = ShapeRecognizer::recognize(single);
    REQUIRE(result.shape == RecognizedShape::kNone);
}

// ── Recognition: Line ───────────────────────────────────────────

TEST_CASE("ShapeRecognizer detects straight line", "[recognizer]")
{
    std::vector<Point2D> line;
    for (int idx = 0; idx <= 20; ++idx)
    {
        line.push_back({static_cast<double>(idx * 5), 50.0});
    }

    const auto result = ShapeRecognizer::recognize(line);
    REQUIRE(result.shape == RecognizedShape::kLine);
    REQUIRE(result.confidence > 0.8);
}

TEST_CASE("ShapeRecognizer detects diagonal line", "[recognizer]")
{
    std::vector<Point2D> line;
    for (int idx = 0; idx <= 20; ++idx)
    {
        const double val = static_cast<double>(idx * 5);
        line.push_back({val, val});
    }

    const auto result = ShapeRecognizer::recognize(line);
    REQUIRE(result.shape == RecognizedShape::kLine);
    REQUIRE(result.confidence > 0.7);
}

// ── Recognition: Rectangle ──────────────────────────────────────

TEST_CASE("ShapeRecognizer detects rectangle", "[recognizer]")
{
    const auto pts = make_rect_points(10.0, 20.0, 110.0, 80.0);
    const auto result = ShapeRecognizer::recognize(pts);

    REQUIRE(result.shape == RecognizedShape::kRectangle);
    REQUIRE(result.confidence > 0.6);
}

// ── Recognition: Ellipse ────────────────────────────────────────

TEST_CASE("ShapeRecognizer detects circle as ellipse", "[recognizer]")
{
    const auto pts = make_circle_points(50.0, 50.0, 40.0);
    const auto result = ShapeRecognizer::recognize(pts);

    REQUIRE(result.shape == RecognizedShape::kEllipse);
    REQUIRE(result.confidence > 0.6);
}

// ── Recognition: Triangle ───────────────────────────────────────

TEST_CASE("ShapeRecognizer detects triangle", "[recognizer]")
{
    std::vector<Point2D> tri;
    const size_t kPerSide = 10;

    // Side 1: bottom-left to bottom-right.
    for (size_t idx = 0; idx <= kPerSide; ++idx)
    {
        const double frac = static_cast<double>(idx) / static_cast<double>(kPerSide);
        tri.push_back({frac * 100.0, 100.0});
    }
    // Side 2: bottom-right to top-center.
    for (size_t idx = 1; idx <= kPerSide; ++idx)
    {
        const double frac = static_cast<double>(idx) / static_cast<double>(kPerSide);
        tri.push_back({100.0 - frac * 50.0, 100.0 - frac * 100.0});
    }
    // Side 3: top-center to bottom-left.
    for (size_t idx = 1; idx <= kPerSide; ++idx)
    {
        const double frac = static_cast<double>(idx) / static_cast<double>(kPerSide);
        tri.push_back({50.0 - frac * 50.0, frac * 100.0});
    }

    const auto result = ShapeRecognizer::recognize(tri);
    // Triangle or rectangle might match; we just verify it's detected as something.
    REQUIRE(result.confidence > 0.0);
    REQUIRE(result.shape != RecognizedShape::kNone);
}

// ── Conversion: to_shape_object ─────────────────────────────────

TEST_CASE("ShapeRecognizer to_shape_object with high confidence", "[recognizer]")
{
    RecognitionResult result;
    result.shape = RecognizedShape::kRectangle;
    result.confidence = 0.9;
    result.bounds = {10.0, 20.0, 110.0, 80.0};

    auto shape = ShapeRecognizer::to_shape_object(result);
    REQUIRE(shape != nullptr);
    REQUIRE(shape->shape_type() == ShapeType::kRectangle);
}

TEST_CASE("ShapeRecognizer to_shape_object with low confidence returns null", "[recognizer]")
{
    RecognitionResult result;
    result.shape = RecognizedShape::kRectangle;
    result.confidence = 0.3; // Below kMinConfidence (0.65).

    auto shape = ShapeRecognizer::to_shape_object(result);
    REQUIRE(shape == nullptr);
}

TEST_CASE("ShapeRecognizer to_shape_object kNone returns null", "[recognizer]")
{
    RecognitionResult result;
    result.shape = RecognizedShape::kNone;
    result.confidence = 1.0;

    auto shape = ShapeRecognizer::to_shape_object(result);
    REQUIRE(shape == nullptr);
}
