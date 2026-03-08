#pragma once

#include "ui/IconCache.h"
#include "ui/IconPipeline.h"
#include "ui/IconRegistry.h"

#include <wx/dc.h>

#include <string>

namespace markamp::ui
{

/// A singleton manager tying the registry, cache, pipeline, and renderer together.
/// Provides high-level drawing APIs for icons onto contexts.
///
/// V16: Now integrates IconPipeline for manifest-driven icon resolution.
/// The pipeline is tried first; legacy registry is used as fallback.
class IconManager
{
public:
    IconManager(const IconManager&) = delete;
    IconManager& operator=(const IconManager&) = delete;
    IconManager(IconManager&&) = delete;
    IconManager& operator=(IconManager&&) = delete;

    /// Gets the global singleton instance.
    static auto get() -> IconManager&;

    /// Direct access to the registry for adding icons (legacy path).
    [[nodiscard]] auto registry() -> IconRegistry&;

    /// Direct access to the cache to clear or inspect size.
    [[nodiscard]] auto cache() -> IconCache&;

    /// V16: Direct access to the icon pipeline.
    [[nodiscard]] auto pipeline() -> IconPipeline&;

    /// V16: Initialize the icon pipeline with the given config.
    void initialize_pipeline(const IconPipelineConfig& config);

    /// V16: Called on theme change to invalidate all cached icons.
    void on_theme_changed();

    /// Re-renders an icon or fetches it from cache.
    /// V16: Now tries pipeline first, then legacy registry, then fallback.
    [[nodiscard]] auto get_icon_bitmap(const std::string& name,
                                       const wxSize& size,
                                       const wxColour& color,
                                       double scale = 1.0) -> wxBitmap;

    /// Draws an icon directly to a device context at the given coordinates.
    auto draw_icon(wxDC& drawing_context,
                   const std::string& name,
                   int x_pos,
                   int y_pos,
                   const wxSize& size,
                   const wxColour& color,
                   double scale = 1.0) -> void;

private:
    IconManager() = default;
    ~IconManager() = default;

    IconRegistry registry_;
    IconCache cache_;
    IconPipeline pipeline_; ///< V16: Manifest-driven icon pipeline
};

} // namespace markamp::ui
