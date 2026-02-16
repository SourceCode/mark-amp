#include "CanvasObject.h"

#include <algorithm>
#include <atomic>
#include <cmath>

namespace markamp::canvas
{

// Thread-safe monotonic ID generator.
static std::atomic<uint64_t> s_id_counter{0};

auto CanvasObject::next_id() -> ObjectId
{
    return ++s_id_counter;
}

CanvasObject::CanvasObject(CanvasObjectType type)
    : id_(next_id())
    , type_(type)
    , creation_order_(id_)
{
}

// --- Identity ---

auto CanvasObject::id() const -> ObjectId
{
    return id_;
}

auto CanvasObject::type() const -> CanvasObjectType
{
    return type_;
}

auto CanvasObject::name() const -> const std::string&
{
    return name_;
}

auto CanvasObject::set_name(const std::string& name) -> void
{
    name_ = name;
    mark_dirty();
}

// --- Transform ---

auto CanvasObject::transform() const -> const Transform2D&
{
    return transform_;
}

auto CanvasObject::set_transform(const Transform2D& t) -> void
{
    transform_ = t;
    mark_dirty();
}

auto CanvasObject::set_position(double x, double y) -> void
{
    transform_.tx = x;
    transform_.ty = y;
    mark_dirty();
}

auto CanvasObject::set_rotation(double radians) -> void
{
    transform_.rotation = radians;
    mark_dirty();
}

auto CanvasObject::set_scale(double sx, double sy) -> void
{
    transform_.scale_x = sx;
    transform_.scale_y = sy;
    mark_dirty();
}

auto CanvasObject::position() const -> Point2D
{
    return {transform_.tx, transform_.ty};
}

// --- Bounds ---

auto CanvasObject::world_bounds() const -> AABB
{
    const auto lb = local_bounds();
    if (!lb.is_valid())
    {
        return lb;
    }

    // Transform the four corners of the local bounding box.
    const Point2D corners[4] = {
        transform_.apply({lb.min_x, lb.min_y}),
        transform_.apply({lb.max_x, lb.min_y}),
        transform_.apply({lb.max_x, lb.max_y}),
        transform_.apply({lb.min_x, lb.max_y}),
    };

    AABB result;
    for (const auto& corner : corners)
    {
        result.min_x = std::min(result.min_x, corner.x);
        result.min_y = std::min(result.min_y, corner.y);
        result.max_x = std::max(result.max_x, corner.x);
        result.max_y = std::max(result.max_y, corner.y);
    }

    return result;
}

// --- Z-Order ---

auto CanvasObject::z_index() const -> int
{
    return z_index_;
}

auto CanvasObject::set_z_index(int z) -> void
{
    z_index_ = z;
    mark_dirty();
}

// --- Lock ---

auto CanvasObject::is_locked() const -> bool
{
    return locked_;
}

auto CanvasObject::set_locked(bool locked) -> void
{
    locked_ = locked;
    mark_dirty();
}

// --- Visibility ---

auto CanvasObject::is_visible() const -> bool
{
    return visible_;
}

auto CanvasObject::set_visible(bool visible) -> void
{
    visible_ = visible;
    mark_dirty();
}

// --- Opacity ---

auto CanvasObject::opacity() const -> double
{
    return opacity_;
}

auto CanvasObject::set_opacity(double o) -> void
{
    opacity_ = std::clamp(o, 0.0, 1.0);
    mark_dirty();
}

// --- Metadata ---

auto CanvasObject::metadata() const -> const std::unordered_map<std::string, std::string>&
{
    return metadata_;
}

auto CanvasObject::set_metadata(const std::string& key, const std::string& value) -> void
{
    metadata_[key] = value;
    mark_dirty();
}

auto CanvasObject::remove_metadata(const std::string& key) -> void
{
    metadata_.erase(key);
    mark_dirty();
}

auto CanvasObject::get_metadata(const std::string& key) const -> std::optional<std::string>
{
    const auto it = metadata_.find(key);
    if (it != metadata_.end())
    {
        return it->second;
    }
    return std::nullopt;
}

// --- Tags ---

auto CanvasObject::tags() const -> const std::vector<std::string>&
{
    return tags_;
}

auto CanvasObject::add_tag(const std::string& tag) -> void
{
    if (std::find(tags_.begin(), tags_.end(), tag) == tags_.end())
    {
        tags_.push_back(tag);
        mark_dirty();
    }
}

auto CanvasObject::remove_tag(const std::string& tag) -> void
{
    auto it = std::find(tags_.begin(), tags_.end(), tag);
    if (it != tags_.end())
    {
        tags_.erase(it);
        mark_dirty();
    }
}

// --- Parent / Group ---

auto CanvasObject::parent_id() const -> ObjectId
{
    return parent_id_;
}

auto CanvasObject::set_parent_id(ObjectId parent) -> void
{
    parent_id_ = parent;
    mark_dirty();
}

// --- Tooltip (#1) ---

auto CanvasObject::tooltip() const -> const std::string&
{
    return tooltip_;
}

auto CanvasObject::set_tooltip(const std::string& tip) -> void
{
    tooltip_ = tip;
    mark_dirty();
}

// --- Hyperlink (#2) ---

auto CanvasObject::hyperlink() const -> const std::string&
{
    return hyperlink_;
}

auto CanvasObject::set_hyperlink(const std::string& url) -> void
{
    hyperlink_ = url;
    mark_dirty();
}

// --- Layer (#3) ---

auto CanvasObject::layer() const -> int
{
    return layer_;
}

auto CanvasObject::set_layer(int layer_index) -> void
{
    layer_ = layer_index;
    mark_dirty();
}

// --- Blend Mode (#4) ---

auto CanvasObject::blend_mode() const -> BlendMode
{
    return blend_mode_;
}

auto CanvasObject::set_blend_mode(BlendMode mode) -> void
{
    blend_mode_ = mode;
    mark_dirty();
}

// --- Aspect Lock (#5) ---

auto CanvasObject::is_aspect_locked() const -> bool
{
    return aspect_locked_;
}

auto CanvasObject::set_aspect_locked(bool locked) -> void
{
    aspect_locked_ = locked;
    mark_dirty();
}

// --- Flip (#6) ---

auto CanvasObject::is_flipped_horizontal() const -> bool
{
    return flip_horizontal_;
}

auto CanvasObject::set_flipped_horizontal(bool flipped) -> void
{
    flip_horizontal_ = flipped;
    mark_dirty();
}

auto CanvasObject::is_flipped_vertical() const -> bool
{
    return flip_vertical_;
}

auto CanvasObject::set_flipped_vertical(bool flipped) -> void
{
    flip_vertical_ = flipped;
    mark_dirty();
}

// --- Shadow (#7) ---

auto CanvasObject::shadow() const -> const ObjectShadow&
{
    return shadow_;
}

auto CanvasObject::set_shadow(const ObjectShadow& shd) -> void
{
    shadow_ = shd;
    mark_dirty();
}

// --- Border (#8) ---

auto CanvasObject::border() const -> const ObjectBorder&
{
    return border_;
}

auto CanvasObject::set_border(const ObjectBorder& brd) -> void
{
    border_ = brd;
    mark_dirty();
}

// --- Serialization (base implementation) ---

auto CanvasObject::to_json() const -> std::string
{
    // Base implementation returns minimal JSON. Subclasses override.
    return "{}";
}

auto CanvasObject::from_json(const std::string& /*json*/) -> void
{
    // Base implementation is a no-op. Subclasses override.
}

// --- Dirty tracking ---

auto CanvasObject::is_dirty() const -> bool
{
    return dirty_;
}

auto CanvasObject::mark_dirty() -> void
{
    dirty_ = true;
}

auto CanvasObject::mark_clean() -> void
{
    dirty_ = false;
}

// --- Batch 2 (#7-12) ---

auto CanvasObject::custom_color() const -> const CanvasColor&
{
    return custom_color_;
}

auto CanvasObject::set_custom_color(const CanvasColor& color) -> void
{
    custom_color_ = color;
    mark_dirty();
}

auto CanvasObject::creation_order() const -> uint64_t
{
    return creation_order_;
}

auto CanvasObject::annotation() const -> const std::string&
{
    return annotation_;
}

auto CanvasObject::set_annotation(const std::string& note) -> void
{
    annotation_ = note;
    mark_dirty();
}

auto CanvasObject::is_template() const -> bool
{
    return is_template_;
}

auto CanvasObject::set_template(bool is_tpl) -> void
{
    is_template_ = is_tpl;
    mark_dirty();
}

auto CanvasObject::world_center() const -> Point2D
{
    return world_bounds().center();
}

auto CanvasObject::distance_to(const CanvasObject& other) const -> double
{
    return world_center().distance_to(other.world_center());
}

} // namespace markamp::canvas
