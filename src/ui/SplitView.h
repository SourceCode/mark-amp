#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"

#include <wx/timer.h>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::ui
{

class BevelPanel;
class EditorPanel;
class PreviewPanel;

/// Manages the three view modes: Editor, Split, Preview.
/// In Split mode, shows editor and preview side-by-side with a
/// draggable divider. Preserves scroll/cursor state across mode switches.
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

    // File operations
    void SaveFile(const std::string& path);

    static constexpr int kDividerWidth = 4;
    static constexpr int kDividerHitWidth = 8; // wider hit area for grabbing
    static constexpr double kMinSplitRatio = 0.2;
    static constexpr double kMaxSplitRatio = 0.8;
    static constexpr double kDefaultSplitRatio = 0.5;

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

    // Divider dragging
    bool is_dragging_{false};
    int drag_start_x_{0};
    double drag_start_ratio_{0.0};

    void OnDividerMouseDown(wxMouseEvent& event);
    void OnDividerMouseMove(wxMouseEvent& event);
    void OnDividerMouseUp(wxMouseEvent& event);
    void OnDividerMouseEnter(wxMouseEvent& event);
    void OnDividerMouseLeave(wxMouseEvent& event);

    // Layout
    void UpdateLayout();
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

    // Event subscriptions
    core::Subscription view_mode_sub_;

    // Persistence
    void SaveSplitRatio();
    void RestoreSplitRatio();
};

} // namespace markamp::ui
