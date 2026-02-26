/**
 * @file PanelAreaModel.h
 * @brief Data model for the bottom panel area in MarkAmp.
 */

#pragma once

#include "core/Events.h"

#include <map>
#include <string>
#include <vector>

namespace markamp::ui
{

/**
 * @struct PanelTabConfig
 * @brief Configuration and state for a single panel in the bottom panel area.
 */
struct PanelTabConfig
{
    std::string id;
    std::string title;
    std::string icon_name;
    core::events::BadgeState badge{core::events::BadgeState::kNone};
    int badge_count{0};
    bool closable{true};
    bool reorderable{true};
    int order{0};
    bool visible{true};
};

/**
 * @class PanelAreaModel
 * @brief Stores and manages the state of the bottom panels.
 */
class PanelAreaModel
{
public:
    explicit PanelAreaModel(core::EventBus& event_bus);
    ~PanelAreaModel() = default;

    PanelAreaModel(const PanelAreaModel&) = delete;
    auto operator=(const PanelAreaModel&) -> PanelAreaModel& = delete;
    PanelAreaModel(PanelAreaModel&&) = delete;
    auto operator=(PanelAreaModel&&) -> PanelAreaModel& = delete;

    /**
     * @brief Register a new panel configuration.
     */
    void register_panel(const PanelTabConfig& config);

    /**
     * @brief Unregister and remove a panel by its ID.
     */
    void unregister_panel(const std::string& panel_id);

    /**
     * @brief Sets the currently active panel.
     */
    void set_active(const std::string& panel_id);

    /**
     * @brief Gets the ID of the currently active panel.
     */
    [[nodiscard]] auto active_panel() const -> std::string;

    /**
     * @brief Retrieves all visible panels sorted by their order.
     */
    [[nodiscard]] auto panels() const -> std::vector<PanelTabConfig>;

    /**
     * @brief Checks if a specific panel ID is currently registered.
     */
    [[nodiscard]] auto has_panel(const std::string& panel_id) const -> bool;

    /**
     * @brief Sets the badge state for a specific panel.
     */
    void set_badge(const std::string& panel_id, core::events::BadgeState state, int count);

    /**
     * @brief Sets the display order for a specific panel.
     */
    void set_order(const std::string& panel_id, int order);

    /**
     * @brief Toggles the visibility of a panel.
     */
    void set_visible(const std::string& panel_id, bool visible);

private:
    core::EventBus& event_bus_;
    std::map<std::string, PanelTabConfig> registered_panels_;
    std::string active_panel_id_;
};

} // namespace markamp::ui
