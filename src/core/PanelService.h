#pragma once

#include "core/IPanelService.h"

namespace markamp::ui
{
class PanelAreaModel;
}

namespace markamp::core
{

/**
 * @brief Implementation of IPanelService that wraps the PanelAreaModel.
 */
class PanelService : public IPanelService
{
public:
    explicit PanelService(ui::PanelAreaModel& model);
    ~PanelService() override = default;

    PanelService(const PanelService&) = delete;
    auto operator=(const PanelService&) -> PanelService& = delete;
    PanelService(PanelService&&) = delete;
    auto operator=(PanelService&&) -> PanelService& = delete;

    void register_panel(const std::string& id,
                        const std::string& title,
                        const std::string& icon_name) override;

    void unregister_panel(const std::string& id) override;

    void set_active_panel(const std::string& id) override;

    [[nodiscard]] auto get_active_panel() const -> std::string override;

private:
    ui::PanelAreaModel& model_;
};

} // namespace markamp::core
