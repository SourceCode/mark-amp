// V11 Phase 13: Typed Value Runtime & Conversion Engine

#include "node_editor/ValueConverter.h"

#include <cmath>

namespace markamp::node_editor
{

auto ValueConverter::can_convert(SocketDataType from, SocketDataType to) -> bool
{
    if (from == to)
    {
        return true;
    }

    // Numeric coercion: Float ↔ Int ↔ Bool
    if ((from == SocketDataType::Float || from == SocketDataType::Int ||
         from == SocketDataType::Bool) &&
        (to == SocketDataType::Float || to == SocketDataType::Int || to == SocketDataType::Bool))
    {
        return true;
    }

    // Float → Vector promotion
    if (from == SocketDataType::Float &&
        (to == SocketDataType::Vector2 || to == SocketDataType::Vector3 ||
         to == SocketDataType::Vector4))
    {
        return true;
    }

    // Color ↔ Vector4
    if ((from == SocketDataType::Color && to == SocketDataType::Vector4) ||
        (from == SocketDataType::Vector4 && to == SocketDataType::Color))
    {
        return true;
    }

    // Vector3 → Vector4 (w=1)
    if (from == SocketDataType::Vector3 && to == SocketDataType::Vector4)
    {
        return true;
    }

    // Vector2 → Vector3 (z=0)
    if (from == SocketDataType::Vector2 && to == SocketDataType::Vector3)
    {
        return true;
    }

    return false;
}

auto ValueConverter::convert(const NodeValue& value, SocketDataType target_type)
    -> std::optional<NodeValue>
{
    const auto source_type = value.type();
    if (source_type == target_type)
    {
        return value;
    }

    if (!can_convert(source_type, target_type))
    {
        return std::nullopt;
    }

    // Float conversions
    if (source_type == SocketDataType::Float)
    {
        const float fval = value.as_float().value_or(0.0F);
        switch (target_type)
        {
            case SocketDataType::Int:
                return NodeValue(static_cast<int>(fval));
            case SocketDataType::Bool:
                return NodeValue(fval != 0.0F);
            case SocketDataType::Vector2:
                return NodeValue(Vec2{fval, fval});
            case SocketDataType::Vector3:
                return NodeValue(Vec3{fval, fval, fval});
            case SocketDataType::Vector4:
                return NodeValue(Vec4{fval, fval, fval, fval});
            default:
                break;
        }
    }

    // Int conversions
    if (source_type == SocketDataType::Int)
    {
        const int ival = value.as_int().value_or(0);
        switch (target_type)
        {
            case SocketDataType::Float:
                return NodeValue(static_cast<float>(ival));
            case SocketDataType::Bool:
                return NodeValue(ival != 0);
            default:
                break;
        }
    }

    // Bool conversions
    if (source_type == SocketDataType::Bool)
    {
        const bool bval = value.as_bool().value_or(false);
        switch (target_type)
        {
            case SocketDataType::Float:
                return NodeValue(bval ? 1.0F : 0.0F);
            case SocketDataType::Int:
                return NodeValue(bval ? 1 : 0);
            default:
                break;
        }
    }

    // Color → Vector4
    if (source_type == SocketDataType::Color && target_type == SocketDataType::Vector4)
    {
        const auto col = value.as_color4().value_or(Color4{});
        return NodeValue(Vec4{col.r, col.g, col.b, col.a});
    }

    // Vector4 → Color
    if (source_type == SocketDataType::Vector4 && target_type == SocketDataType::Color)
    {
        const auto vec = value.as_vec4().value_or(Vec4{});
        return NodeValue(Color4{vec.x, vec.y, vec.z, vec.w});
    }

    // Vector2 → Vector3
    if (source_type == SocketDataType::Vector2 && target_type == SocketDataType::Vector3)
    {
        const auto vec = value.as_vec2().value_or(Vec2{});
        return NodeValue(Vec3{vec.x, vec.y, 0.0F});
    }

    // Vector3 → Vector4
    if (source_type == SocketDataType::Vector3 && target_type == SocketDataType::Vector4)
    {
        const auto vec = value.as_vec3().value_or(Vec3{});
        return NodeValue(Vec4{vec.x, vec.y, vec.z, 1.0F});
    }

    return std::nullopt;
}

} // namespace markamp::node_editor
