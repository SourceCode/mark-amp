/// @file CustomPanelRegistry.h
/// @brief V9 Phase 41 — Registry for custom sidebar panels contributed by extensions.
#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::core
{

/// Panel contribution point — where extensions can inject panels.
enum class PanelLocation : uint8_t
{
    kLeftSidebar = 0,  ///< Left sidebar
    kRightSidebar = 1, ///< Right sidebar
    kBottomPanel = 2,  ///< Bottom panel
    kFloating = 3,     ///< Floating window
};

/// A custom panel registered by an extension.
struct CustomPanel
{
    std::string panel_id;     ///< Unique identifier
    std::string title;        ///< Display title
    std::string icon;         ///< Icon name
    std::string extension_id; ///< Contributing extension
    PanelLocation location{PanelLocation::kLeftSidebar};
    int priority{0}; ///< Display priority
    bool visible{true};
    bool pinned{false};    ///< Pinned to always show
    std::string view_type; ///< "webview", "tree", "list", "custom"
};

/// Registry for custom sidebar panels from extensions.
///
/// Extensions register panels that appear in sidebars and panel areas.
/// Supports location management, visibility control, and pinning.
class CustomPanelRegistry
{
public:
    CustomPanelRegistry() = default;

    // ── Registration ──────────────────────────────────────────────────
    void register_panel(CustomPanel panel);
    auto unregister_panel(const std::string& panel_id) -> bool;
    [[nodiscard]] auto find_panel(const std::string& panel_id) const -> const CustomPanel*;
    [[nodiscard]] auto panel_count() const -> int;

    // ── Location ──────────────────────────────────────────────────────
    [[nodiscard]] auto panels_at_location(PanelLocation location) const
        -> std::vector<const CustomPanel*>;
    auto move_panel(const std::string& panel_id, PanelLocation location) -> bool;

    // ── Visibility ────────────────────────────────────────────────────
    auto show_panel(const std::string& panel_id) -> bool;
    auto hide_panel(const std::string& panel_id) -> bool;
    auto pin_panel(const std::string& panel_id) -> bool;
    auto unpin_panel(const std::string& panel_id) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_panels() const -> std::vector<const CustomPanel*>;
    [[nodiscard]] auto visible_panels() const -> std::vector<const CustomPanel*>;
    [[nodiscard]] auto pinned_panels() const -> std::vector<const CustomPanel*>;
    [[nodiscard]] auto panels_by_extension(const std::string& extension_id) const
        -> std::vector<const CustomPanel*>;

    void clear_all();

private:
    std::vector<CustomPanel> panels_;

    auto find_mut(const std::string& panel_id) -> CustomPanel*;
};

} // namespace markamp::core
