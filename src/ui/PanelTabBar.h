/**
 * @file PanelTabBar.h
 * @brief Custom tab bar for the bottom panel area (Phase 10).
 */

#pragma once

#include "DesignSystemContext.h"
#include "PanelAreaModel.h"
#include "ThemeAwareWindow.h"
#include "animation/TransitionManager.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <string>
#include <vector>

namespace markamp::ui
{

/**
 * @class PanelTabBar
 * @brief Renders the tabs and controls for the bottom panel area.
 * Supports icons, badges, close buttons, drag reorder, and maximize toggle.
 */
class PanelTabBar : public ThemeAwareWindow
{
public:
    PanelTabBar(wxWindow* parent,
                DesignSystemContext& ds,
                core::EventBus& event_bus,
                PanelAreaModel& model);
    ~PanelTabBar() override;

    PanelTabBar(const PanelTabBar&) = delete;
    auto operator=(const PanelTabBar&) -> PanelTabBar& = delete;
    PanelTabBar(PanelTabBar&&) = delete;
    auto operator=(PanelTabBar&&) -> PanelTabBar& = delete;

    // Queries
    [[nodiscard]] auto GetActivePanelId() const -> std::string;
    [[nodiscard]] auto GetTabCount() const -> int;

    // Keyboard and Focus
    [[nodiscard]] bool AcceptsFocus() const override
    {
        return true;
    }

    // Layout
    void UpdateLayoutMetrics();

    // Constant Dimensions
    static constexpr int kTabHeight = 30; // From PRD
    static constexpr int kMaxTabWidth = 150;
    static constexpr int kMinTabWidth = 60;
    static constexpr int kCloseButtonMargin = 6;
    static constexpr int kIconSize = 14;
    static constexpr int kBadgeSize = 16;
    static constexpr float kCloseHoverScale = 1.2F;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    struct PanelTabRenderInfo
    {
        std::string id;
        std::string title;
        std::string icon_name;
        core::events::BadgeState badge{core::events::BadgeState::kNone};
        int badge_count{0};
        bool closable{true};
        bool is_active{false};
        bool close_hovered{false};
        int order{0};
        bool is_visible{true};

        wxRect rect;
        wxRect close_rect;
    };

    core::EventBus& event_bus_;
    PanelAreaModel& model_;

    std::vector<PanelTabRenderInfo> tabs_;

    // Interaction state
    int hovered_tab_index_{-1};
    bool maximize_hovered_{false};
    wxRect maximize_button_rect_;
    bool is_maximized_{false}; // synced via zone manager eventually

    // Drag reorder state
    int drag_start_x_{0};
    int drag_tab_index_{-1};
    bool is_dragging_{false};

    // Overflow state
    bool has_overflow_{false};
    bool overflow_hovered_{false};
    wxRect overflow_button_rect_;

    // Subscriptions
    core::Subscription model_sub_;
    core::Subscription badge_sub_;

    // Focus state
    int keyboard_focused_tab_index_{-1};
    bool has_focus_{false};

    // Animation
    animation::TransitionManager transition_manager_{this};

    // Painting
    void OnPaint(wxPaintEvent& event);
    void DrawTab(wxGraphicsContext& gc, const PanelTabRenderInfo& tab) const;
    void DrawMaximizeButton(wxGraphicsContext& gc) const;
    void DrawOverflowButton(wxGraphicsContext& gc) const;
    void DrawBadge(wxGraphicsContext& gc, const PanelTabRenderInfo& tab) const;

    // Mouse events
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);

    // Keyboard events
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Hit testing
    [[nodiscard]] auto HitTestTab(const wxPoint& point) const -> int;
    [[nodiscard]] auto HitTestCloseButton(const wxPoint& point, int tab_index) const -> bool;
    [[nodiscard]] auto HitTestMaximizeButton(const wxPoint& point) const -> bool;
    [[nodiscard]] auto HitTestOverflowButton(const wxPoint& point) const -> bool;

    // State sync
    void SyncWithModel();
    void RecalculateTabRects();
    void OnSize(wxSizeEvent& event);
};

} // namespace markamp::ui
