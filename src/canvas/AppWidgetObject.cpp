#include "AppWidgetObject.h"

namespace markamp::canvas
{

AppWidgetObject::AppWidgetObject()
    : CanvasObject(CanvasObjectType::AppWidget)
{
}

auto AppWidgetObject::type_name() const -> std::string
{
    return "app_widget";
}

auto AppWidgetObject::local_bounds() const -> AABB
{
    return AABB{0.0, 0.0, static_cast<double>(width_), static_cast<double>(height_)};
}

auto AppWidgetObject::clone() const -> std::unique_ptr<CanvasObject>
{
    auto cloned = std::make_unique<AppWidgetObject>();
    cloned->widget_id_ = widget_id_;
    cloned->app_id_ = app_id_;
    cloned->provider_id_ = provider_id_;
    cloned->state_json_ = state_json_;
    cloned->config_json_ = config_json_;
    cloned->width_ = width_;
    cloned->height_ = height_;
    cloned->sync_status_ = sync_status_;
    cloned->last_sync_error_ = last_sync_error_;
    cloned->bindings_ = bindings_;
    return cloned;
}

auto AppWidgetObject::set_widget_size(int width, int height) -> void
{
    width_ = width;
    height_ = height;
}

auto AppWidgetObject::add_binding(const WidgetDataBinding& binding) -> void
{
    bindings_.push_back(binding);
}

auto AppWidgetObject::clear_bindings() -> void
{
    bindings_.clear();
}

auto AppWidgetObject::from_json(const std::string& json) -> void
{
    // Parse JSON fields for AppWidgetObject properties.
    auto extract_string = [&](const std::string& key) -> std::string
    {
        const auto key_pos = json.find("\"" + key + "\"");
        if (key_pos == std::string::npos) return "";
        const auto colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) return "";
        const auto quote_start = json.find('"', colon_pos + 1);
        if (quote_start == std::string::npos) return "";
        const auto quote_end = json.find('"', quote_start + 1);
        if (quote_end == std::string::npos) return "";
        return json.substr(quote_start + 1, quote_end - quote_start - 1);
    };
    auto extract_number = [&](const std::string& key) -> double
    {
        const auto key_pos = json.find("\"" + key + "\"");
        if (key_pos == std::string::npos) return 0.0;
        const auto colon_pos = json.find(':', key_pos);
        if (colon_pos == std::string::npos) return 0.0;
        auto num_start = colon_pos + 1;
        while (num_start < json.size() && (json[num_start] == ' ' || json[num_start] == '\t'))
        {
            ++num_start;
        }
        std::string num_str;
        while (num_start < json.size() &&
               (std::isdigit(static_cast<unsigned char>(json[num_start])) ||
                json[num_start] == '.' || json[num_start] == '-'))
        {
            num_str += json[num_start++];
        }
        if (num_str.empty()) return 0.0;
        return std::stod(num_str);
    };

    auto widget_val = extract_string("widget_id");
    if (!widget_val.empty()) widget_id_ = widget_val;

    auto app_val = extract_string("app_id");
    if (!app_val.empty()) app_id_ = app_val;

    auto provider_val = extract_string("provider_id");
    if (!provider_val.empty()) provider_id_ = provider_val;

    auto state_val = extract_string("state_json");
    if (!state_val.empty()) state_json_ = state_val;

    auto config_val = extract_string("config_json");
    if (!config_val.empty()) config_json_ = config_val;

    auto width_val = extract_number("width");
    if (width_val > 0.0) width_ = static_cast<int>(width_val);

    auto height_val = extract_number("height");
    if (height_val > 0.0) height_ = static_cast<int>(height_val);
}

} // namespace markamp::canvas
