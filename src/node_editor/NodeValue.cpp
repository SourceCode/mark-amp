// V11 Phase 13: Typed Value Runtime & Conversion Engine

#include "node_editor/NodeValue.h"

#include <sstream>

namespace markamp::node_editor
{

// ============================================================================
// Constructors
// ============================================================================

NodeValue::NodeValue(float val)
    : value_(val)
{
}
NodeValue::NodeValue(int val)
    : value_(val)
{
}
NodeValue::NodeValue(bool val)
    : value_(val)
{
}
NodeValue::NodeValue(std::string val)
    : value_(std::move(val))
{
}
NodeValue::NodeValue(Vec2 val)
    : value_(val)
{
}
NodeValue::NodeValue(Vec3 val)
    : value_(val)
{
}
NodeValue::NodeValue(Vec4 val)
    : value_(val)
{
}
NodeValue::NodeValue(Color4 val)
    : value_(val)
{
}

// ============================================================================
// Type query
// ============================================================================

auto NodeValue::type() const -> SocketDataType
{
    return std::visit(
        [](const auto& val) -> SocketDataType
        {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, float>)
            {
                return SocketDataType::Float;
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return SocketDataType::Int;
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                return SocketDataType::Bool;
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return SocketDataType::String;
            }
            else if constexpr (std::is_same_v<T, Vec2>)
            {
                return SocketDataType::Vector2;
            }
            else if constexpr (std::is_same_v<T, Vec3>)
            {
                return SocketDataType::Vector3;
            }
            else if constexpr (std::is_same_v<T, Vec4>)
            {
                return SocketDataType::Vector4;
            }
            else if constexpr (std::is_same_v<T, Color4>)
            {
                return SocketDataType::Color;
            }
            else
            {
                return SocketDataType::Custom;
            }
        },
        value_);
}

// ============================================================================
// Typed accessors
// ============================================================================

auto NodeValue::as_float() const -> std::optional<float>
{
    if (const auto* val = std::get_if<float>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_int() const -> std::optional<int>
{
    if (const auto* val = std::get_if<int>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_bool() const -> std::optional<bool>
{
    if (const auto* val = std::get_if<bool>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_string() const -> std::optional<std::string>
{
    if (const auto* val = std::get_if<std::string>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_vec2() const -> std::optional<Vec2>
{
    if (const auto* val = std::get_if<Vec2>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_vec3() const -> std::optional<Vec3>
{
    if (const auto* val = std::get_if<Vec3>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_vec4() const -> std::optional<Vec4>
{
    if (const auto* val = std::get_if<Vec4>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

auto NodeValue::as_color4() const -> std::optional<Color4>
{
    if (const auto* val = std::get_if<Color4>(&value_))
    {
        return *val;
    }
    return std::nullopt;
}

// ============================================================================
// String conversion
// ============================================================================

auto NodeValue::to_string() const -> std::string
{
    return std::visit(
        [](const auto& val) -> std::string
        {
            using T = std::decay_t<decltype(val)>;
            if constexpr (std::is_same_v<T, float>)
            {
                return std::to_string(val);
            }
            else if constexpr (std::is_same_v<T, int>)
            {
                return std::to_string(val);
            }
            else if constexpr (std::is_same_v<T, bool>)
            {
                return val ? "true" : "false";
            }
            else if constexpr (std::is_same_v<T, std::string>)
            {
                return val;
            }
            else if constexpr (std::is_same_v<T, Vec2>)
            {
                std::ostringstream oss;
                oss << "(" << val.x << ", " << val.y << ")";
                return oss.str();
            }
            else if constexpr (std::is_same_v<T, Vec3>)
            {
                std::ostringstream oss;
                oss << "(" << val.x << ", " << val.y << ", " << val.z << ")";
                return oss.str();
            }
            else if constexpr (std::is_same_v<T, Vec4>)
            {
                std::ostringstream oss;
                oss << "(" << val.x << ", " << val.y << ", " << val.z << ", " << val.w << ")";
                return oss.str();
            }
            else if constexpr (std::is_same_v<T, Color4>)
            {
                std::ostringstream oss;
                oss << "rgba(" << val.r << ", " << val.g << ", " << val.b << ", " << val.a << ")";
                return oss.str();
            }
            else
            {
                return "<unknown>";
            }
        },
        value_);
}

// ============================================================================
// Comparison
// ============================================================================

auto NodeValue::operator==(const NodeValue& other) const -> bool
{
    return value_ == other.value_;
}

auto NodeValue::operator!=(const NodeValue& other) const -> bool
{
    return !(*this == other);
}

auto NodeValue::variant() const -> const ValueVariant&
{
    return value_;
}

} // namespace markamp::node_editor
