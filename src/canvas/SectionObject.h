#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

class Board;

/// A semantic grouping area with a subtle background tint and label header.
/// Similar to FrameObject but for organizational grouping rather than
/// presentation. Sections support collapse (hiding contained objects).
class SectionObject : public CanvasObject
{
public:
    SectionObject();

    // ── Dimensions ──────────────────────────────────────────────

    [[nodiscard]] auto width() const -> double;
    [[nodiscard]] auto height() const -> double;
    auto resize(double w, double h) -> void;

    // ── Title ───────────────────────────────────────────────────

    [[nodiscard]] auto title() const -> const std::string&;
    auto set_title(const std::string& title) -> void;

    // ── Tint Color ──────────────────────────────────────────────

    [[nodiscard]] auto tint_color() const -> CanvasColor;
    auto set_tint_color(CanvasColor color) -> void;

    // ── Collapse ────────────────────────────────────────────────

    [[nodiscard]] auto is_collapsed() const -> bool;
    auto set_collapsed(bool collapsed) -> void;

    // ── Containment ─────────────────────────────────────────────

    /// Check if a given object's world_bounds are fully inside this section.
    [[nodiscard]] auto contains_object(const CanvasObject& obj) const -> bool;

    /// Collect IDs of all board objects whose world_bounds are inside this section.
    [[nodiscard]] auto collect_contained_ids(const Board& board) const -> std::vector<ObjectId>;

    // ── CanvasObject overrides ─────────────────────────────────

    [[nodiscard]] auto local_bounds() const -> AABB override;
    [[nodiscard]] auto clone() const -> std::unique_ptr<CanvasObject> override;
    [[nodiscard]] auto to_json() const -> std::string override;
    auto from_json(const std::string& json) -> void override;

private:
    double width_{600.0};
    double height_{400.0};
    std::string title_{"Section 1"};
    CanvasColor tint_color_{227, 242, 253, 80}; // Light blue #E3F2FD, alpha 80
    bool collapsed_{false};

    static constexpr double kMinWidth = 150.0;
    static constexpr double kMinHeight = 100.0;
};

} // namespace markamp::canvas
