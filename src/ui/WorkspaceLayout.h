/// @file WorkspaceLayout.h
/// @brief V4 Phase 21 – Customizable UI Layout System.

#pragma once

#include <optional>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

namespace markamp::core
{
class EventBus;
class Config;
class ThemeEngine;
} // namespace markamp::core

namespace markamp::ui
{

// ============================================================================
// Panel position enum
// ============================================================================

enum class PanelPosition : uint8_t
{
    kLeftSidebar,
    kRightSidebar,
    kBottomPanel,
    kFloatingWindow,
    kHidden
};

// ============================================================================
// PanelLayout: describes one panel's placement and size
// ============================================================================

struct PanelLayout
{
    std::string panel_id;
    PanelPosition position{PanelPosition::kLeftSidebar};
    int order{0};
    int width{250};
    int height{200};
    bool visible{true};
    bool collapsed{false};
};

// ============================================================================
// LayoutPreset: named collection of panel layouts
// ============================================================================

struct LayoutPreset
{
    std::string name;
    std::string description;
    std::vector<PanelLayout> panels;
    std::string pane_layout_json;
    bool is_builtin{false};
};

// ============================================================================
// WorkspaceLayoutManager
// ============================================================================

class WorkspaceLayoutManager
{
public:
    WorkspaceLayoutManager(markamp::core::EventBus& event_bus,
                           markamp::core::Config& config,
                           markamp::core::ThemeEngine& theme_engine);

    /// Apply a named layout preset.
    auto apply_preset(const std::string& preset_name) -> bool;

    /// Save the current layout as a named preset.
    auto save_preset(const std::string& name, const std::string& description = "") -> void;

    /// Delete a user-defined preset (built-in presets cannot be deleted).
    auto delete_preset(const std::string& name) -> bool;

    /// List all available presets (built-in + user).
    [[nodiscard]] auto list_presets() const -> std::vector<LayoutPreset>;

    /// Get a specific panel's layout.
    [[nodiscard]] auto get_panel_layout(const std::string& panel_id) const
        -> std::optional<PanelLayout>;

    /// Move a panel to a new position.
    auto move_panel(const std::string& panel_id, PanelPosition new_position, int order = -1)
        -> void;

    /// Show or hide a panel.
    auto set_panel_visible(const std::string& panel_id, bool visible) -> void;

    /// Toggle a panel's visibility.
    auto toggle_panel(const std::string& panel_id) -> void;

    /// Resize a panel.
    auto resize_panel(const std::string& panel_id, int width, int height) -> void;

    /// Persist current layout to config.
    auto save_layout() -> std::string;

    /// Restore layout from serialized JSON.
    auto restore_layout(const std::string& json) -> bool;

    /// Register a new panel type.
    auto register_panel(const std::string& panel_id, const std::string& display_name) -> void;

    /// Get all registered panel IDs.
    [[nodiscard]] auto registered_panels() const
        -> std::vector<std::pair<std::string, std::string>>;

    /// Access current panels.
    [[nodiscard]] auto current_layout() const -> const std::vector<PanelLayout>&;

private:
    markamp::core::EventBus& event_bus_;
    markamp::core::Config& config_;
    markamp::core::ThemeEngine& theme_engine_;

    std::vector<PanelLayout> current_layout_;
    std::vector<LayoutPreset> presets_;
    std::vector<std::pair<std::string, std::string>> registered_panels_;

    auto load_builtin_presets() -> void;
    auto find_panel(const std::string& panel_id) -> PanelLayout*;
    [[nodiscard]] auto find_panel(const std::string& panel_id) const -> const PanelLayout*;
};

} // namespace markamp::ui
