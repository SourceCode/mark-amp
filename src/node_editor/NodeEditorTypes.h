#pragma once

// V11 Phase 01: Node Editor Product Spec & Non-Negotiables
// Strong typed IDs, fundamental enums, and type aliases for the node editor subsystem.

#include <cstdint>
#include <functional> // std::hash
#include <string>
#include <string_view>

namespace markamp::node_editor
{

// ============================================================================
// Strong Typed IDs
// ============================================================================
//
// Each ID wraps a uint64_t with a distinct type to prevent accidental mixing.
// Zero is reserved as the "invalid/null" sentinel.

/// Unique identifier for a node within a graph.
struct NodeId
{
    uint64_t value{0};

    constexpr NodeId() = default;
    constexpr explicit NodeId(uint64_t v)
        : value(v)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }

    constexpr auto operator==(const NodeId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const NodeId&) const noexcept = default;
};

/// Unique identifier for a socket (input or output port) on a node.
struct SocketId
{
    uint64_t value{0};

    constexpr SocketId() = default;
    constexpr explicit SocketId(uint64_t v)
        : value(v)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }

    constexpr auto operator==(const SocketId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const SocketId&) const noexcept = default;
};

/// Unique identifier for a link (connection) between two sockets.
struct LinkId
{
    uint64_t value{0};

    constexpr LinkId() = default;
    constexpr explicit LinkId(uint64_t v)
        : value(v)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }

    constexpr auto operator==(const LinkId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const LinkId&) const noexcept = default;
};

/// Unique identifier for a graph (node tree) instance.
struct GraphId
{
    uint64_t value{0};

    constexpr GraphId() = default;
    constexpr explicit GraphId(uint64_t v)
        : value(v)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }

    constexpr auto operator==(const GraphId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const GraphId&) const noexcept = default;
};

/// Unique identifier for an annotation frame.
struct FrameId
{
    uint64_t value{0};

    constexpr FrameId() = default;
    constexpr explicit FrameId(uint64_t val)
        : value(val)
    {
    }

    [[nodiscard]] constexpr auto is_valid() const noexcept -> bool
    {
        return value != 0;
    }

    constexpr auto operator==(const FrameId&) const noexcept -> bool = default;
    constexpr auto operator<=>(const FrameId&) const noexcept = default;
};

// ============================================================================
// Socket Direction
// ============================================================================

/// Whether a socket receives data (Input) or produces data (Output).
enum class SocketDirection : uint8_t
{
    Input,
    Output
};

// ============================================================================
// Socket Data Type
// ============================================================================

/// The semantic data type carried by a socket.
/// Custom types can be registered at runtime via the type registry.
enum class SocketDataType : uint8_t
{
    Float,
    Int,
    Bool,
    String,
    Vector2,
    Vector3,
    Vector4,
    Color,
    Image,
    Geometry,
    Shader,
    Object,
    Enum,
    FilePath,
    Custom // Extended via runtime type registry
};

/// Convert SocketDataType to a display-friendly string.
[[nodiscard]] constexpr auto socket_data_type_name(SocketDataType dt) noexcept -> std::string_view
{
    switch (dt)
    {
        case SocketDataType::Float:
            return "Float";
        case SocketDataType::Int:
            return "Int";
        case SocketDataType::Bool:
            return "Bool";
        case SocketDataType::String:
            return "String";
        case SocketDataType::Vector2:
            return "Vector2";
        case SocketDataType::Vector3:
            return "Vector3";
        case SocketDataType::Vector4:
            return "Vector4";
        case SocketDataType::Color:
            return "Color";
        case SocketDataType::Image:
            return "Image";
        case SocketDataType::Geometry:
            return "Geometry";
        case SocketDataType::Shader:
            return "Shader";
        case SocketDataType::Object:
            return "Object";
        case SocketDataType::Enum:
            return "Enum";
        case SocketDataType::FilePath:
            return "FilePath";
        case SocketDataType::Custom:
            return "Custom";
    }
    return "Unknown";
}

// ============================================================================
// Node Flags
// ============================================================================

/// Bitmask flags for node state and rendering hints.
enum class NodeFlags : uint16_t
{
    None = 0,
    Hidden = 1 << 0,   // Node is hidden from canvas
    Preview = 1 << 1,  // Node shows inline preview
    Active = 1 << 2,   // Node is the active/selected primary
    Muted = 1 << 3,    // Node is bypassed in evaluation
    Error = 1 << 4,    // Node has evaluation errors
    Collapsed = 1 << 5 // Node UI is collapsed
};

[[nodiscard]] constexpr auto operator|(NodeFlags lhs, NodeFlags rhs) noexcept -> NodeFlags
{
    return static_cast<NodeFlags>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs));
}

[[nodiscard]] constexpr auto operator&(NodeFlags lhs, NodeFlags rhs) noexcept -> NodeFlags
{
    return static_cast<NodeFlags>(static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs));
}

constexpr auto operator|=(NodeFlags& lhs, NodeFlags rhs) noexcept -> NodeFlags&
{
    lhs = lhs | rhs;
    return lhs;
}

[[nodiscard]] constexpr auto has_flag(NodeFlags flags, NodeFlags test) noexcept -> bool
{
    return (flags & test) != NodeFlags::None;
}

// ============================================================================
// 2D Position
// ============================================================================

/// Simple 2D position for node placement on the canvas.
struct Vec2
{
    float x{0.0F};
    float y{0.0F};

    constexpr Vec2() = default;
    constexpr Vec2(float px, float py)
        : x(px)
        , y(py)
    {
    }

    constexpr auto operator==(const Vec2&) const noexcept -> bool = default;

    [[nodiscard]] constexpr auto operator+(const Vec2& rhs) const noexcept -> Vec2
    {
        return {x + rhs.x, y + rhs.y};
    }

    [[nodiscard]] constexpr auto operator-(const Vec2& rhs) const noexcept -> Vec2
    {
        return {x - rhs.x, y - rhs.y};
    }

    [[nodiscard]] constexpr auto operator*(float s) const noexcept -> Vec2
    {
        return {x * s, y * s};
    }
};

// ============================================================================
// Axis-Aligned Rectangle
// ============================================================================

/// Axis-aligned rectangle for bounding boxes, viewports, and hit-testing.
struct Rect
{
    float x{0.0F};
    float y{0.0F};
    float width{0.0F};
    float height{0.0F};

    constexpr Rect() = default;
    constexpr Rect(float rx, float ry, float rw, float rh)
        : x(rx)
        , y(ry)
        , width(rw)
        , height(rh)
    {
    }

    [[nodiscard]] constexpr auto right() const noexcept -> float
    {
        return x + width;
    }
    [[nodiscard]] constexpr auto bottom() const noexcept -> float
    {
        return y + height;
    }

    [[nodiscard]] constexpr auto center() const noexcept -> Vec2
    {
        return {x + width * 0.5F, y + height * 0.5F};
    }

    [[nodiscard]] constexpr auto contains(Vec2 point) const noexcept -> bool
    {
        return point.x >= x && point.x <= right() && point.y >= y && point.y <= bottom();
    }

    [[nodiscard]] constexpr auto contains_rect(const Rect& other) const noexcept -> bool
    {
        return other.x >= x && other.right() <= right() && other.y >= y &&
               other.bottom() <= bottom();
    }

    [[nodiscard]] constexpr auto intersects(const Rect& other) const noexcept -> bool
    {
        return x < other.right() && right() > other.x && y < other.bottom() && bottom() > other.y;
    }

    [[nodiscard]] constexpr auto expanded(float margin) const noexcept -> Rect
    {
        return {x - margin, y - margin, width + margin * 2.0F, height + margin * 2.0F};
    }

    [[nodiscard]] constexpr auto union_with(const Rect& other) const noexcept -> Rect
    {
        const float kMinX = (x < other.x) ? x : other.x;
        const float kMinY = (y < other.y) ? y : other.y;
        const float kMaxX = (right() > other.right()) ? right() : other.right();
        const float kMaxY = (bottom() > other.bottom()) ? bottom() : other.bottom();
        return {kMinX, kMinY, kMaxX - kMinX, kMaxY - kMinY};
    }

    constexpr auto operator==(const Rect&) const noexcept -> bool = default;
};

// ============================================================================
// Graph Mode
// ============================================================================

/// The domain mode of a node graph.
enum class GraphMode : uint8_t
{
    Graphics,
    CodeFlow,
    DataTransform,
    DataGeneration,
    Audio,
    Custom
};

[[nodiscard]] constexpr auto graph_mode_name(GraphMode mode) noexcept -> std::string_view
{
    switch (mode)
    {
        case GraphMode::Graphics:
            return "Graphics";
        case GraphMode::CodeFlow:
            return "CodeFlow";
        case GraphMode::DataTransform:
            return "DataTransform";
        case GraphMode::DataGeneration:
            return "DataGeneration";
        case GraphMode::Audio:
            return "Audio";
        case GraphMode::Custom:
            return "Custom";
    }
    return "Unknown";
}

} // namespace markamp::node_editor

// ============================================================================
// std::hash specializations for use in unordered containers
// ============================================================================

template <>
struct std::hash<markamp::node_editor::NodeId>
{
    auto operator()(const markamp::node_editor::NodeId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.value);
    }
};

template <>
struct std::hash<markamp::node_editor::SocketId>
{
    auto operator()(const markamp::node_editor::SocketId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.value);
    }
};

template <>
struct std::hash<markamp::node_editor::LinkId>
{
    auto operator()(const markamp::node_editor::LinkId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.value);
    }
};

template <>
struct std::hash<markamp::node_editor::GraphId>
{
    auto operator()(const markamp::node_editor::GraphId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.value);
    }
};

template <>
struct std::hash<markamp::node_editor::FrameId>
{
    auto operator()(const markamp::node_editor::FrameId& id) const noexcept -> std::size_t
    {
        return std::hash<uint64_t>{}(id.value);
    }
};
