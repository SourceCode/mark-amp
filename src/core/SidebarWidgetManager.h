/// @file SidebarWidgetManager.h
/// @brief V9 Phase 41 — Sidebar widget lifecycle, registration, and layout management.
#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace markamp::core
{

/// Position slots for sidebar widgets.
enum class SidebarSlot : uint8_t
{
    kTop = 0,       ///< Top of sidebar
    kPrimary = 1,   ///< Main content area
    kSecondary = 2, ///< Secondary area
    kBottom = 3,    ///< Bottom of sidebar
};

/// Widget display mode.
enum class WidgetDisplayMode : uint8_t
{
    kExpanded = 0,  ///< Fully expanded
    kCollapsed = 1, ///< Collapsed to header only
    kMinimized = 2, ///< Icon only
    kHidden = 3,    ///< Not visible
};

/// Convert slot to display name.
[[nodiscard]] auto sidebar_slot_name(SidebarSlot slot) -> std::string;

/// A registered sidebar widget.
struct SidebarWidget
{
    std::string widget_id;    ///< Unique identifier
    std::string title;        ///< Display title
    std::string icon;         ///< Icon name
    std::string extension_id; ///< Owning extension (empty = built-in)
    SidebarSlot slot{SidebarSlot::kPrimary};
    WidgetDisplayMode display_mode{WidgetDisplayMode::kExpanded};
    int sort_order{0};         ///< Order within slot (lower = first)
    int min_height{50};        ///< Minimum height in pixels
    int preferred_height{200}; ///< Preferred height in pixels
    bool resizable{true};
    bool closable{true};
};

/// Manages sidebar widget lifecycle and layout.
///
/// Handles widget registration, slot assignment, ordering, and
/// display mode transitions for the sidebar panel system.
class SidebarWidgetManager
{
public:
    SidebarWidgetManager() = default;

    // ── Registration ──────────────────────────────────────────────────
    void register_widget(SidebarWidget widget);
    auto unregister_widget(const std::string& widget_id) -> bool;
    [[nodiscard]] auto find_widget(const std::string& widget_id) const -> const SidebarWidget*;
    [[nodiscard]] auto widget_count() const -> int;

    // ── Layout ────────────────────────────────────────────────────────
    [[nodiscard]] auto widgets_in_slot(SidebarSlot slot) const -> std::vector<const SidebarWidget*>;
    auto move_to_slot(const std::string& widget_id, SidebarSlot slot) -> bool;
    auto set_sort_order(const std::string& widget_id, int order) -> bool;

    // ── Display ───────────────────────────────────────────────────────
    auto set_display_mode(const std::string& widget_id, WidgetDisplayMode mode) -> bool;
    auto toggle_collapsed(const std::string& widget_id) -> bool;
    auto show_widget(const std::string& widget_id) -> bool;
    auto hide_widget(const std::string& widget_id) -> bool;

    // ── Queries ───────────────────────────────────────────────────────
    [[nodiscard]] auto all_widgets() const -> std::vector<const SidebarWidget*>;
    [[nodiscard]] auto visible_widgets() const -> std::vector<const SidebarWidget*>;
    [[nodiscard]] auto active_slots() const -> std::vector<SidebarSlot>;
    [[nodiscard]] auto widgets_by_extension(const std::string& extension_id) const
        -> std::vector<const SidebarWidget*>;

    void clear_all();

private:
    std::vector<SidebarWidget> widgets_;

    auto find_mut(const std::string& widget_id) -> SidebarWidget*;
};

} // namespace markamp::core
