#include "DiagramShapeObject.h"

#include <sstream>

namespace markamp::canvas
{

DiagramShapeObject::DiagramShapeObject()
    : CanvasObject(CanvasObjectType::DiagramShape)
{
    set_name("Diagram Shape");
}

auto DiagramShapeObject::library() const -> DiagramLibrary
{
    return library_;
}
auto DiagramShapeObject::set_library(DiagramLibrary lib) -> void
{
    library_ = lib;
    mark_dirty();
}

auto DiagramShapeObject::uml_type() const -> UMLShapeType
{
    return uml_type_;
}
auto DiagramShapeObject::set_uml_type(UMLShapeType type) -> void
{
    uml_type_ = type;
    mark_dirty();
}

auto DiagramShapeObject::bpmn_type() const -> BPMNShapeType
{
    return bpmn_type_;
}
auto DiagramShapeObject::set_bpmn_type(BPMNShapeType type) -> void
{
    bpmn_type_ = type;
    mark_dirty();
}

auto DiagramShapeObject::compartments() const -> const std::vector<std::string>&
{
    return compartments_;
}
auto DiagramShapeObject::set_compartments(const std::vector<std::string>& compartments) -> void
{
    compartments_ = compartments;
    mark_dirty();
}

auto DiagramShapeObject::title() const -> const std::string&
{
    return title_;
}
auto DiagramShapeObject::set_title(const std::string& title) -> void
{
    title_ = title;
    mark_dirty();
}

auto DiagramShapeObject::width() const -> double
{
    return width_;
}
auto DiagramShapeObject::height() const -> double
{
    return height_;
}
auto DiagramShapeObject::set_dimensions(double w, double h) -> void
{
    width_ = w;
    height_ = h;
    mark_dirty();
}

auto DiagramShapeObject::fill_color() const -> const CanvasColor&
{
    return fill_color_;
}
auto DiagramShapeObject::set_fill_color(const CanvasColor& color) -> void
{
    fill_color_ = color;
    mark_dirty();
}
auto DiagramShapeObject::stroke_color() const -> const CanvasColor&
{
    return stroke_color_;
}
auto DiagramShapeObject::set_stroke_color(const CanvasColor& color) -> void
{
    stroke_color_ = color;
    mark_dirty();
}

auto DiagramShapeObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, width_, height_};
}

auto DiagramShapeObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"DiagramShape\""
        << ",\"library\":" << static_cast<int>(library_)
        << ",\"uml_type\":" << static_cast<int>(uml_type_)
        << ",\"bpmn_type\":" << static_cast<int>(bpmn_type_) << ",\"title\":\"" << title_ << "\""
        << ",\"width\":" << width_ << ",\"height\":" << height_ << ",\"compartments\":[";
    for (size_t idx = 0; idx < compartments_.size(); ++idx)
    {
        if (idx > 0)
        {
            oss << ",";
        }
        oss << "\"" << compartments_[idx] << "\"";
    }
    oss << "]}";
    return oss.str();
}

auto DiagramShapeObject::from_json(const std::string& /*json*/) -> void
{ /* stub */
}

auto DiagramShapeObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<DiagramShapeObject>();
    copy->set_library(library_);
    copy->set_uml_type(uml_type_);
    copy->set_bpmn_type(bpmn_type_);
    copy->set_compartments(compartments_);
    copy->set_title(title_);
    copy->set_dimensions(width_, height_);
    copy->set_fill_color(fill_color_);
    copy->set_stroke_color(stroke_color_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
