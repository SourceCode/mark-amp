#pragma once

#include "ThemeAwareWindow.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/SurfaceLink.h"

#include <wx/timer.h>

#include <array>
#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{
class Config;
class FeatureRegistry;
class IMermaidRenderer;
class IMathRenderer;
} // namespace markamp::core

namespace markamp::ui
{

class BevelPanel;
class EditorPanel;
class PreviewPanel;

/// Snap presets for split ratio (double-click divider to cycle).
enum class SnapPreset
{
    Even,        // 50/50
    EditorWide,  // 70/30
    PreviewWide, // 30/70

    // V8 Phase 9: Named workspace presets with animated transitions
    EditorFocus, // 85/15 — minimal preview, keyboard-centric editing
    Balanced,    // 50/50 — alias for Even (canonical preset name)
    Review,      // 40/60 — emphasis on rendered preview
    PreviewFocus // 15/85 — full preview, editor as sidebar
};

// V8 Phase 12 (Phase 37): Paired traverse surface combinations
enum class PairMode : std::uint8_t
{
    kNone,          ///< No pairing
    kEditorPreview, ///< Editor + Preview (default split)
    kEditorCanvas,  ///< Editor + Canvas
    kPreviewGraph,  ///< Preview + Graph
    kEditorGraph,   ///< Editor + Graph
    kEditorNotebook ///< Editor + Notebook
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
              core::Config* config,
              core::IMermaidRenderer* mermaid_renderer = nullptr,
              core::IMathRenderer* math_renderer = nullptr);

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

    // Phase 09 Task 3: Sync accuracy indicator
    enum class SyncHealth : std::uint8_t
    {
        kSynced,    // green — preview up to date
        kRendering, // yellow — re-rendering in progress
        kOutOfSync  // red — pending content not yet rendered
    };
    [[nodiscard]] auto GetSyncHealth() const -> SyncHealth;

    // Phase 09 Task 4: Breadcrumbs query
    [[nodiscard]] auto GetCurrentBreadcrumb() const -> std::string;

    // File operations
    void SaveFile(const std::string& path);

    /// Inject FeatureRegistry for feature-guard checks (forwards to EditorPanel).
    void set_feature_registry(core::FeatureRegistry* registry);

    /// Enable or disable Mermaid rendering (forwards to PreviewPanel).
    void set_mermaid_enabled(bool enabled);

    // V8 Phase 12 (Phase 37): Paired traverse mode
    void SetPairMode(PairMode mode);
    [[nodiscard]] auto GetPairMode() const -> PairMode;
    [[nodiscard]] auto IsPaired() const -> bool;

    // Phase 09 Task 6: Split direction
    void SetSplitDirection(core::events::SplitDirection direction);
    [[nodiscard]] auto GetSplitDirection() const -> core::events::SplitDirection;
    void ToggleSplitDirection();

    // Phase 09 Task 8: Pin preview
    void SetPinPreview(bool pinned);
    [[nodiscard]] auto IsPinPreview() const -> bool;
    void TogglePinPreview();

    // Phase 09 Task 9: Open in side
    void OpenInSide(const std::string& file_path);

    // Phase 09 Task 10: Per-file state
    void SavePerFileState(const std::string& file_path);
    void RestorePerFileState(const std::string& file_path);

    // Phase 09 Task 11: Typewriter scroll mode
    void SetTypewriterMode(bool enabled);
    [[nodiscard]] auto IsTypewriterMode() const -> bool;
    void ToggleTypewriterMode();

    // Phase 09 Task 13: Export rendered HTML
    void ExportHtml();

    // Phase 09 Task 14: Reveal commands
    void RevealInEditor(int heading_index);
    void RevealInPreview(int editor_line);

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

    // Phase 09 Task 5: Adaptive throttle thresholds (bytes)
    static constexpr std::size_t kSmallDocThreshold = std::size_t{5} * 1024;  // 5KB
    static constexpr std::size_t kLargeDocThreshold = std::size_t{50} * 1024; // 50KB
    static constexpr int kSmallDocDebounceMs = 100;
    static constexpr int kMediumDocDebounceMs = 300;
    static constexpr int kLargeDocDebounceMs = 600;

    // Phase 09 Task 7: Snap point thresholds
    static constexpr double kSnapThreshold = 0.03; // ±3% to snap
    static constexpr int kSnapPointCount = 5;
    static constexpr std::array<double, kSnapPointCount> kSnapPoints = {
        0.25, 0.33, 0.50, 0.67, 0.75};

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
    std::vector<int> heading_positions_;     // editor line numbers of headings
    std::vector<std::string> heading_texts_; // heading text for breadcrumbs
    std::vector<int> heading_levels_;        // heading levels (1-6)

    // Phase 09 Task 1: Cursor-anchored sync
    int last_cursor_line_{0};
    int total_line_count_{1};
    void OnCursorPositionChanged(int line, int total_lines);

    // Phase 09 Task 2: Selection mirroring
    std::string last_selection_text_;
    void OnSelectionChanged(const std::string& selected_text);

    // Phase 09 Task 3: Sync accuracy indicator
    std::chrono::steady_clock::time_point last_edit_time_;
    std::chrono::steady_clock::time_point last_render_time_;
    bool render_pending_{false};

    // Phase 09 Task 4: Breadcrumbs
    std::string current_breadcrumb_;
    void UpdateBreadcrumb(int cursor_line);

    // Phase 09 Task 5: Adaptive throttling
    void UpdateRenderThrottle(std::size_t content_size);

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
    core::Subscription cursor_sync_sub_;      // Phase 09 Task 1
    core::Subscription selection_mirror_sub_; // Phase 09 Task 2

    // V8 Phase 12 (Phase 37): Paired traverse mode
    PairMode pair_mode_{PairMode::kNone};
    core::Subscription pair_request_sub_;

    // Phase 09 Task 6: Split direction
    core::events::SplitDirection split_direction_{core::events::SplitDirection::Horizontal};
    core::Subscription split_direction_sub_;

    // Phase 09 Task 7: Snap points helper
    static auto FindNearestSnapPoint(double ratio) -> double;

    // Phase 09 Task 8: Pin preview
    bool pin_preview_{false};
    std::string pinned_content_; // frozen preview content when pinned

    // Phase 09 Task 10: Per-file state persistence
    struct PerFileState
    {
        double split_ratio{kDefaultSplitRatio};
        core::events::ViewMode view_mode{core::events::ViewMode::Split};
        core::events::SplitDirection direction{core::events::SplitDirection::Horizontal};
    };
    std::unordered_map<std::string, PerFileState> per_file_states_;
    std::string current_file_path_;

    // Phase 09 Task 11: Typewriter mode
    bool typewriter_mode_{false};

    // Phase 09 Task 13: Export HTML subscription
    core::Subscription export_html_sub_;

    // Persistence
    void SaveSplitRatio();
    void RestoreSplitRatio();

    // Easing
    static auto EaseOutCubic(double progress) -> double;
};

} // namespace markamp::ui
