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

auto CrossBoardLinkObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"CrossBoardLink\""
        << ",\"board_id\":\"" << target_.board_id << "\""
        << ",\"board_name\":\"" << target_.board_name << "\""
        << ",\"display_text\":\"" << display_text_ << "\"";
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
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
