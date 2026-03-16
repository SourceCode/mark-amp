#include "FrameObject.h"

#include "canvas/Board.h"

#include <algorithm>
#include <sstream>

namespace markamp::canvas
{

FrameObject::FrameObject()
    : CanvasObject(CanvasObjectType::Frame)
{
}

// ── Dimensions ──────────────────────────────────────────────────

auto FrameObject::width() const -> double
{
    return width_;
}

auto FrameObject::height() const -> double
{
    return height_;
}

auto FrameObject::resize(double w, double h) -> void
{
    width_ = std::max(kMinWidth, w);
    height_ = std::max(kMinHeight, h);
    mark_dirty();
}

// ── Title ───────────────────────────────────────────────────────

auto FrameObject::title() const -> const std::string&
{
    return title_;
}

auto FrameObject::set_title(const std::string& title) -> void
{
    title_ = title;
    mark_dirty();
}

auto FrameObject::show_title() const -> bool
{
    return show_title_;
}

auto FrameObject::set_show_title(bool show) -> void
{
    show_title_ = show;
    mark_dirty();
}

// ── Sequence Number ─────────────────────────────────────────────

auto FrameObject::sequence_number() const -> int
{
    return sequence_number_;
}

auto FrameObject::set_sequence_number(int seq) -> void
{
    sequence_number_ = seq;
    mark_dirty();
}

// ── Colors ──────────────────────────────────────────────────────

auto FrameObject::background_color() const -> CanvasColor
{
    return background_color_;
}

auto FrameObject::set_background_color(CanvasColor color) -> void
{
    background_color_ = color;
    mark_dirty();
}

auto FrameObject::border_color() const -> CanvasColor
{
    return border_color_;
}

auto FrameObject::set_border_color(CanvasColor color) -> void
{
    border_color_ = color;
    mark_dirty();
}

// ── Containment ─────────────────────────────────────────────────

auto FrameObject::contains_object(const CanvasObject& obj) const -> bool
{
    if (obj.id() == id())
    {
        return false; // A frame doesn't contain itself.
    }

    const auto frame_bounds = world_bounds();
    const auto obj_bounds = obj.world_bounds();

    // Fully inside: all four corners of obj are within frame bounds.
    return obj_bounds.min_x >= frame_bounds.min_x && obj_bounds.min_y >= frame_bounds.min_y &&
           obj_bounds.max_x <= frame_bounds.max_x && obj_bounds.max_y <= frame_bounds.max_y;
}

auto FrameObject::collect_contained_ids(const Board& board) const -> std::vector<ObjectId>
{
    std::vector<ObjectId> contained;

    for (const auto& obj_ptr : board.objects())
    {
        if (obj_ptr && contains_object(*obj_ptr))
        {
            contained.push_back(obj_ptr->id());
        }
    }

    return contained;
}

// ── CanvasObject overrides ──────────────────────────────────────

auto FrameObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, width_, height_};
}

auto FrameObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<FrameObject>();
    copy->width_ = width_;
    copy->height_ = height_;
    copy->title_ = title_;
    copy->show_title_ = show_title_;
    copy->sequence_number_ = sequence_number_;
    copy->background_color_ = background_color_;
    copy->border_color_ = border_color_;
    copy->set_transform(transform());
    copy->set_z_index(z_index());
    copy->set_name(name());
    return copy;
}

auto FrameObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Frame\""
        << ",\"title\":\"" << title_ << "\""
        << ",\"sequence\":" << sequence_number_
        << ",\"show_title\":" << (show_title_ ? "true" : "false") << ",\"width\":" << width_
        << ",\"height\":" << height_ << ",\"bg_color\":\"" << background_color_.to_hex() << "\""
        << ",\"border_color\":\"" << border_color_.to_hex() << "\""
        << "}";
    return oss.str();
}

auto FrameObject::from_json(const std::string& json) -> void
{
    auto extract_string = [&](const std::string& key) -> std::string
    {
        const std::string needle = "\"" + key + "\":\"";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return {};
        }
        const auto val_start = pos + needle.size();
        const auto val_end = json.find('"', val_start);
        if (val_end == std::string::npos)
        {
            return {};
        }
        return json.substr(val_start, val_end - val_start);
    };

    auto extract_number = [&](const std::string& key) -> double
    {
        const std::string needle = "\"" + key + "\":";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return 0.0;
        }
        const auto val_start = pos + needle.size();
        return std::stod(json.substr(val_start));
    };

    auto extract_bool = [&](const std::string& key) -> bool
    {
        const std::string needle = "\"" + key + "\":";
        const auto pos = json.find(needle);
        if (pos == std::string::npos)
        {
            return false;
        }
        const auto val_start = pos + needle.size();
        return json.substr(val_start, 4) == "true";
    };

    set_title(extract_string("title"));
    set_sequence_number(static_cast<int>(extract_number("sequence")));
    set_show_title(extract_bool("show_title"));
    resize(extract_number("width"), extract_number("height"));

    const std::string bg_hex = extract_string("bg_color");
    if (!bg_hex.empty())
    {
        set_background_color(CanvasColor::from_hex(bg_hex));
    }
    const std::string border_hex = extract_string("border_color");
    if (!border_hex.empty())
    {
        set_border_color(CanvasColor::from_hex(border_hex));
    }
}

} // namespace markamp::canvas
