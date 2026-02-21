#pragma once

#include "canvas/CanvasObject.h"
#include "canvas/CanvasTypes.h"
#include "canvas/ViewportTransform.h"

#include <functional>
#include <memory>
#include <unordered_map>
#include <vector>

// Forward declare wxGraphicsContext to avoid wx dependency in the header.
class wxGraphicsContext;

namespace markamp::canvas
{

/// Abstract renderer for a specific CanvasObjectType.
/// Phase-specific renderers (sticky note, shape, connector, etc.) implement this.
class IObjectRenderer
{
public:
    virtual ~IObjectRenderer() = default;

    /// The object type this renderer handles.
    [[nodiscard]] virtual auto object_type() const -> CanvasObjectType = 0;

    /// Render the object onto the given graphics context.
    /// The context is pre-configured with the viewport transform.
    virtual auto render(wxGraphicsContext& gc,
                        const CanvasObject& obj,
                        const ViewportTransform& viewport,
                        bool selected) -> void = 0;
};

/// Grid visual style.
enum class GridStyle : uint8_t
{
    kDots,
    kLines,
    kCross
};

/// Grid rendering settings.
struct GridSettings
{
    bool visible{true};
    double spacing{20.0}; // Grid spacing in world units
    CanvasColor color{200, 200, 200, 60};
    CanvasColor major_color{180, 180, 180, 100};
    int major_every{5}; // Every Nth line is a major line
    CanvasColor background{245, 245, 245, 255};
    GridStyle style{GridStyle::kLines}; // (#23)
    bool snap_to_grid{false};           // (#24)
    bool show_rulers{false};            // (#25)
};

/// Minimap rendering settings.
struct MinimapSettings
{
    CanvasColor background{30, 30, 38, 180};
    CanvasColor border{100, 100, 100, 200};
    CanvasColor viewport_rect{100, 99, 255, 200};
    CanvasColor object_rect{120, 180, 255, 120};
};

/// Render statistics for performance monitoring.
struct RenderStats
{
    size_t objects_rendered{0};
    size_t objects_culled{0};
    double frame_time_ms{0.0};
};

/// Coordinates rendering of canvas objects using registered IObjectRenderers.
/// This is the core rendering coordinator; it does not own wxWidgets objects.
class CanvasRenderer
{
public:
    CanvasRenderer() = default;

    /// Register a renderer for a specific object type.
    auto register_renderer(std::unique_ptr<IObjectRenderer> renderer) -> void;

    /// Check if a renderer is registered for the given type.
    [[nodiscard]] auto has_renderer(CanvasObjectType type) const -> bool;

    /// Render a complete frame: background → grid → objects → selection overlays.
    /// Objects are rendered in z-order (ascending).
    auto render_frame(wxGraphicsContext& gc,
                      const std::vector<CanvasObject*>& objects,
                      const ViewportTransform& viewport,
                      const std::vector<ObjectId>& selected_ids) -> RenderStats;

    /// Render just the background fill.
    auto render_background(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

    /// Render the grid overlay.
    auto render_grid(wxGraphicsContext& gc, const ViewportTransform& viewport) -> void;

    /// Grid settings.
    [[nodiscard]] auto grid_settings() const -> const GridSettings&;
    auto set_grid_settings(const GridSettings& settings) -> void;

    /// Render a minimap overview showing all object bounds and visible region (#26).
    auto render_minimap(wxGraphicsContext& gc,
                        const std::vector<CanvasObject*>& objects,
                        const ViewportTransform& viewport,
                        double minimap_width = 200.0,
                        double minimap_height = 150.0) -> void;

    /// Minimap settings.
    [[nodiscard]] auto minimap_settings() const -> const MinimapSettings&;
    auto set_minimap_settings(const MinimapSettings& settings) -> void;

    // ── Batch 3 (#17-18) ──────────────────────────────────────────

    /// Override the grid_settings background with a user-specified canvas background.
    [[nodiscard]] auto background_color() const -> const CanvasColor&;
    auto set_background_color(const CanvasColor& color) -> void;

    /// Toggle debug wireframe overlay on all objects for development.
    [[nodiscard]] auto debug_wireframes() const -> bool;
    auto set_debug_wireframes(bool enabled) -> void;

private:
    std::unordered_map<uint8_t, std::unique_ptr<IObjectRenderer>> renderers_;
    GridSettings grid_settings_;
    MinimapSettings minimap_settings_;
    CanvasColor background_color_{245, 245, 245, 255};
    bool debug_wireframes_{false};

    /// Fallback wireframe rendering for unregistered types.
    auto render_wireframe(wxGraphicsContext& gc,
                          const CanvasObject& obj,
                          const ViewportTransform& viewport) -> void;
};

} // namespace markamp::canvas
