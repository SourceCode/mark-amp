#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <wx/timer.h>

#include <string>
#include <vector>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

class BevelPanel;
class EditorPanel;
class PreviewPanel;

/// Snap presets for split ratio (double-click divider to cycle).
enum class SnapPreset
{
    Even,       // 50/50
    EditorWide, // 70/30
    PreviewWide // 30/70
};

/// Manages the three view modes: Editor, Split, Preview.
/// In Split mode, shows editor and preview side-by-side with a
/// draggable divider. Preserves scroll/cursor state across mode switches.
/// Features: snap presets, animated transitions, focus mode, scroll sync.
class SplitView : public ThemeAwareWindow
{
public:
    SplitView(wxWindow* parent,
              core::ThemeEngine& theme_engine,
              core::EventBus& event_bus,
              core::Config* config);

    // View mode control
    void SetViewMode(core::events::ViewMode mode);
    [[nodiscard]] auto GetViewMode() const -> core::events::ViewMode;

    // Access child panels
    [[nodiscard]] auto GetEditorPanel() -> EditorPanel*;
    [[nodiscard]] auto GetPreviewPanel() -> PreviewPanel*;

    // Split ratio (0.0 = all editor, 1.0 = all preview, 0.5 = even split)
    void SetSplitRatio(double ratio);
    [[nodiscard]] auto GetSplitRatio() const -> double;

    // Snap presets (double-click divider to cycle)
    void CycleSnapPreset();
    [[nodiscard]] auto GetCurrentSnap() const -> SnapPreset;

    // Focus mode (editor-only center column, max-width 80ch)
    void ToggleFocusMode();
    [[nodiscard]] auto IsFocusMode() const -> bool;

    // Scroll sync
    void SetScrollSyncMode(core::events::ScrollSyncMode mode);
    [[nodiscard]] auto GetScrollSyncMode() const -> core::events::ScrollSyncMode;

    // File operations
    void SaveFile(const std::string& path);

    // Divider constants
    static constexpr int kDividerWidth = 6;     // visual width
    static constexpr int kDividerHitWidth = 12; // wider hit area for grabbing
    static constexpr double kMinSplitRatio = 0.2;
    static constexpr double kMaxSplitRatio = 0.8;
    static constexpr double kDefaultSplitRatio = 0.5;

    // Focus mode constants
    static constexpr int kFocusMaxChars = 80;
    static constexpr int kFocusPaddingH = 48;

    // Animation constants
    static constexpr int kAnimFrameMs = 16; // ~60fps
    static constexpr double kTransitionDurationMs = 200.0;

protected:
    void OnThemeChanged(const core::Theme& new_theme) override;

private:
    core::EventBus& event_bus_;
    core::Config* config_;

    // Child panels
    EditorPanel* editor_panel_{nullptr};
    PreviewPanel* preview_panel_{nullptr};
    wxPanel* divider_panel_{nullptr};
    BevelPanel* editor_bevel_{nullptr};

    // State
    core::events::ViewMode current_mode_{core::events::ViewMode::Split};
    double split_ratio_{kDefaultSplitRatio};

    // Snap presets
    SnapPreset current_snap_{SnapPreset::Even};

    // Focus mode
    bool focus_mode_{false};

    // Scroll sync
    core::events::ScrollSyncMode scroll_sync_mode_{core::events::ScrollSyncMode::Proportional};
    std::vector<int> heading_positions_; // editor line numbers of headings

    // Divider dragging
    bool is_dragging_{false};
    int drag_start_x_{0};
    double drag_start_ratio_{0.0};

    // Divider hover
    bool divider_hovered_{false};

    void OnDividerPaint(wxPaintEvent& event);
    void OnDividerMouseDown(wxMouseEvent& event);
    void OnDividerMouseMove(wxMouseEvent& event);
    void OnDividerMouseUp(wxMouseEvent& event);
    void OnDividerMouseEnter(wxMouseEvent& event);
    void OnDividerMouseLeave(wxMouseEvent& event);
    void OnDividerDoubleClick(wxMouseEvent& event);

    // Animated transitions
    wxTimer transition_timer_;
    double transition_progress_{1.0}; // 1.0 = complete
    double transition_start_ratio_{0.5};
    double transition_target_ratio_{0.5};
    bool transition_show_editor_{true};
    bool transition_show_preview_{true};
    core::events::ViewMode transition_target_mode_{core::events::ViewMode::Split};

    void OnTransitionTimer(wxTimerEvent& event);
    void StartTransition(core::events::ViewMode target_mode);

    // Layout
    void UpdateLayout();
    void UpdateFocusLayout();
    void OnSize(wxSizeEvent& event);

    // State preservation
    struct EditorState
    {
        int cursor_line{0};
        int cursor_col{0};
        std::string content;
    };

    auto SaveEditorState() -> EditorState;
    void RestoreEditorState(const EditorState& state);

    // Heading index for scroll sync
    void RebuildHeadingIndex(const std::string& content);
    auto FindNearestHeading(int editor_line) const -> int;

    // Event subscriptions
    core::Subscription view_mode_sub_;
    core::Subscription content_sub_;
    core::Subscription scroll_sync_sub_;
    core::Subscription focus_mode_sub_;

    // Persistence
    void SaveSplitRatio();
    void RestoreSplitRatio();

    // Easing
    static auto EaseOutCubic(double progress) -> double;
};

} // namespace markamp::ui
