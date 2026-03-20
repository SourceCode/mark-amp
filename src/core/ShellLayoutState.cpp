/// @file ShellLayoutState.cpp
/// @brief P03-T04: Centralized shell layout visibility state model.

#include "ShellLayoutState.h"

#include "Config.h"
#include "EventBus.h"
#include "Events.h"
#include "Logger.h"

namespace markamp::core
{

ShellLayoutState::ShellLayoutState(EventBus& bus, Config& cfg)
    : event_bus_(&bus)
    , config_(&cfg)
{
}

void ShellLayoutState::toggle_sidebar()
{
    sidebar_visible_ = !sidebar_visible_;
    if (event_bus_ != nullptr)
    {
        event_bus_->publish(events::SidebarToggleEvent{});
    }
    save_to_config();
    MARKAMP_LOG_DEBUG("Sidebar visibility: {}", sidebar_visible_);
}

void ShellLayoutState::toggle_secondary_sidebar()
{
    secondary_sidebar_visible_ = !secondary_sidebar_visible_;
    save_to_config();
    MARKAMP_LOG_DEBUG("Secondary sidebar visibility: {}", secondary_sidebar_visible_);
}

void ShellLayoutState::toggle_bottom_panel()
{
    bottom_panel_visible_ = !bottom_panel_visible_;
    if (event_bus_ != nullptr)
    {
        event_bus_->publish(events::ToggleBottomPanelRequestEvent{});
    }
    save_to_config();
    MARKAMP_LOG_DEBUG("Bottom panel visibility: {}", bottom_panel_visible_);
}

void ShellLayoutState::toggle_status_bar()
{
    status_bar_visible_ = !status_bar_visible_;
    save_to_config();
    MARKAMP_LOG_DEBUG("Status bar visibility: {}", status_bar_visible_);
}

void ShellLayoutState::set_sidebar_mode(int mode)
{
    sidebar_mode_ = mode;
    save_to_config();
}

void ShellLayoutState::set_bottom_panel_height(int height)
{
    bottom_panel_height_ = height;
    save_to_config();
}

void ShellLayoutState::enter_zen_mode()
{
    if (zen_mode_)
    {
        return;
    }
    // Save pre-zen state
    pre_zen_sidebar_ = sidebar_visible_;
    pre_zen_secondary_sidebar_ = secondary_sidebar_visible_;
    pre_zen_bottom_panel_ = bottom_panel_visible_;
    pre_zen_status_bar_ = status_bar_visible_;

    // Hide everything
    sidebar_visible_ = false;
    secondary_sidebar_visible_ = false;
    bottom_panel_visible_ = false;
    status_bar_visible_ = false;
    zen_mode_ = true;

    MARKAMP_LOG_INFO("Entered zen mode");
}

void ShellLayoutState::exit_zen_mode()
{
    if (!zen_mode_)
    {
        return;
    }
    // Restore pre-zen state
    sidebar_visible_ = pre_zen_sidebar_;
    secondary_sidebar_visible_ = pre_zen_secondary_sidebar_;
    bottom_panel_visible_ = pre_zen_bottom_panel_;
    status_bar_visible_ = pre_zen_status_bar_;
    zen_mode_ = false;

    MARKAMP_LOG_INFO("Exited zen mode, restored panel state");
}

void ShellLayoutState::save_to_config()
{
    if (config_ == nullptr)
    {
        return;
    }
    config_->set("shell_sidebar_visible", std::string(sidebar_visible_ ? "true" : "false"));
    config_->set("shell_secondary_sidebar_visible",
                 std::string(secondary_sidebar_visible_ ? "true" : "false"));
    config_->set("shell_bottom_panel_visible",
                 std::string(bottom_panel_visible_ ? "true" : "false"));
    config_->set("shell_status_bar_visible", std::string(status_bar_visible_ ? "true" : "false"));
    config_->set("shell_sidebar_mode", std::to_string(sidebar_mode_));
    config_->set("shell_bottom_panel_height", std::to_string(bottom_panel_height_));
}

void ShellLayoutState::load_from_config()
{
    if (config_ == nullptr)
    {
        return;
    }
    sidebar_visible_ = config_->get_bool("shell_sidebar_visible", true);
    secondary_sidebar_visible_ = config_->get_bool("shell_secondary_sidebar_visible", false);
    bottom_panel_visible_ = config_->get_bool("shell_bottom_panel_visible", false);
    status_bar_visible_ = config_->get_bool("shell_status_bar_visible", true);
    sidebar_mode_ = config_->get_int("shell_sidebar_mode", 0);
    bottom_panel_height_ = config_->get_int("shell_bottom_panel_height", 200);

    MARKAMP_LOG_DEBUG("Shell layout state loaded from config");
}

} // namespace markamp::core
