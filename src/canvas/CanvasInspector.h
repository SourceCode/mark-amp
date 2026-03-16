// ============================================================================
// File: src/canvas/CanvasInspector.h
// Phase 11: Canvas Workbench Shell — object property inspector panel
// ============================================================================
#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/UndoRedoStack.h"
#include "core/EventBus.h"

#include <cstddef>
#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace markamp::canvas
{

class Board;

// ── Property Value Types ───────────────────────────────────────────

/// Supported property value types in the inspector.
using PropertyValue = std::variant<double, int, bool, std::string, CanvasColor, Point2D, Size2D>;

/// A single property descriptor for display in the inspector.
struct PropertyDescriptor
{
    std::string name;        ///< Display name (e.g. "Width", "Fill Color")
    std::string key;         ///< Machine key (e.g. "width", "fill_color")
    std::string category;    ///< Grouping category (e.g. "Transform", "Style")
    PropertyValue value;     ///< Current value
    bool is_readonly{false}; ///< Greyed out in the UI
    bool is_visible{true};   ///< Whether to show in inspector

    // ── Round 2 Batch 10 (#91) ──────────────────────────────────

    /// (#91) Whether this property is editable (not readonly).
    [[nodiscard]] auto is_editable() const noexcept -> bool
    {
        return !is_readonly;
    }
};

/// Result of applying a property change.
enum class PropertyChangeResult : uint8_t
{
    kSuccess,
    kObjectNotFound,
    kPropertyNotFound,
    kTypeMismatch,
    kValueOutOfRange,
    kObjectLocked
};

/// Describes a pending property change for undo/redo.
struct PropertyChangeRecord
{
    ObjectId object_id{kInvalidObjectId};
    std::string property_key;
    PropertyValue old_value;
    PropertyValue new_value;
};

// ── Inspector Panel ────────────────────────────────────────────────

/// Inspects and edits object properties.  Wired to
/// SelectionManager::on_selection_changed for automatic refresh.
class CanvasInspector
{
public:
    CanvasInspector(Board& board,
                    UndoRedoStack& undo_stack,
                    std::shared_ptr<core::EventBus> event_bus);

    // ── Inspection ─────────────────────────────────────────────────

    /// Load properties for a specific object.
    auto inspect(ObjectId obj_id) -> void;

    /// Load properties for multiple selected objects (common subset).
    auto inspect_multi(const std::vector<ObjectId>& obj_ids) -> void;

    /// Clear the inspector (no selection).
    auto clear() -> void;

    /// Currently inspected object ID (nullopt if multi or none).
    [[nodiscard]] auto inspected_object_id() const -> std::optional<ObjectId>;

    /// Whether multi-selection mode is active.
    [[nodiscard]] auto is_multi_inspect() const -> bool;

    /// Get the list of properties currently shown.
    [[nodiscard]] auto properties() const -> const std::vector<PropertyDescriptor>&;

    // ── Property Editing ───────────────────────────────────────────

    /// Apply a property change.  Pushes an undoable action.
    auto apply_property_change(ObjectId obj_id,
                               const std::string& property_key,
                               const PropertyValue& new_value) -> PropertyChangeResult;

    /// Apply the same property change to all currently selected objects.
    auto apply_to_all_selected(const std::string& property_key, const PropertyValue& new_value)
        -> PropertyChangeResult;

    // ── Common Property Helpers ────────────────────────────────────

    /// Extract common properties (name, position, size, rotation, layer, locked, visible).
    [[nodiscard]] static auto common_properties(const CanvasObject& obj)
        -> std::vector<PropertyDescriptor>;

    /// Extract type-specific properties (e.g. text content for TextBox, color for StickyNote).
    [[nodiscard]] static auto type_properties(const CanvasObject& obj)
        -> std::vector<PropertyDescriptor>;

    // ── Event Subscription ─────────────────────────────────────────

    /// Notify the inspector that selection has changed (called by SelectionManager).
    auto on_selection_changed(const std::vector<ObjectId>& selected_ids) -> void;

    /// Callback type for property-changed notifications to UI.
    using PropertyChangedCallback = std::function<void(const PropertyChangeRecord&)>;

    /// Register a callback that fires after any property change.
    auto set_on_property_changed(PropertyChangedCallback callback) -> void;

    // ── Round 2 Batch 10 (#92-95) ────────────────────────────────

    /// (#92) Number of properties currently shown.
    [[nodiscard]] auto property_count() const noexcept -> size_t
    {
        return current_properties_.size();
    }

    /// (#93) Whether the inspector has properties loaded.
    [[nodiscard]] auto has_properties() const noexcept -> bool
    {
        return !current_properties_.empty();
    }

    /// (#94) Whether inspecting a single object.
    [[nodiscard]] auto is_single_inspect() const noexcept -> bool
    {
        return inspected_id_.has_value();
    }

    /// (#95) Whether a property-changed callback is registered.
    [[nodiscard]] auto has_callback() const noexcept -> bool
    {
        return static_cast<bool>(on_changed_);
    }

private:
    Board& board_;
    [[maybe_unused]] UndoRedoStack& undo_stack_;
    std::shared_ptr<core::EventBus> event_bus_;

    std::optional<ObjectId> inspected_id_;
    std::vector<ObjectId> multi_ids_;
    std::vector<PropertyDescriptor> current_properties_;
    PropertyChangedCallback on_changed_;

    auto rebuild_properties() -> void;
    auto apply_value_to_object(CanvasObject& obj, const std::string& key, const PropertyValue& val)
        -> PropertyChangeResult;
    auto read_value_from_object(const CanvasObject& obj, const std::string& key)
        -> std::optional<PropertyValue>;
};

} // namespace markamp::canvas
