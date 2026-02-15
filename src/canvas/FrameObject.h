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
