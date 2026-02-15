#pragma once

#include "CanvasTypes.h"

#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Base class for all objects that live on the infinite canvas.
/// Every canvas object has an ID, transform, bounds, z-index, metadata,
/// lock state, and a parent reference for grouping.
class CanvasObject
{
public:
    explicit CanvasObject(CanvasObjectType type);
    virtual ~CanvasObject() = default;

    // Non-copyable, movable
    CanvasObject(const CanvasObject&) = delete;
    auto operator=(const CanvasObject&) -> CanvasObject& = delete;
    CanvasObject(CanvasObject&&) = default;
    auto operator=(CanvasObject&&) -> CanvasObject& = default;

    // --- Identity ---
    [[nodiscard]] auto id() const -> ObjectId;
    [[nodiscard]] auto type() const -> CanvasObjectType;
    [[nodiscard]] auto name() const -> const std::string&;
    auto set_name(const std::string& name) -> void;

    // --- Transform ---
    [[nodiscard]] auto transform() const -> const Transform2D&;
    auto set_transform(const Transform2D& t) -> void;
    auto set_position(double x, double y) -> void;
    auto set_rotation(double radians) -> void;
    auto set_scale(double sx, double sy) -> void;
    [[nodiscard]] auto position() const -> Point2D;

    // --- Bounds ---
    /// Local-space bounding box (before transform). Subclasses override.
    [[nodiscard]] virtual auto local_bounds() const -> AABB = 0;
    /// World-space AABB (local bounds transformed).
    [[nodiscard]] auto world_bounds() const -> AABB;

    // --- Z-Order ---
    [[nodiscard]] auto z_index() const -> int;
    auto set_z_index(int z) -> void;

    // --- Lock ---
    [[nodiscard]] auto is_locked() const -> bool;
    auto set_locked(bool locked) -> void;

    // --- Visibility ---
    [[nodiscard]] auto is_visible() const -> bool;
    auto set_visible(bool visible) -> void;

    // --- Opacity ---
    [[nodiscard]] auto opacity() const -> double;
    auto set_opacity(double o) -> void;

    // --- Metadata (arbitrary key-value) ---
    [[nodiscard]] auto metadata() const -> const std::unordered_map<std::string, std::string>&;
    auto set_metadata(const std::string& key, const std::string& value) -> void;
    auto remove_metadata(const std::string& key) -> void;
    [[nodiscard]] auto get_metadata(const std::string& key) const -> std::optional<std::string>;

    // --- Tags ---
    [[nodiscard]] auto tags() const -> const std::vector<std::string>&;
    auto add_tag(const std::string& tag) -> void;
    auto remove_tag(const std::string& tag) -> void;

    // --- Parent / Group ---
    [[nodiscard]] auto parent_id() const -> ObjectId;
    auto set_parent_id(ObjectId parent) -> void;

    // --- Serialization ---
    /// Serialize to JSON object. Subclasses override to add type-specific fields.
    [[nodiscard]] virtual auto to_json() const -> std::string;
    /// Deserialize from JSON. Subclasses override.
    virtual auto from_json(const std::string& json) -> void;

    // --- Clone ---
    [[nodiscard]] virtual auto clone() const -> std::unique_ptr<CanvasObject> = 0;

    // --- Dirty tracking ---
    [[nodiscard]] auto is_dirty() const -> bool;
    auto mark_dirty() -> void;
    auto mark_clean() -> void;

protected:
    ObjectId id_;
    CanvasObjectType type_;
    std::string name_;
    Transform2D transform_;
    int z_index_{0};
    bool locked_{false};
    bool visible_{true};
    double opacity_{1.0};
    std::unordered_map<std::string, std::string> metadata_;
    std::vector<std::string> tags_;
    ObjectId parent_id_{kInvalidObjectId};
    bool dirty_{true};

    /// Monotonic ID generator.
    [[nodiscard]] static auto next_id() -> ObjectId;
};

} // namespace markamp::canvas
