/// @file ToolWindowHost.cpp
/// @brief V8 Phase 18 — Tool window host implementation.
/// Supports dock constraints, auto-hide, size constraints, ordering,
/// and JSON layout serialization.

#include "ui/ToolWindowHost.h"

#include "core/Config.h"
#include "core/EventBus.h"

#include <algorithm>
#include <sstream>

namespace markamp::ui
{

ToolWindowHost::ToolWindowHost(core::EventBus& event_bus, core::Config& config)
    : event_bus_(event_bus)
    , config_(config)
{
    toggle_sub_ = event_bus_.subscribe<core::events::ToolWindowToggleRequestEvent>(
        [this](const core::events::ToolWindowToggleRequestEvent& evt)
        { set_visible(evt.panel_id, evt.visible); });

    dock_changed_sub_ = event_bus_.subscribe<core::events::ToolWindowDockPositionChangedEvent>(
        [this](const core::events::ToolWindowDockPositionChangedEvent& evt)
        { set_dock_position(evt.panel_id, evt.dock_position); });
}

// ── Registration ────────────────────────────────────────────────────

void ToolWindowHost::register_panel(const std::string& panel_id,
                                    const std::string& title,
                                    core::events::DockPosition default_position)
{
    if (panels_.contains(panel_id))
    {
        return;
    }
    ToolWindowState state;
    state.panel_id = panel_id;
    state.title = title;
    state.dock_position = default_position;
    state.order = next_order_++;
    panels_[panel_id] = std::move(state);
}

void ToolWindowHost::unregister_panel(const std::string& panel_id)
{
    panels_.erase(panel_id);
}

// ── Visibility ──────────────────────────────────────────────────────

void ToolWindowHost::toggle_panel(const std::string& panel_id)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.visible = !iter->second.visible;
}

void ToolWindowHost::set_visible(const std::string& panel_id, bool visible)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    // Task 11: Track history when hiding a visible panel
    if (!visible && iter->second.visible)
    {
        panel_history_.push_back(panel_id);
    }
    iter->second.visible = visible;
}

auto ToolWindowHost::is_visible(const std::string& panel_id) const -> bool
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    return iter->second.visible;
}

// ── Dock Position ───────────────────────────────────────────────────

void ToolWindowHost::set_dock_position(const std::string& panel_id,
                                       core::events::DockPosition position)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    // Task 1: Enforce dock constraints
    if (!iter->second.allowed_positions.empty() &&
        iter->second.allowed_positions.find(position) == iter->second.allowed_positions.end())
    {
        return; // Position not allowed
    }
    iter->second.dock_position = position;
}

auto ToolWindowHost::dock_position(const std::string& panel_id) const -> core::events::DockPosition
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return core::events::DockPosition::kBottom;
    }
    return iter->second.dock_position;
}

// ── Pinning ─────────────────────────────────────────────────────────

void ToolWindowHost::set_pinned(const std::string& panel_id, bool pinned)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.pinned = pinned;
}

auto ToolWindowHost::is_pinned(const std::string& panel_id) const -> bool
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    return iter->second.pinned;
}

// ── Task 1: Dock Constraints ────────────────────────────────────────

void ToolWindowHost::set_allowed_positions(
    const std::string& panel_id, const std::unordered_set<core::events::DockPosition>& positions)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.allowed_positions = positions;

    // If current position is no longer allowed, move to first allowed position
    if (!positions.empty() && positions.find(iter->second.dock_position) == positions.end())
    {
        iter->second.dock_position = *positions.begin();
    }
}

auto ToolWindowHost::allowed_positions(const std::string& panel_id) const
    -> std::unordered_set<core::events::DockPosition>
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return {};
    }
    return iter->second.allowed_positions;
}

auto ToolWindowHost::is_position_allowed(const std::string& panel_id,
                                         core::events::DockPosition position) const -> bool
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    const auto& positions = iter->second.allowed_positions;
    if (positions.empty())
    {
        return true; // No constraints means all positions allowed
    }
    return positions.find(position) != positions.end();
}

// ── Task 2: Auto-Hide ───────────────────────────────────────────────

void ToolWindowHost::set_auto_hide(const std::string& panel_id, bool enabled)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.auto_hide = enabled;
}

auto ToolWindowHost::is_auto_hide(const std::string& panel_id) const -> bool
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return false;
    }
    return iter->second.auto_hide;
}

void ToolWindowHost::set_auto_hide_timeout(const std::string& panel_id, int timeout_ms)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.auto_hide_timeout_ms = std::max(100, timeout_ms); // Minimum 100ms
}

auto ToolWindowHost::auto_hide_timeout(const std::string& panel_id) const -> int
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return 2000;
    }
    return iter->second.auto_hide_timeout_ms;
}

// ── Task 3: Size Constraints ────────────────────────────────────────

void ToolWindowHost::set_min_size(const std::string& panel_id, int min_width, int min_height)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.size_constraint.min_width = std::max(0, min_width);
    iter->second.size_constraint.min_height = std::max(0, min_height);

    // Enforce: current size respects new minimums
    iter->second.width = std::max(iter->second.width, iter->second.size_constraint.min_width);
    iter->second.height = std::max(iter->second.height, iter->second.size_constraint.min_height);
}

void ToolWindowHost::set_max_size(const std::string& panel_id, int max_width, int max_height)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.size_constraint.max_width = std::max(1, max_width);
    iter->second.size_constraint.max_height = std::max(1, max_height);

    // Enforce: current size respects new maximums
    iter->second.width = std::min(iter->second.width, iter->second.size_constraint.max_width);
    iter->second.height = std::min(iter->second.height, iter->second.size_constraint.max_height);
}

auto ToolWindowHost::size_constraint(const std::string& panel_id) const -> PanelSizeConstraint
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return {};
    }
    return iter->second.size_constraint;
}

// ── Task 4: Ordering ────────────────────────────────────────────────

void ToolWindowHost::set_order(const std::string& panel_id, int order_value)
{
    auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return;
    }
    iter->second.order = order_value;
}

auto ToolWindowHost::order(const std::string& panel_id) const -> int
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return 0;
    }
    return iter->second.order;
}

void ToolWindowHost::move_panel_before(const std::string& panel_id,
                                       const std::string& before_panel_id)
{
    auto iter = panels_.find(panel_id);
    const auto before_iter = panels_.find(before_panel_id);
    if (iter == panels_.end() || before_iter == panels_.end())
    {
        return;
    }
    const int target_order = before_iter->second.order;
    // Shift all panels at or after target_order up by 1
    for (auto& [pid, state] : panels_)
    {
        if (state.order >= target_order && pid != panel_id)
        {
            ++state.order;
        }
    }
    iter->second.order = target_order;
}

void ToolWindowHost::move_panel_after(const std::string& panel_id,
                                      const std::string& after_panel_id)
{
    auto iter = panels_.find(panel_id);
    const auto after_iter = panels_.find(after_panel_id);
    if (iter == panels_.end() || after_iter == panels_.end())
    {
        return;
    }
    const int target_order = after_iter->second.order + 1;
    // Shift all panels at or after target_order up by 1
    for (auto& [pid, state] : panels_)
    {
        if (state.order >= target_order && pid != panel_id)
        {
            ++state.order;
        }
    }
    iter->second.order = target_order;
}

// ── Queries ─────────────────────────────────────────────────────────

auto ToolWindowHost::panel_state(const std::string& panel_id) const -> const ToolWindowState*
{
    const auto iter = panels_.find(panel_id);
    if (iter == panels_.end())
    {
        return nullptr;
    }
    return &iter->second;
}

auto ToolWindowHost::panel_ids() const -> std::vector<std::string>
{
    std::vector<std::string> ids;
    ids.reserve(panels_.size());
    for (const auto& [panel_id, state] : panels_)
    {
        ids.push_back(panel_id);
    }
    return ids;
}

auto ToolWindowHost::panels_at(core::events::DockPosition position) const
    -> std::vector<std::string>
{
    // Collect panels at the requested position
    std::vector<std::pair<int, std::string>> ordered;
    for (const auto& [panel_id, state] : panels_)
    {
        if (state.dock_position == position)
        {
            ordered.emplace_back(state.order, panel_id);
        }
    }
    // Sort by order (Task 4)
    std::sort(ordered.begin(),
              ordered.end(),
              [](const std::pair<int, std::string>& left, const std::pair<int, std::string>& right)
              { return left.first < right.first; });

    std::vector<std::string> result;
    result.reserve(ordered.size());
    for (auto& [ord, pid] : ordered)
    {
        result.push_back(std::move(pid));
    }
    return result;
}

auto ToolWindowHost::panel_count() const -> int
{
    return static_cast<int>(panels_.size());
}

// ── Task 5: State Serialization ─────────────────────────────────────

namespace
{

auto dock_to_string(core::events::DockPosition pos) -> std::string
{
    switch (pos)
    {
        case core::events::DockPosition::kLeft:
            return "left";
        case core::events::DockPosition::kRight:
            return "right";
        case core::events::DockPosition::kBottom:
        default:
            return "bottom";
    }
}

auto string_to_dock(const std::string& str) -> core::events::DockPosition
{
    if (str == "left")
    {
        return core::events::DockPosition::kLeft;
    }
    if (str == "right")
    {
        return core::events::DockPosition::kRight;
    }
    return core::events::DockPosition::kBottom;
}

} // namespace

auto ToolWindowHost::save_layout() const -> std::string
{
    // Minimal JSON serialization without nlohmann dependency
    std::ostringstream oss;
    oss << "{\"panels\":[";
    bool first = true;
    for (const auto& [panel_id, state] : panels_)
    {
        if (!first)
        {
            oss << ",";
        }
        first = false;
        oss << "{";
        oss << "\"id\":\"" << state.panel_id << "\"";
        oss << ",\"title\":\"" << state.title << "\"";
        oss << ",\"dock\":\"" << dock_to_string(state.dock_position) << "\"";
        oss << ",\"pinned\":" << (state.pinned ? "true" : "false");
        oss << ",\"visible\":" << (state.visible ? "true" : "false");
        oss << ",\"width\":" << state.width;
        oss << ",\"height\":" << state.height;
        oss << ",\"auto_hide\":" << (state.auto_hide ? "true" : "false");
        oss << ",\"auto_hide_timeout\":" << state.auto_hide_timeout_ms;
        oss << ",\"order\":" << state.order;
        oss << ",\"min_w\":" << state.size_constraint.min_width;
        oss << ",\"min_h\":" << state.size_constraint.min_height;
        oss << ",\"max_w\":" << state.size_constraint.max_width;
        oss << ",\"max_h\":" << state.size_constraint.max_height;
        // Allowed positions
        oss << ",\"allowed\":[";
        bool first_pos = true;
        for (const auto& pos : state.allowed_positions)
        {
            if (!first_pos)
            {
                oss << ",";
            }
            first_pos = false;
            oss << "\"" << dock_to_string(pos) << "\"";
        }
        oss << "]";
        oss << "}";
    }
    oss << "]}";
    return oss.str();
}

auto ToolWindowHost::restore_layout(const std::string& json_str) -> bool
{
    // Lightweight restore: parse panel IDs and update existing registered panels.
    // Looks for "id":"<value>" patterns and corresponding fields.
    // This is a simplified parser that handles the save_layout() output format.
    if (json_str.empty() || json_str == "{}")
    {
        return true;
    }

    // Find each panel block between { and }
    std::string::size_type search_pos = 0;
    while (true)
    {
        const auto block_start = json_str.find('{', search_pos);
        if (block_start == std::string::npos)
        {
            break;
        }
        const auto block_end = json_str.find('}', block_start);
        if (block_end == std::string::npos)
        {
            break;
        }
        search_pos = block_end + 1;

        const std::string block = json_str.substr(block_start, block_end - block_start + 1);

        // Extract panel ID
        const auto id_pos = block.find("\"id\":\"");
        if (id_pos == std::string::npos)
        {
            continue;
        }
        const auto id_start = id_pos + 6;
        const auto id_end = block.find('\"', id_start);
        if (id_end == std::string::npos)
        {
            continue;
        }
        const std::string restored_id = block.substr(id_start, id_end - id_start);

        auto iter = panels_.find(restored_id);
        if (iter == panels_.end())
        {
            continue; // Only restore state for currently registered panels
        }

        // Restore dock position
        const auto dock_pos = block.find("\"dock\":\"");
        if (dock_pos != std::string::npos)
        {
            const auto dock_start = dock_pos + 8;
            const auto dock_end = block.find('\"', dock_start);
            if (dock_end != std::string::npos)
            {
                iter->second.dock_position =
                    string_to_dock(block.substr(dock_start, dock_end - dock_start));
            }
        }

        // Restore boolean: pinned
        const auto pinned_pos = block.find("\"pinned\":");
        if (pinned_pos != std::string::npos)
        {
            iter->second.pinned = (block.substr(pinned_pos + 9, 4) == "true");
        }

        // Restore boolean: visible
        const auto visible_pos = block.find("\"visible\":");
        if (visible_pos != std::string::npos)
        {
            iter->second.visible = (block.substr(visible_pos + 10, 4) == "true");
        }

        // Restore boolean: auto_hide
        // Search for "auto_hide": but NOT "auto_hide_timeout":
        std::size_t ah_search_pos = 0;
        bool auto_hide_found = false;
        while (!auto_hide_found)
        {
            const auto ah_pos = block.find("\"auto_hide\"", ah_search_pos);
            if (ah_pos == std::string::npos)
            {
                break;
            }
            // Check the character right after "auto_hide" — if it's '_' then
            // this is "auto_hide_timeout", skip it
            const auto after_key = ah_pos + 11; // length of "auto_hide"
            if (after_key < block.size() && block[after_key] == '_')
            {
                ah_search_pos = after_key;
                continue;
            }
            // Found the right key — skip to value after ':'
            const auto colon = block.find(':', ah_pos);
            if (colon != std::string::npos && colon + 1 < block.size())
            {
                iter->second.auto_hide = (block.substr(colon + 1, 4) == "true");
                auto_hide_found = true;
            }
            break;
        }

        // Restore integer fields with a helper
        auto restore_int = [&](const std::string& key, int& target)
        {
            const auto pos = block.find("\"" + key + "\":");
            if (pos != std::string::npos)
            {
                const auto val_start = pos + key.size() + 3;
                const auto val_end = block.find_first_not_of("0123456789-", val_start);
                if (val_end != std::string::npos && val_start < block.size())
                {
                    try
                    {
                        target = std::stoi(block.substr(val_start, val_end - val_start));
                    }
                    catch (...)
                    {
                        // Parse error, keep default
                    }
                }
            }
        };

        restore_int("width", iter->second.width);
        restore_int("height", iter->second.height);
        restore_int("auto_hide_timeout", iter->second.auto_hide_timeout_ms);
        restore_int("order", iter->second.order);
        restore_int("min_w", iter->second.size_constraint.min_width);
        restore_int("min_h", iter->second.size_constraint.min_height);
        restore_int("max_w", iter->second.size_constraint.max_width);
        restore_int("max_h", iter->second.size_constraint.max_height);
    }

    return true;
}

// ── Task 6: Panel Commands ──────────────────────────────────────────

void ToolWindowHost::register_command(const std::string& panel_id, const std::string& command_id)
{
    auto& cmds = panel_commands_[panel_id];
    if (std::find(cmds.begin(), cmds.end(), command_id) == cmds.end())
    {
        cmds.push_back(command_id);
    }
}

void ToolWindowHost::unregister_command(const std::string& panel_id, const std::string& command_id)
{
    auto cmd_iter = panel_commands_.find(panel_id);
    if (cmd_iter == panel_commands_.end())
    {
        return;
    }
    auto& cmds = cmd_iter->second;
    cmds.erase(std::remove(cmds.begin(), cmds.end(), command_id), cmds.end());
}

auto ToolWindowHost::commands(const std::string& panel_id) const -> std::vector<std::string>
{
    const auto cmd_iter = panel_commands_.find(panel_id);
    if (cmd_iter == panel_commands_.end())
    {
        return {};
    }
    return cmd_iter->second;
}

void ToolWindowHost::execute_command(const std::string& panel_id, const std::string& command_id)
{
    core::events::PanelCommandEvent evt;
    evt.panel_id = panel_id;
    evt.command_id = command_id;
    event_bus_.publish(evt);
}

// ── Task 7: Keyboard Navigation ─────────────────────────────────────

void ToolWindowHost::focus_panel(const std::string& panel_id)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    focused_panel_id_ = panel_id;

    core::events::PanelFocusChangedEvent evt;
    evt.panel_id = panel_id;
    evt.focused = true;
    event_bus_.publish(evt);
}

void ToolWindowHost::focus_next_panel()
{
    if (focused_panel_id_.empty())
    {
        // Focus the first visible panel
        for (const auto& [pid, state] : panels_)
        {
            if (state.visible)
            {
                focus_panel(pid);
                return;
            }
        }
        return;
    }

    // Get panels at the focused panel's dock position, sorted by order
    const auto focused_state = panels_.find(focused_panel_id_);
    if (focused_state == panels_.end())
    {
        return;
    }
    const auto ordered = panels_at(focused_state->second.dock_position);

    // Find current index and advance
    for (std::size_t idx = 0; idx < ordered.size(); ++idx)
    {
        if (ordered[idx] == focused_panel_id_)
        {
            const std::size_t next_idx = (idx + 1) % ordered.size();
            focus_panel(ordered[next_idx]);
            return;
        }
    }
}

void ToolWindowHost::focus_previous_panel()
{
    if (focused_panel_id_.empty())
    {
        return;
    }

    const auto focused_state = panels_.find(focused_panel_id_);
    if (focused_state == panels_.end())
    {
        return;
    }
    const auto ordered = panels_at(focused_state->second.dock_position);

    for (std::size_t idx = 0; idx < ordered.size(); ++idx)
    {
        if (ordered[idx] == focused_panel_id_)
        {
            const std::size_t prev_idx = (idx == 0) ? ordered.size() - 1 : idx - 1;
            focus_panel(ordered[prev_idx]);
            return;
        }
    }
}

auto ToolWindowHost::focused_panel() const -> const std::string&
{
    return focused_panel_id_;
}

// ── Task 8: Badges ──────────────────────────────────────────────────

void ToolWindowHost::set_badge(const std::string& panel_id,
                               const core::events::PanelBadge& panel_badge)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    panel_badges_[panel_id] = panel_badge;

    core::events::PanelBadgeChangedEvent evt;
    evt.panel_id = panel_id;
    evt.badge = panel_badge;
    event_bus_.publish(evt);
}

auto ToolWindowHost::badge(const std::string& panel_id) const -> core::events::PanelBadge
{
    const auto badge_iter = panel_badges_.find(panel_id);
    if (badge_iter == panel_badges_.end())
    {
        return {};
    }
    return badge_iter->second;
}

void ToolWindowHost::clear_badge(const std::string& panel_id)
{
    panel_badges_.erase(panel_id);

    core::events::PanelBadgeChangedEvent evt;
    evt.panel_id = panel_id;
    event_bus_.publish(evt);
}

// ── Task 9: Context Menus ───────────────────────────────────────────

void ToolWindowHost::set_context_menu(const std::string& panel_id,
                                      const std::vector<core::events::PanelMenuItem>& items)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    panel_menus_[panel_id] = items;
}

auto ToolWindowHost::context_menu(const std::string& panel_id) const
    -> std::vector<core::events::PanelMenuItem>
{
    const auto menu_iter = panel_menus_.find(panel_id);
    if (menu_iter == panel_menus_.end())
    {
        return {};
    }
    return menu_iter->second;
}

// ── Task 10: Panel Grouping ─────────────────────────────────────────

void ToolWindowHost::set_group(const std::string& panel_id, const std::string& group_name)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    panel_groups_[panel_id] = group_name;
}

auto ToolWindowHost::group(const std::string& panel_id) const -> std::string
{
    const auto grp_iter = panel_groups_.find(panel_id);
    if (grp_iter == panel_groups_.end())
    {
        return {};
    }
    return grp_iter->second;
}

auto ToolWindowHost::panels_in_group(const std::string& group_name) const
    -> std::vector<std::string>
{
    std::vector<std::string> result;
    for (const auto& [pid, grp] : panel_groups_)
    {
        if (grp == group_name && panels_.contains(pid))
        {
            result.push_back(pid);
        }
    }
    return result;
}

// ── Task 11: Panel History ──────────────────────────────────────────

auto ToolWindowHost::panel_history() const -> const std::vector<std::string>&
{
    return panel_history_;
}

void ToolWindowHost::show_last_panel()
{
    while (!panel_history_.empty())
    {
        const std::string last_id = panel_history_.back();
        panel_history_.pop_back();
        auto panel_iter = panels_.find(last_id);
        if (panel_iter != panels_.end() && !panel_iter->second.visible)
        {
            panel_iter->second.visible = true;
            return;
        }
    }
}

// ── Task 12: Panel Zoom ─────────────────────────────────────────────

static constexpr double kMinZoom = 0.1;
static constexpr double kMaxZoom = 5.0;
static constexpr double kZoomStep = 0.1;

void ToolWindowHost::set_zoom_level(const std::string& panel_id, double zoom)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    panel_zoom_[panel_id] = std::clamp(zoom, kMinZoom, kMaxZoom);
}

auto ToolWindowHost::zoom_level(const std::string& panel_id) const -> double
{
    const auto zoom_iter = panel_zoom_.find(panel_id);
    if (zoom_iter == panel_zoom_.end())
    {
        return 1.0;
    }
    return zoom_iter->second;
}

void ToolWindowHost::zoom_in(const std::string& panel_id)
{
    set_zoom_level(panel_id, zoom_level(panel_id) + kZoomStep);
}

void ToolWindowHost::zoom_out(const std::string& panel_id)
{
    set_zoom_level(panel_id, zoom_level(panel_id) - kZoomStep);
}

void ToolWindowHost::reset_zoom(const std::string& panel_id)
{
    set_zoom_level(panel_id, 1.0);
}

// ── Task 13: Panel Search ───────────────────────────────────────────

void ToolWindowHost::set_searchable(const std::string& panel_id, bool searchable)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    if (searchable)
    {
        searchable_panels_.insert(panel_id);
    }
    else
    {
        searchable_panels_.erase(panel_id);
        panel_search_text_.erase(panel_id);
    }
}

auto ToolWindowHost::is_searchable(const std::string& panel_id) const -> bool
{
    return searchable_panels_.contains(panel_id);
}

void ToolWindowHost::set_search_text(const std::string& panel_id, const std::string& text)
{
    if (!searchable_panels_.contains(panel_id))
    {
        return;
    }
    panel_search_text_[panel_id] = text;

    core::events::PanelSearchChangedEvent evt;
    evt.panel_id = panel_id;
    evt.search_text = text;
    event_bus_.publish(evt);
}

auto ToolWindowHost::search_text(const std::string& panel_id) const -> std::string
{
    const auto text_iter = panel_search_text_.find(panel_id);
    if (text_iter == panel_search_text_.end())
    {
        return {};
    }
    return text_iter->second;
}

void ToolWindowHost::clear_search(const std::string& panel_id)
{
    set_search_text(panel_id, "");
}

// ── Task 14: Panel Theme Integration ────────────────────────────────

void ToolWindowHost::set_theme_callback(const std::string& panel_id, ThemeCallback callback)
{
    if (!panels_.contains(panel_id))
    {
        return;
    }
    theme_callbacks_[panel_id] = std::move(callback);
}

void ToolWindowHost::apply_theme_to_all()
{
    for (const auto& [pid, callback] : theme_callbacks_)
    {
        if (callback && panels_.contains(pid))
        {
            callback(pid);
        }
    }
}

// ── Task 15: Panel Extension Support ────────────────────────────────

void ToolWindowHost::register_extension_panel(const std::string& extension_id,
                                              const std::string& panel_id,
                                              const std::string& title)
{
    // Register the panel normally
    register_panel(panel_id, title);

    // Track ownership
    extension_panel_map_[extension_id].push_back(panel_id);
}

void ToolWindowHost::unregister_extension_panels(const std::string& extension_id)
{
    auto ext_iter = extension_panel_map_.find(extension_id);
    if (ext_iter == extension_panel_map_.end())
    {
        return;
    }
    for (const auto& pid : ext_iter->second)
    {
        unregister_panel(pid);
    }
    extension_panel_map_.erase(ext_iter);
}

auto ToolWindowHost::extension_panels(const std::string& extension_id) const
    -> std::vector<std::string>
{
    const auto ext_iter = extension_panel_map_.find(extension_id);
    if (ext_iter == extension_panel_map_.end())
    {
        return {};
    }
    return ext_iter->second;
}

} // namespace markamp::ui
