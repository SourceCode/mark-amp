#include "ToolbarModel.h"

#include <sstream>

namespace markamp::ui
{

void ToolbarModel::set_global_actions(std::vector<ToolbarButtonModel> actions)
{
    global_actions_ = std::move(actions);
}

void ToolbarModel::add_context_slot(ActionSlot slot)
{
    context_slots_.push_back(std::move(slot));
}

void ToolbarModel::set_mode(const std::string& mode)
{
    active_mode_ = mode;
}

auto ToolbarModel::active_mode() const -> std::string
{
    return active_mode_;
}

auto ToolbarModel::visible_buttons() const -> std::vector<ToolbarButtonModel>
{
    auto result = global_actions_;
    const auto ctx = context_buttons();
    result.insert(result.end(), ctx.begin(), ctx.end());
    return result;
}

auto ToolbarModel::global_buttons() const -> const std::vector<ToolbarButtonModel>&
{
    return global_actions_;
}

auto ToolbarModel::context_buttons() const -> std::vector<ToolbarButtonModel>
{
    for (const auto& slot : context_slots_)
    {
        if (slot.mode == active_mode_)
        {
            return slot.buttons;
        }
    }
    return {};
}

auto ToolbarModel::toggle(const std::string& button_id) -> bool
{
    auto toggle_in = [&button_id](std::vector<ToolbarButtonModel>& buttons) -> bool
    {
        for (auto& btn : buttons)
        {
            if (btn.button_id == button_id && btn.kind == ToolbarButtonKind::kToggle)
            {
                btn.is_toggled = !btn.is_toggled;
                return true;
            }
        }
        return false;
    };

    if (toggle_in(global_actions_))
    {
        return true;
    }

    for (auto& slot : context_slots_)
    {
        if (slot.mode == active_mode_ && toggle_in(slot.buttons))
        {
            return true;
        }
    }
    return false;
}

auto ToolbarModel::overflowed_indices(int viewport_width, int button_width) const
    -> std::vector<int>
{
    std::vector<int> result;
    if (button_width <= 0)
    {
        return result;
    }

    const auto all = visible_buttons();
    int used_width = 0;
    for (int idx = 0; idx < static_cast<int>(all.size()); ++idx)
    {
        used_width += button_width;
        if (used_width > viewport_width)
        {
            result.push_back(idx);
        }
    }
    return result;
}

auto ToolbarModel::build_tooltip(const ToolbarButtonModel& button) -> std::string
{
    std::ostringstream tooltip;
    tooltip << button.tooltip;

    if (!button.shortcut_hint.empty())
    {
        tooltip << " (" << button.shortcut_hint << ")";
    }

    if (button.kind == ToolbarButtonKind::kToggle)
    {
        tooltip << " [" << (button.is_toggled ? "ON" : "OFF") << "]";
    }

    return tooltip.str();
}

} // namespace markamp::ui
