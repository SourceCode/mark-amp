#pragma once

/// @file WidgetRenderer.h
/// @brief Phase 14: Rendering pipeline for in-canvas app widgets.
///
/// Provides rendering contexts and style modes for `AppWidgetObject`
/// instances. Supports card, compact, minimal, and fullscreen layouts
/// with visual indicators for sync status, errors, and selection.

#include "canvas/AppWidgetObject.h"
#include "canvas/CanvasTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace markamp::canvas
{

/// Visual style for widget rendering.
enum class WidgetRenderStyle : uint8_t
{
    kCard,      ///< Full card with header, content, and status bar
    kCompact,   ///< Condensed view (title + status only)
    kMinimal,   ///< Icon + badge only
    kFullscreen ///< Expanded to fill viewport
};

/// Describes the render region for a widget.
struct WidgetRenderRegion
{
    double region_x{0.0};
    double region_y{0.0};
    double region_width{200.0};
    double region_height{150.0};
};

/// Context passed to the renderer for each widget draw call.
struct WidgetRenderContext
{
    WidgetRenderRegion bounds; ///< Widget bounds in world coords
    double zoom_level{1.0};    ///< Current viewport zoom
    bool is_selected{false};   ///< Whether the widget is selected
    bool is_hovered{false};    ///< Whether the cursor hovers this widget
    WidgetRenderStyle style{WidgetRenderStyle::kCard};
    CanvasColor theme_bg{255, 255, 255, 255};    ///< Theme background
    CanvasColor theme_foreground{0, 0, 0, 255};  ///< Theme text color
    CanvasColor theme_accent{66, 133, 244, 255}; ///< Theme accent color
};

/// Computed layout regions within a rendered widget.
struct WidgetLayoutRegions
{
    WidgetRenderRegion header;     ///< Title bar / toolbar region
    WidgetRenderRegion content;    ///< Main content area
    WidgetRenderRegion status_bar; ///< Bottom status / sync indicator
};

/// Renderer for `AppWidgetObject` instances on the canvas.
///
/// Computes layout regions, generates visual descriptions for the
/// UI layer, and handles placeholder / error states.
class WidgetRenderer
{
public:
    WidgetRenderer() = default;

    // ── Layout ────────────────────────────────────────────────────

    /// Compute sub-regions for a widget at the given context.
    [[nodiscard]] auto compute_layout(const WidgetRenderContext& context) const
        -> WidgetLayoutRegions;

    // ── Render descriptions ───────────────────────────────────────

    /// Render the widget in its current style.
    [[nodiscard]] auto render_widget(const AppWidgetObject& widget,
                                     const WidgetRenderContext& context) const -> std::string;

    /// Render a placeholder for a widget that's still loading.
    [[nodiscard]] auto render_placeholder(const WidgetRenderContext& context) const -> std::string;

    /// Render an error state for a failed widget.
    [[nodiscard]] auto render_error_state(const std::string& error_message,
                                          const WidgetRenderContext& context) const -> std::string;

    /// Render sync status indicator overlay.
    [[nodiscard]] auto render_sync_indicator(WidgetSyncStatus status,
                                             const WidgetRenderContext& context) const
        -> std::string;

    // ── Style queries ─────────────────────────────────────────────

    /// Human-readable name for a render style.
    [[nodiscard]] static auto style_name(WidgetRenderStyle style) -> std::string;

    /// Header height based on style and zoom level.
    [[nodiscard]] static auto header_height(WidgetRenderStyle style, double zoom_level) -> double;

    /// Status bar height based on style.
    [[nodiscard]] static auto status_bar_height(WidgetRenderStyle style) -> double;
};

} // namespace markamp::canvas
