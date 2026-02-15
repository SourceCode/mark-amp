#include "SectionObject.h"

#include "canvas/Board.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

SectionObject::SectionObject()
    : CanvasObject(CanvasObjectType::Section)
{
}

// ── Dimensions ──────────────────────────────────────────────────

auto SectionObject::width() const -> double
{
    return width_;
}

auto SectionObject::height() const -> double
{
    return height_;
}

auto SectionObject::resize(double w, double h) -> void
{
    width_ = std::max(kMinWidth, w);
    height_ = std::max(kMinHeight, h);
    mark_dirty();
}

// ── Title ───────────────────────────────────────────────────────

auto SectionObject::title() const -> const std::string&
{
    return title_;
}

auto SectionObject::set_title(const std::string& title) -> void
{
    title_ = title;
    mark_dirty();
}

// ── Tint Color ──────────────────────────────────────────────────

auto SectionObject::tint_color() const -> CanvasColor
{
    return tint_color_;
}

auto SectionObject::set_tint_color(CanvasColor color) -> void
{
    tint_color_ = color;
    mark_dirty();
}

// ── Collapse ────────────────────────────────────────────────────

auto SectionObject::is_collapsed() const -> bool
{
    return collapsed_;
}

auto SectionObject::set_collapsed(bool collapsed) -> void
{
    collapsed_ = collapsed;
    mark_dirty();
}

// ── Containment ─────────────────────────────────────────────────

auto SectionObject::contains_object(const CanvasObject& obj) const -> bool
{
    if (obj.id() == id())
    {
        return false; // A section doesn't contain itself.
    }

    const auto section_bounds = world_bounds();
    const auto obj_bounds = obj.world_bounds();

    // Fully inside: all four corners of obj are within section bounds.
    return obj_bounds.min_x >= section_bounds.min_x && obj_bounds.min_y >= section_bounds.min_y &&
           obj_bounds.max_x <= section_bounds.max_x && obj_bounds.max_y <= section_bounds.max_y;
}

auto SectionObject::collect_contained_ids(const Board& board) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> contained;

    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr && contains_object(*obj_ptr))
        {
            contained.push_back(obj_ptr->id());
        }
    }

    return contained;
}

// ── CanvasObject overrides ──────────────────────────────────────

auto SectionObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto SectionObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<SectionObject>();
    copy->width_ = width_;
    copy->height_ = height_;
    copy->title_ = title_;
    copy->tint_color_ = tint_color_;
    copy->collapsed_ = collapsed_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto SectionObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Section\""
        << ",\"title\":\"" << title_ << "\""
        << ",\"collapsed\":" << (collapsed_ ? "true" : "false") << ",\"width\":" << width_
        << ",\"height\":" << height_ << ",\"tint_color\":\"" << tint_color_.to_hex() << "\""
        << "}";
    return oss.str();
}

auto SectionObject::from_json(const std::string& /*json*/) -> void
{
    // Stub: real JSON parsing would populate fields.
}

} // namespace markamp::canvas
