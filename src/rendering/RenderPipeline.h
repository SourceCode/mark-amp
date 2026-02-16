/// RenderPipeline.h — Phase 18: Rendering Pipeline Coordinator
///
/// Wires together DirtyRegionAccumulator, ScrollBlit, ViewportState,
/// IncrementalLineWrap, and LineLayoutCache into a single coordinator
/// for incremental rendering.
///
/// Patterns implemented:
///   #2  Incremental rendering — only update changed regions
///   #13 Viewport virtualization
///   #27 Lazy line wrapping
///   #30 Tuned repaint strategy for scrolling

#pragma once

#include "DirtyRegion.h"
#include "IncrementalLineWrap.h"
#include "ScrollBlit.h"
#include "ViewportCache.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace markamp::rendering
{

/// Describes the work a renderer must do in a single frame.
struct FrameWork
{
    /// Dirty rectangles to repaint (coalesced).
    std::vector<Rect> dirty_rects;

    /// First logical line in the render range (with prefetch).
    std::size_t render_start_line{0};

    /// One past the last logical line in the render range (with prefetch).
    std::size_t render_end_line{0};

    /// Whether a full repaint is needed (overrides dirty_rects).
    bool full_repaint{false};
};

/// Coordinates all rendering subsystems for incremental rendering.
///
/// Typical frame loop:
///   1. Events fire on_scroll() / on_edit() / on_resize()
///   2. Renderer calls begin_frame() to get the FrameWork
///   3. Renderer paints only dirty regions within the viewport range
///   4. Renderer calls end_frame() to reset per-frame state
class RenderPipeline
{
public:
    RenderPipeline() = default;

    /// Initialize with viewport/back-buffer dimensions.
    void initialize(int32_t width, int32_t height) noexcept
    {
        scroll_blit_.initialize(width, height);
        viewport_width_ = width;
        viewport_height_ = height;
        initialized_ = true;
    }

    // ── Event handlers ──

    /// Called when the viewport scrolls. Computes revealed rect and
    /// adds it to the dirty region accumulator.
    void on_scroll(int32_t dx_pixels, int32_t dy_pixels)
    {
        scroll_blit_.record_scroll(dx_pixels, dy_pixels);

        if (scroll_blit_.is_full_repaint_needed(dx_pixels, dy_pixels)) [[unlikely]]
        {
            // Scroll exceeded viewport — mark everything dirty
            dirty_regions_.invalidate(Rect{0, 0, viewport_width_, viewport_height_});
            return;
        }

        // Vertical reveal
        if (dy_pixels != 0)
        {
            auto revealed = scroll_blit_.compute_revealed_rect(dy_pixels);
            if (!revealed.empty())
            {
                dirty_regions_.invalidate(revealed);
            }
        }

        // Horizontal reveal
        if (dx_pixels != 0)
        {
            auto revealed = scroll_blit_.compute_revealed_rect_horizontal(dx_pixels);
            if (!revealed.empty())
            {
                dirty_regions_.invalidate(revealed);
            }
        }
    }

    /// Called when text is edited. Invalidates affected lines in the
    /// line wrap cache and layout cache.
    void on_edit(std::size_t start_line, std::size_t end_line)
    {
        line_wrap_.invalidate_range(start_line, end_line);
        layout_cache_.invalidate_range(start_line, end_line);

        // Mark the visual area of the edited lines as dirty.
        // We approximate: invalidate from the start line to the bottom of viewport,
        // since edits can shift subsequent content.
        auto approx_top = static_cast<int32_t>(start_line) * line_height_estimate_;
        dirty_regions_.invalidate(Rect{0, approx_top, viewport_width_, viewport_height_});
    }

    /// Called when the viewport is resized. Reinitializes scroll blit
    /// and invalidates all wrap caches (wrap width changed).
    void on_resize(int32_t new_width, int32_t new_height) noexcept
    {
        scroll_blit_.initialize(new_width, new_height);
        viewport_width_ = new_width;
        viewport_height_ = new_height;

        // Wrap width changed — invalidate all line wraps
        line_wrap_.invalidate_all();
        layout_cache_.invalidate_all();

        // Full repaint on resize
        dirty_regions_.invalidate(Rect{0, 0, new_width, new_height});
    }

    /// Update the visible line range in the viewport.
    void set_viewport(std::size_t first_visible_line, std::size_t visible_line_count)
    {
        viewport_.first_visible_line = first_visible_line;
        viewport_.visible_line_count = visible_line_count;
    }

    /// Set the estimated line height in pixels (used for edit dirty region approximation).
    void set_line_height_estimate(int32_t height) noexcept
    {
        line_height_estimate_ = height;
    }

    // ── Frame lifecycle ──

    /// Prepare the frame's work list. Returns dirty rects + viewport range.
    [[nodiscard]] auto begin_frame(std::size_t total_lines) -> FrameWork
    {
        FrameWork work;
        work.dirty_rects = dirty_regions_.consume();
        work.render_start_line = viewport_.render_start(total_lines);
        work.render_end_line = viewport_.render_end(total_lines);

        // If the entire viewport is dirty, set full_repaint
        if (!work.dirty_rects.empty())
        {
            auto combined_area = 0;
            for (const auto& rect : work.dirty_rects)
            {
                combined_area += rect.width() * rect.height();
            }
            auto viewport_area = viewport_width_ * viewport_height_;
            if (viewport_area > 0 && combined_area >= viewport_area)
            {
                work.full_repaint = true;
            }
        }

        return work;
    }

    /// End the current frame. Resets per-frame scroll tracking.
    void end_frame() noexcept
    {
        scroll_blit_.reset_tracking();
    }

    // ── Accessors ──

    [[nodiscard]] auto dirty_regions() const noexcept -> const DirtyRegionAccumulator&
    {
        return dirty_regions_;
    }

    [[nodiscard]] auto viewport() const noexcept -> const ViewportState&
    {
        return viewport_;
    }

    [[nodiscard]] auto line_wrap() -> IncrementalLineWrap&
    {
        return line_wrap_;
    }

    [[nodiscard]] auto layout_cache() -> LineLayoutCache&
    {
        return layout_cache_;
    }

    [[nodiscard]] auto scroll_blit() const noexcept -> const ScrollBlit&
    {
        return scroll_blit_;
    }

    [[nodiscard]] auto is_initialized() const noexcept -> bool
    {
        return initialized_;
    }

private:
    DirtyRegionAccumulator dirty_regions_;
    ScrollBlit scroll_blit_;
    ViewportState viewport_;
    IncrementalLineWrap line_wrap_;
    LineLayoutCache layout_cache_;

    int32_t viewport_width_{0};
    int32_t viewport_height_{0};
    int32_t line_height_estimate_{20}; // default 20px per line
    bool initialized_{false};
};

} // namespace markamp::rendering
