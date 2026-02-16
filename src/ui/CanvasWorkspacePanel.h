#pragma once

#include "core/EventBus.h"
#include "core/ThemeEngine.h"

#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/stattext.h>

#include <memory>
#include <string>

namespace markamp::core
{
class Config;
} // namespace markamp::core

namespace markamp::canvas
{
class CanvasPanel;
class MinimapPanel;
} // namespace markamp::canvas

namespace markamp::ui
{

/// Composite workspace host for canvas mode.
///
/// Layout:
///   ┌────────────────────────────────────────────┐
///   │  Contextual Bar (title, zoom, mode)        │
///   ├──────┬───────────────────────┬──────────────┤
///   │ Tool │                       │  Properties  │
///   │ Rail │     CanvasPanel       │  Inspector   │
///   │ 40px │       (flex)          │    240px     │
///   ├──────┴───────────────────────┴──────────────┤
///   │           Minimap Strip (120px)             │
///   └────────────────────────────────────────────┘
class CanvasWorkspacePanel : public wxPanel
{
public:
    CanvasWorkspacePanel(wxWindow* parent,
                         core::EventBus& event_bus,
                         core::ThemeEngine& theme_engine,
                         core::Config* config);
    ~CanvasWorkspacePanel() override = default;

    // ── Board Management ──────────────────────────────────────────

    /// Load an existing board by ID.
    void LoadBoard(const std::string& board_id);

    /// Create and display a new empty board.
    void NewBoard();

    // ── Accessors ─────────────────────────────────────────────────

    /// Access the inner CanvasPanel.
    [[nodiscard]] auto canvas_panel() -> canvas::CanvasPanel*;
    [[nodiscard]] auto canvas_panel() const -> const canvas::CanvasPanel*;

    /// Current board ID, empty if none loaded.
    [[nodiscard]] auto board_id() const -> const std::string&;

    // ── Inspector / Minimap Visibility ────────────────────────────

    void ToggleInspector();
    void ToggleMinimap();
    [[nodiscard]] bool is_inspector_visible() const;
    [[nodiscard]] bool is_minimap_visible() const;

    // ── Theme ─────────────────────────────────────────────────────

    void ApplyTheme();

    // ── Constants ─────────────────────────────────────────────────

    static constexpr int kToolRailWidth = 40;
    static constexpr int kInspectorWidth = 240;
    static constexpr int kMinimapHeight = 120;
    static constexpr int kContextBarHeight = 36;

private:
    void CreateLayout();
    void CreateToolRail();
    void CreateContextBar();
    void CreateInspector();
    void CreateMinimapStrip();
    void OnToolSelected(int tool_index);
    void OnZoomChanged(int zoom_percent);

    // Core references
    core::EventBus& event_bus_;
    core::ThemeEngine& theme_engine_;
    core::Config* config_{nullptr};

    // Layout containers
    wxBoxSizer* root_sizer_{nullptr};
    wxBoxSizer* middle_sizer_{nullptr};

    // Child panels
    wxPanel* context_bar_{nullptr};
    wxPanel* tool_rail_{nullptr};
    canvas::CanvasPanel* canvas_panel_{nullptr};
    wxPanel* inspector_{nullptr};
    wxPanel* minimap_strip_{nullptr};

    // Context bar controls
    wxStaticText* board_title_label_{nullptr};
    wxStaticText* zoom_label_{nullptr};

    // State
    std::string current_board_id_;
    bool inspector_visible_{true};
    bool minimap_visible_{true};
};

} // namespace markamp::ui
