#include "CrossBoardLink.h"

#include <sstream>

namespace markamp::canvas
{

CrossBoardLinkObject::CrossBoardLinkObject()
    : CanvasObject(CanvasObjectType::CrossBoardLink)
{
    set_name("Cross-Board Link");
}

auto CrossBoardLinkObject::target() const -> const BoardLinkTarget&
{
    return target_;
}

auto CrossBoardLinkObject::set_target(const BoardLinkTarget& target) -> void
{
    target_ = target;
    mark_dirty();
}

auto CrossBoardLinkObject::display_text() const -> const std::string&
{
    return display_text_;
}

auto CrossBoardLinkObject::set_display_text(const std::string& text) -> void
{
    display_text_ = text;
    mark_dirty();
}

auto CrossBoardLinkObject::width() const -> double
{
    return width_;
}

auto CrossBoardLinkObject::height() const -> double
{
    return height_;
}

auto CrossBoardLinkObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, width_, height_};
}

// ── Improvements (#33-36) ───────────────────────────────────────

auto CrossBoardLinkObject::tooltip() const -> const std::string&
{
    return tooltip_;
}
auto CrossBoardLinkObject::set_tooltip(const std::string& tip) -> void
{
    tooltip_ = tip;
    mark_dirty();
}

auto CrossBoardLinkObject::link_color() const -> const CanvasColor&
{
    return link_color_;
}
auto CrossBoardLinkObject::set_link_color(const CanvasColor& color) -> void
{
    link_color_ = color;
    mark_dirty();
}

auto CrossBoardLinkObject::is_bidirectional() const -> bool
{
    return bidirectional_;
}
auto CrossBoardLinkObject::set_bidirectional(bool bidirectional) -> void
{
    bidirectional_ = bidirectional;
    mark_dirty();
}

auto CrossBoardLinkObject::last_verified_at() const -> const std::string&
{
    return last_verified_at_;
}
auto CrossBoardLinkObject::set_last_verified_at(const std::string& timestamp) -> void
{
    last_verified_at_ = timestamp;
    mark_dirty();
}

auto CrossBoardLinkObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"CrossBoardLink\""
        << ",\"board_id\":\"" << target_.board_id << "\""
        << ",\"board_name\":\"" << target_.board_name << "\""
        << ",\"display_text\":\"" << display_text_ << "\""
        << ",\"tooltip\":\"" << tooltip_ << "\""
        << ",\"bidirectional\":" << (bidirectional_ ? "true" : "false")
        << ",\"last_verified_at\":\"" << last_verified_at_ << "\"";
    if (target_.frame_id.has_value())
    {
        oss << ",\"frame_id\":" << target_.frame_id.value();
    }
    if (target_.object_id.has_value())
    {
        oss << ",\"object_id\":" << target_.object_id.value();
    }
    oss << "}";
    return oss.str();
}

auto CrossBoardLinkObject::from_json(const std::string& /*json*/) -> void
{
    // Stub — real implementation with nlohmann::json or similar.
}

auto CrossBoardLinkObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<CrossBoardLinkObject>();
    copy->set_target(target_);
    copy->set_display_text(display_text_);
    copy->set_tooltip(tooltip_);
    copy->set_link_color(link_color_);
    copy->set_bidirectional(bidirectional_);
    copy->set_last_verified_at(last_verified_at_);
    copy->set_name(name());
    return copy;
}

// ── Batch 10 (#55-56) ─────────────────────────────────────────────

auto CrossBoardLinkObject::is_broken() const -> bool
{
    return target_.board_id.empty();
}

auto CrossBoardLinkObject::badge_text() const -> const std::string&
{
    return badge_text_;
}

auto CrossBoardLinkObject::set_badge_text(const std::string& text) -> void
{
    badge_text_ = text;
    mark_dirty();
}

} // namespace markamp::canvas
