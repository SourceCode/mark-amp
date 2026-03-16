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

auto BookmarkCardObject::from_json(const std::string& json) -> void
{
    // Helper lambdas for lightweight JSON parsing (matches to_json() string format).
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

    set_url(extract_string("url"));

    BookmarkMetadata meta;
    meta.title = extract_string("title");
    meta.description = extract_string("description");
    meta.site_name = extract_string("site_name");
    set_bookmark_metadata(meta);

    set_show_image(extract_bool("show_image"));
    set_card_dimensions(extract_number("width"), extract_number("height"));
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
