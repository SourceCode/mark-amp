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

auto DiagramShapeObject::from_json(const std::string& json) -> void
{
    // Helper to extract a numeric value.
    auto get_num = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        pos += needle.size();
        return std::stod(json.substr(pos));
    };

    // Helper to extract a string value.
    auto get_str = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return "";
        }
        pos += needle.size();
        const auto end_pos = json.find('"', pos);
        if (end_pos == std::string::npos)
        {
            return "";
        }
        return json.substr(pos, end_pos - pos);
    };

    library_ = static_cast<DiagramLibrary>(static_cast<int>(get_num("library")));
    uml_type_ = static_cast<UMLShapeType>(static_cast<int>(get_num("uml_type")));
    bpmn_type_ = static_cast<BPMNShapeType>(static_cast<int>(get_num("bpmn_type")));
    title_ = get_str("title");
    width_ = get_num("width");
    height_ = get_num("height");

    // Parse compartments array.
    compartments_.clear();
    auto comp_pos = json.find("\"compartments\":[");
    if (comp_pos != std::string::npos)
    {
        comp_pos += 16; // Skip past "compartments":[
        const auto arr_end = json.find(']', comp_pos);
        if (arr_end != std::string::npos)
        {
            const std::string arr_str = json.substr(comp_pos, arr_end - comp_pos);
            size_t pos = 0;
            while ((pos = arr_str.find('"', pos)) != std::string::npos)
            {
                ++pos; // Skip opening quote.
                const auto str_end = arr_str.find('"', pos);
                if (str_end == std::string::npos)
                {
                    break;
                }
                compartments_.push_back(arr_str.substr(pos, str_end - pos));
                pos = str_end + 1;
            }
        }
    }
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
