/// @file CanvasShellStateModel.h
/// @brief P07-T02: Shared canvas shell state for tool rail, inspector, minimap.
///
/// All canvas shell controls read/write this model for synchronized behavior.
#pragma once

#include <string>

namespace markamp::core
{
class EventBus;

/// Active canvas tool.
enum class CanvasTool
{
    kSelect,
    kPan,
    kDraw,
    kText,
    kShape,
    kConnector,
};

/// Shared state model for canvas shell controls.
class CanvasShellStateModel
{
public:
    explicit CanvasShellStateModel(EventBus& bus);

    // ── Tool state ──
    void set_tool(CanvasTool tool);
    [[nodiscard]] auto tool() const -> CanvasTool { return active_tool_; }

    // ── Zoom ──
    void set_zoom(double zoom_percent);
    [[nodiscard]] auto zoom() const -> double { return zoom_percent_; }
    void zoom_in();
    void zoom_out();
    void zoom_to_fit();
    void zoom_reset();

    // ── Inspector ──
    void toggle_inspector();
    void set_inspector_visible(bool visible);
    [[nodiscard]] auto inspector_visible() const -> bool { return inspector_visible_; }

    // ── Minimap ──
    void toggle_minimap();
    void set_minimap_visible(bool visible);
    [[nodiscard]] auto minimap_visible() const -> bool { return minimap_visible_; }

    // ── Board dirty state ──
    void set_board_dirty(bool dirty);
    [[nodiscard]] auto board_dirty() const -> bool { return board_dirty_; }

    // ── Board loaded ──
    [[nodiscard]] auto has_board() const -> bool { return has_board_; }
    void set_has_board(bool loaded) { has_board_ = loaded; }

private:
    EventBus& event_bus_;
    CanvasTool active_tool_{CanvasTool::kSelect};
    double zoom_percent_{100.0};
    bool inspector_visible_{false};
    bool minimap_visible_{true};
    bool board_dirty_{false};
    bool has_board_{false};

    static constexpr double kMinZoom = 10.0;
    static constexpr double kMaxZoom = 500.0;
    static constexpr double kZoomStep = 25.0;
};

} // namespace markamp::core
