#pragma once

#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Describes the type of a metadata field for type-aware editing.
enum class MetadataFieldType : uint8_t
{
    kText,
    kNumber,
    kDate,
    kSelect,
    kBoolean
};

/// A single metadata field displayed in the inspector panel.
struct MetadataField
{
    std::string key;
    std::string value;
    bool is_system{false};     ///< System fields are read-only (id, type, z_index).
    bool is_predefined{false}; ///< Predefined fields have type hints (status, priority).
    MetadataFieldType type{MetadataFieldType::kText};
    std::vector<std::string> options; ///< For Select type only.
};

/// Inspector panel that displays and edits arbitrary key-value metadata on any
/// canvas object. Shows system fields (read-only), predefined fields, and
/// user-defined custom fields.
class MetadataPanel
{
public:
    using OnFieldChanged =
        std::function<void(ObjectId, const std::string& key, const std::string& value)>;
    using OnFieldRemoved = std::function<void(ObjectId, const std::string& key)>;

    /// Populate the panel for the given object.
    auto set_object(ObjectId obj_id,
                    CanvasObjectType obj_type,
                    int z_index,
                    const std::unordered_map<std::string, std::string>& metadata,
                    const std::vector<std::string>& tags) -> void;

    /// Clear the panel state.
    auto clear() -> void;

    /// Number of fields currently displayed.
    [[nodiscard]] auto field_count() const -> size_t;

    /// Access field by index.
    [[nodiscard]] auto field_at(size_t index) const -> const MetadataField&;

    /// Add a custom key-value field.
    auto add_custom_field(const std::string& key, const std::string& value) -> void;

    /// Remove a non-system field by key.  Fires on_field_removed_.
    auto remove_field(const std::string& key) -> bool;

    /// Update a non-system field value.  Fires on_field_changed_.
    auto set_field_value(const std::string& key, const std::string& value) -> bool;

    /// Set the field type for a specific key.
    auto set_field_type(const std::string& key, MetadataFieldType field_type) -> bool;

    // ── Callbacks ──────────────────────────────────────────────────
    auto set_on_field_changed(OnFieldChanged callback) -> void;
    auto set_on_field_removed(OnFieldRemoved callback) -> void;

    // ── Visibility ─────────────────────────────────────────────────
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    /// The object currently loaded in the panel.
    [[nodiscard]] auto current_object_id() const -> ObjectId;

private:
    ObjectId current_id_{kInvalidObjectId};
    std::vector<MetadataField> fields_;
    bool visible_{false};
    OnFieldChanged on_field_changed_;
    OnFieldRemoved on_field_removed_;

    /// Build the fields list from object properties and metadata.
    auto rebuild_fields(CanvasObjectType obj_type,
                        int z_index,
                        const std::unordered_map<std::string, std::string>& metadata,
                        const std::vector<std::string>& tags) -> void;
};

} // namespace markamp::canvas
