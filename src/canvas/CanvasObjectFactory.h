// ============================================================================
// File: src/canvas/CanvasObjectFactory.h
// Phase 11: Canvas Workbench Shell — typed object creation factory
// ============================================================================
#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"

#include <cstddef>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Configuration for new object creation.
struct ObjectCreationParams
{
    Point2D position{0.0, 0.0};
    Size2D size{200.0, 150.0};                  ///< Default size in world units
    std::string name;                           ///< Optional display name
    CanvasColor fill_color{255, 255, 200, 255}; ///< Default fill
    CanvasColor stroke_color{0, 0, 0, 255};     ///< Default stroke
    std::string text_content;                   ///< Initial text (for TextBox, StickyNote)
    int layer_index{0};
};

/// Result of a factory creation attempt.
struct ObjectCreationResult
{
    std::unique_ptr<CanvasObject> object;
    bool success{false};
    std::string error_message; ///< Non-empty if !success
};

/// Factory for creating typed canvas objects with sane defaults.
///
/// Centralises object construction so that callers never need to
/// include every specific object header — they just pass a
/// CanvasObjectType and get back a ready-to-use CanvasObject.
class CanvasObjectFactory
{
public:
    CanvasObjectFactory() = default;

    /// Create an object of the given type with default parameters.
    [[nodiscard]] auto create(CanvasObjectType type) const -> ObjectCreationResult;

    /// Create an object with custom parameters.
    [[nodiscard]] auto create(CanvasObjectType type, const ObjectCreationParams& params) const
        -> ObjectCreationResult;

    /// Get the default size for an object type.
    [[nodiscard]] static auto default_size(CanvasObjectType type) -> Size2D;

    /// Get a human-readable name for an object type.
    [[nodiscard]] static auto type_display_name(CanvasObjectType type) -> std::string;

    /// Get the icon name (for toolbar/palette display) for an object type.
    [[nodiscard]] static auto type_icon_name(CanvasObjectType type) -> std::string;

    /// All creatable object types (excluded: Group, Connector — created via context actions).
    [[nodiscard]] static auto creatable_types() -> std::vector<CanvasObjectType>;

    /// Duplicate an existing object, returning a deep clone at an offset position.
    [[nodiscard]] static auto duplicate(const CanvasObject& source, Point2D offset = {20.0, 20.0})
        -> std::unique_ptr<CanvasObject>;

    /// Batch-create N objects of the same type in a grid layout.
    [[nodiscard]] auto
    create_grid(CanvasObjectType type, size_t count, Point2D origin, double spacing = 30.0) const
        -> std::vector<std::unique_ptr<CanvasObject>>;

private:
    [[nodiscard]] auto create_sticky_note(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_text_box(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_shape(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_frame(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_section(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_image(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_diagram_shape(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_comment(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_icon(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
    [[nodiscard]] auto create_table(const ObjectCreationParams& params) const
        -> std::unique_ptr<CanvasObject>;
};

} // namespace markamp::canvas
