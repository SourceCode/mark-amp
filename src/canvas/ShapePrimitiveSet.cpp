#include "ShapePrimitiveSet.h"

#include <algorithm>

namespace markamp::canvas
{

auto ShapePrimitive::type_name() const -> std::string
{
    switch (type)
    {
        case ShapeType::kRect:
            return "rect";
        case ShapeType::kEllipse:
            return "ellipse";
        case ShapeType::kTriangle:
            return "triangle";
        case ShapeType::kDiamond:
            return "diamond";
        case ShapeType::kArrow:
            return "arrow";
        case ShapeType::kLine:
            return "line";
        case ShapeType::kPolygon:
            return "polygon";
        case ShapeType::kStar:
            return "star";
    }
    return "unknown";
}

auto ShapePrimitive::center_x() const -> double
{
    return pos_x + width / 2.0;
}

auto ShapePrimitive::center_y() const -> double
{
    return pos_y + height / 2.0;
}

auto ShapePrimitive::area() const -> double
{
    return width * height;
}

void ShapePrimitiveSet::add_shape(const ShapePrimitive& shape)
{
    shapes_.push_back(shape);
}

void ShapePrimitiveSet::remove_shape(const std::string& shape_id)
{
    shapes_.erase(std::remove_if(shapes_.begin(),
                                 shapes_.end(),
                                 [&shape_id](const ShapePrimitive& shape)
                                 { return shape.shape_id == shape_id; }),
                  shapes_.end());
}

auto ShapePrimitiveSet::find_shape(const std::string& shape_id) const -> const ShapePrimitive*
{
    for (const auto& shape : shapes_)
    {
        if (shape.shape_id == shape_id)
        {
            return &shape;
        }
    }
    return nullptr;
}

auto ShapePrimitiveSet::shape_count() const -> int
{
    return static_cast<int>(shapes_.size());
}

void ShapePrimitiveSet::set_last_style(const ShapeDefaults& style)
{
    last_style_ = style;
}

auto ShapePrimitiveSet::last_style() const -> const ShapeDefaults&
{
    return last_style_;
}

auto ShapePrimitiveSet::create_with_style(const std::string& shape_id,
                                          ShapeType type,
                                          double pos_x,
                                          double pos_y,
                                          double width,
                                          double height) const -> ShapePrimitive
{
    ShapePrimitive shape;
    shape.shape_id = shape_id;
    shape.type = type;
    shape.pos_x = pos_x;
    shape.pos_y = pos_y;
    shape.width = width;
    shape.height = height;
    shape.defaults = last_style_;
    return shape;
}

auto ShapePrimitiveSet::shapes_of_type(ShapeType type) const -> std::vector<const ShapePrimitive*>
{
    std::vector<const ShapePrimitive*> result;
    for (const auto& shape : shapes_)
    {
        if (shape.type == type)
        {
            result.push_back(&shape);
        }
    }
    return result;
}

} // namespace markamp::canvas
