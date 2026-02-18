// ============================================================================
// File: src/canvas/WidgetRenderer.cpp
// Phase 14: Canvas Extensibility — widget rendering pipeline
// ============================================================================
#include "canvas/WidgetRenderer.h"

#include <sstream>

namespace markamp::canvas
{

// ── Layout ────────────────────────────────────────────────────────

auto WidgetRenderer::compute_layout(const WidgetRenderContext& context) const -> WidgetLayoutRegions
{
    WidgetLayoutRegions layout;

    const double hdr_height = header_height(context.style, context.zoom_level);
    const double bar_height = status_bar_height(context.style);
    const double content_height = context.bounds.region_height - hdr_height - bar_height;

    layout.header = {
        context.bounds.region_x, context.bounds.region_y, context.bounds.region_width, hdr_height};

    layout.content = {context.bounds.region_x,
                      context.bounds.region_y + hdr_height,
                      context.bounds.region_width,
                      content_height > 0.0 ? content_height : 0.0};

    layout.status_bar = {context.bounds.region_x,
                         context.bounds.region_y + context.bounds.region_height - bar_height,
                         context.bounds.region_width,
                         bar_height};

    return layout;
}

// ── Render descriptions ───────────────────────────────────────────

auto WidgetRenderer::render_widget(const AppWidgetObject& widget,
                                   const WidgetRenderContext& context) const -> std::string
{
    std::ostringstream oss;

    oss << "[Widget:" << widget.widget_id() << "]";
    oss << " app=" << widget.app_id();
    oss << " style=" << style_name(context.style);
    oss << " size=" << context.bounds.region_width << "x" << context.bounds.region_height;

    if (context.is_selected)
    {
        oss << " [SELECTED]";
    }

    if (context.is_hovered)
    {
        oss << " [HOVERED]";
    }

    // Add sync status
    oss << " sync=" << render_sync_indicator(widget.sync_status(), context);

    return oss.str();
}

auto WidgetRenderer::render_placeholder(const WidgetRenderContext& context) const -> std::string
{
    std::ostringstream oss;
    oss << "[Placeholder]";
    oss << " size=" << context.bounds.region_width << "x" << context.bounds.region_height;
    oss << " style=" << style_name(context.style);
    oss << " loading...";
    return oss.str();
}

auto WidgetRenderer::render_error_state(const std::string& error_message,
                                        const WidgetRenderContext& context) const -> std::string
{
    std::ostringstream oss;
    oss << "[Error]";
    oss << " size=" << context.bounds.region_width << "x" << context.bounds.region_height;
    oss << " error=\"" << error_message << "\"";
    return oss.str();
}

auto WidgetRenderer::render_sync_indicator(WidgetSyncStatus status,
                                           const WidgetRenderContext& /*context*/) const
    -> std::string
{
    switch (status)
    {
        case WidgetSyncStatus::kIdle:
            return "idle";
        case WidgetSyncStatus::kSyncing:
            return "syncing";
        case WidgetSyncStatus::kSynced:
            return "synced";
        case WidgetSyncStatus::kError:
            return "error";
    }
    return "unknown";
}

// ── Style queries ─────────────────────────────────────────────────

auto WidgetRenderer::style_name(WidgetRenderStyle style) -> std::string
{
    switch (style)
    {
        case WidgetRenderStyle::kCard:
            return "card";
        case WidgetRenderStyle::kCompact:
            return "compact";
        case WidgetRenderStyle::kMinimal:
            return "minimal";
        case WidgetRenderStyle::kFullscreen:
            return "fullscreen";
    }
    return "unknown";
}

auto WidgetRenderer::header_height(WidgetRenderStyle style, double zoom_level) -> double
{
    const double base_header = 32.0;

    switch (style)
    {
        case WidgetRenderStyle::kCard:
            return base_header * zoom_level;
        case WidgetRenderStyle::kCompact:
            return 24.0 * zoom_level;
        case WidgetRenderStyle::kMinimal:
            return 0.0; // No header in minimal mode
        case WidgetRenderStyle::kFullscreen:
            return 40.0 * zoom_level;
    }
    return base_header;
}

auto WidgetRenderer::status_bar_height(WidgetRenderStyle style) -> double
{
    switch (style)
    {
        case WidgetRenderStyle::kCard:
            return 20.0;
        case WidgetRenderStyle::kCompact:
            return 16.0;
        case WidgetRenderStyle::kMinimal:
            return 0.0;
        case WidgetRenderStyle::kFullscreen:
            return 24.0;
    }
    return 20.0;
}

} // namespace markamp::canvas
