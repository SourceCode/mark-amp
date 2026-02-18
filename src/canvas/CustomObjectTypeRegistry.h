#pragma once

/// @file CustomObjectTypeRegistry.h
/// @brief Phase 14: Extension-contributed custom canvas object types.
///
/// Allows extensions to register new object types with the canvas engine
/// at runtime. Each custom type has a descriptor (display name, icon,
/// default size) and a factory callback for object creation.

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "core/EventBus.h"

#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Factory callback: given a position and size, produce a CanvasObject.
using CustomObjectFactory = std::function<std::unique_ptr<CanvasObject>(Point2D, Size2D)>;

/// Describes a custom object type contributed by an extension.
struct CustomObjectTypeDescriptor
{
    std::string type_id;      ///< Unique type identifier (e.g. "ext.kanban-card")
    std::string display_name; ///< Human-readable name for toolbars
    std::string icon;         ///< Icon path (relative to extension root)
    std::string extension_id; ///< Owning extension
    std::string category;     ///< Palette category ("shapes", "data", "custom")
    Size2D default_size{200.0, 150.0};
    CustomObjectFactory factory; ///< Creates an instance of this type
};

/// Result of a registration attempt.
struct TypeRegistrationResult
{
    bool success{false};
    std::string error_message;
};

/// Registry for extension-contributed canvas object types.
///
/// Extensions register descriptors at activation time. The registry
/// validates for uniqueness, stores the descriptor, and exposes
/// factory methods for creating instances of the custom type.
class CustomObjectTypeRegistry
{
public:
    explicit CustomObjectTypeRegistry(core::EventBus& event_bus);

    // ── Registration ──────────────────────────────────────────────

    /// Register a new custom object type.
    auto register_type(const CustomObjectTypeDescriptor& descriptor) -> TypeRegistrationResult;

    /// Unregister a custom type by its type_id.
    auto unregister_type(const std::string& type_id) -> bool;

    /// Unregister all types contributed by a specific extension.
    auto unregister_extension(const std::string& extension_id) -> size_t;

    // ── Query ─────────────────────────────────────────────────────

    /// Find a registered type descriptor by ID.
    [[nodiscard]] auto find_type(const std::string& type_id) const
        -> const CustomObjectTypeDescriptor*;

    /// All registered custom types.
    [[nodiscard]] auto registered_types() const
        -> const std::unordered_map<std::string, CustomObjectTypeDescriptor>&;

    /// Types belonging to a specific extension.
    [[nodiscard]] auto types_for_extension(const std::string& extension_id) const
        -> std::vector<const CustomObjectTypeDescriptor*>;

    /// Types in a given category.
    [[nodiscard]] auto types_in_category(const std::string& category) const
        -> std::vector<const CustomObjectTypeDescriptor*>;

    /// Whether a type ID is registered.
    [[nodiscard]] auto is_registered(const std::string& type_id) const -> bool;

    /// Total count of registered types.
    [[nodiscard]] auto type_count() const -> size_t;

    // ── Factory ───────────────────────────────────────────────────

    /// Create an instance of a custom object type.
    [[nodiscard]] auto create_object(const std::string& type_id,
                                     Point2D position = {0.0, 0.0}) const
        -> std::unique_ptr<CanvasObject>;

    /// Create with explicit size override.
    [[nodiscard]] auto create_object(const std::string& type_id,
                                     Point2D position,
                                     Size2D size) const -> std::unique_ptr<CanvasObject>;

    // ── Cleanup ───────────────────────────────────────────────────

    /// Remove all registered types.
    auto clear() -> void;

private:
    core::EventBus& event_bus_;
    std::unordered_map<std::string, CustomObjectTypeDescriptor> types_;
};

} // namespace markamp::canvas
