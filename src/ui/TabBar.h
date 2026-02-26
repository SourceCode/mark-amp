#pragma once

#include "DesignSystemContext.h"
#include "ThemeAwareWindow.h"
#include "animation/TransitionManager.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/ThemeEngine.h"

#include <wx/timer.h>

#include <functional>
#include <string>
#include <vector>

namespace markamp::ui
{

/// Horizontally scrollable tab bar for multi-file editing.
/// Renders themed tabs with active indicator, modified dot (●),
/// hoverable close (×) buttons, and right-click context menu.
/// Height: 32px, integrates between Toolbar and SplitView.
class TabBar : public ThemeAwareWindow
{
    friend class TabDropTarget;

public:
    TabBar(wxWindow* parent, DesignSystemContext& ds, core::EventBus& event_bus);

    enum class TabSizeMode
    {
        kShrink,
        kFixedWidth
    };

    void SetTabSizeMode(TabSizeMode mode);
    [[nodiscard]] auto GetTabSizeMode() const -> TabSizeMode
    {
        return size_mode_;
    }

    // Tab management
    void
    AddTab(const std::string& file_path, const std::string& display_name, bool is_preview = false);
    void RemoveTab(const std::string& file_path);
    void SetActiveTab(const std::string& file_path);
    void SetTabModified(const std::string& file_path, bool modified);
    void RenameTab(const std::string& old_path,
                   const std::string& new_path,
                   const std::string& new_display_name);

    // Queries
    [[nodiscard]] auto GetActiveTabPath() const -> std::string;
    [[nodiscard]] auto GetTabCount() const -> int;
    [[nodiscard]] auto HasTab(const std::string& file_path) const -> bool;
    [[nodiscard]] auto IsTabModified(const std::string& file_path) const -> bool;
    [[nodiscard]] auto GetAllTabPaths() const -> std::vector<std::string>;

    // Tab cycling
    void ActivateNextTab();
    void ActivatePreviousTab();

    // Batch operations
    void CloseAllTabs();
    void CloseSavedTabs(); // R4 Fix 8
    void CloseOtherTabs(const std::string& keep_path);
    void CloseTabsToLeft(const std::string& of_path);
    void CloseTabsToRight(const std::string& of_path);
    void DuplicateTab(const std::string& file_path); // R19 Fix 4

    // R3 Fix 7: Pinned tab management
    void PinTab(const std::string& file_path);
    void UnpinTab(const std::string& file_path);

    // Workspace root for relative path calculation
    void SetWorkspaceRoot(const std::string& root_path)
    {
        workspace_root_ = root_path;
    }

    void UpdateLayoutMetrics();

    // Constants
    static constexpr int kMaxTabWidth = 200;
    static constexpr int kMinTabWidth = 100;
    static constexpr int kFixedWidth = 160; // Phase 11 Task 5
    static constexpr int kCloseButtonMargin = 6;
    static constexpr int kPinnedStripeWidth = 2;    // R19 Fix 2
    static constexpr float kWidthAnimSpeed = 0.15F; // R19 Fix 1: interpolation factor
    static constexpr int kModifiedDotSize = 6;
    static constexpr float kCloseHoverScale = 1.2F;  // R20 Fix 1: close × scale on hover
    static constexpr int kGlowLineHeight = 2;        // R20 Fix 3: active tab glow
    static constexpr int kOverflowChevronWidth = 20; // R20 Fix 4: overflow chevron width
    static constexpr int kGroupColorCount = 6;       // R20 Fix 5: distinct group hues

    /// Tab data visible for testing.
    struct TabInfo
    {
        std::string file_path;
        std::string display_name;
        bool is_modified{false};
        bool is_active{false};
        bool close_hovered{false};
        bool is_pinned{false};  // R3 Fix 7: Pinned tabs
        bool is_preview{false}; // Phase 11 Task 3: Preview tab mode
        bool is_saving{false};  // Phase 11 Task 12: Saving spinner
        bool is_closing{false}; // Phase 11 Task 17: Close animation
        std::string group_id;   // Phase 11 Task 7: Tab Groups
        wxColour group_color;   // Phase 11 Task 7: Tab Groups
        float opacity{1.0F};    // R18 Fix 1: Fade-in animation opacity
        int target_width{0};    // R19 Fix 1: animated target width
        int anim_width{0};      // R19 Fix 1: current interpolated width
        wxRect rect;
        wxRect close_rect;
    };

    [[nodiscard]] auto tabs() const -> const std::vector<TabInfo>&
    {
        return tabs_;
    }

    void set_group_focused(bool focused)
    {
        is_group_focused_ = focused;
        Refresh();
    }

    [[nodiscard]] auto GetEventBus() -> core::EventBus&
    {
        return event_bus_;
    }

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    DesignSystemContext& ds_;
    core::EventBus& event_bus_;

    // Tab state
    std::vector<TabInfo> tabs_;
    int scroll_offset_{0};
    std::string workspace_root_;

    // Interaction state
    int hovered_tab_index_{-1};
    bool new_file_hovered_{false};     // Phase 11 Task 15
    bool split_right_hovered_{false};  // Phase 12 Task 8
    bool more_actions_hovered_{false}; // Phase 12 Task 8
    bool is_group_focused_{false};     // Phase 12 Task 7
    TabSizeMode size_mode_{TabSizeMode::kShrink};

    core::Subscription keyboard_mode_sub_; // Phase 05 Task 3
    core::Subscription tab_save_req_sub_;  // Phase 11 Task 12
    core::Subscription file_saved_sub_;    // Phase 11 Task 12

    // R18 Fix 1: Fade-in transition manager
    animation::TransitionManager transition_manager_{this};

    // R18 Fix 4: Parent folder disambiguation for duplicate display names
    [[nodiscard]] auto GetDisambiguationSuffix(const TabInfo& tab) const -> std::string;

    // Phase 11 Task 7: Assign groups based on directory and assign a static color
    void UpdateTabGroups();

    // R3 Fix 5: Drag reorder state
    int drag_start_x_{0};
    int drag_start_y_{0};
    int drag_tab_index_{-1};
    bool is_dragging_{false};

    // Phase 11 Task 11: Pulsing modified dot
    wxTimer pulse_timer_;
    void OnPulseTimer(wxTimerEvent& event);
    void UpdatePulseTimer();

    // Painting
    void OnPaint(wxPaintEvent& event);
    void DrawTab(wxGraphicsContext& gc, const TabInfo& tab, const core::Theme& theme) const;

    // Mouse interaction
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event); // R3 Fix 5
    void OnDoubleClick(wxMouseEvent& event);
    void OnMouseLeave(wxMouseEvent& event);
    void OnRightDown(wxMouseEvent& event);
    void OnMiddleDown(wxMouseEvent& event);
    void OnMouseWheel(wxMouseEvent& event);

    // Keyboard & Focus interaction
    int focused_tab_index_{-1};
    void OnSetFocus(wxFocusEvent& event);
    void OnKillFocus(wxFocusEvent& event);
    void OnKeyDown(wxKeyEvent& event);

    // Context menu
    void ShowTabContextMenu(int tab_index);
    void ShowOverflowMenu(); // Phase 11 Task 9

    // Hit testing
    [[nodiscard]] auto HitTestTab(const wxPoint& point) const -> int;
    [[nodiscard]] auto HitTestCloseButton(const wxPoint& point, int tab_index) const -> bool;
    [[nodiscard]] auto HitTestOverflowChevron(const wxPoint& point) const
        -> bool;                                                                 // Phase 11 Task 9
    [[nodiscard]] auto HitTestNewFileButton(const wxPoint& point) const -> bool; // Phase 11 Task 15
    [[nodiscard]] auto HitTestSplitRightButton(const wxPoint& point) const
        -> bool; // Phase 12 Task 8
    [[nodiscard]] auto HitTestMoreActionsButton(const wxPoint& point) const
        -> bool; // Phase 12 Task 8

    // Layout
    void RecalculateTabRects();
    void OnSize(wxSizeEvent& event);
    void EnsureTabVisible(int tab_index);

    // Helpers
    [[nodiscard]] auto FindTabIndex(const std::string& file_path) const -> int;

    // R20 Fix 5: Get a tint color for a directory-based group
    [[nodiscard]] auto GetGroupColorTint(const std::string& file_path) const -> wxColour;
};

} // namespace markamp::ui
