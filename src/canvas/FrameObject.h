#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;

/// A logical container rectangle that defines a board area usable as
/// a slide or page for presentation mode. Objects whose world_bounds
/// fall inside the frame are considered "contained".
class FrameObject : public CanvasObject
{
public:
    FrameObject();

    // ── Dimensions ──────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto resize(double w, double h) -> void;

    // ── Title ───────────────────────────────────────────────────

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;

    [[nodiscard]] auto show_title() const -> bool;
    auto set_show_title(bool show) -> void;

    // ── Sequence Number ─────────────────────────────────────────

    [[nodiscard]] auto sequence_number() const -> int;
    auto set_sequence_number(int seq) -> void;

    // ── Colors ──────────────────────────────────────────────────

    [[nodiscard]] auto background_color() const -> CanvasColor;
    auto set_background_color(CanvasColor color) -> void;

    [[nodiscard]] auto border_color() const -> CanvasColor;
    auto set_border_color(CanvasColor color) -> void;

    // ── Containment ─────────────────────────────────────────────

    /// Check if a given object's world_bounds are fully inside this frame.
    [[nodiscard]] auto contains_object(const CanvasObject& obj) const -> bool;

    /// Collect IDs of all board objects whose world_bounds are inside this frame.
    [[nodiscard]] auto collect_contained_ids(const Board& board) const -> std::vector<ObjectId>;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

    /// Whether the frame has a non-default title.
    [[nodiscard]] auto has_title_set() const noexcept -> bool
    {
        return !title_.empty() && title_ != "Frame 1";
    }

    /// Whether the frame is landscape (wider than tall).
    [[nodiscard]] auto is_landscape() const noexcept -> bool
    {
        return width_ > height_;
    }

    /// Whether the frame is portrait (taller than wide).
    [[nodiscard]] auto is_portrait() const noexcept -> bool
    {
        return height_ > width_;
    }

    // ── Round 2 Batch 1 (#1-5) ────────────────────────────────────

    /// (#1) Frame area in world units².
    [[nodiscard]] auto area() const noexcept -> double
    {
        return width_ * height_;
    }

    /// (#2) Width-to-height aspect ratio.
    [[nodiscard]] auto aspect_ratio() const noexcept -> double
    {
        return height_ > 0.0 ? width_ / height_ : 0.0;
    }

    /// (#3) Whether the frame is a square.
    [[nodiscard]] auto is_square() const noexcept -> bool
    {
        return width_ == height_;
    }

    /// (#4) Whether the background color differs from default white.
    [[nodiscard]] auto has_custom_background() const noexcept -> bool
    {
        return background_color_.r != 255 || background_color_.g != 255 ||
               background_color_.b != 255;
    }

    /// (#5) Whether this is the first frame in the sequence.
    [[nodiscard]] auto is_first_frame() const noexcept -> bool
    {
        return sequence_number_ == 1;
    }

private:
    double width_{800.0};
    double height_{600.0};
    std::string title_{"Frame 1"};
    bool show_title_{true};
    int sequence_number_{1};
    CanvasColor background_color_{255, 255, 255, 255}; // White
    CanvasColor border_color_{180, 180, 180, 255};     // Gray

    static constexpr double kMinWidth = 100.0;
    static constexpr double kMinHeight = 100.0;
};

} // namespace markamp::canvas
