#pragma once

// V11 Phase 13: Typed Value Runtime & Conversion Engine
// Type-safe value container for node socket data.

#include "node_editor/NodeEditorTypes.h"

#include <cstdint>
#include <optional>
#include <string>
#include <variant>

namespace markamp::node_editor
{

// ============================================================================
// Vec3 / Vec4 / Color4 — Extended vector types
// ============================================================================

/// 3D vector for Vector3 socket data.
struct Vec3
{
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};

    constexpr Vec3() = default;
    constexpr Vec3(float pos_x, float pos_y, float pos_z)
        : x(pos_x)
        , y(pos_y)
        , z(pos_z)
    {
    }

    constexpr auto operator==(const Vec3&) const noexcept -> bool = default;
};

/// 4D vector for Vector4 socket data.
struct Vec4
{
    float x{0.0F};
    float y{0.0F};
    float z{0.0F};
    float w{0.0F};

    constexpr Vec4() = default;
    constexpr Vec4(float pos_x, float pos_y, float pos_z, float pos_w)
        : x(pos_x)
        , y(pos_y)
        , z(pos_z)
        , w(pos_w)
    {
    }

    constexpr auto operator==(const Vec4&) const noexcept -> bool = default;
};

/// RGBA color for Color socket data.
struct Color4
{
    float r{0.0F};
    float g{0.0F};
    float b{0.0F};
    float a{1.0F};

    constexpr Color4() = default;
    constexpr Color4(float red, float green, float blue, float alpha = 1.0F)
        : r(red)
        , g(green)
        , b(blue)
        , a(alpha)
    {
    }

    constexpr auto operator==(const Color4&) const noexcept -> bool = default;
};

// ============================================================================
// NodeValue
// ============================================================================

/// Type-safe value container for node socket data.
///
/// Wraps a variant of all supported concrete types and provides type-safe
/// accessors. The contained type corresponds to a SocketDataType enum value.
class NodeValue
{
public:
    /// Underlying variant type for all concrete value types.
    using ValueVariant = std::variant<float, int, bool, std::string, Vec2, Vec3, Vec4, Color4>;

    /// Default-construct as a float with value 0.
    NodeValue() = default;

    /// Construct from a specific value.
    explicit NodeValue(float val);
    explicit NodeValue(int val);
    explicit NodeValue(bool val);
    explicit NodeValue(std::string val);
    explicit NodeValue(Vec2 val);
    explicit NodeValue(Vec3 val);
    explicit NodeValue(Vec4 val);
    explicit NodeValue(Color4 val);

    /// Get the SocketDataType corresponding to the held value.
    [[nodiscard]] auto type() const -> SocketDataType;

    /// Try to get the value as the given type. Returns nullopt on type mismatch.
    [[nodiscard]] auto as_float() const -> std::optional<float>;
    [[nodiscard]] auto as_int() const -> std::optional<int>;
    [[nodiscard]] auto as_bool() const -> std::optional<bool>;
    [[nodiscard]] auto as_string() const -> std::optional<std::string>;
    [[nodiscard]] auto as_vec2() const -> std::optional<Vec2>;
    [[nodiscard]] auto as_vec3() const -> std::optional<Vec3>;
    [[nodiscard]] auto as_vec4() const -> std::optional<Vec4>;
    [[nodiscard]] auto as_color4() const -> std::optional<Color4>;

    /// Convert to string representation (for serialization/display).
    [[nodiscard]] auto to_string() const -> std::string;

    /// Equality comparison (type + value).
    [[nodiscard]] auto operator==(const NodeValue& other) const -> bool;
    [[nodiscard]] auto operator!=(const NodeValue& other) const -> bool;

    /// Access the raw variant.
    [[nodiscard]] auto variant() const -> const ValueVariant&;

private:
    ValueVariant value_{0.0F};
};

} // namespace markamp::node_editor
