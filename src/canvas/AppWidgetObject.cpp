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

} // namespace markamp::canvas
