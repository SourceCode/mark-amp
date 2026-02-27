#pragma once

#include "DesignSystemContext.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "ui/ActivityBarModel.h"
#include "ui/animation/TransitionManager.h"

#include <wx/panel.h>
#include <wx/timer.h>

#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
}

namespace markamp::ui
{

/// Thin vertical icon rail on the left side of the window, inspired by VS Code's activity bar.
/// Shows icons for File Explorer, Search, Settings, and Themes.
/// Clicking an icon fires an ActivityBarSelectionEvent.
class ActivityBar : public wxPanel
{
public:
    ActivityBar(wxWindow* parent,
                DesignSystemContext& ds,
                core::EventBus& event_bus,
                core::Config* config = nullptr);

    /// Set the currently active item (visually highlights it)
    void SetActiveItem(core::events::ActivityBarItem item);

    /// Get the currently active item
    [[nodiscard]] auto GetActiveItem() const -> core::events::ActivityBarItem;

    /// R18 Fix 25: Set badge count on an activity bar item
    void SetBadge(core::events::ActivityBarItem item, int count);

    /// Set the currently active item in the secondary sidebar
    void SetSecondaryActiveItem(core::events::ActivityBarItem item);

    /// Get the currently active secondary item
    [[nodiscard]] auto GetSecondaryActiveItem() const -> core::events::ActivityBarItem;

    /// Phase 06 Task 6: Focus management
    void FocusItem(int index);

    void AddItem(const ActivityBarItemModel& item);
    void RemoveItem(const std::string& item_id);
    void SetItemVisible(const std::string& item_id, bool visible);

    DesignSystemContext& ds_;
    core::EventBus& event_bus_;
    core::Config* config_;

    ActivityBarModel model_;
    std::vector<wxRect> item_bounds_;
    std::unordered_map<std::string, float> badge_scales_;
    core::events::ActivityBarItem active_item_{core::events::ActivityBarItemId::kFileExplorer};
    core::events::ActivityBarItem secondary_active_item_;

    bool overflow_active_{false};
    wxRect overflow_button_bounds_;
    int last_top_item_index_{-1};
    int first_bottom_item_index_{-1};

    int hover_index_{-1};
    int pressed_index_{-1}; // R20 Fix 18: index of item being pressed
    int focus_index_{-1};   // Phase 06 Task 6: keyboard focus index

    animation::TransitionManager transition_manager_{this};

    // Phase 06 Task 12: Drag reorder
    int drag_index_{-1};
    int drag_target_index_{-1};
    bool is_dragging_{false};
    wxPoint drag_start_pos_;
    wxPoint drag_current_pos_;

    wxTimer tooltip_timer_;
    bool tooltip_visible_{false};

    core::Subscription theme_sub_;
    core::Subscription search_count_sub_;      // Phase 06 Task 7
    core::Subscription diagnostics_sub_;       // Phase 06 Task 7
    core::Subscription extension_updates_sub_; // Phase 06 Task 7
    core::Subscription keyboard_mode_sub_;     // Phase 05 Task 3
    core::Subscription custom_panel_sub_;      // Phase 07 Task 18
    core::Subscription custom_panel_unsub_;    // Phase 07 Task 18
    core::Subscription git_status_sub_;        // Phase 18 Task 16

    core::Subscription density_sub_;

    // Phase 07 Task 20: Config Persistence
    void SaveLayoutToConfig();
    void LoadLayoutFromConfig();

    void CreateItems();
    void ApplyTheme();
    void UpdateLayoutMetrics();
    void UpdateItemBounds();
    void AnnounceCurrentItem(); // Helper to announce selection

    void OnSize(wxSizeEvent& event);

    void OnPaint(wxPaintEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);     // R20 Fix 18: release press
    void OnDoubleClick(wxMouseEvent& event); // R20 Fix 16: collapse sidebar
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);     // Phase 06 Task 6: keyboard nav
    void OnSetFocus(wxFocusEvent& event);  // Phase 06 Task 6: focus ring
    void OnKillFocus(wxFocusEvent& event); // Phase 06 Task 6: focus ring

    /// Phase 06 Task 13: Right-click context menu
    void OnRightClick(wxMouseEvent& event);

    /// Phase 06 Task 12: Reorder items via drag
    void FinishDrag();

    auto HitTest(const wxPoint& pos) const -> int;
};

} // namespace markamp::ui
