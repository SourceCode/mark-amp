/// @file WorkspaceLayout.cpp
/// @brief V4 Phase 21 – Customizable UI Layout System implementation.

#include "ui/WorkspaceLayout.h"

#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <algorithm>
#include <sstream>

namespace markamp::ui
{

// ============================================================================
// Constructor
// ============================================================================

WorkspaceLayoutManager::WorkspaceLayoutManager(markamp::core::EventBus& event_bus,
                                               markamp::core::Config& config,
                                               markamp::core::ThemeEngine& theme_engine)
    : event_bus_(event_bus)
    , config_(config)
    , theme_engine_(theme_engine)
{
    load_builtin_presets();
    // Apply default preset on construction
    if (!presets_.empty())
    {
        current_layout_ = presets_.front().panels;
    }
}

// ============================================================================
// Built-in presets
// ============================================================================

auto WorkspaceLayoutManager::load_builtin_presets() -> void
{
    // Default: file tree left, editor center, preview right
    {
        LayoutPreset preset;
        preset.name = "Default";
        preset.description = "Standard layout with file tree, editor, and preview";
        preset.is_builtin = true;
        preset.panels = {
            {"file_tree", PanelPosition::kLeftSidebar, 0, 250, 0, true, false},
            {"outline", PanelPosition::kRightSidebar, 0, 200, 0, true, false},
            {"backlinks", PanelPosition::kRightSidebar, 1, 200, 0, false, false},
            {"graph", PanelPosition::kBottomPanel, 0, 0, 200, false, false},
            {"search", PanelPosition::kLeftSidebar, 1, 250, 0, false, false},
            {"terminal", PanelPosition::kBottomPanel, 1, 0, 200, false, false},
        };
        presets_.push_back(std::move(preset));
    }

    // Writing: editor only, outline right
    {
        LayoutPreset preset;
        preset.name = "Writing";
        preset.description = "Distraction-free writing with outline";
        preset.is_builtin = true;
        preset.panels = {
            {"file_tree", PanelPosition::kHidden, 0, 250, 0, false, false},
            {"outline", PanelPosition::kRightSidebar, 0, 200, 0, true, false},
            {"backlinks", PanelPosition::kHidden, 0, 200, 0, false, false},
            {"graph", PanelPosition::kHidden, 0, 0, 200, false, false},
            {"search", PanelPosition::kHidden, 0, 250, 0, false, false},
            {"terminal", PanelPosition::kHidden, 0, 0, 200, false, false},
        };
        presets_.push_back(std::move(preset));
    }

    // Research: editor left, backlinks right, graph bottom
    {
        LayoutPreset preset;
        preset.name = "Research";
        preset.description = "Research layout with backlinks and graph";
        preset.is_builtin = true;
        preset.panels = {
            {"file_tree", PanelPosition::kLeftSidebar, 0, 250, 0, true, false},
            {"outline", PanelPosition::kRightSidebar, 1, 200, 0, false, false},
            {"backlinks", PanelPosition::kRightSidebar, 0, 250, 0, true, false},
            {"graph", PanelPosition::kBottomPanel, 0, 0, 250, true, false},
            {"search", PanelPosition::kLeftSidebar, 1, 250, 0, true, false},
            {"terminal", PanelPosition::kHidden, 0, 0, 200, false, false},
        };
        presets_.push_back(std::move(preset));
    }

    // Review: split editor with diff
    {
        LayoutPreset preset;
        preset.name = "Review";
        preset.description = "Code review layout with side-by-side view";
        preset.is_builtin = true;
        preset.panels = {
            {"file_tree", PanelPosition::kLeftSidebar, 0, 200, 0, true, false},
            {"outline", PanelPosition::kRightSidebar, 0, 200, 0, true, false},
            {"backlinks", PanelPosition::kHidden, 0, 200, 0, false, false},
            {"graph", PanelPosition::kHidden, 0, 0, 200, false, false},
            {"search", PanelPosition::kHidden, 0, 250, 0, false, false},
            {"terminal", PanelPosition::kBottomPanel, 0, 0, 200, true, false},
        };
        presets_.push_back(std::move(preset));
    }

    // Presentation: preview only, minimal UI
    {
        LayoutPreset preset;
        preset.name = "Presentation";
        preset.description = "Fullscreen preview for presentations";
        preset.is_builtin = true;
        preset.panels = {
            {"file_tree", PanelPosition::kHidden, 0, 250, 0, false, false},
            {"outline", PanelPosition::kHidden, 0, 200, 0, false, false},
            {"backlinks", PanelPosition::kHidden, 0, 200, 0, false, false},
            {"graph", PanelPosition::kHidden, 0, 0, 200, false, false},
            {"search", PanelPosition::kHidden, 0, 250, 0, false, false},
            {"terminal", PanelPosition::kHidden, 0, 0, 200, false, false},
        };
        presets_.push_back(std::move(preset));
    }
}

// ============================================================================
// Apply preset
// ============================================================================

auto WorkspaceLayoutManager::apply_preset(const std::string& preset_name) -> bool
{
    auto iter =
        std::find_if(presets_.begin(),
                     presets_.end(),
                     [&](const LayoutPreset& preset) { return preset.name == preset_name; });

    if (iter == presets_.end())
    {
        return false;
    }

    current_layout_ = iter->panels;

    // Publish event
    core::events::LayoutChangedEvent event;
    event.preset_name = preset_name;
    event_bus_.publish(event);

    return true;
}

// ============================================================================
// Save/delete presets
// ============================================================================

auto WorkspaceLayoutManager::save_preset(const std::string& name, const std::string& description)
    -> void
{
    // Check if preset already exists (overwrite if user-defined)
    auto iter = std::find_if(presets_.begin(),
                             presets_.end(),
                             [&](const LayoutPreset& preset) { return preset.name == name; });

    if (iter != presets_.end() && !iter->is_builtin)
    {
        iter->panels = current_layout_;
        iter->description = description;
        return;
    }

    LayoutPreset preset;
    preset.name = name;
    preset.description = description;
    preset.panels = current_layout_;
    preset.is_builtin = false;
    presets_.push_back(std::move(preset));
}

auto WorkspaceLayoutManager::delete_preset(const std::string& name) -> bool
{
    auto iter = std::find_if(presets_.begin(),
                             presets_.end(),
                             [&](const LayoutPreset& preset) { return preset.name == name; });

    if (iter == presets_.end() || iter->is_builtin)
    {
        return false; // Cannot delete built-in or nonexistent
    }

    presets_.erase(iter);
    return true;
}

// ============================================================================
// List presets
// ============================================================================

auto WorkspaceLayoutManager::list_presets() const -> std::vector<LayoutPreset>
{
    return presets_;
}

// ============================================================================
// Panel operations
// ============================================================================

auto WorkspaceLayoutManager::find_panel(const std::string& panel_id) -> PanelLayout*
{
    auto iter = std::find_if(current_layout_.begin(),
                             current_layout_.end(),
                             [&](const PanelLayout& panel) { return panel.panel_id == panel_id; });
    return (iter != current_layout_.end()) ? &(*iter) : nullptr;
}

auto WorkspaceLayoutManager::find_panel(const std::string& panel_id) const -> const PanelLayout*
{
    auto iter = std::find_if(current_layout_.begin(),
                             current_layout_.end(),
                             [&](const PanelLayout& panel) { return panel.panel_id == panel_id; });
    return (iter != current_layout_.end()) ? &(*iter) : nullptr;
}

auto WorkspaceLayoutManager::get_panel_layout(const std::string& panel_id) const
    -> std::optional<PanelLayout>
{
    const auto* panel = find_panel(panel_id);
    if (panel != nullptr)
    {
        return *panel;
    }
    return std::nullopt;
}

auto WorkspaceLayoutManager::move_panel(const std::string& panel_id,
                                        PanelPosition new_position,
                                        int order) -> void
{
    auto* panel = find_panel(panel_id);
    if (panel == nullptr)
    {
        return;
    }

    panel->position = new_position;
    if (order >= 0)
    {
        panel->order = order;
    }
    panel->visible = (new_position != PanelPosition::kHidden);
}

auto WorkspaceLayoutManager::set_panel_visible(const std::string& panel_id, bool visible) -> void
{
    auto* panel = find_panel(panel_id);
    if (panel == nullptr)
    {
        return;
    }

    panel->visible = visible;
    if (!visible)
    {
        panel->position = PanelPosition::kHidden;
    }
}

auto WorkspaceLayoutManager::toggle_panel(const std::string& panel_id) -> void
{
    auto* panel = find_panel(panel_id);
    if (panel == nullptr)
    {
        return;
    }

    panel->visible = !panel->visible;
    if (!panel->visible)
    {
        panel->position = PanelPosition::kHidden;
    }
}

auto WorkspaceLayoutManager::resize_panel(const std::string& panel_id, int width, int height)
    -> void
{
    auto* panel = find_panel(panel_id);
    if (panel == nullptr)
    {
        return;
    }

    panel->width = width;
    panel->height = height;
}

// ============================================================================
// Layout serialization
// ============================================================================

auto WorkspaceLayoutManager::save_layout() -> std::string
{
    std::ostringstream oss;
    oss << "{\"panels\":[";
    for (size_t idx = 0; idx < current_layout_.size(); ++idx)
    {
        const auto& panel = current_layout_[idx];
        if (idx > 0)
        {
            oss << ",";
        }
        oss << "{\"id\":\"" << panel.panel_id << "\""
            << ",\"position\":" << static_cast<int>(panel.position) << ",\"order\":" << panel.order
            << ",\"width\":" << panel.width << ",\"height\":" << panel.height
            << ",\"visible\":" << (panel.visible ? "true" : "false")
            << ",\"collapsed\":" << (panel.collapsed ? "true" : "false") << "}";
    }
    oss << "]}";
    return oss.str();
}

auto WorkspaceLayoutManager::restore_layout(const std::string& json) -> bool
{
    // Simple JSON parser for our known format
    if (json.empty() || json.find("\"panels\"") == std::string::npos)
    {
        return false;
    }

    // Find panels array content
    auto panels_start = json.find('[');
    auto panels_end = json.rfind(']');
    if (panels_start == std::string::npos || panels_end == std::string::npos)
    {
        return false;
    }

    std::vector<PanelLayout> restored;
    std::string panels_str = json.substr(panels_start + 1, panels_end - panels_start - 1);

    // Parse each panel object
    size_t pos = 0;
    while (pos < panels_str.size())
    {
        auto obj_start = panels_str.find('{', pos);
        auto obj_end = panels_str.find('}', pos);
        if (obj_start == std::string::npos || obj_end == std::string::npos)
        {
            break;
        }

        std::string obj = panels_str.substr(obj_start, obj_end - obj_start + 1);

        PanelLayout panel;

        // Extract id
        auto id_pos = obj.find("\"id\":\"");
        if (id_pos != std::string::npos)
        {
            auto id_start = id_pos + 6;
            auto id_end = obj.find('"', id_start);
            panel.panel_id = obj.substr(id_start, id_end - id_start);
        }

        // Extract position
        auto pos_pos = obj.find("\"position\":");
        if (pos_pos != std::string::npos)
        {
            auto val_start = pos_pos + 11;
            panel.position = static_cast<PanelPosition>(std::stoi(obj.substr(val_start)));
        }

        // Extract order
        auto order_pos = obj.find("\"order\":");
        if (order_pos != std::string::npos)
        {
            auto val_start = order_pos + 8;
            panel.order = std::stoi(obj.substr(val_start));
        }

        // Extract width
        auto width_pos = obj.find("\"width\":");
        if (width_pos != std::string::npos)
        {
            auto val_start = width_pos + 8;
            panel.width = std::stoi(obj.substr(val_start));
        }

        // Extract height
        auto height_pos = obj.find("\"height\":");
        if (height_pos != std::string::npos)
        {
            auto val_start = height_pos + 9;
            panel.height = std::stoi(obj.substr(val_start));
        }

        // Extract visible
        auto vis_pos = obj.find("\"visible\":");
        if (vis_pos != std::string::npos)
        {
            panel.visible = (obj.substr(vis_pos + 10, 4) == "true");
        }

        // Extract collapsed
        auto col_pos = obj.find("\"collapsed\":");
        if (col_pos != std::string::npos)
        {
            panel.collapsed = (obj.substr(col_pos + 12, 4) == "true");
        }

        if (!panel.panel_id.empty())
        {
            restored.push_back(std::move(panel));
        }

        pos = obj_end + 1;
    }

    if (restored.empty())
    {
        return false;
    }

    current_layout_ = std::move(restored);
    return true;
}

// ============================================================================
// Panel registration
// ============================================================================

auto WorkspaceLayoutManager::register_panel(const std::string& panel_id,
                                            const std::string& display_name) -> void
{
    // Avoid duplicates
    for (const auto& [existing_id, existing_name] : registered_panels_)
    {
        if (existing_id == panel_id)
        {
            return;
        }
    }
    registered_panels_.emplace_back(panel_id, display_name);
}

auto WorkspaceLayoutManager::registered_panels() const
    -> std::vector<std::pair<std::string, std::string>>
{
    return registered_panels_;
}

auto WorkspaceLayoutManager::current_layout() const -> const std::vector<PanelLayout>&
{
    return current_layout_;
}

} // namespace markamp::ui
