#pragma once

#include "CanvasTypes.h"

#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::canvas
{

/// Blend mode for compositing canvas objects.
enum class BlendMode : uint8_t
{
    kNormal,
    kMultiply,
    kScreen,
    kOverlay
};

/// Drop shadow settings for a canvas object.
struct ObjectShadow
{
    bool enabled{false};
    double offset_x{4.0};
    double offset_y{4.0};
    double blur{8.0};
    CanvasColor color{0, 0, 0, 80};

    /// Whether the shadow effect is active.
    [[nodiscard]] auto is_enabled() const noexcept -> bool
    {
        return enabled;
    }
};

/// Border/stroke settings for a canvas object.
struct ObjectBorder
{
    enum class Style : uint8_t
    {
        kSolid,
        kDashed,
        kDotted
    };
    bool enabled{false};
    double width{1.0};
    CanvasColor color{0, 0, 0, 255};
    Style style{Style::kSolid};

    /// Whether the border is active.
    [[nodiscard]] auto is_visible() const noexcept -> bool
    {
        return enabled && width > 0.0;
    }
};

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

    // --- Tooltip (#1) ---
    [[nodiscard]] auto tooltip() const -> const std::string&;
    auto set_tooltip(const std::string& tip) -> void;

    // --- Hyperlink (#2) ---
    [[nodiscard]] auto hyperlink() const -> const std::string&;
    auto set_hyperlink(const std::string& url) -> void;

    // --- Layer (#3) ---
    [[nodiscard]] auto layer() const -> int;
    auto set_layer(int layer_index) -> void;

    // --- Blend Mode (#4) ---
    [[nodiscard]] auto blend_mode() const -> BlendMode;
    auto set_blend_mode(BlendMode mode) -> void;

    // --- Aspect Lock (#5) ---
    [[nodiscard]] auto is_aspect_locked() const -> bool;
    auto set_aspect_locked(bool locked) -> void;

    // --- Flip (#6) ---
    [[nodiscard]] auto is_flipped_horizontal() const -> bool;
    auto set_flipped_horizontal(bool flipped) -> void;
    [[nodiscard]] auto is_flipped_vertical() const -> bool;
    auto set_flipped_vertical(bool flipped) -> void;

    // --- Shadow (#7) ---
    [[nodiscard]] auto shadow() const -> const ObjectShadow&;
    auto set_shadow(const ObjectShadow& shd) -> void;

    // --- Border (#8) ---
    [[nodiscard]] auto border() const -> const ObjectBorder&;
    auto set_border(const ObjectBorder& brd) -> void;

    // --- Batch 2 (#7-12) ───────────────────────────────────────────

    /// Per-object custom accent color for selection highlights/decoration.
    [[nodiscard]] auto custom_color() const -> const CanvasColor&;
    auto set_custom_color(const CanvasColor& color) -> void;

    /// Monotonic creation order for temporal sorting.
    [[nodiscard]] auto creation_order() const -> uint64_t;

    /// Freeform annotation text distinct from metadata/tooltip.
    [[nodiscard]] auto annotation() const -> const std::string&;
    auto set_annotation(const std::string& note) -> void;

    /// Mark an object as a reusable template prototype.
    [[nodiscard]] auto is_template() const -> bool;
    auto set_template(bool is_tpl) -> void;

    /// Convenience: center point of world_bounds AABB.
    [[nodiscard]] auto world_center() const -> Point2D;

    /// Center-to-center distance between two objects.
    [[nodiscard]] auto distance_to(const CanvasObject& other) const -> double;

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

    /// Whether the object has any tags.
    [[nodiscard]] auto has_tags() const noexcept -> bool
    {
        return !tags_.empty();
    }

    /// Whether the object has a parent group.
    [[nodiscard]] auto has_parent() const noexcept -> bool
    {
        return parent_id_ != kInvalidObjectId;
    }

    /// Whether the object has a hyperlink set.
    [[nodiscard]] auto has_hyperlink() const noexcept -> bool
    {
        return !hyperlink_.empty();
    }

    /// Whether the object has a tooltip set.
    [[nodiscard]] auto has_tooltip() const noexcept -> bool
    {
        return !tooltip_.empty();
    }

    /// Whether the object has an annotation.
    [[nodiscard]] auto has_annotation() const noexcept -> bool
    {
        return !annotation_.empty();
    }

    /// Whether the object has a non-default custom color.
    [[nodiscard]] auto has_shadow() const noexcept -> bool
    {
        return shadow_.enabled;
    }

    /// Number of metadata entries.
    [[nodiscard]] auto metadata_count() const noexcept -> std::size_t
    {
        return metadata_.size();
    }

    /// Tag count.
    [[nodiscard]] auto tag_count() const noexcept -> std::size_t
    {
        return tags_.size();
    }

    // ── Batch 2 (#11-20) ──────────────────────────────────────────

    /// (#11) Whether the object is fully opaque (opacity == 1.0).
    [[nodiscard]] auto is_fully_opaque() const noexcept -> bool
    {
        return opacity_ == 1.0;
    }

    /// (#12) Whether the object is fully transparent (opacity <= 0.0).
    [[nodiscard]] auto is_transparent() const noexcept -> bool
    {
        return opacity_ <= 0.0;
    }

    /// (#13) Whether the object has an active border.
    [[nodiscard]] auto has_border() const noexcept -> bool
    {
        return border_.enabled;
    }

    /// (#14) Whether a non-default custom color is set.
    [[nodiscard]] auto has_custom_color() const noexcept -> bool
    {
        return !(custom_color_.r == 128 && custom_color_.g == 128 &&
                 custom_color_.b == 128 && custom_color_.a == 255);
    }

    /// (#15) Whether the object is on the default layer (layer 0).
    [[nodiscard]] auto is_on_default_layer() const noexcept -> bool
    {
        return layer_ == 0;
    }

    /// (#16) Whether a metadata key exists.
    [[nodiscard]] auto has_metadata_key(const std::string& key) const noexcept -> bool
    {
        return metadata_.find(key) != metadata_.end();
    }

    /// (#17) Width of the world_bounds AABB.
    [[nodiscard]] auto world_width() const -> double
    {
        return world_bounds().width();
    }

    /// (#18) Height of the world_bounds AABB.
    [[nodiscard]] auto world_height() const -> double
    {
        return world_bounds().height();
    }

    /// (#19) Whether the object has a display name.
    [[nodiscard]] auto has_name() const noexcept -> bool
    {
        return !name_.empty();
    }

    /// (#20) Whether the object is flipped on either axis.
    [[nodiscard]] auto is_flipped() const noexcept -> bool
    {
        return flip_horizontal_ || flip_vertical_;
    }

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
    std::string tooltip_;
    std::string hyperlink_;
    int layer_{0};
    BlendMode blend_mode_{BlendMode::kNormal};
    bool aspect_locked_{false};
    bool flip_horizontal_{false};
    bool flip_vertical_{false};
    ObjectShadow shadow_;
    ObjectBorder border_;
    CanvasColor custom_color_{128, 128, 128, 255};
    uint64_t creation_order_{0};
    std::string annotation_;
    bool is_template_{false};

    /// Monotonic ID generator.
    [[nodiscard]] static auto next_id() -> ObjectId;
};

} // namespace markamp::canvas
