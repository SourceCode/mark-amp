#include "core/PanelService.h"

#include "ui/PanelAreaModel.h"

namespace markamp::core
{

PanelService::PanelService(ui::PanelAreaModel& model)
    : model_(model)
{
}

void PanelService::register_panel(const std::string& id,
                                  const std::string& title,
                                  const std::string& icon_name)
{
    markamp::ui::PanelTabConfig config;
    config.id = id;
    config.title = title;
    config.icon_name = icon_name;
    config.closable = true;
    config.reorderable = true;
    config.visible = true;

    model_.register_panel(config);
}

void PanelService::unregister_panel(const std::string& id)
{
    model_.unregister_panel(id);
}

void PanelService::set_active_panel(const std::string& id)
{
    model_.set_active(id);
}

auto PanelService::get_active_panel() const -> std::string
{
    return model_.active_panel();
}

} // namespace markamp::core
