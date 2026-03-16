#include "IconObject.h"

#include <sstream>

namespace markamp::canvas
{

IconObject::IconObject()
    : CanvasObject(CanvasObjectType::Icon)
{
    set_name("Icon");
}

auto IconObject::icon_id() const -> const std::string&
{
    return icon_id_;
}

auto IconObject::set_icon_id(const std::string& icon_identifier) -> void
{
    icon_id_ = icon_identifier;
    mark_dirty();
}

auto IconObject::svg_content() const -> const std::string&
{
    return svg_content_;
}

auto IconObject::set_svg_content(const std::string& svg) -> void
{
    svg_content_ = svg;
    mark_dirty();
}

auto IconObject::icon_color() const -> const CanvasColor&
{
    return icon_color_;
}

auto IconObject::set_icon_color(const CanvasColor& color) -> void
{
    icon_color_ = color;
    mark_dirty();
}

auto IconObject::icon_size() const -> double
{
    return icon_size_;
}

auto IconObject::set_icon_size(double size) -> void
{
    icon_size_ = size;
    mark_dirty();
}

auto IconObject::local_bounds() const -> AABB
{
    return {0.0, 0.0, icon_size_, icon_size_};
}

auto IconObject::to_json() const -> std::string
{
    std::ostringstream oss;
    oss << "{\"type\":\"Icon\""
        << ",\"icon_id\":\"" << icon_id_ << "\""
        << ",\"size\":" << icon_size_ << ",\"color\":[" << static_cast<int>(icon_color_.r) << ","
        << static_cast<int>(icon_color_.g) << "," << static_cast<int>(icon_color_.b) << ","
        << static_cast<int>(icon_color_.a) << "]"
        << "}";
    return oss.str();
}

auto IconObject::from_json(const std::string& json) -> void
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

    set_icon_id(extract_string("icon_id"));
    set_icon_size(extract_number("size"));

    // Parse color array: "color":[r,g,b,a]
    const auto color_pos = json.find("\"color\":[");
    if (color_pos != std::string::npos)
    {
        auto cursor = color_pos + 9; // length of "color":[
        int components[4] = {0, 0, 0, 255};
        for (int idx = 0; idx < 4 && cursor < json.size(); ++idx)
        {
            components[idx] = std::stoi(json.substr(cursor));
            const auto next = json.find_first_of(",]", cursor);
            if (next == std::string::npos)
            {
                break;
            }
            cursor = next + 1;
        }
        set_icon_color({static_cast<uint8_t>(components[0]),
                        static_cast<uint8_t>(components[1]),
                        static_cast<uint8_t>(components[2]),
                        static_cast<uint8_t>(components[3])});
    }
}

auto IconObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto copy = std::make_unique<IconObject>();
    copy->set_icon_id(icon_id_);
    copy->set_svg_content(svg_content_);
    copy->set_icon_color(icon_color_);
    copy->set_icon_size(icon_size_);
    copy->set_name(name());
    return copy;
}

} // namespace markamp::canvas
