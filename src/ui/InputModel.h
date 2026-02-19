#pragma once

#include <cstdint>
#include <functional>
#include <string>

namespace markamp::ui
{

/// Canonical input gesture types recognized across all control surfaces.
enum class InputGesture : uint8_t
{
    kClick,
    kDoubleClick,
    kRightClick,
    kCtrlClick,
    kShiftClick,
    kScrollVertical,
    kScrollHorizontal,
    kCtrlScroll,
    kShiftScroll,
    kKeyUp,
    kKeyDown,
    kKeyLeft,
    kKeyRight,
    kKeyEnter,
    kKeySpace,
    kKeyEscape,
    kKeyTab,
    kKeyHome,
    kKeyEnd,
    kKeyDelete,
    kContextMenu, ///< Right-click or Menu key
};

/// Scroll normalization parameters.
struct ScrollConfig
{
    int lines_per_notch{3};       ///< Lines scrolled per mouse wheel notch
    bool smooth_scrolling{false}; ///< Enable pixel-level smooth scrolling
    bool respect_inertia{true};   ///< Respect platform-native inertia
    float horizontal_speed{1.0F}; ///< Horizontal scroll speed multiplier
};

/// Double-click safety rules.
struct DoubleClickPolicy
{
    int timing_ms{500};           ///< Max ms between clicks to count as double
    bool guard_destructive{true}; ///< Prevent double-click on close/delete buttons
};

/// Context invocation policy — ensures keyboard Menu key and right-click
/// produce equivalent context menus.
struct ContextMenuPolicy
{
    bool keyboard_menu_enabled{true}; ///< Menu/Shift+F10 triggers context menu
    bool right_click_enabled{true};   ///< Right-click triggers context menu
    bool show_at_focus{true};         ///< Keyboard invocation shows at focused item
};

/// Centralized input model configuration.
/// Used by individual controls to query canonical gesture behavior.
class InputModel
{
public:
    static auto get() -> InputModel&;

    [[nodiscard]] auto scroll_config() const -> const ScrollConfig&
    {
        return scroll_config_;
    }
    void set_scroll_config(const ScrollConfig& config)
    {
        scroll_config_ = config;
    }

    [[nodiscard]] auto double_click_policy() const -> const DoubleClickPolicy&
    {
        return double_click_policy_;
    }
    void set_double_click_policy(const DoubleClickPolicy& policy)
    {
        double_click_policy_ = policy;
    }

    [[nodiscard]] auto context_menu_policy() const -> const ContextMenuPolicy&
    {
        return context_menu_policy_;
    }
    void set_context_menu_policy(const ContextMenuPolicy& policy)
    {
        context_menu_policy_ = policy;
    }

private:
    InputModel() = default;
    ScrollConfig scroll_config_;
    DoubleClickPolicy double_click_policy_;
    ContextMenuPolicy context_menu_policy_;
};

} // namespace markamp::ui
