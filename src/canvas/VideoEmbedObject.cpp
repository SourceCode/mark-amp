#include "VideoEmbedObject.h"

#include <sstream>

namespace markamp::canvas
{

VideoEmbedObject::VideoEmbedObject()
    : CanvasObject(CanvasObjectType::VideoEmbed)
{
    set_name("Video Embed");
}

auto VideoEmbedObject::url() const -> const std::string&
{
    return url_;
}

auto VideoEmbedObject::set_url(const std::string& video_url) -> void
{
    url_ = video_url;
    mark_dirty();
}

auto VideoEmbedObject::oembed() const -> const OEmbedData&
{
    return oembed_;
}

auto VideoEmbedObject::set_oembed(const OEmbedData& data) -> void
{
    oembed_ = data;
    mark_dirty();
}

auto VideoEmbedObject::display_width() const -> double
{
    return display_width_;
}

auto VideoEmbedObject::display_height() const -> double
{
    return display_height_;
}

auto VideoEmbedObject::set_display_dimensions(double wid, double hei) -> void
{
    display_width_ = wid;
    display_height_ = hei;
    mark_dirty();
}

auto VideoEmbedObject::is_local_file() const -> bool
{
    return !local_file_.empty();
}

auto VideoEmbedObject::set_local_file(const std::filesystem::path& path) -> void
{
    local_file_ = path;
    mark_dirty();
}

auto VideoEmbedObject::local_file() const -> const std::filesystem::path&
{
    return local_file_;
}

auto VideoEmbedObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, display_width_, display_height_};
}

auto VideoEmbedObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"VideoEmbed\""
        << ",\"url\":\"" << url_ << "\""
        << ",\"title\":\"" << oembed_.title << "\""
        << ",\"provider\":\"" << oembed_.provider_name << "\""
        << ",\"width\":" << display_width_ << ",\"height\":" << display_height_;
    if (!local_file_.empty())
    {
        oss << ",\"local_file\":\"" << local_file_.string() << "\"";
    }
    oss << "}";
    return oss.str();
}

auto VideoEmbedObject::from_json(const std::string& json) -> void
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

    set_url(extract_string("url"));

    OEmbedData data;
    data.title = extract_string("title");
    data.provider_name = extract_string("provider");
    set_oembed(data);

    set_display_dimensions(extract_number("width"), extract_number("height"));

    const std::string local = extract_string("local_file");
    if (!local.empty())
    {
        set_local_file(std::filesystem::path{local});
    }
}

auto VideoEmbedObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<VideoEmbedObject>();
    copy->set_url(url_);
    copy->set_oembed(oembed_);
    copy->set_display_dimensions(display_width_, display_height_);
    if (!local_file_.empty())
    {
        copy->set_local_file(local_file_);
    }
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
