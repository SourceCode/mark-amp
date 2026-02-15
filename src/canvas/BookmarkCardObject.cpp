#include "BookmarkCardObject.h"

#include <sstream>

namespace markamp::canvas
{

BookmarkCardObject::BookmarkCardObject()
    : CanvasObject(CanvasObjectType::BookmarkCard)
{
    set_name("Bookmark Card");
}

auto BookmarkCardObject::url() const -> const std::string&
{
    return url_;
}

auto BookmarkCardObject::set_url(const std::string& page_url) -> void
{
    url_ = page_url;
    mark_dirty();
}

auto BookmarkCardObject::bookmark_metadata() const -> const BookmarkMetadata&
{
    return metadata_;
}

auto BookmarkCardObject::set_bookmark_metadata(const BookmarkMetadata& meta) -> void
{
    metadata_ = meta;
    mark_dirty();
}

auto BookmarkCardObject::card_width() const -> double
{
    return card_width_;
}

auto BookmarkCardObject::card_height() const -> double
{
    return card_height_;
}

auto BookmarkCardObject::set_card_dimensions(double wid, double hei) -> void
{
    card_width_ = wid;
    card_height_ = hei;
    mark_dirty();
}

auto BookmarkCardObject::show_image() const -> bool
{
    return show_image_;
}

auto BookmarkCardObject::set_show_image(bool show) -> void
{
    show_image_ = show;
    mark_dirty();
}

auto BookmarkCardObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, card_width_, card_height_};
}

auto BookmarkCardObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"BookmarkCard\""
        << ",\"url\":\"" << url_ << "\""
        << ",\"title\":\"" << metadata_.title << "\""
        << ",\"description\":\"" << metadata_.description << "\""
        << ",\"site_name\":\"" << metadata_.site_name << "\""
        << ",\"show_image\":" << (show_image_ ? "true" : "false") << ",\"width\":" << card_width_
        << ",\"height\":" << card_height_ << "}";
    return oss.str();
}

auto BookmarkCardObject::from_json(const std::string& /*json*/) -> void
{
    // Stub — real implementation with nlohmann::json or similar.
}

auto BookmarkCardObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<BookmarkCardObject>();
    copy->set_url(url_);
    copy->set_bookmark_metadata(metadata_);
    copy->set_card_dimensions(card_width_, card_height_);
    copy->set_show_image(show_image_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
