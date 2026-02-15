#include "ShapeData.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

ShapeObject::ShapeObject()
    : CanvasObject(CanvasObjectType::Shape)
{
}

ShapeObject::ShapeObject(ShapeType type)
    : CanvasObject(CanvasObjectType::Shape)
    , shape_type_(type)
{
}

// ── Shape Type ─────────────────────────────────────────────────

auto ShapeObject::shape_type() const -> ShapeType
{
    return shape_type_;
}
auto ShapeObject::set_shape_type(ShapeType type) -> void
{
    shape_type_ = type;
    mark_dirty();
}

// ── Style ──────────────────────────────────────────────────────

auto ShapeObject::style() const -> const ShapeStyle&
{
    return style_;
}
auto ShapeObject::set_style(const ShapeStyle& style) -> void
{
    style_ = style;
    mark_dirty();
}

// ── Dimensions ─────────────────────────────────────────────────

auto ShapeObject::width() const -> double
{
    return width_;
}
auto ShapeObject::height() const -> double
{
    return height_;
}

auto ShapeObject::resize(double w, double h) -> void
{
    width_ = std::max(kMinSize, w);
    height_ = std::max(kMinSize, h);
    mark_dirty();
}

// ── Embedded Text ──────────────────────────────────────────────

auto ShapeObject::text() const -> const std::string&
{
    return text_;
}
auto ShapeObject::set_text(const std::string& text) -> void
{
    text_ = text;
    mark_dirty();
}

auto ShapeObject::text_style() const -> const TextStyle&
{
    return text_style_;
}
auto ShapeObject::set_text_style(const TextStyle& style) -> void
{
    text_style_ = style;
    mark_dirty();
}

// ── CanvasObject overrides ─────────────────────────────────────

auto ShapeObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto ShapeObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<ShapeObject>(shape_type_);
    copy->style_ = style_;
    copy->width_ = width_;
    copy->height_ = height_;
    copy->text_ = text_;
    copy->text_style_ = text_style_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto ShapeObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Shape\""
        << ",\"shape_type\":" << static_cast<int>(shape_type_) << ",\"width\":" << width_
        << ",\"height\":" << height_ << ",\"stroke_width\":" << style_.stroke_width
        << ",\"corner_radius\":" << style_.corner_radius << "}";
    return oss.str();
}

auto ShapeObject::from_json(const std::string& /*json*/) -> void
{
    // Stub.
}

} // namespace markamp::canvas
