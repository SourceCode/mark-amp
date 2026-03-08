/// @file IconPipeline.h
/// @brief V16 Phase 03-04 — Unified icon ingestion, caching, and rendering pipeline.
///
/// Replaces scattered IconProvider, IconRenderer, and IconLibrary paths
/// with a single pipeline that routes all icon requests through the manifest.
#pragma once

#include "ui/IconCache.h"
#include "ui/IconManifest.h"
#include "ui/IconRegistry.h"

#include <wx/bitmap.h>
#include <wx/colour.h>
#include <wx/gdicmn.h>

#include <filesystem>
#include <string>

namespace markamp::ui
{

/// Configuration for the icon pipeline.
struct IconPipelineConfig
{
    /// Root directory for icon assets (SVG files).
    std::filesystem::path asset_root;

    /// Path to the icon manifest JSON file.
    std::filesystem::path manifest_path;

    /// Maximum number of cached rendered bitmaps.
    size_t max_cache_entries{1024};

    /// Default icon size in logical pixels.
    int default_size{16};
};

/// Unified icon pipeline: manifest → SVG loading → rendering → caching.
///
/// Usage:
/// ```cpp
/// IconPipelineConfig config;
/// config.asset_root = "resources/icons/";
/// config.manifest_path = "resources/icons/icon_manifest.json";
///
/// IconPipeline pipe;
/// pipe.initialize(config);
///
/// // Get a rendered bitmap for a file in the tree
/// auto bmp = pipe.get_file_icon("main.cpp", {16, 16}, theme_color);
/// ```
class IconPipeline
{
public:
    IconPipeline() = default;

    /// Initialize the pipeline: load manifest, set up asset root.
    /// Returns true if manifest loaded successfully.
    [[nodiscard]] auto initialize(const IconPipelineConfig& config) -> bool;

    /// Check if the pipeline is initialized.
    [[nodiscard]] auto is_initialized() const -> bool;

    // ── File icon rendering ──

    /// Get a rendered bitmap for a file icon, resolved by filename.
    [[nodiscard]] auto get_file_icon(const std::string& filename,
                                     const wxSize& size,
                                     const wxColour& color,
                                     double scale = 1.0) -> wxBitmap;

    /// Get a rendered bitmap by canonical icon ID.
    [[nodiscard]] auto get_icon_by_id(const std::string& canonical_id,
                                      const wxSize& size,
                                      const wxColour& color,
                                      double scale = 1.0) -> wxBitmap;

    // ── Folder icon rendering ──

    /// Get a rendered bitmap for an open folder icon, resolved by folder name.
    [[nodiscard]] auto get_open_folder_icon(const std::string& folder_name,
                                            const wxSize& size,
                                            const wxColour& color,
                                            double scale = 1.0) -> wxBitmap;

    /// Get a rendered bitmap for a closed folder icon, resolved by folder name.
    [[nodiscard]] auto get_closed_folder_icon(const std::string& folder_name,
                                              const wxSize& size,
                                              const wxColour& color,
                                              double scale = 1.0) -> wxBitmap;

    // ── Command icon rendering ──

    /// Get a rendered bitmap for a command icon.
    [[nodiscard]] auto get_command_icon(const std::string& command_id,
                                        const wxSize& size,
                                        const wxColour& color,
                                        double scale = 1.0) -> wxBitmap;

    // ── Pipeline management ──

    /// Invalidate all cached icons (e.g., after theme change).
    void invalidate_cache();

    /// Get the current cache size.
    [[nodiscard]] auto cache_size() const -> size_t;

    /// Warm the cache for commonly used icons at the given size/color.
    void warm_cache(const wxSize& size, const wxColour& color, double scale = 1.0);

    /// Direct access to the underlying manifest for introspection.
    [[nodiscard]] auto manifest() const -> const IconManifest&;

    /// Get any errors from initialization or rendering.
    [[nodiscard]] auto errors() const -> const std::vector<std::string>&;

private:
    IconPipelineConfig config_;
    IconManifest manifest_;
    IconRegistry registry_; ///< Fallback: existing SVG registry
    IconCache cache_;
    bool initialized_{false};
    std::vector<std::string> errors_;

    /// Load an SVG asset from the asset root and register it.
    [[nodiscard]] auto ensure_icon_loaded(const std::string& canonical_id) -> bool;

    /// Get asset path resolved to the asset root.
    [[nodiscard]] auto resolve_asset_path(const std::string& relative_path) const
        -> std::filesystem::path;

    /// Render a bitmap from a loaded icon, with caching.
    [[nodiscard]] auto render_cached(const std::string& icon_name,
                                     const wxSize& size,
                                     const wxColour& color,
                                     double scale) -> wxBitmap;
};

} // namespace markamp::ui
